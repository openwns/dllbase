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

#include <DLL/services/management/InterferenceCache.hpp>
#include <WNS/node/Interface.hpp>
#include <cmath>


STATIC_FACTORY_REGISTER_WITH_CREATOR(
	dll::services::management::InterferenceCache,
	wns::ldk::ManagementServiceInterface,
	"dll.services.management.InterferenceCache",
	wns::ldk::MSRConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	dll::services::management::InterferenceCache::ConstantValue,
	dll::services::management::InterferenceCache::NotFoundStrategy,
	"dll.services.management.InterferenceCache.ConstantValue",
	wns::ldk::PyConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	dll::services::management::InterferenceCache::Complain,
	dll::services::management::InterferenceCache::NotFoundStrategy,
	"dll.services.management.InterferenceCache.Complain",
	wns::ldk::PyConfigCreator);

using namespace dll::services::management;

InterferenceCache::ConstantValue::ConstantValue(const wns::pyconfig::View& config)
{
	averageCarrier = config.get<wns::Power>("averageCarrier");
	averageInterference = config.get<wns::Power>("averageInterference");
	deviationCarrier = config.get<wns::Power>("deviationCarrier");
	deviationInterference = config.get<wns::Power>("deviationInterference");
	averagePathloss = config.get<wns::Ratio>("averagePathloss");
}


InterferenceCache::InterferenceCache( wns::ldk::ManagementServiceRegistry* msr, const wns::pyconfig::View& config )
	: wns::ldk::ManagementService( msr ),
	  alphaLocal_(config.get<double>("alphaLocal")),
	  alphaRemote_(config.get<double>("alphaRemote")),
	  logger(config.get("logger"))
{

	wns::pyconfig::View notFoundView(config, "notFoundStrategy");

	notFoundStrategy.reset
		(wns::StaticFactory< wns::ldk::PyConfigCreator<dll::services::management::InterferenceCache::NotFoundStrategy> >
		 ::creator(notFoundView.get<std::string>("__plugin__"))->create(notFoundView));

	MESSAGE_SINGLE(NORMAL, logger, "Created InterferenceCache Service");


}

void InterferenceCache::storeMeasurements( wns::node::Interface* node, wns::service::phy::power::PowerMeasurementPtr rxPowerMeasurement, ValueOrigin origin, int subBand )
{
	wns::Power carrier = rxPowerMeasurement->getRxPower();
	wns::Power interference = rxPowerMeasurement->getOmniInterferencePower();
	wns::Ratio pathloss = rxPowerMeasurement->getPathLoss();
	InterferenceCacheKey key(node, subBand);
	double alpha;
	if (origin == Local)
		alpha = alphaLocal_;
	else
		alpha = alphaRemote_;

	Node2Power::iterator n2cait =
		node2CarrierAverage_.find( key );
	if ( n2cait == node2CarrierAverage_.end() )
		node2CarrierAverage_[key] = carrier;
	else
		node2CarrierAverage_[key] =
			node2CarrierAverage_[key] * (1.0 - alpha) + carrier * alpha;
	Node2Double::iterator n2caseit =
		node2CarrierSqExp_.find( key );
	if ( n2caseit == node2CarrierSqExp_.end() )
		node2CarrierSqExp_[key] = carrier.get_mW() * carrier.get_mW();
	else
		node2CarrierSqExp_[key] =
			(1.0 - alpha) * node2CarrierSqExp_[key] +  carrier.get_mW() * carrier.get_mW() * alpha;
	Node2Power::iterator n2iait =
		node2InterferenceAverage_.find( key );
	if ( n2iait == node2InterferenceAverage_.end() )
		node2InterferenceAverage_[key] = interference;
	else
		node2InterferenceAverage_[key] =
			 node2InterferenceAverage_[key] * (1.0 - alpha) + interference * alpha;

	Node2Double::iterator n2iaseit =
		node2InterferenceSqExp_.find( key );
	if ( n2iaseit == node2InterferenceSqExp_.end() )
		node2InterferenceSqExp_[key] = interference.get_mW() * interference.get_mW();
	else
		node2InterferenceSqExp_[key] =
			 node2InterferenceSqExp_[key] * (1.0 - alpha) + interference.get_mW() * interference.get_mW() * alpha ;
	Node2Double::iterator n2pit =
		node2pathloss.find( key );
	if ( n2pit == node2pathloss.end() )
		node2pathloss[key] = pathloss.get_factor();
	else
		node2pathloss[key] = (1.0 - alpha) * node2pathloss[key] + alpha * pathloss.get_factor();

	MESSAGE_BEGIN(VERBOSE, logger, m, "Storing C for ");
	m << node->getName() << ": " << carrier << ". (origin=" << ( origin==Local ? "Local" : "Remote" ) << ")";
	MESSAGE_END();
	MESSAGE_BEGIN(VERBOSE, logger, m, "Storing I for ");
	m << node->getName() << ": " << interference << ". (origin=" << ( origin==Local ? "Local" : "Remote" ) << ")";
	MESSAGE_END();
	MESSAGE_BEGIN(VERBOSE, logger, m, "Storing Pathloss for ");
	m << node->getName() << ": " << pathloss << ". (origin=" << ( origin==Local ? "Local" : "Remote" ) << ")";
	MESSAGE_END();
}

