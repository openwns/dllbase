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

#include <DLL/services/management/ESMFunc.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	dll::services::management::LinearESM,
	dll::services::management::IESMFunc,
	"dll.services.management.InterferenceCache.LinearESM",
	wns::ldk::PyConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	dll::services::management::LogESM,
	dll::services::management::IESMFunc,
	"dll.services.management.InterferenceCache.LogESM",
	wns::ldk::PyConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	dll::services::management::MIESM,
	dll::services::management::IESMFunc,
	"dll.services.management.InterferenceCache.MIESM",
	wns::ldk::PyConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	dll::services::management::ESMStub,
	dll::services::management::IESMFunc,
	"dll.services.management.InterferenceCache.ESMStub",
	wns::ldk::PyConfigCreator);

using namespace dll::services::management;

ESMFunc::ESMFunc(const wns::pyconfig::View& config) :
    pmm_(wns::service::phy::phymode::PhyModeMapperInterface::getPhyModeMapper(
        config.getView("phyModeMapper")))
{
}

ESMStub::ESMStub(const wns::pyconfig::View&)
{
}

wns::Ratio
ESMStub::operator()(const std::set<wns::Ratio>&)
{
    return wns::Ratio::from_factor(1);
}


LinearESM::LinearESM(const wns::pyconfig::View& config):
    ESMFunc(config)
{
}

wns::Ratio
LinearESM::operator()(const std::set<wns::Ratio>& sinrs)
{
    std::set<wns::Ratio>::const_iterator it;
    double sum = 0;
    for(it = sinrs.begin(); it != sinrs.end(); it++)
        sum += it->get_factor();

    return wns::Ratio::from_factor(sum / double(sinrs.size()));
}

LogESM::LogESM(const wns::pyconfig::View& config):
    ESMFunc(config)
{
}

wns::Ratio
LogESM::operator()(const std::set<wns::Ratio>& sinrs)
{
    std::set<wns::Ratio>::const_iterator it;
    double sum = 0;
    for(it = sinrs.begin(); it != sinrs.end(); it++)
        sum += it->get_dB();

    return wns::Ratio::from_dB(sum / double(sinrs.size()));
}

MIESM::MIESM(const wns::pyconfig::View& config):
    ESMFunc(config),
    maxBLER_(config.get<double>("maxBLER")),
    blockSize_(config.get<int>("blockSize"))
{
}

wns::Ratio
MIESM::operator()(const std::set<wns::Ratio>& sinrs)
{
    assure(pmm_ != NULL, "Need access to PhyModeMapper");
    std::vector<wns::service::phy::phymode::PhyModeInterfacePtr> pmVec;
	pmVec =	pmm_->getListOfPhyModePtr();

    for(int i = pmVec.size() - 1; i >= 0; i++)
    {
        std::set<wns::Ratio>::const_iterator it;   
        double sumMiB = 0.0;
        for(it = sinrs.begin(); it != sinrs.end(); it++)
        {
            sumMiB += pmVec[i]->getSINR2MIB(*it);
        }
        double mib = sumMiB / double(sinrs.size());
        if(pmVec[i]->getMI2PER(mib, blockSize_) < maxBLER_)
            return wns::Ratio::from_factor(pmm_->getMinSINR(pmVec[i])); 
    }

    return wns::Ratio::from_factor(pmm_->getMinimumSINR());
}

