#include <DLL/QoSClassProvider.hpp>
#include <WNS/StaticFactory.hpp>

using namespace dll;

STATIC_FACTORY_REGISTER_WITH_CREATOR(QoSClassProvider,
									 wns::ldk::FunctionalUnit,
									 "dll.QoSClassProvider",
									 wns::ldk::FUNConfigCreator);


wns::ldk::CommandPool*
QoSClassProvider::createReply(const wns::ldk::CommandPool* original) const
{
	std::cout<< "QoSClassProvider-createReply called!!!!!"<<std::endl;
	wns::ldk::CommandPool* commandPool = getFUN()->createCommandPool();

	QoSClassProviderCommand* inCommand = getCommand(original);
	QoSClassProviderCommand* outCommand = activateCommand(commandPool);

	outCommand->local.qosClass = inCommand->local.qosClass;
	std::cout<< "QoSClassProvider-createReply called!!!!!"<<inCommand->local.qosClass<<std::endl;

	return commandPool;
} // createReply
