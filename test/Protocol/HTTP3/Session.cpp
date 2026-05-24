//
//  Session.cpp
//  This file is part of the "Protocol HTTP3" project and released under the MIT License.
//
//  Created by Samuel Williams on 24/5/2026.
//  Copyright, 2026, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Protocol/HTTP3/Session.hpp>
#include <Protocol/QUIC/Configuration.hpp>

namespace Protocol
{
	namespace HTTP3
	{
		using namespace UnitTest::Expectations;
		
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
		};
	}
}
