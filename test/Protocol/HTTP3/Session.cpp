//
//  Session.cpp
//  This file is part of the "Protocol HTTP3" project and released under the MIT License.
//
//  Created by Samuel Williams on 24/5/2026.
//  Copyright, 2026, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Protocol/HTTP3/BufferedStream.hpp>
#include <Protocol/HTTP3/Session.hpp>
#include <Protocol/HTTP3/Server.hpp>
#include <Protocol/QUIC/Configuration.hpp>

#include <cstring>
#include <string>
#include <vector>

namespace Protocol
{
	namespace HTTP3
	{
		using namespace UnitTest::Expectations;

		class RecordingSession : public Session
		{
		public:
			using Session::Session;

			std::size_t settings_count = 0;
			std::vector<std::pair<std::string, std::string>> headers;
			std::vector<std::string> chunks;
			std::size_t headers_finished_count = 0;
			std::size_t stream_finished_count = 0;
			std::size_t acknowledged = 0;

			void settings_received(const nghttp3_proto_settings *settings) override
			{
				(void)settings;

				settings_count += 1;
			}

			void header_received(Protocol::QUIC::StreamID stream_id, std::int32_t token, nghttp3_rcbuf *name, nghttp3_rcbuf *value, std::uint8_t flags, void *stream_data) override
			{
				(void)stream_id;
				(void)token;
				(void)flags;
				(void)stream_data;

				auto name_buffer = nghttp3_rcbuf_get_buf(name);
				auto value_buffer = nghttp3_rcbuf_get_buf(value);

				headers.emplace_back(
					std::string(reinterpret_cast<const char *>(name_buffer.base), name_buffer.len),
					std::string(reinterpret_cast<const char *>(value_buffer.base), value_buffer.len)
				);
			}

			void headers_finished(Protocol::QUIC::StreamID stream_id, bool is_final, void *stream_data) override
			{
				(void)stream_id;
				(void)is_final;
				(void)stream_data;

				headers_finished_count += 1;
			}

			void stream_finished(Protocol::QUIC::StreamID stream_id, void *stream_data) override
			{
				(void)stream_id;
				(void)stream_data;

				stream_finished_count += 1;
			}

			void stream_data_received(Protocol::QUIC::StreamID stream_id, const std::uint8_t *data, std::size_t size, void *stream_data) override
			{
				(void)stream_id;
				(void)stream_data;

				chunks.emplace_back(reinterpret_cast<const char *>(data), size);
			}

			void stream_data_acknowledged(Protocol::QUIC::StreamID stream_id, std::uint64_t size, void *stream_data) override
			{
				(void)stream_id;

				acknowledged += size;

				if (stream_data) {
					static_cast<OutputBuffer *>(stream_data)->acknowledge(size);
				}
			}
		};

		std::size_t transfer(Session & source, Session & destination)
		{
			nghttp3_vec vectors[16];
			std::size_t transferred = 0;

			while (true) {
				Protocol::QUIC::StreamID stream_id = -1;
				bool is_final = false;
				auto count = source.write_stream_data(stream_id, is_final, vectors, 16);

				if (stream_id < 0)
					break;

				std::size_t size = 0;

				if (count == 0) {
					destination.receive_stream_data(stream_id, nullptr, 0, is_final);
				} else {
					for (nghttp3_ssize index = 0; index < count; ++index) {
						auto & vector = vectors[index];
						auto is_last_vector = index == count - 1;

						destination.receive_stream_data(stream_id, vector.base, vector.len, is_final && is_last_vector);

						size += vector.len;
					}
				}

				source.add_write_offset(stream_id, size);
				source.add_ack_offset(stream_id, size);

				transferred += size;
			}

			return transferred;
		}

		nghttp3_nv header(const char *name, const char *value)
		{
			return nghttp3_nv{
				.name = reinterpret_cast<std::uint8_t *>(const_cast<char *>(name)),
				.value = reinterpret_cast<std::uint8_t *>(const_cast<char *>(value)),
				.namelen = std::strlen(name),
				.valuelen = std::strlen(value),
				.flags = NGHTTP3_NV_FLAG_NONE,
			};
		}

