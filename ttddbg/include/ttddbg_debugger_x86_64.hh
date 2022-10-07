#ifndef __TTDDBG_DEBUGGERX86_64__
#define __TTDDBG_DEBUGGERX86_64__

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
	class DebuggerX86_64 : public Debugger
	{
	public:
		/*!
		 * \brief	ctor
		 * \param	logger	logger interface to print informations messages
		 */
		explicit DebuggerX86_64(std::shared_ptr<ttddbg::Logger> logger, std::shared_ptr<Plugin> plugin);
	};

	/*!
	 * \brief	specialized debugger manager for x86_64
	 */
	class DebuggerManagerX86_64 : public DebuggerManager
	{
	public:
		/*!
		 * \brief	ctor
		 */
		explicit DebuggerManagerX86_64(std::shared_ptr<ttddbg::Logger> logger, std::shared_ptr<Plugin> plugin);

		/*!
		 * \brief	use to inform the debugger to read register state
		 * \param	tid	thread id
		 * \param	clsmask	class mask (for example x86 class mask)
		 * \param	values	output register state, follow declaration of the debugger
		 * \param	errbuf	buffer error to inform state
		 */
		ssize_t onReadRegisters(thid_t tid, int clsmask, regval_t* values, qstring* errbuf) override;
	};
}

#endif