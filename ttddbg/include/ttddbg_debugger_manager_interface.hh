#ifndef __TTDDBG_DEBUGGER_MANAGER_INTERFACE__
#define __TTDDBG_DEBUGGER_MANAGER_INTERFACE__

#include <ida.hpp>
#include <idd.hpp>

#include <Windows.h>
#include "../../ttd-bindings/TTD/TTD.hpp"

namespace ttddbg 
{
	/*!
	 * \brief	IDA debugger interface
	 */
	class IDebuggerManager
	{
	public:
		/*!
		 * \brief	First state of the automata
		 * \param	hostname	use in case of remote debugging
		 * \param	portNumber	host port in case of remote debugging
		 * \param	password	remote password
		 * \param	errbuf		use to signal error
		 */
		virtual ssize_t onInit(std::string& hostname, int portNumber, std::string& password, qstring* errBuf) = 0;

		/*!
		 * \brief	When terminate a debugger
		 */
		virtual ssize_t OnTermDebugger() = 0;

		/*!
		 * \brief	Event use to get information about the current debugging process
		 * \param	infos	list of processes started by debugger (name and pid)
		 */
		virtual ssize_t onGetProcess(procinfo_vec_t* infos, qstring* errBuf) = 0;

		/*!
		 * \brief	Debugger ask to start a process
		 * \param	path	path to the executable
		 * \param	args	arguments path to the executable
		 * \param	startdir	working directory
		 * \param	flags	startup flags
		 * \param	image input files
		 * \param	inputFileCRC32	use to check if image is aligned with debugging process
		 */
		virtual ssize_t onStartProcess(const char* path, const char* args, const char* startdir, uint32 dbg_proc_flags, const char* input_path, uint32 input_file_crc32, qstring* errbuf = nullptr) = 0;

		/*!
		 * \brief	use to get debug attributes
		 * \param	attrs	debug attributes
		 */
		virtual ssize_t onGetDebappAttrs(debapp_attrs_t* out_pattrs) = 0;

		/*!
		 * \brief	Main event to get current debug event
		 *			Will be used to pop from the event queue
		 * \param	code	use to to set if pending events exist in the queue
		 * \param	event	output event
		 * \param	timeout_ms
		 */
		virtual ssize_t onGetDebugEvent(gdecode_t* code, debug_event_t* event, int timeout_ms) = 0;

		/*!
		 * \brief	Get memory information, like segment with module named
		 *			We can know wich address is used by which module
		 * \param	infos	list of memory infos
		 */
		virtual ssize_t onGetMemoryInfo(meminfo_vec_t* infos, qstring* errbuf = nullptr) = 0;

		/*!
		 * \brief	Set exception configuration of the debugger
		 * \param	info	exception informations
		 */
		virtual ssize_t onSetExceptionInfo(exception_info_t* info, int qty) = 0;

		/*!
		 * \brief	Read memory at a special process address
		 * \param	nbytes	number of bytes read (out)
		 * \param	ea		memory address to read
		 * \param	buffer	output buffer
		 * \param	size	buffer size
		 */
		virtual ssize_t onReadMemory(size_t* nbytes, ea_t ea, void* buffer, size_t size, qstring* errbuf = nullptr) = 0;

		/*!
		 * \brief	If a rebase is required, we have to rebase the image
		 * \param	newBase	new base address of the IDA database
		 */
		virtual ssize_t onRebaseIfRequiredTo(ea_t newBase) = 0;

		/*!
		 * \brief	use to inform the plugin to the resume state
		 * \param	event	the event before the resume
		 */
		virtual ssize_t onResume(debug_event_t* event) = 0;

		/*!
		 * \brief	use to inform the debugger to read register state
		 * \param	tid	thread id
		 * \param	clsmask	class mask (for xample x86 class mask)
		 * \param	values	output register state, follow declaration of the debugger
		 * \param	errbuf	buffer error to inform state
		 */
		virtual ssize_t onReadRegisters(thid_t tid, int clsmask, regval_t* values, qstring* errbuf) = 0;

		/*!
		 * \brief	Inform the backend that the debugger is in the suspended state
		 * \param	dllsAdded	boolean inform that new dll has been added
		 * \param	thrNames	current thread
		 */
		virtual ssize_t onSuspended(bool dllsAdded, thread_name_vec_t* thrNames) = 0;

		/*!
		 * \brief	Ask debugger to exit the debugging process
		 * \param	errbuf	error buffer
		 */
		virtual ssize_t onExitProcess(qstring* errbuf = nullptr) = 0;

		/*!
		 * \brief	use to retrieve compiler source files
		 */
		virtual ssize_t onGetSrcinfoPath(qstring* path, ea_t base) = 0;

		/*!
		 * \brief	use to inform new and deleted breakpoint
		 */
		virtual ssize_t onUpdateBpts(int* nbpts, update_bpt_info_t* bpts, int nadd, int ndel, qstring* errbuf) = 0;

		/*!
		 * \brief	use to know if the resume is due to step of play
		 */
		virtual ssize_t onSetResumeMode(thid_t tid, resume_mode_t resmod) = 0;

		/*!
		* \brief	Sets the m_nextPosition attribute to the parameter.
		* \param	The next position which should be assumed by the cursor.
		*/
		virtual void setNextPosition(TTD::Position) = 0;

		virtual ssize_t onUpdateCallStack(thid_t tid, call_stack_t* trace) = 0;
		
		virtual void switchWay() = 0;
		virtual void requestBackwardsSingleStep() = 0;
		virtual void openPositionChooser() = 0;
	};
}

#endif