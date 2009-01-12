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

#ifndef DLL_RANG_HPP
#define DLL_RANG_HPP

#include <WNS/logger/Logger.hpp>

#include <WNS/service/dll/DataTransmission.hpp>
#include <WNS/service/dll/Handler.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/container/Registry.hpp>

#include <WNS/osi/PDU.hpp>

namespace dll {
	/**
	 * @brief RANG Radio Access Network Gateway.
	 *
	 * This is the gateway to the core network for all APs.  The RANG is
	 * connected to the Network Layer (IP) of the server. The APs don't
	 * have an IP component, they terminate with the Layer2. So, the
	 * communication between the server and an AP goes via the RANG. The
	 * RANG has the funtion of a router, i.e. it forwards outgoing packets
	 * to the correct AP's DLL. From the Viewpoint of the server's NL, it
	 * behaves like a DLL itself
	 */
	class RANG :
		virtual public wns::service::dll::UnicastDataTransmission,
		virtual public wns::service::dll::Notification,
		virtual public wns::service::dll::Handler,
		public wns::node::component::Component
	{
		typedef wns::container::Registry<wns::service::dll::UnicastAddress, wns::service::dll::UnicastDataTransmission*> AccessPointLookup;
	public:
		RANG(wns::node::Interface*, const wns::pyconfig::View&);
		virtual ~RANG(){};

		/** @name wns::service::dll::DataTransmission service */
		//@{
		virtual void
		sendData(
			const wns::service::dll::UnicastAddress& _peer,
			const wns::osi::PDUPtr& _data,
			wns::service::dll::protocolNumber protocol);
		//@}

		virtual wns::service::dll::UnicastAddress
		getMACAddress() const { return wns::service::dll::UnicastAddress();}


		/** @name wns::service::dll::Notification service */
		//@{
		virtual void
		registerHandler(wns::service::dll::protocolNumber protocol, 
				wns::service::dll::Handler* _dh);
		//@}

		/** @name wns::service::dll::Handler Interface */
		//@{
		/** @brief standard onData method */
		virtual void onData(const wns::osi::PDUPtr& _data);

		/** @brief Modified Handler Interface for APs.
		 *
		 * Modified Handler Interface for APs. It gets two parameters in
		 * addition to the PDUPtr to keep the RANG's routing table up to
		 * date. */
		bool knowsAddress(wns::service::dll::UnicastAddress _sourceMACAddress);

		wns::service::dll::UnicastDataTransmission*
		getAccessPointFor(wns::service::dll::UnicastAddress _sourceMACAddress);

		void updateAPLookUp(wns::service::dll::UnicastAddress _sourceMACAddress,
				    wns::service::dll::UnicastDataTransmission* _ap);

		void removeAddress(wns::service::dll::UnicastAddress _sourceMACAddress,
		                   wns::service::dll::UnicastDataTransmission* _ap);

		void onData(const wns::osi::PDUPtr& _data,
				  wns::service::dll::UnicastAddress _sourceMACAddress,
				  wns::service::dll::UnicastDataTransmission* _ap);
		//@}

		/** @name wns::node::component::Component Interface */
		//@{
		virtual void onNodeCreated();
		virtual void onWorldCreated();
		virtual void onShutdown();
		//@}

	protected:
		virtual void
		doStartup();

	private:
		wns::pyconfig::View config;
		AccessPointLookup accessPointLookup;

		/**
		 * @brief Needed for demultiplexing of upper layer protocols.
		 */
		typedef wns::container::Registry<wns::service::dll::protocolNumber, wns::service::dll::Handler*> DataHandlerRegistry;

		/**
		 * @brief Registry for datahandlers. Each datahandler is select
		 * by the protocol number.
		 */
		DataHandlerRegistry dataHandlerRegistry;

		wns::logger::Logger logger;
	};
}

#endif // NOT defined DLL_RANG_HPP


