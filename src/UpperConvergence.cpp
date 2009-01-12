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

#include <DLL/PriorityProvider.hpp>

#include <WNS/service/nl/Address.hpp>
#include <WNS/service/dll/ProtocolNumber.hpp>
#include <WNS/ReferenceModifier.hpp>
#include <WNS/ReferenceModifier.hpp>

#include <DLL/UpperConvergence.hpp>
#include <DLL/RANG.hpp>

using namespace dll;

STATIC_FACTORY_REGISTER_WITH_CREATOR(NoUpperConvergence, wns::ldk::FunctionalUnit,
				     "dll.upperconvergence.None", wns::ldk::FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(UTUpperConvergence, wns::ldk::FunctionalUnit,
				     "dll.upperconvergence.UT", wns::ldk::FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(APUpperConvergence, wns::ldk::FunctionalUnit,
				     "dll.upperconvergence.AP", wns::ldk::FUNConfigCreator);

UpperConvergence::UpperConvergence(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
	wns::ldk::CommandTypeSpecifier<UpperCommand>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<wns::ldk::FirstServeConnector>(),
	wns::ldk::HasDeliverer<>(),

	sourceMACAddress(),
	logger(config.get("logger")),
	pp(NULL)
{}

void
UpperConvergence::onFUNCreated()
{
	/** @todo pab,dbn: remove the try..catch when DLLBase has been merged
	 * with WinProSt. */
	try
	{
		pp = getFUN()->findFriend<PriorityProvider*>("DLL.PriorityProvider");
	}
	catch (wns::Exception)
	{
		MESSAGE_SINGLE(NORMAL, logger, "WARNING: You should consider adding a PriorityProvider to your FUN!");
		pp = NULL;
	}
}

void
UpperConvergence::sendData(
	const wns::service::dll::UnicastAddress& _peer,
	const wns::osi::PDUPtr& pdu,
	wns::service::dll::protocolNumber protocol)
{
	pdu->setPDUType(protocol);

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << ": doSendData() called in convergence::Upper, target DLLAddress: " << _peer;
	MESSAGE_END();

	wns::ldk::CompoundPtr compound(new wns::ldk::Compound(getFUN()->createCommandPool(), pdu));

	activateCommand(compound->getCommandPool());

	UpperCommand* sgc = getCommand(compound->getCommandPool());

	sgc->peer.targetMACAddress = _peer;
	sgc->peer.sourceMACAddress = sourceMACAddress;
	if (pp != NULL)
	{
		/* Activate Priority Command */
		PriorityProviderCommand* ppCommand = pp->activateCommand(compound->getCommandPool());

		/* For MIH Signalling we enable high priority */
		if (protocol == wns::service::dll::MIH)
		{
			ppCommand->local.priority = true;
		}
	}

	if(this->getConnector()->hasAcceptor(compound))
	{
		this->wns::ldk::FunctionalUnit::sendData(compound);
	}
	else
	{
		MESSAGE_SINGLE(NORMAL, logger, "Dropped Outgoing Compound because DLL cannot handle it.");
	}
}


void
UpperConvergence::setMACAddress(const wns::service::dll::UnicastAddress& _sourceMACAddress)
{
	sourceMACAddress = _sourceMACAddress;
}

wns::service::dll::UnicastAddress
UpperConvergence::getMACAddress() const
{
	return sourceMACAddress;
}

wns::ldk::CommandPool*
UpperConvergence::createReply(const wns::ldk::CommandPool* original) const
{
	wns::ldk::CommandPool* reply = getFUN()->createCommandPool();

	UpperCommand* originalCommand = getCommand(original);
	UpperCommand* replyCommand = activateCommand(reply);

	replyCommand->peer.sourceMACAddress = sourceMACAddress;
	replyCommand->peer.targetMACAddress = originalCommand->peer.sourceMACAddress;

	return reply;
} // UnicastUpper::createReply

UTUpperConvergence::UTUpperConvergence(wns::ldk::fun::FUN* fun,const wns::pyconfig::View& config) :
	UpperConvergence(fun,config),
	wns::ldk::Forwarding<UTUpperConvergence>(),
	wns::Cloneable<UTUpperConvergence>()
{}

void
UTUpperConvergence::processIncoming(const wns::ldk::CompoundPtr& compound)
{
	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << ": doOnData(), forwarding to upper Component (IP) ";
	MESSAGE_END();

	dataHandlerRegistry.find(wns::service::dll::protocolNumberOf(compound->getData()))->onData(compound->getData());

	MESSAGE_BEGIN(VERBOSE, logger, m, getFUN()->getName());
	m << ": Compound backtrace"
	  << compound->dumpJourney(); // JOURNEY
	MESSAGE_END();

} // processIncoming

void
UTUpperConvergence::registerHandler(wns::service::dll::protocolNumber protocol,
				    wns::service::dll::Handler* dh)
{
	assureNotNull(dh);
	dataHandlerRegistry.insert(protocol, dh);

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << ": UTUpperConv registered dataHandler for protocol number " << protocol;
	MESSAGE_END();
}


APUpperConvergence::APUpperConvergence(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config) :
	UpperConvergence(fun,config),
	wns::ldk::Forwarding<APUpperConvergence>(),
	wns::Cloneable<APUpperConvergence>(),
	dataHandler(NULL)
{}

void
APUpperConvergence::processIncoming(const wns::ldk::CompoundPtr& compound)
{
	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << ": doOnData(), forwarding to RANG";
	MESSAGE_END();
	assure(dataHandler, "no data handler set");

	// as opposed to the UT upper convergence, we have to tell the RANG who we
	// are and where the Packet comes from.
	UpperCommand* myCommand = getCommand(compound->getCommandPool());
	dataHandler->onData(compound->getData(),
			    myCommand->peer.sourceMACAddress,
			    this);

	MESSAGE_BEGIN(VERBOSE, logger, m, getFUN()->getName());
	m << ": Compound backtrace"
	  << compound->dumpJourney(); // JOURNEY
	MESSAGE_END();

} // processIncoming

void
APUpperConvergence::registerHandler(wns::service::dll::protocolNumber /*protocol*/,
				    wns::service::dll::Handler* dh)
{
	// Upper layer protocol demultiplexing is done in RANG. Therefore we
	// ignore the protocol field.

	assureType(dh, dll::RANG*);
	dataHandler = dynamic_cast<dll::RANG*>(dh);
	assure(dataHandler, "APUpperConvergence failed to register wns::service::dll::Handler");

	MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
	m << ": APUpperConv registered dataHandler";
	MESSAGE_END();
}

bool
APUpperConvergence::hasRANG()
{
        if (dataHandler)
		return true;
	else
		return false;
}

dll::RANG*
APUpperConvergence::getRANG()
{
	assure(dataHandler, "RANG hasn't been set");
	return dataHandler;
}



