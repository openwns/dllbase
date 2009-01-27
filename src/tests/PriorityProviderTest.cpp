/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <WNS/CppUnit.hpp>
#include <DLL/PriorityProvider.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/pyconfig/Parser.hpp>

namespace dll { namespace tests {

	class PriorityProviderTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( PriorityProviderTest );

		CPPUNIT_TEST( testPriorityCommand );

		CPPUNIT_TEST_SUITE_END();

		dll::PriorityProvider* pp;
		wns::ldk::Layer* layer;
		wns::ldk::fun::FUN* fuNet;
		wns::ldk::CommandProxy* proxy;

		void
		prepare()
		{
			layer = new wns::ldk::tests::LayerStub();
			fuNet = new wns::ldk::fun::Main(layer);
			wns::pyconfig::Parser emptyConfig;
			pp = new dll::PriorityProvider(fuNet, emptyConfig);
			fuNet->addFunctionalUnit("WINPROST.PriorityProvider",
						 "WINPROST.PriorityProvider",
						 pp);
			proxy = fuNet->getProxy();
		}

		void
		cleanup()
		{
			delete fuNet; fuNet = NULL;
			delete layer; layer = NULL;
			proxy = NULL;
		}

	public:

		void
		testPriorityCommand()
		{
			wns::ldk::CommandPool* pool = proxy->createCommandPool();

			CPPUNIT_ASSERT_NO_THROW( pp->activateCommand(pool));

			dll::PriorityProviderCommand* ppc = NULL;

			CPPUNIT_ASSERT_NO_THROW( ppc = pp->getCommand(pool) );
			CPPUNIT_ASSERT( ppc->local.priority == false );
			CPPUNIT_ASSERT_EQUAL( ppc->getSize(), 0 );

			delete pool;
		}
	};


	CPPUNIT_TEST_SUITE_REGISTRATION( PriorityProviderTest );

} // tests
} // dll




