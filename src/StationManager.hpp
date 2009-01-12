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

#ifndef DLL_STATIONMANAGER_HPP
#define DLL_STATIONMANAGER_HPP

#include <WNS/logger/Logger.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/Singleton.hpp>
#include <WNS/container/Registry.hpp>

#include <DLL/Layer2.hpp>

#include <cstdlib>
#include <vector>

namespace wns { namespace node {
	class Node;
}}

namespace dll {
// 	class RANG;

	/** @brief The StationManager is an information database for all DLLs in the
	 * simulator.
	 *
	 * - Upon simulation startup, the RANG Node has to be registered at the
     *   Stationmanager.
	 * - After creation, each Layer2 instance also registers at the
	 * stationManager.
	 * @todo On the long run, we should get rid of the stationIDs here and only
	 * work on wns::service::dll::Address
	 */
	class StationManager
	{
		typedef wns::container::Registry<Layer2::StationIDType, Layer2*> LayerLookup;
		typedef wns::container::Registry<wns::node::Interface*, Layer2*> NodeLookup;
		typedef wns::container::Registry<wns::service::dll::UnicastAddress, Layer2*> MACAdrLookup;

	public:
		/** @brief Constructor */
		StationManager();
		/** @brief Destructor */
		virtual ~StationManager();

		/** @brief to register a station (Layer2) after its creation */
		void registerStation(Layer2::StationIDType id, wns::service::dll::UnicastAddress adr, Layer2* layer);

		/** @name Lookup methods */
		//@{
		Layer2* getStationByMAC(wns::service::dll::UnicastAddress mac) const;
		Layer2* getStationByID(Layer2::StationIDType) const;
		Layer2* getStationByNode(wns::node::Interface*) const;
		wns::node::Interface* getNodeByID(Layer2::StationIDType) const;
		//@}

	private:
		MACAdrLookup macAdrLookup;
		LayerLookup layerLookup;
		NodeLookup nodeLookup;
	};

	typedef wns::SingletonHolder<StationManager> TheStationManager;
}


#endif // NOT defined DLL_STATIONMANAGER_HPP


