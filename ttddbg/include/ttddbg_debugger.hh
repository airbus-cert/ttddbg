#ifndef __TTDDBG_DEBUGGER__
#define __TTDDBG_DEBUGGER__

#include <ida.hpp>
#include <idd.hpp>
#include <Windows.h>
#include "ttddbg_logger.hh"
#include "ttddbg_debugger_manager_interface.hh"
#include "ttddbg_plugin.hh"

namespace ttddbg 
{
	/*!
	 * \brief	Main debugger object 
	 */
	class Debugger : public debugger_t
	{
	protected:
		/*!
		 * \brief	logger interface 
		 */
		std::shared_ptr<Logger> m_logger;

		/*!
		 * \brief	Debugger manager
		 *			Use to manage debugger autoata
		 */
		std::unique_ptr<IDebuggerManager> m_manager;

		/*!
		 *	\brief	main callback use by IDA to check debugger internal state 
		 */
		static ssize_t idaapi debugger_callback(void*, int notification_code, va_list va);
		
	public:
		/*!
		 * \brief	ctor
		 * \param	logger	logger interface to print informations messages
		 * \param	manager	debugger automata
		 */
		explicit Debugger(std::shared_ptr<ttddbg::Logger> logger, std::unique_ptr<IDebuggerManager>&& manager);

		/*!
		 * \brief	Retrieve the manager for 
		 */
		IDebuggerManager& getManager();
	};
}

#endif