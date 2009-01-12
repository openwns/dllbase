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

#include <DLL/StationManager.hpp>
#include <DLL/UpperConvergence.hpp>
#include <DLL/RANG.hpp>
#include <DLL/Layer2.hpp>

#include <WNS/node/Node.hpp>
#include <WNS/Functor.hpp>
#include <WNS/Assure.hpp>


using namespace dll;

StationManager::StationManager() :
	macAdrLookup(),
	layerLookup()
{}

StationManager::~StationManager()
{
} // ~StationManager

void
StationManager::registerStation(Layer2::StationIDType id,
								wns::service::dll::UnicastAddress adr,
								Layer2* layer)
{
	//assure(! layerLookup.knows(id), "Station already registered.");

	assure(! macAdrLookup.knows(adr), "Station already registered.");
	macAdrLookup.insert(adr, layer);

	if (layerLookup.knows(id))
	{
		// id is already known -> station has more than one address
		assure(this->getStationByID(id) == layer, "A station with id " << id << " is already registered using a differend Layer2*");
	}
	else
	{
		layerLookup.insert(id, layer);
	}

	if(nodeLookup.knows(layer->getNode()))
	{
		assure(this->getStationByNode(layer->getNode()) == layer, "A station with this node is already registered using a different Layer2*");
	}
	else
	{
		nodeLookup.insert(layer->getNode(), layer);
	}
} // registerStation


Layer2*
StationManager::getStationByID(Layer2::StationIDType id) const
{
	return layerLookup.find(id);
} // getStationByID

Layer2*
StationManager::getStationByNode(wns::node::Interface* node) const
{
	return nodeLookup.find(node);
}


wns::node::Interface*
StationManager::getNodeByID(Layer2::StationIDType id) const
{
	return this->getStationByID(id)->getNode();
} // getStationByID


Layer2*
StationManager::getStationByMAC(wns::service::dll::UnicastAddress adr) const
{
	return macAdrLookup.find(adr);
}


