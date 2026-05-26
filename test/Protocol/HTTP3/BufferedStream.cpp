//
//  BufferedStream.cpp
//  This file is part of the "Protocol HTTP3" project and released under the MIT License.
//
//  Created by Samuel Williams on 26/5/2026.
//  Copyright, 2026, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Protocol/HTTP3/BufferedStream.hpp>
#include <Protocol/QUIC/Configuration.hpp>

#include <string>

namespace Protocol
{
	namespace HTTP3
	{
		using namespace UnitTest::Expectations;

		class MockConnection : public Protocol::QUIC::Connection
		{
		public:
			using Protocol::QUIC::Connection::Connection;

		protected:
			Protocol::QUIC::Stream * create_stream(Protocol::QUIC::StreamID stream_id) override
			{
				(void)stream_id;
				return nullptr;
			}
		};

		class CountingStream : public BufferedStream
		{
		public:
			using BufferedStream::BufferedStream;

			std::size_t input_available_count = 0;

			void input_available() override
			{
				input_available_count += 1;
			}
		};

		UnitTest::Suite BufferedStreamTestSuite {
			"Protocol::HTTP3::BufferedStream",

			{"it can buffer output chunks",
				[](UnitTest::Examiner & examiner) {
					OutputBuffer output;
					nghttp3_vec vectors[1];
					std::uint32_t flags = 0;

					examiner.expect(OutputBuffer::read_data(nullptr, 0, vectors, 1, &flags, nullptr, &output)).to(be == NGHTTP3_ERR_WOULDBLOCK);

					output.append("Hello");
					output.append("World!");

					examiner.expect(output.pending()).to(be == 2);
					examiner.expect(output.retained()).to(be == 0);

					examiner.expect(OutputBuffer::read_data(nullptr, 0, vectors, 1, &flags, nullptr, &output)).to(be == 1);
					examiner.expect(std::string(reinterpret_cast<char *>(vectors[0].base), vectors[0].len)).to(be == "Hello");
					examiner.expect(output.pending()).to(be == 1);
					examiner.expect(output.retained()).to(be == 1);

					output.acknowledge(5);

					examiner.expect(output.retained()).to(be == 0);

					output.close();

					examiner.expect(OutputBuffer::read_data(nullptr, 0, vectors, 1, &flags, nullptr, &output)).to(be == 1);
					examiner.expect(std::string(reinterpret_cast<char *>(vectors[0].base), vectors[0].len)).to(be == "World!");
					examiner.expect((flags & NGHTTP3_DATA_FLAG_EOF) != 0).to(be == true);

					output.acknowledge(6);

					examiner.expect(output.pending()).to(be == 0);
					examiner.expect(output.retained()).to(be == 0);
				}
			},

			{"it can buffer input chunks",
				[](UnitTest::Examiner & examiner) {
					InputBuffer input;

					input.append("Hello");
					input.append(" ");
					input.append("World!");

					examiner.expect(input.size()).to(be == 3);
					examiner.expect(input.pop()).to(be == "Hello");
					examiner.expect(input.pop()).to(be == " ");
					examiner.expect(input.pop()).to(be == "World!");
					examiner.expect(input.empty()).to(be == true);

					input.close();

					examiner.expect(input.closed()).to(be == true);
				}
			},

			{"it invokes input available when decoded chunks arrive",
				[](UnitTest::Examiner & examiner) {
					Session session(Session::Role::SERVER);
					Protocol::QUIC::Configuration configuration;
					MockConnection connection(configuration);
					CountingStream stream(session, connection, 0);

					stream.receive_input("Hello");

					examiner.expect(stream.input_buffer().size()).to(be == 1);
					examiner.expect(stream.input_buffer().front()).to(be == "Hello");
					examiner.expect(stream.input_available_count).to(be == 1);

					stream.finish_input();

					examiner.expect(stream.input_buffer().closed()).to(be == true);
					examiner.expect(stream.input_available_count).to(be == 2);
				}
			},

			{"it releases retained output chunks when data is acknowledged",
				[](UnitTest::Examiner & examiner) {
					Session session(Session::Role::SERVER);
					Protocol::QUIC::Configuration configuration;
					MockConnection connection(configuration);
					CountingStream stream(session, connection, 0);
					nghttp3_vec vectors[1];
					std::uint32_t flags = 0;

					stream.output_buffer().append("Hello");

					examiner.expect(OutputBuffer::read_data(nullptr, 0, vectors, 1, &flags, nullptr, &stream.output_buffer())).to(be == 1);
					examiner.expect(stream.output_buffer().pending()).to(be == 0);
					examiner.expect(stream.output_buffer().retained()).to(be == 1);

					stream.acknowledge_output(5);

					examiner.expect(stream.output_buffer().retained()).to(be == 0);
				}
			},
		};
	}
}
