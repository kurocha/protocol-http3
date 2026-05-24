//
//  Stream.hpp
//  This file is part of the "Protocol HTTP3" project and released under the MIT License.
//
//  Created by Samuel Williams on 25/5/2026.
//  Copyright, 2026, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Session.hpp"

#include <Protocol/QUIC/Stream.hpp>

namespace Protocol
{
	namespace HTTP3
	{
		class Stream : public Protocol::QUIC::Stream
		{
		public:
			Stream(Session & session, Protocol::QUIC::Connection & connection, Protocol::QUIC::StreamID stream_id);
			virtual ~Stream();

			void receive_data(std::size_t offset, const void *data, std::size_t size, Protocol::QUIC::StreamDataFlags flags) override;
			Status send_data() override;
			void acknowledge_data(std::size_t length) override;

			void close(std::uint32_t flags, std::uint64_t error_code) override;
			void reset(std::size_t final_size, std::uint64_t error_code) override;
			void stop_sending(std::uint64_t error_code) override;

		protected:
			Session & _session;
		};
	}
}
