//
//  Server.cpp
//  This file is part of the "Protocol HTTP3" project and released under the MIT License.
//
//  Created by Samuel Williams on 25/5/2026.
//  Copyright, 2026, by Samuel Williams. All rights reserved.
//

#include "Server.hpp"
#include "BufferedStream.hpp"

#include <Protocol/QUIC/Socket.hpp>

#include <array>
#include <vector>

namespace Protocol
{
	namespace HTTP3
	{
		Server::Server(Protocol::QUIC::Dispatcher & dispatcher, Protocol::QUIC::Configuration & configuration, Protocol::QUIC::TLS::ServerContext & tls_context, Protocol::QUIC::Socket & socket, const Protocol::QUIC::Address & remote_address, const ngtcp2_pkt_hd & packet_header, ngtcp2_cid *original_connection_id) :
			Protocol::QUIC::Server(dispatcher, configuration, tls_context, socket, remote_address, packet_header, original_connection_id),
			Session(Session::Role::SERVER)
		{
		}

		Server::~Server()
		{
		}

		Protocol::QUIC::Stream * Server::create_stream(Protocol::QUIC::StreamID stream_id)
		{
			return new BufferedStream(*this, *this, stream_id);
		}

		void Server::handshake_completed()
		{
			auto control_stream = open_unidirectional_stream();
			auto encoder_stream = open_unidirectional_stream();
			auto decoder_stream = open_unidirectional_stream();

			bind_control_stream(control_stream->stream_id());
			bind_qpack_streams(encoder_stream->stream_id(), decoder_stream->stream_id());

			send_packets();
		}

		Protocol::QUIC::Connection::Status Server::send_stream_data()
		{
			std::array<Protocol::QUIC::Byte, 1024*64> packet;
			std::array<nghttp3_vec, 16> http_vectors;

			while (true) {
				Protocol::QUIC::StreamID stream_id = -1;
				bool is_final = false;

				auto vector_count = write_stream_data(stream_id, is_final, http_vectors.data(), http_vectors.size());

				if (stream_id < 0) {
					break;
				}

				std::vector<ngtcp2_vec> stream_vectors;
				stream_vectors.reserve(static_cast<std::size_t>(vector_count));

				for (nghttp3_ssize index = 0; index < vector_count; ++index) {
					stream_vectors.push_back(ngtcp2_vec{
						.base = http_vectors[index].base,
						.len = http_vectors[index].len,
					});
				}

				ngtcp2_path_storage path_storage;
				ngtcp2_path_storage_zero(&path_storage);
				ngtcp2_pkt_info packet_info;
				ngtcp2_ssize written_length = 0;
				Protocol::QUIC::StreamDataFlags flags = is_final ? NGTCP2_WRITE_STREAM_FLAG_FIN : 0;

				auto result = ngtcp2_conn_writev_stream(Protocol::QUIC::Server::native_handle(), &path_storage.path, &packet_info, packet.data(), packet.size(), &written_length, flags, stream_id, stream_vectors.data(), stream_vectors.size(), Protocol::QUIC::timestamp());

				if (result == NGTCP2_ERR_STREAM_DATA_BLOCKED || result == NGTCP2_ERR_STREAM_SHUT_WR) {
					block_stream(stream_id);
					return Status(result);
				}

				if (result < 0) {
					return Status(result);
				}

				add_write_offset(stream_id, static_cast<std::size_t>(written_length));

				if (result > 0) {
					send_packet(path_storage.path, packet_info, packet.data(), result);
				}

				if (result == 0 && written_length == 0) {
					break;
				}
			}

			return Status::OK;
		}

		void Server::stream_data_acknowledged(Protocol::QUIC::StreamID stream_id, std::uint64_t size, void *stream_data)
		{
			(void)stream_data;

			auto *stream = reinterpret_cast<Protocol::QUIC::Stream *>(ngtcp2_conn_get_stream_user_data(Protocol::QUIC::Server::native_handle(), stream_id));
			auto *http_stream = dynamic_cast<Stream *>(stream);

			if (http_stream) {
				http_stream->acknowledge_output(size);
			}
		}

		void Server::stream_data_received(Protocol::QUIC::StreamID stream_id, const std::uint8_t *data, std::size_t size, void *stream_data)
		{
			(void)stream_data;

			auto *stream = reinterpret_cast<Protocol::QUIC::Stream *>(ngtcp2_conn_get_stream_user_data(Protocol::QUIC::Server::native_handle(), stream_id));
			auto *http_stream = dynamic_cast<Stream *>(stream);

			if (http_stream) {
				http_stream->receive_input(data, size);
			}
		}

		void Server::stream_finished(Protocol::QUIC::StreamID stream_id, void *stream_data)
		{
			(void)stream_data;

			auto *stream = reinterpret_cast<Protocol::QUIC::Stream *>(ngtcp2_conn_get_stream_user_data(Protocol::QUIC::Server::native_handle(), stream_id));
			auto *http_stream = dynamic_cast<Stream *>(stream);

			if (http_stream) {
				http_stream->finish_input();
			}
		}

		void Server::stop_sending(Protocol::QUIC::StreamID stream_id, std::uint64_t error_code, void *stream_data)
		{
			(void)stream_data;

			auto *stream = reinterpret_cast<Protocol::QUIC::Stream *>(ngtcp2_conn_get_stream_user_data(Protocol::QUIC::Server::native_handle(), stream_id));

			if (stream) {
				stream->stop_sending(error_code);
			}
		}

		void Server::reset_stream(Protocol::QUIC::StreamID stream_id, std::uint64_t error_code, void *stream_data)
		{
			(void)stream_data;

			ngtcp2_conn_shutdown_stream_write(Protocol::QUIC::Server::native_handle(), 0, stream_id, error_code);
		}
	}
}
