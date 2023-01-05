#ifndef __TTDDBG_ACTION__
#define __TTDDBG_ACTION__

#include <ida.hpp>
#include <idp.hpp>

namespace ttddbg 
{
	/*!
	 * \brief	Describe the new action introduce by the time travel debugging
	 *			The backward action will back in time until the debugger saw a debugger 
	 *			breakpoint or reach the starting point
	 */
	struct BackwardStateRequest : public action_handler_t
	{
		inline static const char* actionName = "ttddbg:Backward";
		inline static const char* actionLabel = "Continue backwards (Ctrl+F10)";
		inline static const char* actionHotkey = "Ctrl+F10";

		virtual int idaapi activate(action_activation_ctx_t*) override;
		virtual action_state_t idaapi update(action_update_ctx_t*) override;
	};

	/*!
	 * \brief	Describe the new action introduce by the time travel debugging
	 *			Single step will just use backward debugger with a singe step
	 *			Be carefull it will not understand jump (like in the function calling case)
	 *			It will just decrement the EIP correctly
	 */
	struct BackwardSingleStepRequest : public action_handler_t
	{
		inline static const char* actionName = "ttddbg:BackwardSingle";
		inline static const char* actionLabel = "Single-step backwards (Ctrl+F8)";
		inline static const char* actionHotkey = "Ctrl+F8";

		virtual int idaapi activate(action_activation_ctx_t*) override;
		virtual action_state_t idaapi update(action_update_ctx_t*) override;
	};

	/*!
	 * \brief	Action use to manage the position chooser (aka the timeline)
	 */
	struct OpenPositionChooserAction : public action_handler_t 
	{
		inline static const char* actionName = "ttddbg:ChoosePosition";
		inline static const char* actionLabel = "Timeline (F3)";
		inline static const char* actionHotkey = "F3";

		virtual int idaapi activate(action_activation_ctx_t*) override;
		virtual action_state_t idaapi update(action_update_ctx_t*) override;
	};

	/*!
	 * \brief	Action use to display the traced functions chooser
	 */
	struct OpenTraceChooserAction : public action_handler_t
	{
		inline static const char* actionName = "ttddbg:ChooseTracedFunction";
		inline static const char* actionLabel = "Traed functions";
		inline static const char* actionHotkey = "";

		virtual int idaapi activate(action_activation_ctx_t*) override;
		virtual action_state_t idaapi update(action_update_ctx_t*) override;
	};
}

#endif