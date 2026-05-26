//
//  BufferedStream.hpp
//  This file is part of the "Protocol HTTP3" project and released under the MIT License.
//
//  Created by Samuel Williams on 26/5/2026.
//  Copyright, 2026, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Stream.hpp"

#include <deque>
#include <string>
#include <string_view>

namespace Protocol
{
	namespace HTTP3
	{
		class ChunkBuffer
		{
			bool _closed = false;
			std::uint64_t _error_code = 0;

		public:
			void close() {_closed = true;}
			void close(std::uint64_t error_code) {close(); _error_code = error_code;}

			bool closed() const noexcept {return _closed;}
			std::uint64_t error_code() const noexcept {return _error_code;}
		};

		class InputBuffer final : public ChunkBuffer
		{
			std::deque<std::string> _chunks;

		public:
			void append(const void *data, std::size_t size);
			void append(std::string_view data);

			bool empty() const noexcept {return _chunks.empty();}
			std::size_t size() const noexcept {return _chunks.size();}

			const std::string & front() const {return _chunks.front();}
			std::string pop();
		};

		class OutputBuffer final : public ChunkBuffer
		{
			nghttp3_data_reader _reader;
			std::deque<std::string> _pending;
			std::deque<std::string> _retained;
			std::size_t _acknowledged = 0;

		public:
			OutputBuffer();

			nghttp3_data_reader * reader() noexcept {return &_reader;}
			const nghttp3_data_reader * reader() const noexcept {return &_reader;}

			void append(const void *data, std::size_t size);
			void append(std::string_view data);

			bool empty() const noexcept {return _pending.empty();}
			std::size_t pending() const noexcept {return _pending.size();}
			std::size_t retained() const noexcept {return _retained.size();}

			void acknowledge(std::size_t size);
			void stop_sending();

			static nghttp3_ssize read_data(nghttp3_conn *connection, std::int64_t stream_id, nghttp3_vec *vectors, std::size_t vector_count, std::uint32_t *flags, void *connection_data, void *stream_data);
		};

		class BufferedStream : public Stream
		{
		protected:
			InputBuffer _input_buffer;
			OutputBuffer _output_buffer;

		public:
			BufferedStream(Session & session, Protocol::QUIC::Connection & connection, Protocol::QUIC::StreamID stream_id);
			virtual ~BufferedStream();

			void receive_data(std::size_t offset, const void *data, std::size_t size, Protocol::QUIC::StreamDataFlags flags) override;
			void receive_input(const void *data, std::size_t size) override;
			void receive_input(std::string_view data);
			void finish_input() override;
			void acknowledge_output(std::size_t length) override;
			virtual void input_available();

			void acknowledge_data(std::size_t length) override;

			void close(std::uint32_t flags, std::uint64_t error_code) override;
			void reset(std::size_t final_size, std::uint64_t error_code) override;
			void stop_sending(std::uint64_t error_code) override;

			void write(const void *data, std::size_t size);
			void write(std::string_view data);
			void finish();

			InputBuffer & input_buffer() noexcept {return _input_buffer;}
			OutputBuffer & output_buffer() noexcept {return _output_buffer;}

			const InputBuffer & input_buffer() const noexcept {return _input_buffer;}
			const OutputBuffer & output_buffer() const noexcept {return _output_buffer;}
		};
	}
}
