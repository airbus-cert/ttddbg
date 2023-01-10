#pragma once

#include <ida.hpp>
#include <kernwin.hpp>

namespace ttddbg {
	struct TraceActionHandler : action_handler_t {
		int activate(action_activation_ctx_t* ctx) override;
		action_state_t update(action_update_ctx_t* ctx) override;

		ea_t ea;
	};

	struct CopyArgActionHandler : action_handler_t {
		CopyArgActionHandler(size_t n) : argNum(n) {};
		int activate(action_activation_ctx_t* ctx) override;
		action_state_t update(action_update_ctx_t* ctx) override;

		size_t argNum;
		size_t eventNum;
	};


	class Hooks {
	public:
		Hooks();
		~Hooks();
		void registerHooks();
		void unregisterHooks();

		// UI Hooks
		void ui_finish_populating_widget_popup(TWidget* widget, TPopupMenu* popup_handle, const action_activation_ctx_t* ctx);

	private:
		TraceActionHandler *m_actionHandler;
		CopyArgActionHandler* m_copyArg1Handler;
		CopyArgActionHandler* m_copyArg2Handler;
		CopyArgActionHandler* m_copyArg3Handler;
		CopyArgActionHandler* m_copyArg4Handler;
		int count;
	};

	ssize_t onUINotification(void* user_data, int notification_code, va_list va);
}