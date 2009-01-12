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

#ifndef DLL_UPPERCONVERGENCE_HPP
#define DLL_UPPERCONVERGENCE_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/Forwarding.hpp>
#include <WNS/ldk/FUNConfigCreator.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Forwarding.hpp>
#include <WNS/ldk/FirstServeConnector.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/service/dll/DataTransmission.hpp>
#include <WNS/service/dll/Handler.hpp>
#include <WNS/pyconfig/View.hpp>

namespace dll {
	class RANG;
	class PriorityProvider;

	/** @brief Command contributed by the UpperConvergence FU of the Data
	 * Link Layer (DLL) */
	class UpperCommand :
		public wns::ldk::Command
	{
	public:
		UpperCommand()
		{
			peer.sourceMACAddress = wns::service::dll::UnicastAddress();
			peer.targetMACAddress = wns::service::dll::UnicastAddress();
		}

		struct {
		} local;
		struct {
			wns::service::dll::UnicastAddress sourceMACAddress;
			wns::service::dll::UnicastAddress targetMACAddress;
		} peer; /**< @brief The source and target MAC addresses are signalled to the
			 * remote end. */
		struct {} magic;
	};

	/**
	 * @brief UpperConvergence base class, connecting the DLL-FUN with a Network Layer.
	 *
	 * Each Layer2 has an UpperConvergence FunctionalUnit that accepts compounds from below and
	 * IDUs from above respectively and forwards them to the corresponding TCPIP instance and
	 * the lower FunctionalUnit respectively.
	 */
	class UpperConvergence :
		virtual public wns::ldk::FunctionalUnit,
		virtual public wns::service::dll::UnicastDataTransmission,
		virtual public wns::service::dll::Notification,
		public wns::ldk::CommandTypeSpecifier<UpperCommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<wns::ldk::FirstServeConnector>,
		public wns::ldk::HasDeliverer<>
	{
	public:
		UpperConvergence(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);
		virtual ~UpperConvergence() {}

		// DataTransmission service
		virtual void
		sendData(
			const wns::service::dll::UnicastAddress& _peer,
			const wns::osi::PDUPtr& _data,
			wns::service::dll::protocolNumber protocol);

		virtual std::string
		getSubnetIdentifier() { return "DLLBase";}

		virtual void
		setMACAddress(const wns::service::dll::UnicastAddress& _sourceMACAddress);

		virtual wns::service::dll::UnicastAddress
		getMACAddress() const;

		virtual void
		onFUNCreated();

		virtual wns::ldk::CommandPool*
		createReply(const wns::ldk::CommandPool* original) const;

	protected:
		wns::service::dll::UnicastAddress sourceMACAddress;
		wns::logger::Logger logger;
	private:
		dll::PriorityProvider* pp;
	};


	/** @brief UT implementation of UpperConvergence, collaborates directly
	 * with the Network Layer */
	class UTUpperConvergence :
		public UpperConvergence,
		public wns::ldk::Forwarding<UTUpperConvergence>,
		public wns::Cloneable<UTUpperConvergence>
	{
	public:
		UTUpperConvergence(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);
		virtual ~UTUpperConvergence(){};
		virtual void processIncoming(const wns::ldk::CompoundPtr& compound);
		// Notification Service
		virtual void
		registerHandler(wns::service::dll::protocolNumber protocol,
				wns::service::dll::Handler* _dh);
	private:
		/**
		 * @brief Needed for demultiplexing of upper layer protocols.
		 */
		typedef wns::container::Registry<wns::service::dll::protocolNumber, wns::service::dll::Handler*> DataHandlerRegistry;

		/**
		 * @brief Registry for datahandlers. Each datahandler is select
		 * by the protocol number.
		 */
		DataHandlerRegistry dataHandlerRegistry;
	};

	/** @brief AP implementation of UpperConvergence, collaborates with the RANG */
	class APUpperConvergence :
		public UpperConvergence,
		public wns::ldk::Forwarding<APUpperConvergence>,
		public wns::Cloneable<APUpperConvergence>
	{
	public:
		APUpperConvergence(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);
		virtual ~APUpperConvergence(){};
		virtual void processIncoming(const wns::ldk::CompoundPtr& compound);
		// Notification Service
		virtual void
		registerHandler(wns::service::dll::protocolNumber protocol,
				wns::service::dll::Handler* _dh);
		bool hasRANG();
		dll::RANG* getRANG();
	private:
		RANG* dataHandler;
	};

	/**
	 * @brief Dummy UpperConvergence interface realisation.
	 *
	 * Relay Stations don't have a TCPIP stack on top, so they don't need an
	 * UpperConvergence FunctionalUnit. This is the forwarding-only UpperConvergence
	 * implementation for them. Note that instances of this FunctionalUnit get
	 * instanciated, but they will never see a single PDU.
	 */
	class NoUpperConvergence :
		public UpperConvergence,
		public wns::ldk::Forwarding<NoUpperConvergence>,
		public wns::Cloneable<NoUpperConvergence>
	{
	public:
		NoUpperConvergence(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
			UpperConvergence(fun,config),
			wns::ldk::Forwarding<NoUpperConvergence>(),
			wns::Cloneable<NoUpperConvergence>()
		{}
		virtual void
		registerHandler(wns::service::dll::protocolNumber /*protocol*/, wns::service::dll::Handler* /*_dh*/){};

/*		wns::service::dll::UnicastAddress
		getMACAddress() const { return wns::service::dll::UnicastAddress(); }*/
	};
}

#endif // NOT defined DLL_UPPERCONVERGENCE_HPP


