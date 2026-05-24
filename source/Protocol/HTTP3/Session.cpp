//
//  Session.cpp
//  This file is part of the "Protocol HTTP3" project and released under the MIT License.
//
//  Created by Samuel Williams on 24/5/2026.
//  Copyright, 2026, by Samuel Williams. All rights reserved.
//

#include "Session.hpp"

#include <string>

namespace Protocol
{
	namespace HTTP3
	{
		class Nghttp3ErrorCategory : public std::error_category
		{
		public:
			const char * name() const noexcept override
			{
				return "nghttp3";
			}
			
			std::string message(int condition) const override
			{
				return nghttp3_strerror(condition);
			}
		};
		
		const std::error_category & nghttp3_category()
		{
			static Nghttp3ErrorCategory category;
			return category;
		}
		
		Session::Session(Role role) : _role(role)
		{
			initialize();
		}
		
		Session::~Session()
		{
			if (_connection) {
				nghttp3_conn_del(_connection);
			}
		}
		
		void Session::initialize()
		{
			nghttp3_settings_default(&_settings);
			
			int result = 0;
			
			if (_role == Role::CLIENT) {
				result = nghttp3_conn_client_new(&_connection, &_callbacks, &_settings, nullptr, this);
			} else {
				result = nghttp3_conn_server_new(&_connection, &_callbacks, &_settings, nullptr, this);
			}
			
			check(result, "nghttp3_conn_new");
		}
		
		void Session::check(int result, const char *operation)
		{
			if (result < 0) {
				throw std::system_error(result, nghttp3_category(), operation);
			}
		}
		
		void Session::bind_control_stream(Protocol::QUIC::StreamID stream_id)
		{
			check(nghttp3_conn_bind_control_stream(_connection, stream_id), "nghttp3_conn_bind_control_stream");
		}
		
		void Session::bind_qpack_streams(Protocol::QUIC::StreamID encoder_stream_id, Protocol::QUIC::StreamID decoder_stream_id)
		{
			check(nghttp3_conn_bind_qpack_streams(_connection, encoder_stream_id, decoder_stream_id), "nghttp3_conn_bind_qpack_streams");
		}
		
		nghttp3_ssize Session::receive_stream_data(Protocol::QUIC::StreamID stream_id, const void *data, std::size_t size, bool fin)
		{
			auto result = nghttp3_conn_read_stream2(
				_connection,
				stream_id,
				static_cast<const std::uint8_t *>(data),
				size,
				fin ? 1 : 0,
				Protocol::QUIC::timestamp()
			);
			
			if (result < 0) {
				throw std::system_error(static_cast<int>(result), nghttp3_category(), "nghttp3_conn_read_stream2");
			}
			
			return result;
		}
		
		nghttp3_ssize Session::write_stream_data(Protocol::QUIC::StreamID & stream_id, bool & fin, nghttp3_vec *vectors, std::size_t count)
		{
			int fin_value = 0;
			auto result = nghttp3_conn_writev_stream(_connection, &stream_id, &fin_value, vectors, count);
			
			if (result < 0) {
				throw std::system_error(static_cast<int>(result), nghttp3_category(), "nghttp3_conn_writev_stream");
			}
			
			fin = fin_value != 0;
			return result;
		}
		
		void Session::add_write_offset(Protocol::QUIC::StreamID stream_id, std::size_t amount)
		{
			check(nghttp3_conn_add_write_offset(_connection, stream_id, amount), "nghttp3_conn_add_write_offset");
		}
		
		void Session::add_ack_offset(Protocol::QUIC::StreamID stream_id, std::uint64_t amount)
		{
			check(nghttp3_conn_add_ack_offset(_connection, stream_id, amount), "nghttp3_conn_add_ack_offset");
		}
	}
}