void InterferenceCache::storeCarrier( wns::node::Interface* node, const wns::Power& carrier, ValueOrigin origin, int subBand )
{
	InterferenceCacheKey key(node, subBand);

	double alpha;
	if (origin == Local)
		alpha = alphaLocal_;
	else
		alpha = alphaRemote_;

	Node2Power::iterator n2cait =
		node2CarrierAverage_.find( key );
	if ( n2cait == node2CarrierAverage_.end() )
		node2CarrierAverage_[key] = carrier;
	else
		node2CarrierAverage_[key] =
			node2CarrierAverage_[key] * (1.0 - alpha) + carrier * alpha;

	Node2Double::iterator n2caseit =
		node2CarrierSqExp_.find( key );
	if ( n2caseit == node2CarrierSqExp_.end() )
		node2CarrierSqExp_[key] = carrier.get_mW() * carrier.get_mW();
	else
		node2CarrierSqExp_[key] =
			(1.0 - alpha) * node2CarrierSqExp_[key] +  carrier.get_mW() * carrier.get_mW() * alpha;

	MESSAGE_BEGIN(VERBOSE, logger, m, "Storing C for ");
	m << node->getName() << ": " << carrier << ". (origin=" << ( origin==Local ? "Local" : "Remote" ) << ")";
	MESSAGE_END();
}

void InterferenceCache::storeInterference( wns::node::Interface* node, const wns::Power& interference, ValueOrigin origin, int subBand )
{
	InterferenceCacheKey key(node, subBand);

	double alpha;
	if (origin == Local)
		alpha = alphaLocal_;
	else
		alpha = alphaRemote_;

	Node2Power::iterator n2iait =
		node2InterferenceAverage_.find( key );
	if ( n2iait == node2InterferenceAverage_.end() )
		node2InterferenceAverage_[key] = interference;
	else
		node2InterferenceAverage_[key] =
			 node2InterferenceAverage_[key] * (1.0 - alpha) + interference * alpha;

	Node2Double::iterator n2iaseit =
		node2InterferenceSqExp_.find( key );
	if ( n2iaseit == node2InterferenceSqExp_.end() )
		node2InterferenceSqExp_[key] = interference.get_mW() * interference.get_mW();
	else
		node2InterferenceSqExp_[key] =
			 node2InterferenceSqExp_[key] * (1.0 - alpha) + interference.get_mW() * interference.get_mW() * alpha ;

	MESSAGE_BEGIN(VERBOSE, logger, m, "Storing I for ");
	m << node->getName() << ": " << interference << ". (origin=" << ( origin==Local ? "Local" : "Remote" ) << ")";
	MESSAGE_END();
}

