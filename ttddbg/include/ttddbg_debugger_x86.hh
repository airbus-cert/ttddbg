#ifndef __TTDDBG_DEBUGGERX86__
#define __TTDDBG_DEBUGGERX86__

#include <ida.hpp>
#include <idd.hpp>
#include <Windows.h>
#include "ttddbg_debugger.hh"
#include "ttddbg_debugger_manager.hh"

namespace ttddbg 
{
	/*!
	 * \brief	x86 debugger object
	 */
	class DebuggerX86 : public Debugger
	{
	public:
		/*!
		 * \brief	ctor
		 * \param	logger	logger interface to print informations messages
		 */
		explicit DebuggerX86(std::shared_ptr<ttddbg::Logger> logger);
	};

	/*!
	 * \brief	specialized debugger manager for x86
	 */
	class DebuggerManagerX86 : public DebuggerManager
	{
	public:
		/*!
		 * \brief	ctor
		 */
		explicit DebuggerManagerX86(std::shared_ptr< ttddbg::Logger> logger);

		/*!
		 * \brief	use to inform the debugger to read register state
		 * \param	tid	thread id
		 * \param	clsmask	class mask (for xample x86 class mask)
		 * \param	values	output register state, follow declaration of the debugger
		 * \param	errbuf	buffer error to inform state
		 */
		ssize_t onReadRegisters(thid_t tid, int clsmask, regval_t* values, qstring* errbuf) override;
	};
}

#endif