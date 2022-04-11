#ifndef __TTDDBG_DEBUGGER__
#define __TTDDBG_DEBUGGER__

#include <ida.hpp>
#include <idd.hpp>
#include <Windows.h>
#include "ttddbg_logger.hh"
#include "ttddbg_debugger_manager_interface.hh"

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
		 * \param	mnager	debugger automata
		 */
		explicit Debugger(std::shared_ptr<ttddbg::Logger> logger, std::unique_ptr<IDebuggerManager>&& manager);

		/*!
		 * \brief	Use to change the way of the dubugger
		 *			We are a time travel debugger !!!
		 */
		IDebuggerManager& getManager();
	};
}

#endif