

#ifndef DLL_QOSCLASSPROVIDER_HPP
#define DLL_QOSCLASSPROVIDER_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/Forwarding.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/service/qos/QoSClasses.hpp>

namespace dll {

	class QoSClassProviderCommand :
		public wns::ldk::Command
	{
	public:
		QoSClassProviderCommand()
		{
			local.qosClass = 0;
		};
		struct {
			wns::service::qos::QoSClass qosClass;
		} local;
		struct {
		} peer;
		struct {
		} magic;
	};

	class QoSClassProvider :
		public virtual wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<QoSClassProviderCommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::ldk::Forwarding<QoSClassProvider>,
		public wns::Cloneable<QoSClassProvider>
	{
	public:
		QoSClassProvider(wns::ldk::fun::FUN* fun, const wns::pyconfig::View&) :
			wns::ldk::CommandTypeSpecifier<QoSClassProviderCommand>(fun),
			wns::ldk::HasReceptor<>(),
			wns::ldk::HasConnector<>(),
			wns::ldk::HasDeliverer<>(),
			wns::ldk::Forwarding<QoSClassProvider>(),
			wns::Cloneable<QoSClassProvider>()
			{}

		virtual
		~QoSClassProvider(){}

		virtual wns::ldk::CommandPool*
		createReply(const wns::ldk::CommandPool* original) const;
	};

}
#endif // not defined WINPROST_HELPER_QOSCLASSPROVIDER_HPP
