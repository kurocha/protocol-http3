//
//  Session.hpp
//  This file is part of the "Protocol HTTP3" project and released under the MIT License.
//
//  Created by Samuel Williams on 24/5/2026.
//  Copyright, 2026, by Samuel Williams. All rights reserved.
//

#pragma once

#include <nghttp3/nghttp3.h>

#include <Protocol/QUIC/Connection.hpp>

#include <cstddef>
#include <cstdint>
#include <system_error>

namespace Protocol
{
	namespace HTTP3
	{
		const std::error_category & nghttp3_category();

		class Session
		{
		public:
			enum class Role
			{
				CLIENT,
				SERVER,
			};

			Session(Role role);
			virtual ~Session();

			Session(const Session &) = delete;
			Session & operator=(const Session &) = delete;

			nghttp3_conn * native_handle() noexcept {return _connection;}
			const nghttp3_conn * native_handle() const noexcept {return _connection;}

			Role role() const noexcept {return _role;}

			void bind_control_stream(Protocol::QUIC::StreamID stream_id);
			void bind_qpack_streams(Protocol::QUIC::StreamID encoder_stream_id, Protocol::QUIC::StreamID decoder_stream_id);

			nghttp3_ssize receive_stream_data(Protocol::QUIC::StreamID stream_id, const void *data, std::size_t size, bool is_final);
			nghttp3_ssize write_stream_data(Protocol::QUIC::StreamID & stream_id, bool & is_final, nghttp3_vec *vectors, std::size_t count);

			void add_write_offset(Protocol::QUIC::StreamID stream_id, std::size_t amount);
			void add_ack_offset(Protocol::QUIC::StreamID stream_id, std::uint64_t amount);
			void close_stream(Protocol::QUIC::StreamID stream_id, std::uint64_t error_code);
			void shutdown_stream_read(Protocol::QUIC::StreamID stream_id);
			void shutdown_stream_write(Protocol::QUIC::StreamID stream_id);
			void block_stream(Protocol::QUIC::StreamID stream_id);
			void unblock_stream(Protocol::QUIC::StreamID stream_id);
			void resume_stream(Protocol::QUIC::StreamID stream_id);

			void submit_request(Protocol::QUIC::StreamID stream_id, const nghttp3_nv *headers, std::size_t count, const nghttp3_data_reader *reader = nullptr, void *stream_data = nullptr);
			void submit_response(Protocol::QUIC::StreamID stream_id, const nghttp3_nv *headers, std::size_t count, const nghttp3_data_reader *reader = nullptr);
			void submit_trailers(Protocol::QUIC::StreamID stream_id, const nghttp3_nv *headers, std::size_t count);
			void submit_shutdown_notice();

			virtual void stream_data_acknowledged(Protocol::QUIC::StreamID stream_id, std::uint64_t size, void *stream_data);
			virtual void stream_closed(Protocol::QUIC::StreamID stream_id, std::uint64_t error_code, void *stream_data);
			virtual void stream_data_received(Protocol::QUIC::StreamID stream_id, const std::uint8_t *data, std::size_t size, void *stream_data);
			virtual void deferred_stream_data_consumed(Protocol::QUIC::StreamID stream_id, std::size_t size, void *stream_data);
			virtual void headers_started(Protocol::QUIC::StreamID stream_id, void *stream_data);
			virtual void header_received(Protocol::QUIC::StreamID stream_id, std::int32_t token, nghttp3_rcbuf *name, nghttp3_rcbuf *value, std::uint8_t flags, void *stream_data);
			virtual void headers_finished(Protocol::QUIC::StreamID stream_id, bool is_final, void *stream_data);
			virtual void trailers_started(Protocol::QUIC::StreamID stream_id, void *stream_data);
			virtual void trailer_received(Protocol::QUIC::StreamID stream_id, std::int32_t token, nghttp3_rcbuf *name, nghttp3_rcbuf *value, std::uint8_t flags, void *stream_data);
			virtual void trailers_finished(Protocol::QUIC::StreamID stream_id, bool is_final, void *stream_data);
			virtual void stop_sending(Protocol::QUIC::StreamID stream_id, std::uint64_t error_code, void *stream_data);
			virtual void stream_finished(Protocol::QUIC::StreamID stream_id, void *stream_data);
			virtual void reset_stream(Protocol::QUIC::StreamID stream_id, std::uint64_t error_code, void *stream_data);
			virtual void shutdown(std::int64_t identifier);
			virtual void settings_received(const nghttp3_proto_settings *settings);
			virtual void origin_received(const std::uint8_t *origin, std::size_t size);
			virtual void origin_finished();

		protected:
			Role _role;
			nghttp3_conn *_connection = nullptr;
			nghttp3_callbacks _callbacks = {};
			nghttp3_settings _settings = {};

			void initialize();
			void check(int result, const char *operation);
		};
	}
}