		UnitTest::Suite SessionTestSuite {
			"Protocol::HTTP3::Session",

			{"it can create a client session",
				[](UnitTest::Examiner & examiner) {
					Session session(Session::Role::CLIENT);

					examiner.expect(session.native_handle()).to(be != nullptr);
					examiner.expect(session.role() == Session::Role::CLIENT).to(be == true);
				}
			},

			{"it can create a server session",
				[](UnitTest::Examiner & examiner) {
					Session session(Session::Role::SERVER);

					examiner.expect(session.native_handle()).to(be != nullptr);
					examiner.expect(session.role() == Session::Role::SERVER).to(be == true);
				}
			},

			{"it links against protocol-quic",
				[](UnitTest::Examiner & examiner) {
					Protocol::QUIC::Configuration configuration;

					examiner.expect(&configuration).to(be != nullptr);
				}
			},

			{"it can exchange request and response headers",
				[](UnitTest::Examiner & examiner) {
					RecordingSession client(Session::Role::CLIENT);
					RecordingSession server(Session::Role::SERVER);

					client.bind_control_stream(2);
					client.bind_qpack_streams(6, 10);

					server.bind_control_stream(3);
					server.bind_qpack_streams(7, 11);

					transfer(client, server);
					transfer(server, client);

					examiner.expect(client.settings_count).to(be == 1);
					examiner.expect(server.settings_count).to(be == 1);

					auto request_headers = std::vector<nghttp3_nv>{
						header(":method", "GET"),
						header(":scheme", "https"),
						header(":authority", "localhost"),
						header(":path", "/"),
					};

					client.submit_request(0, request_headers.data(), request_headers.size());
					transfer(client, server);

					examiner.expect(server.headers.size()).to(be == request_headers.size());
					examiner.expect(server.headers_finished_count).to(be == 1);
					examiner.expect(server.stream_finished_count).to(be == 1);

					auto response_headers = std::vector<nghttp3_nv>{
						header(":status", "200"),
					};

					server.submit_response(0, response_headers.data(), response_headers.size());
					transfer(server, client);

					examiner.expect(client.headers.size()).to(be == response_headers.size());
					examiner.expect(client.headers_finished_count).to(be == 1);
					examiner.expect(client.stream_finished_count).to(be == 1);
				}
			},

			{"it can exchange chunked response data",
				[](UnitTest::Examiner & examiner) {
					RecordingSession client(Session::Role::CLIENT);
					RecordingSession server(Session::Role::SERVER);

					client.bind_control_stream(2);
					client.bind_qpack_streams(6, 10);

					server.bind_control_stream(3);
					server.bind_qpack_streams(7, 11);

					transfer(client, server);
					transfer(server, client);

					auto request_headers = std::vector<nghttp3_nv>{
						header(":method", "GET"),
						header(":scheme", "https"),
						header(":authority", "localhost"),
						header(":path", "/"),
					};

					client.submit_request(0, request_headers.data(), request_headers.size());
					transfer(client, server);

					OutputBuffer output;

					auto response_headers = std::vector<nghttp3_nv>{
						header(":status", "200"),
					};

					server.submit_response(0, response_headers.data(), response_headers.size(), output.reader(), &output);

					output.append("Hello");
					output.append(" ");
					output.append("World!");
					output.close();

					transfer(server, client);

					examiner.expect(client.chunks.size()).to(be == 3);
					examiner.expect(client.chunks[0]).to(be == "Hello");
					examiner.expect(client.chunks[1]).to(be == " ");
					examiner.expect(client.chunks[2]).to(be == "World!");
					examiner.expect(client.stream_finished_count).to(be == 1);
					examiner.expect(output.pending()).to(be == 0);
					examiner.expect(output.retained()).to(be == 0);
				}
			},
		};
	}
}
