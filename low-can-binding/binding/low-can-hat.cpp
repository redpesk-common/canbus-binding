#include <mutex>

#include <afb/afb-binding>

#include <openxc.pb.h>

#include <canbus-binding/binding/low-can-hat.hpp>
#include <canbus-binding/can/signals.hpp>
#include <canbus-binding/can/can-bus.hpp>
#include <canbus-binding/utils/signals.hpp>
#include <canbus-binding/can/message-definition.hpp>
#include <canbus-binding/binding/low-can-subscription.hpp>
#include <canbus-binding/diagnostic/diagnostic-message.hpp>

///******************************************************************************
///
///		SystemD event loop Callbacks
///
///*******************************************************************************/

static void push_n_notify(std::shared_ptr<message_t> m)
{
	can_bus_t& cbm = application_t::instance().get_can_bus_manager();
	{
		std::lock_guard<std::mutex> can_message_lock(cbm.get_can_message_mutex());
	 	cbm.push_new_can_message(m);
	}
	cbm.get_new_can_message_cv().notify_one();
}

void on_no_clients(std::shared_ptr<low_can_subscription_t> can_subscription, uint32_t pid, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	bool is_permanent_recurring_request = false;
	const std::shared_ptr<diagnostic_message_t> dm = can_subscription->get_diagnostic_message(pid);

	if( dm != nullptr)
	{
		DiagnosticRequest diag_req = dm->build_diagnostic_request();
		active_diagnostic_request_t* adr = application_t::instance().get_diagnostic_manager().find_recurring_request(diag_req);
		if( adr != nullptr)
		{
			is_permanent_recurring_request = adr->get_permanent();

			if(! is_permanent_recurring_request)
				application_t::instance().get_diagnostic_manager().cleanup_request(adr, true);
		}
	}

	if(! is_permanent_recurring_request)
		on_no_clients(can_subscription, s);
}


void on_no_clients(std::shared_ptr<low_can_subscription_t> can_subscription, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	auto it = s.find(can_subscription->get_index());
	s.erase(it);
}

void read_message(low_can_subscription_t &can_subscription)
{
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();
	std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
	if(can_subscription.get_index() != -1)
	{
		std::shared_ptr<utils::socketcan_t> s = can_subscription.get_socket();
		if(s->socket() > 0)
		{
			std::shared_ptr<message_t> message = s->read_message();

			// Sure we got a valid CAN message ?
			if (message->get_id() &&
			    (message->get_length() || message->is_timeout()) &&
			    ! (message->get_flags() & INVALID_FLAG) )
			{
				if(can_subscription.get_signal() != nullptr && can_subscription.get_signal()->get_message()->get_flags() & BYTE_FRAME_IS_BIG_ENDIAN)
					message->frame_swap();
				push_n_notify(message);
			}
		}
	}
}

