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
			
			nghttp3_ssize receive_stream_data(Protocol::QUIC::StreamID stream_id, const void *data, std::size_t size, bool fin);
			nghttp3_ssize write_stream_data(Protocol::QUIC::StreamID & stream_id, bool & fin, nghttp3_vec *vectors, std::size_t count);
			
			void add_write_offset(Protocol::QUIC::StreamID stream_id, std::size_t amount);
			void add_ack_offset(Protocol::QUIC::StreamID stream_id, std::uint64_t amount);
			
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