void InterferenceCache::storePathloss( wns::node::Interface* node, const wns::Ratio& pathloss, ValueOrigin origin, int subBand )
{
	InterferenceCacheKey key(node, subBand);

	double alpha;
	if (origin == Local)
		alpha = alphaLocal_;
	else
		alpha = alphaRemote_;

	Node2Double::iterator n2pit =
		node2pathloss.find( key );
	if ( n2pit == node2pathloss.end() )
		node2pathloss[key] = pathloss.get_factor();
	else
		node2pathloss[key] = (1.0 - alpha) * node2pathloss[key] + alpha * pathloss.get_factor();

	MESSAGE_BEGIN(VERBOSE, logger, m, "Storing Pathloss for ");
	m << node->getName() << ": " << pathloss << ". (origin=" << ( origin==Local ? "Local" : "Remote" ) << ")";
	MESSAGE_END();
}

wns::Power InterferenceCache::getAveragedCarrier( wns::node::Interface* node, int subBand ) const
{
	InterferenceCacheKey key(node, subBand);

	Node2Power::const_iterator it = node2CarrierAverage_.find( key );
	if ( it == node2CarrierAverage_.end() )
		return notFoundStrategy->notFoundAverageCarrier();
	MESSAGE_SINGLE(VERBOSE,logger,"Getting C for " << node->getName() <<": " << it->second );
	return it->second;
}

wns::Power InterferenceCache::getAveragedInterference( wns::node::Interface* node, int subBand ) const
{
	InterferenceCacheKey key(node, subBand);

	Node2Power::const_iterator it = node2InterferenceAverage_.find( key );
	if ( it == node2InterferenceAverage_.end() )
		return notFoundStrategy->notFoundAverageInterference();
	MESSAGE_SINGLE(VERBOSE,logger,"Getting I for " << node->getName() <<": " << it->second );
	return it->second;
}

wns::Ratio InterferenceCache::getAveragedPathloss( wns::node::Interface* node, int subBand ) const
{
	InterferenceCacheKey key(node, subBand);

	Node2Double::const_iterator itpl = node2pathloss.find(key);

	if ( itpl == node2pathloss.end() )
		return notFoundStrategy->notFoundAveragePathloss();

	return wns::Ratio::from_factor(itpl->second);
}

wns::Power InterferenceCache::getCarrierDeviation( wns::node::Interface* node, int subBand ) const
{
	InterferenceCacheKey key(node, subBand);

	Node2Double::const_iterator itsq = node2CarrierSqExp_.find( key );
	if ( itsq == node2CarrierSqExp_.end() )
		return notFoundStrategy->notFoundDeviationCarrier();

	Node2Power::const_iterator itp = node2CarrierAverage_.find( key );
	if ( itp == node2CarrierAverage_.end() )
		return notFoundStrategy->notFoundDeviationCarrier();

	return wns::Power::from_mW( sqrt( itsq->second -
		itp->second.get_mW() * itp->second.get_mW() ) );
}

wns::Power InterferenceCache::getInterferenceDeviation( wns::node::Interface* node, int subBand ) const
{
	InterferenceCacheKey key(node, subBand);

	Node2Double::const_iterator itsq = node2InterferenceSqExp_.find( key );
	if ( itsq == node2InterferenceSqExp_.end() )
		return notFoundStrategy->notFoundDeviationInterference();

	Node2Power::const_iterator itp = node2InterferenceAverage_.find( key );
	if ( itp == node2InterferenceAverage_.end() )
		return notFoundStrategy->notFoundDeviationInterference();

	return wns::Power::from_mW( sqrt( itsq->second -
		itp->second.get_mW() * itp->second.get_mW() ) );
}

