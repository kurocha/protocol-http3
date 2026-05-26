//
//  Session.cpp
//  This file is part of the "Protocol HTTP3" project and released under the MIT License.
//
//  Created by Samuel Williams on 24/5/2026.
//  Copyright, 2026, by Samuel Williams. All rights reserved.
//

#include "Session.hpp"

#include <string>
#include <iostream>

namespace Protocol
{
	namespace HTTP3
	{
		int stream_data_acknowledged_callback(nghttp3_conn *connection, std::int64_t stream_id, std::uint64_t size, void *connection_data, void *stream_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->stream_data_acknowledged(stream_id, size, stream_data);
			} catch (const std::exception & error) {
				std::cerr << "stream_data_acknowledged_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int stream_closed_callback(nghttp3_conn *connection, std::int64_t stream_id, std::uint64_t error_code, void *connection_data, void *stream_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->stream_closed(stream_id, error_code, stream_data);
			} catch (const std::exception & error) {
				std::cerr << "stream_closed_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int stream_data_received_callback(nghttp3_conn *connection, std::int64_t stream_id, const std::uint8_t *data, std::size_t size, void *connection_data, void *stream_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->stream_data_received(stream_id, data, size, stream_data);
			} catch (const std::exception & error) {
				std::cerr << "stream_data_received_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int deferred_stream_data_consumed_callback(nghttp3_conn *connection, std::int64_t stream_id, std::size_t size, void *connection_data, void *stream_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->deferred_stream_data_consumed(stream_id, size, stream_data);
			} catch (const std::exception & error) {
				std::cerr << "deferred_stream_data_consumed_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int headers_started_callback(nghttp3_conn *connection, std::int64_t stream_id, void *connection_data, void *stream_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->headers_started(stream_id, stream_data);
			} catch (const std::exception & error) {
				std::cerr << "headers_started_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int header_received_callback(nghttp3_conn *connection, std::int64_t stream_id, std::int32_t token, nghttp3_rcbuf *name, nghttp3_rcbuf *value, std::uint8_t flags, void *connection_data, void *stream_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->header_received(stream_id, token, name, value, flags, stream_data);
			} catch (const std::exception & error) {
				std::cerr << "header_received_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int headers_finished_callback(nghttp3_conn *connection, std::int64_t stream_id, int final_value, void *connection_data, void *stream_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->headers_finished(stream_id, final_value != 0, stream_data);
			} catch (const std::exception & error) {
				std::cerr << "headers_finished_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int trailers_started_callback(nghttp3_conn *connection, std::int64_t stream_id, void *connection_data, void *stream_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->trailers_started(stream_id, stream_data);
			} catch (const std::exception & error) {
				std::cerr << "trailers_started_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int trailer_received_callback(nghttp3_conn *connection, std::int64_t stream_id, std::int32_t token, nghttp3_rcbuf *name, nghttp3_rcbuf *value, std::uint8_t flags, void *connection_data, void *stream_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->trailer_received(stream_id, token, name, value, flags, stream_data);
			} catch (const std::exception & error) {
				std::cerr << "trailer_received_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int trailers_finished_callback(nghttp3_conn *connection, std::int64_t stream_id, int final_value, void *connection_data, void *stream_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->trailers_finished(stream_id, final_value != 0, stream_data);
			} catch (const std::exception & error) {
				std::cerr << "trailers_finished_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int stop_sending_callback(nghttp3_conn *connection, std::int64_t stream_id, std::uint64_t error_code, void *connection_data, void *stream_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->stop_sending(stream_id, error_code, stream_data);
			} catch (const std::exception & error) {
				std::cerr << "stop_sending_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int stream_finished_callback(nghttp3_conn *connection, std::int64_t stream_id, void *connection_data, void *stream_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->stream_finished(stream_id, stream_data);
			} catch (const std::exception & error) {
				std::cerr << "stream_finished_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int reset_stream_callback(nghttp3_conn *connection, std::int64_t stream_id, std::uint64_t error_code, void *connection_data, void *stream_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->reset_stream(stream_id, error_code, stream_data);
			} catch (const std::exception & error) {
				std::cerr << "reset_stream_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int shutdown_callback(nghttp3_conn *connection, std::int64_t identifier, void *connection_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->shutdown(identifier);
			} catch (const std::exception & error) {
				std::cerr << "shutdown_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int settings_received_callback(nghttp3_conn *connection, const nghttp3_proto_settings *settings, void *connection_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->settings_received(settings);
			} catch (const std::exception & error) {
				std::cerr << "settings_received_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int origin_received_callback(nghttp3_conn *connection, const std::uint8_t *origin, std::size_t size, void *connection_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->origin_received(origin, size);
			} catch (const std::exception & error) {
				std::cerr << "origin_received_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

		int origin_finished_callback(nghttp3_conn *connection, void *connection_data)
		{
			(void)connection;

			try {
				reinterpret_cast<Session *>(connection_data)->origin_finished();
			} catch (const std::exception & error) {
				std::cerr << "origin_finished_callback: " << error.what() << std::endl;
				return NGHTTP3_ERR_CALLBACK_FAILURE;
			}

			return 0;
		}

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
			_callbacks.acked_stream_data = stream_data_acknowledged_callback;
			_callbacks.stream_close = stream_closed_callback;
			_callbacks.recv_data = stream_data_received_callback;
			_callbacks.deferred_consume = deferred_stream_data_consumed_callback;
			_callbacks.begin_headers = headers_started_callback;
			_callbacks.recv_header = header_received_callback;
			_callbacks.end_headers = headers_finished_callback;
			_callbacks.begin_trailers = trailers_started_callback;
			_callbacks.recv_trailer = trailer_received_callback;
			_callbacks.end_trailers = trailers_finished_callback;
			_callbacks.stop_sending = stop_sending_callback;
			_callbacks.end_stream = stream_finished_callback;
			_callbacks.reset_stream = reset_stream_callback;
			_callbacks.shutdown = shutdown_callback;
			_callbacks.recv_origin = origin_received_callback;
			_callbacks.end_origin = origin_finished_callback;
			_callbacks.recv_settings2 = settings_received_callback;

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

		nghttp3_ssize Session::receive_stream_data(Protocol::QUIC::StreamID stream_id, const void *data, std::size_t size, bool is_final)
		{
			auto result = nghttp3_conn_read_stream2(
				_connection,
				stream_id,
				static_cast<const std::uint8_t *>(data),
				size,
				is_final ? 1 : 0,
				Protocol::QUIC::timestamp()
			);

			if (result < 0) {
				throw std::system_error(static_cast<int>(result), nghttp3_category(), "nghttp3_conn_read_stream2");
			}

			return result;
		}

		nghttp3_ssize Session::write_stream_data(Protocol::QUIC::StreamID & stream_id, bool & is_final, nghttp3_vec *vectors, std::size_t count)
		{
			int final_value = 0;
			auto result = nghttp3_conn_writev_stream(_connection, &stream_id, &final_value, vectors, count);

			if (result < 0) {
				throw std::system_error(static_cast<int>(result), nghttp3_category(), "nghttp3_conn_writev_stream");
			}

			is_final = final_value != 0;
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

		void Session::close_stream(Protocol::QUIC::StreamID stream_id, std::uint64_t error_code)
		{
			check(nghttp3_conn_close_stream(_connection, stream_id, error_code), "nghttp3_conn_close_stream");
		}

		void Session::shutdown_stream_read(Protocol::QUIC::StreamID stream_id)
		{
			check(nghttp3_conn_shutdown_stream_read(_connection, stream_id), "nghttp3_conn_shutdown_stream_read");
		}

		void Session::shutdown_stream_write(Protocol::QUIC::StreamID stream_id)
		{
			nghttp3_conn_shutdown_stream_write(_connection, stream_id);
		}

		void Session::block_stream(Protocol::QUIC::StreamID stream_id)
		{
			nghttp3_conn_block_stream(_connection, stream_id);
		}

		void Session::unblock_stream(Protocol::QUIC::StreamID stream_id)
		{
			check(nghttp3_conn_unblock_stream(_connection, stream_id), "nghttp3_conn_unblock_stream");
		}

		void Session::resume_stream(Protocol::QUIC::StreamID stream_id)
		{
			check(nghttp3_conn_resume_stream(_connection, stream_id), "nghttp3_conn_resume_stream");
		}

		void Session::submit_request(Protocol::QUIC::StreamID stream_id, const nghttp3_nv *headers, std::size_t count, const nghttp3_data_reader *reader, void *stream_data)
		{
			check(nghttp3_conn_submit_request(_connection, stream_id, headers, count, reader, stream_data), "nghttp3_conn_submit_request");
		}

		void Session::submit_response(Protocol::QUIC::StreamID stream_id, const nghttp3_nv *headers, std::size_t count, const nghttp3_data_reader *reader, void *stream_data)
		{
			check(nghttp3_conn_submit_response(_connection, stream_id, headers, count, reader), "nghttp3_conn_submit_response");

			if (stream_data) {
				check(nghttp3_conn_set_stream_user_data(_connection, stream_id, stream_data), "nghttp3_conn_set_stream_user_data");
			}
		}

		void Session::submit_trailers(Protocol::QUIC::StreamID stream_id, const nghttp3_nv *headers, std::size_t count)
		{
			check(nghttp3_conn_submit_trailers(_connection, stream_id, headers, count), "nghttp3_conn_submit_trailers");
		}

		void Session::submit_shutdown_notice()
		{
			check(nghttp3_conn_submit_shutdown_notice(_connection), "nghttp3_conn_submit_shutdown_notice");
		}

		void Session::stream_data_acknowledged(Protocol::QUIC::StreamID stream_id, std::uint64_t size, void *stream_data)
		{
			(void)stream_id;
			(void)size;
			(void)stream_data;
		}

		void Session::stream_closed(Protocol::QUIC::StreamID stream_id, std::uint64_t error_code, void *stream_data)
		{
			(void)stream_id;
			(void)error_code;
			(void)stream_data;
		}

		void Session::stream_data_received(Protocol::QUIC::StreamID stream_id, const std::uint8_t *data, std::size_t size, void *stream_data)
		{
			(void)stream_id;
			(void)data;
			(void)size;
			(void)stream_data;
		}

		void Session::deferred_stream_data_consumed(Protocol::QUIC::StreamID stream_id, std::size_t size, void *stream_data)
		{
			(void)stream_id;
			(void)size;
			(void)stream_data;
		}

		void Session::headers_started(Protocol::QUIC::StreamID stream_id, void *stream_data)
		{
			(void)stream_id;
			(void)stream_data;
		}

		void Session::header_received(Protocol::QUIC::StreamID stream_id, std::int32_t token, nghttp3_rcbuf *name, nghttp3_rcbuf *value, std::uint8_t flags, void *stream_data)
		{
			(void)stream_id;
			(void)token;
			(void)name;
			(void)value;
			(void)flags;
			(void)stream_data;
		}

		void Session::headers_finished(Protocol::QUIC::StreamID stream_id, bool is_final, void *stream_data)
		{
			(void)stream_id;
			(void)is_final;
			(void)stream_data;
		}

		void Session::trailers_started(Protocol::QUIC::StreamID stream_id, void *stream_data)
		{
			(void)stream_id;
			(void)stream_data;
		}

		void Session::trailer_received(Protocol::QUIC::StreamID stream_id, std::int32_t token, nghttp3_rcbuf *name, nghttp3_rcbuf *value, std::uint8_t flags, void *stream_data)
		{
			(void)stream_id;
			(void)token;
			(void)name;
			(void)value;
			(void)flags;
			(void)stream_data;
		}

		void Session::trailers_finished(Protocol::QUIC::StreamID stream_id, bool is_final, void *stream_data)
		{
			(void)stream_id;
			(void)is_final;
			(void)stream_data;
		}

		void Session::stop_sending(Protocol::QUIC::StreamID stream_id, std::uint64_t error_code, void *stream_data)
		{
			(void)stream_id;
			(void)error_code;
			(void)stream_data;
		}

		void Session::stream_finished(Protocol::QUIC::StreamID stream_id, void *stream_data)
		{
			(void)stream_id;
			(void)stream_data;
		}

		void Session::reset_stream(Protocol::QUIC::StreamID stream_id, std::uint64_t error_code, void *stream_data)
		{
			(void)stream_id;
			(void)error_code;
			(void)stream_data;
		}

		void Session::shutdown(std::int64_t identifier)
		{
			(void)identifier;
		}

		void Session::settings_received(const nghttp3_proto_settings *settings)
		{
			(void)settings;
		}

		void Session::origin_received(const std::uint8_t *origin, std::size_t size)
		{
			(void)origin;
			(void)size;
		}

		void Session::origin_finished()
		{
		}
	}
}
