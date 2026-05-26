//
//  Server.hpp
//  This file is part of the "Protocol HTTP3" project and released under the MIT License.
//
//  Created by Samuel Williams on 25/5/2026.
//  Copyright, 2026, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Session.hpp"

#include <Protocol/QUIC/Server.hpp>

namespace Protocol
{
	namespace HTTP3
	{
		class Server : public Protocol::QUIC::Server, public Session
		{
		public:
			Server(Protocol::QUIC::Dispatcher & dispatcher, Protocol::QUIC::Configuration & configuration, Protocol::QUIC::TLS::ServerContext & tls_context, Protocol::QUIC::Socket & socket, const Protocol::QUIC::Address & remote_address, const ngtcp2_pkt_hd & packet_header, ngtcp2_cid *original_connection_id = nullptr);
			virtual ~Server();

			void handshake_completed() override;
			Protocol::QUIC::Connection::Status send_stream_data() override;

			void stream_data_acknowledged(Protocol::QUIC::StreamID stream_id, std::uint64_t size, void *stream_data) override;
			void stream_data_received(Protocol::QUIC::StreamID stream_id, const std::uint8_t *data, std::size_t size, void *stream_data) override;
			void stream_finished(Protocol::QUIC::StreamID stream_id, void *stream_data) override;

			void stop_sending(Protocol::QUIC::StreamID stream_id, std::uint64_t error_code, void *stream_data) override;
			void reset_stream(Protocol::QUIC::StreamID stream_id, std::uint64_t error_code, void *stream_data) override;

		protected:
			Protocol::QUIC::Stream * create_stream(Protocol::QUIC::StreamID stream_id) override;
		};
	}
}
