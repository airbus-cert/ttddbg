#ifndef __TTDDBG_DEBUGGER__
#define __TTDDBG_DEBUGGER__

#include <ida.hpp>
#include <idd.hpp>
#include <Windows.h>
#include "ttddbg_logger.hh"
#include "ttddbg_debugger_manager_interface.hh"

namespace ttddbg 
{
	class Debugger : public debugger_t
	{
	protected:
		std::shared_ptr<Logger> m_logger;
		std::unique_ptr<IDebuggerManager> m_manager;

		static ssize_t idaapi debugger_callback(void*, int notification_code, va_list va);
		
	public:
		explicit Debugger(std::shared_ptr<ttddbg::Logger> logger, std::unique_ptr<IDebuggerManager>&& manager);

	};
}

#endif