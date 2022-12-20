#include "ttddbg_hooks.hh"
#include "ttddbg_tracer.hh"

#include <kernwin.hpp>
#include <funcs.hpp>

namespace ttddbg {
	Hooks::Hooks() {
		count = 0;
		m_actionHandler = new TraceActionHandler();

		register_action(ACTION_DESC_LITERAL_OWNER("ttddbg_traceFunc", "Trace function in TTD", m_actionHandler, nullptr, "", "", -1, 0));
	}

	void Hooks::registerHooks() {
		bool ok = hook_to_notification_point(HT_UI, &onUINotification, this);
		if (!ok) {
			msg("Error hooking to UI notifications\n");
		}
	}

	ssize_t onUINotification(void* user_data, int notification_code, va_list va) {
		Hooks* obj = (Hooks*)user_data;

		if (notification_code == ::ui_finish_populating_widget_popup) {
			TWidget* widget = va_arg(va, TWidget*);
			TPopupMenu* popup_handle = va_arg(va, TPopupMenu*);
			const action_activation_ctx_t* ctx = va_arg(va, action_activation_ctx_t*);

			obj->ui_finish_populating_widget_popup(widget, popup_handle, ctx);
		}

		return 0;
	}

	void Hooks::ui_finish_populating_widget_popup(TWidget* widget, TPopupMenu* popup_handle, const action_activation_ctx_t* ctx) {
		// Case 1 : "Module: " choosers, containing a bunch of DLL functions
		if (ctx->widget_type == BWN_CHOOSER && ctx->widget_title.length() > 8 && ctx->widget_title.substr(0, 8) == "Module: ") {

			if (ctx->chooser_selection.size() != 1) {
				return;
			}

			uval_t sel = ctx->chooser_selection.at(0);
			ea_t ea = ctx->source.chooser->get_ea(sel);

			m_actionHandler->ea = ea;
			attach_action_to_popup(nullptr, popup_handle, "ttddbg_traceFunc", nullptr, SETMENU_FIRST);

			return;
		}

		// Case 2 : a function from the "Functions" window
		if (ctx->widget_type == BWN_FUNCS) {
			if (ctx->chooser_selection.size() != 1) {
				return;
			}

			uval_t sel = ctx->chooser_selection.at(0);
			ea_t ea = ctx->source.chooser->get_ea(sel);

			m_actionHandler->ea = ea;
			attach_action_to_popup(nullptr, popup_handle, "ttddbg_traceFunc", nullptr, SETMENU_FIRST);

			return;
		}
	}

	

	/***************************************************************************/
	int TraceActionHandler::activate(action_activation_ctx_t* ctx) {
		func_t* func = get_func(this->ea);
		FunctionTracer::getInstance()->traceFunction(func);
		return 0;

	}


	action_state_t TraceActionHandler::update(action_update_ctx_t* ctx) {
		return AST_ENABLE_ALWAYS;
	}
}