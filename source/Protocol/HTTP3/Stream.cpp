//
//  Stream.cpp
//  This file is part of the "Protocol HTTP3" project and released under the MIT License.
//
//  Created by Samuel Williams on 25/5/2026.
//  Copyright, 2026, by Samuel Williams. All rights reserved.
//

#include "Stream.hpp"

#include <ngtcp2/ngtcp2.h>

namespace Protocol
{
	namespace HTTP3
	{
		Stream::Stream(Session & session, Protocol::QUIC::Connection & connection, Protocol::QUIC::StreamID stream_id) :
			Protocol::QUIC::Stream(connection, stream_id),
			_session(session)
		{
		}

		Stream::~Stream()
		{
		}

		void Stream::receive_data(std::size_t offset, const void *data, std::size_t size, Protocol::QUIC::StreamDataFlags flags)
		{
			(void)offset;

			_session.receive_stream_data(_stream_id, data, size, flags & NGTCP2_STREAM_DATA_FLAG_FIN);
		}

		Stream::Status Stream::send_data()
		{
			return Status::OK;
		}

		void Stream::acknowledge_data(std::size_t length)
		{
			_session.add_ack_offset(_stream_id, length);
		}

		void Stream::receive_input(const void *data, std::size_t size)
		{
			(void)data;
			(void)size;
		}

		void Stream::finish_input()
		{
		}

		void Stream::acknowledge_output(std::size_t length)
		{
			(void)length;
		}

		void Stream::close(std::uint32_t flags, std::uint64_t error_code)
		{
			(void)flags;

			_session.close_stream(_stream_id, error_code);
		}

		void Stream::reset(std::size_t final_size, std::uint64_t error_code)
		{
			(void)final_size;

			_session.shutdown_stream_write(_stream_id);
			_session.close_stream(_stream_id, error_code);
		}

		void Stream::stop_sending(std::uint64_t error_code)
		{
			(void)error_code;

			_session.shutdown_stream_read(_stream_id);
		}
	}
}
