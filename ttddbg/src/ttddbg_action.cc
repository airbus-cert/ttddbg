#include "ttddbg_action.hh"
#include "ttddbg_debugger.hh"
#include <dbg.hpp>

namespace ttddbg
{
	/**********************************************************************/
	int idaapi ttddbg::BackwardStateRequest::activate(action_activation_ctx_t*)
	{
		if (dbg != nullptr)
		{
			static_cast<ttddbg::Debugger*>(dbg)->getManager().switchWay();
			static_cast<ttddbg::Debugger*>(dbg)->getManager().onSetResumeMode(0, resume_mode_t::RESMOD_NONE);
			continue_process();
			static_cast<ttddbg::Debugger*>(dbg)->getManager().switchWay();
		}
		return false;
	}

	/**********************************************************************/
	action_state_t idaapi ttddbg::BackwardStateRequest::update(action_update_ctx_t*)
	{
		return AST_ENABLE_ALWAYS;
	}

	/**********************************************************************/
	int idaapi ttddbg::BackwardSingleStepRequest::activate(action_activation_ctx_t*)
	{
		if (dbg != nullptr)
		{
			static_cast<ttddbg::Debugger*>(dbg)->getManager().switchWay();
			static_cast<ttddbg::Debugger*>(dbg)->getManager().onSetResumeMode(0, resume_mode_t::RESMOD_INTO);
			step_into();
			static_cast<ttddbg::Debugger*>(dbg)->getManager().switchWay();
		}
		return false;
	}

	/**********************************************************************/
	action_state_t idaapi ttddbg::BackwardSingleStepRequest::update(action_update_ctx_t*)
	{
		return AST_ENABLE_ALWAYS;
	}

	/**********************************************************************/
	int idaapi ttddbg::OpenPositionChooserAction::activate(action_activation_ctx_t*) {
		if (dbg != nullptr)
		{
			static_cast<ttddbg::Debugger*>(dbg)->getManager().openPositionChooser();
		}
		return false;
	}

	/**********************************************************************/
	action_state_t idaapi ttddbg::OpenPositionChooserAction::update(action_update_ctx_t*) {
		return AST_ENABLE_ALWAYS;
	}

	/**********************************************************************/
	int idaapi ttddbg::OpenTraceChooserAction::activate(action_activation_ctx_t*) {
		if (dbg != nullptr)
		{
			static_cast<ttddbg::Debugger*>(dbg)->getManager().openTraceChooser();
		}
		return false;
	}

	/**********************************************************************/
	action_state_t idaapi ttddbg::OpenTraceChooserAction::update(action_update_ctx_t*) {
		return AST_ENABLE_ALWAYS;
	}

	/**********************************************************************/
	int idaapi ttddbg::OpenTraceEventChooserAction::activate(action_activation_ctx_t*) {
		if (dbg != nullptr)
		{
			static_cast<ttddbg::Debugger*>(dbg)->getManager().openTraceEventsChooser();
		}
		return false;
	}

	/**********************************************************************/
	action_state_t idaapi ttddbg::OpenTraceEventChooserAction::update(action_update_ctx_t*) {
		return AST_ENABLE_ALWAYS;
	}

	/**********************************************************************/
	int idaapi ttddbg::FullRunActionRequest::activate(action_activation_ctx_t*) {
		if (dbg != nullptr)
		{
			static_cast<ttddbg::Debugger*>(dbg)->getManager().requestFullRun();
		}
		return false;
	}

	/**********************************************************************/
	action_state_t idaapi ttddbg::FullRunActionRequest::update(action_update_ctx_t*) {
		return AST_ENABLE_ALWAYS;
	}

	/**********************************************************************/
	int idaapi ttddbg::GotoPositionAction::activate(action_activation_ctx_t*) {
		if (dbg != nullptr)
		{
			static_cast<ttddbg::Debugger*>(dbg)->getManager().gotoPosition();
		}
		return false;
	}

	/**********************************************************************/
	action_state_t idaapi ttddbg::GotoPositionAction::update(action_update_ctx_t*) {
		return AST_ENABLE_ALWAYS;
	}
}