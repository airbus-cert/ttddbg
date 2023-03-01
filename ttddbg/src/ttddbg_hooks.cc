#include "ttddbg_hooks.hh"
#include "ttddbg_tracer.hh"

#include <kernwin.hpp>
#include <funcs.hpp>

namespace ttddbg {
	Hooks::Hooks() {
		count = 0;
		m_actionHandler = new TraceActionHandler();
		m_copyArg1Handler = new CopyArgActionHandler(0);
		m_copyArg2Handler = new CopyArgActionHandler(1);
		m_copyArg3Handler = new CopyArgActionHandler(2);
		m_copyArg4Handler = new CopyArgActionHandler(3);
		//m_editArgHandler = new EditArgActionHandler();

		register_action(ACTION_DESC_LITERAL_OWNER("ttddbg_traceFunc", "Trace function in TTD", m_actionHandler, nullptr, "", "", -1, 0));
		register_action(ACTION_DESC_LITERAL_OWNER("ttddbg_copyArg1", "Copy arg1 address", m_copyArg1Handler, nullptr, "", "", -1, 0));
		register_action(ACTION_DESC_LITERAL_OWNER("ttddbg_copyArg2", "Copy arg2 address", m_copyArg2Handler, nullptr, "", "", -1, 0));
		register_action(ACTION_DESC_LITERAL_OWNER("ttddbg_copyArg3", "Copy arg3 address", m_copyArg3Handler, nullptr, "", "", -1, 0));
		register_action(ACTION_DESC_LITERAL_OWNER("ttddbg_copyArg4", "Copy arg4 address", m_copyArg4Handler, nullptr, "", "", -1, 0));
		//register_action(ACTION_DESC_LITERAL_OWNER("ttddbg_editArg", "Edit an argument...", m_editArgHandler, nullptr, "", "", -1, 0));
	}

	Hooks::~Hooks() {
		unregister_action("ttddbg_traceFunc");
		unregister_action("ttddbg_copyArg1");
		unregister_action("ttddbg_copyArg2");
		unregister_action("ttddbg_copyArg3");
		unregister_action("ttddbg_copyArg4");
	}

	void Hooks::registerHooks() {
		bool ok = hook_to_notification_point(HT_UI, &onUINotification, this);
		if (!ok) {
			msg("Error hooking to UI notifications\n");
		}
	}

	void Hooks::unregisterHooks() {
		bool ok = unhook_from_notification_point(HT_UI, &onUINotification, this);
		if (!ok) {
			msg("Error unhooking from UI notifications\n");
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
		//msg("[hooks] Type: %d | Title: %s\n", ctx->widget_type, ctx->widget_title.c_str());
		
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

		// Case 3 : a function from the "Imports" window
		if (ctx->widget_type == BWN_IMPORTS) {
			if (ctx->chooser_selection.size() != 1) {
				return;
			}

			uval_t sel = ctx->chooser_selection.at(0);
			ea_t ea = ctx->source.chooser->get_ea(sel);

			m_actionHandler->ea = ea;
			attach_action_to_popup(nullptr, popup_handle, "ttddbg_traceFunc", nullptr, SETMENU_FIRST);

			return;
		}

		// Case 4: We're in the "Tracing events" window: add option to copy argument addresses
		if (ctx->widget_title == "Tracing events") {
			if (ctx->chooser_selection.size() != 1) {
				return;
			}

			uval_t sel = ctx->chooser_selection.at(0);
			m_copyArg1Handler->eventNum = sel;
			m_copyArg2Handler->eventNum = sel;
			m_copyArg3Handler->eventNum = sel;
			m_copyArg4Handler->eventNum = sel;

			size_t nargs = FunctionTracer::getInstance()->eventAt(sel).args.size();

			if (nargs > 0) {
				attach_action_to_popup(nullptr, popup_handle, "ttddbg_copyArg1", nullptr, 0);
			}
			if (nargs > 1) {
				attach_action_to_popup(nullptr, popup_handle, "ttddbg_copyArg2", nullptr, 0);
			}
			if (nargs > 2) {
				attach_action_to_popup(nullptr, popup_handle, "ttddbg_copyArg3", nullptr, 0);
			}
			if (nargs > 3) {
				attach_action_to_popup(nullptr, popup_handle, "ttddbg_copyArg4", nullptr, 0);
			}
			
			return;
		}
	}

	

	/***************************************************************************/
	int TraceActionHandler::activate(action_activation_ctx_t* ctx) {
		func_t* func = get_func(this->ea);
		if (func == nullptr) {
			warning("Cannot trace this function: function not found");
			return 0;
		}
		FunctionTracer::getInstance()->traceFunction(func);
		return 0;

	}


	action_state_t TraceActionHandler::update(action_update_ctx_t* ctx) {
		return AST_ENABLE_ALWAYS;
	}

	/***************************************************************************/
	int CopyArgActionHandler::activate(action_activation_ctx_t* ctx) {
		FunctionTracer::getInstance()->copyArgumentAddress(eventNum, argNum);
		return 0;
	}

	action_state_t CopyArgActionHandler::update(action_activation_ctx_t* ctx) {
		return AST_ENABLE_ALWAYS;
	}
}