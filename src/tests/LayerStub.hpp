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

#ifndef DLL_TESTS_LAYERSTUB_HPP
#define DLL_TESTS_LAYERSTUB_HPP

#include <DLL/Layer2.hpp>
#include <WNS/node/tests/Stub.hpp>
#include <WNS/pyconfig/View.hpp>

namespace dll { namespace tests {

	/**
	 * @brief Simple dll::Layer Stub for unit tests
	 * @author Ismet Aktas <isi@comnets.rwth-aachen.de>
	 *
	 * This LayerStub will automatically create a node::tests::Stub
	 */
	class LayerStub :
		public dll::Layer2
	{
	public:
		LayerStub(wns::node::Interface*, const wns::pyconfig::View&, StationManager*);

		virtual
		~LayerStub();

		virtual std::string getName() const;

		virtual wns::node::Interface* getNode() const;

		virtual void onNodeCreated();

		virtual void onWorldCreated();

		virtual void onShutdown();

		virtual std::string getNodeName() const;

	private:
		virtual void
		doStartup();

		wns::node::Interface* node;
	};
} // tests
} // dll

#endif // NOT defined WNS_LDK_LAYER_HPP


