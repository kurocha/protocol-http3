//
//  BufferedStream.cpp
//  This file is part of the "Protocol HTTP3" project and released under the MIT License.
//
//  Created by Samuel Williams on 26/5/2026.
//  Copyright, 2026, by Samuel Williams. All rights reserved.
//

#include "BufferedStream.hpp"

#include <ngtcp2/ngtcp2.h>

#include <stdexcept>

namespace Protocol
{
	namespace HTTP3
	{
		void InputBuffer::append(const void *data, std::size_t size)
		{
			append(std::string_view(static_cast<const char *>(data), size));
		}

		void InputBuffer::append(std::string_view data)
		{
			if (closed()) {
				throw std::runtime_error("Cannot append to closed input buffer!");
			}

			_chunks.emplace_back(data);
		}

		std::string InputBuffer::pop()
		{
			auto chunk = std::move(_chunks.front());
			_chunks.pop_front();
			return chunk;
		}

		OutputBuffer::OutputBuffer() : _reader{read_data}
		{
		}

		void OutputBuffer::append(const void *data, std::size_t size)
		{
			append(std::string_view(static_cast<const char *>(data), size));
		}

		void OutputBuffer::append(std::string_view data)
		{
			if (closed()) {
				throw std::runtime_error("Cannot append to closed output buffer!");
			}

			_pending.emplace_back(data);
		}

		void OutputBuffer::acknowledge(std::size_t size)
		{
			_acknowledged += size;

			while (!_retained.empty() && _acknowledged >= _retained.front().size()) {
				_acknowledged -= _retained.front().size();
				_retained.pop_front();
			}
		}

		void OutputBuffer::stop_sending()
		{
			close();
			_pending.clear();
			_retained.clear();
		}

		nghttp3_ssize OutputBuffer::read_data(nghttp3_conn *connection, std::int64_t stream_id, nghttp3_vec *vectors, std::size_t vector_count, std::uint32_t *flags, void *connection_data, void *stream_data)
		{
			(void)connection;
			(void)stream_id;
			(void)connection_data;

			if (vector_count == 0) {
				return 0;
			}

			auto output_buffer = static_cast<OutputBuffer *>(stream_data);

			if (!output_buffer) {
				*flags |= NGHTTP3_DATA_FLAG_EOF;
				return 0;
			}

			if (output_buffer->_pending.empty()) {
				if (!output_buffer->closed()) {
					return NGHTTP3_ERR_WOULDBLOCK;
				}

				*flags |= NGHTTP3_DATA_FLAG_EOF;
				return 0;
			}

			output_buffer->_retained.push_back(std::move(output_buffer->_pending.front()));
			output_buffer->_pending.pop_front();

			auto & chunk = output_buffer->_retained.back();

			vectors[0].base = reinterpret_cast<std::uint8_t *>(chunk.data());
			vectors[0].len = chunk.size();

			if (output_buffer->closed() && output_buffer->_pending.empty()) {
				*flags |= NGHTTP3_DATA_FLAG_EOF;
			}

			return 1;
		}

		BufferedStream::BufferedStream(Session & session, Protocol::QUIC::Connection & connection, Protocol::QUIC::StreamID stream_id) :
			Stream(session, connection, stream_id)
		{
		}

		BufferedStream::~BufferedStream()
		{
		}

		void BufferedStream::receive_data(std::size_t offset, const void *data, std::size_t size, Protocol::QUIC::StreamDataFlags flags)
		{
			Stream::receive_data(offset, data, size, flags);
		}

		void BufferedStream::receive_input(const void *data, std::size_t size)
		{
			_input_buffer.append(data, size);
			input_available();
		}

		void BufferedStream::receive_input(std::string_view data)
		{
			receive_input(data.data(), data.size());
		}

		void BufferedStream::finish_input()
		{
			_input_buffer.close();
			input_available();
		}

		void BufferedStream::input_available()
		{
		}

		void BufferedStream::acknowledge_output(std::size_t length)
		{
			_output_buffer.acknowledge(length);
		}

		void BufferedStream::acknowledge_data(std::size_t length)
		{
			Stream::acknowledge_data(length);
		}

		void BufferedStream::close(std::uint32_t flags, std::uint64_t error_code)
		{
			_input_buffer.close(error_code);
			_output_buffer.close(error_code);

			Stream::close(flags, error_code);
		}

		void BufferedStream::reset(std::size_t final_size, std::uint64_t error_code)
		{
			_output_buffer.close(error_code);
			Stream::reset(final_size, error_code);
		}

		void BufferedStream::stop_sending(std::uint64_t error_code)
		{
			_input_buffer.close(error_code);
			_output_buffer.stop_sending();

			Stream::stop_sending(error_code);
		}

		void BufferedStream::write(const void *data, std::size_t size)
		{
			auto was_empty = _output_buffer.empty();
			_output_buffer.append(data, size);

			if (was_empty) {
				_session.resume_stream(_stream_id);
			}
		}

		void BufferedStream::write(std::string_view data)
		{
			write(data.data(), data.size());
		}

		void BufferedStream::finish()
		{
			auto was_empty = _output_buffer.empty();
			_output_buffer.close();

			if (was_empty) {
				_session.resume_stream(_stream_id);
			}
		}
	}
}
