/******************************************************************************
 * DLLBase (DLL Base classes to create FUN-based DLL)                         *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2006                                                         *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
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
//	class RANG;

	typedef std::list<wns::node::Interface*> NodeList;
	typedef std::list<wns::service::dll::UnicastAddress> MACList;

	inline std::string
	printNodeList(const dll::NodeList& list) {
		std::stringstream s;
		s << "(";
		for (dll::NodeList::const_iterator iter = list.begin();
		     iter != list.end(); ++iter)
		{
		  s << (*iter)->getName() << ",";
		}
		s << ")";
		return s.str();
	}

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
		NodeList getNodeList() const;
		//@}

	private:
		MACAdrLookup macAdrLookup;
		LayerLookup layerLookup;
		NodeLookup nodeLookup;
	};

	typedef wns::SingletonHolder<StationManager> TheStationManager;
}


#endif // NOT defined DLL_STATIONMANAGER_HPP


