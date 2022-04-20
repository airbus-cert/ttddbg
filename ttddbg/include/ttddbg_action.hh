#ifndef __TTDDBG_ACTION__
#define __TTDDBG_ACTION__

#include <ida.hpp>
#include <idp.hpp>

namespace ttddbg 
{
	struct BackwardStateRequest : public action_handler_t
	{
		inline static const char* actionName = "ttddbg:Backward";
		inline static const char* actionLabel = "ttddbg Backward";
		inline static const char* actionHotkey = "F3";

		virtual int idaapi activate(action_activation_ctx_t*) override;
		virtual action_state_t idaapi update(action_update_ctx_t*) override;
	};

	struct BackwardSingleStepRequest : public action_handler_t
	{
		inline static const char* actionName = "ttddbg:BackwardSingle";
		inline static const char* actionLabel = "Single-step backwards";
		inline static const char* actionHotkey = "Ctrl+F8";

		virtual int idaapi activate(action_activation_ctx_t*) override;
		virtual action_state_t idaapi update(action_update_ctx_t*) override;
	};

	struct OpenPositionChooserAction : public action_handler_t 
	{
		inline static const char* actionName = "ttddbg:ChoosePosition";
		inline static const char* actionLabel = "Timeline";
		//inline static const char* actionHotkey = "F3";

		virtual int idaapi activate(action_activation_ctx_t*) override;
		virtual action_state_t idaapi update(action_update_ctx_t*) override;
	};
}

#endif