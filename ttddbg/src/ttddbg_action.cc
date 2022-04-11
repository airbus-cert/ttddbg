#include "ttddbg_action.hh"
#include "ttddbg_debugger.hh"
#include <dbg.hpp>

namespace ttddbg
{
	int idaapi ttddbg::BackwardStateRequest::activate(action_activation_ctx_t*)
	{
		if (dbg != nullptr)
		{
			static_cast<ttddbg::Debugger*>(dbg)->getManager().switchWay();
			auto widget = find_widget("IDA View-RIP");
			
			
		}
		return false;
	}

	action_state_t idaapi ttddbg::BackwardStateRequest::update(action_update_ctx_t*)
	{
		return AST_ENABLE_ALWAYS;
	}
}