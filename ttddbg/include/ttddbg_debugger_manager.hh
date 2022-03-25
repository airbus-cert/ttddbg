#ifndef __TTDDBG_DEBUGGER_MANGER__
#define __TTDDBG_DEBUGGER_MANGER__

#include "ttddbg_debugger_manager_interface.hh"
#include "ttddbg_logger.hh"
#include "ttddbg_event_deque.hh"
#include <deque>
#include <memory>
#include <Windows.h>
#include "../../ttd-bindings/TTD/TTD.hpp"

namespace ttddbg 
{
	/*!
	 * \brief	Main implementation of the Time Travel debugger
	 *			Automato for IDA
	 */
	class DebuggerManager : public IDebuggerManager
	{
	protected:
		/*!
		 * \brief	Time travel debugger interface
		 */
		TTD::ReplayEngine m_engine;

		/*!
		 * \brief	current state of the time travel debugger
		 */
		std::unique_ptr<TTD::Cursor> m_cursor;

		/*!
		 * \brief	main logger use to print informationals messages
		 */
		std::shared_ptr<ttddbg::Logger> m_logger;
		std::unique_ptr<TTD::Position> m_currentPosition;

		/*!
		 * \brief	main event queue use to send message to IDA debugger thread
		 */
		EventDeque m_events;

		/*!
		 * \brief	I'm forward or backward state
		 *			Time travel debugging baby!
		 */
		bool m_isForward;

	public:
		/*!
		 * \brief	ctor
		 * \param	logger	logger use to print message
		 */
		explicit DebuggerManager(std::shared_ptr< ttddbg::Logger> logger);

		/*!
		 * \brief	First state of the automata
		 * \param	hostname	use in case of remote debugging
		 * \param	portNumber	host port in case of remote debugging
		 * \param	password	remote password
		 * \param	errbuf		use to signal error
		 */
		ssize_t onInit(std::string& hostname, int portNumber, std::string& password, qstring* errBuf) override;

		/*!
		 * \brief	Event use to get information about the current debugging process
		 * \param	infos	list of processes started by debugger (name and pid)
		 */
		ssize_t onGetProcess(procinfo_vec_t* infos, qstring* errBuf) override;

		/*!
		 * \brief	Debugger ask to start a process
		 * \param	path	path to the executable, we will use this parameter for input run files
		 * \param	args	arguments path to the executable
		 * \param	startdir	working directory
		 * \param	flags	startup flags
		 * \param	image input files
		 * \param	inputFileCRC32	use to check if image is aligned with debugging process
		 */
		ssize_t onStartProcess(const char* path, const char* args, const char* startdir, uint32 flags, const char* inputPath, uint32 inputFileCRC32, qstring* errbuf = nullptr) override;
		
		/*!
		 * \brief	use to get debug attributes
		 * \param	attrs	debug attributes
		 */
		ssize_t onGetDebappAttrs(debapp_attrs_t* attrs) override;

		/*!
		 * \brief	Main event to get current debug event
		 *			Will be used to pop from the event queue
		 * \param	code	use to to set if pending events exist in the queue
		 * \param	event	output event
		 * \param	timeout_ms
		 */
		ssize_t onGetDebugEvent(gdecode_t* code, debug_event_t* event, int timeout_ms) override;

		/*!
		 * \brief	Get memory information, like segment with module named
		 *			We can know wich address is used by which module
		 * \param	infos	list of memory infos
		 */
		ssize_t onGetMemoryInfo(meminfo_vec_t* infos, qstring* errbuf = nullptr) override;

		/*!
		 * \brief	Read memory at a special process address
		 * \param	nbytes	number of bytes read (ou)
		 * \param	ea		memory address to read
		 * \param	buffer	output buffer
		 * \param	size	buffer size
		 */
		ssize_t onReadMemory(size_t* nbytes, ea_t ea, void* buffer, size_t size, qstring* errbuf = nullptr) override;

		/*!
		 * \brief	If a rebase is required, we have to rebase the image
		 * \param	newBase	new base address of the IDA database	
		 */
		ssize_t onRebaseIfRequiredTo(ea_t newBase) override;

		/*!
		 * \brief	use to inform the plugin to the resume state
		 * \param	event	the event before the resume
		 */
		ssize_t onResume(debug_event_t* event) override;

		/*!
		 * \param	
		 */
		ssize_t onReadRegisters(thid_t tid, int clsmask, regval_t* values, qstring* errbuf) override;
		ssize_t onSuspended(bool dllsAdded, thread_name_vec_t* thrNames) override;
		ssize_t onExitProcess(qstring* errbuf = nullptr) override;
		ssize_t onGetSrcinfoPath(qstring* path, ea_t base) override;
		ssize_t onUpdateBpts(int* nbpts, update_bpt_info_t* bpts, int nadd, int ndel, qstring* errbuf) override;

		void switchWay() override;
	};
}

#endif