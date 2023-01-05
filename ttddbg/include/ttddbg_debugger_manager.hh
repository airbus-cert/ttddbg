#ifndef __TTDDBG_DEBUGGER_MANGER__
#define __TTDDBG_DEBUGGER_MANGER__

#include "ttddbg_debugger_manager_interface.hh"
#include "ttddbg_logger.hh"
#include "ttddbg_event_deque.hh"
#include "ttddbg_position_chooser.hh"
#include "ttddbg_plugin.hh"
#include "ttddbg_tracer_choosers.hh"
#include <deque>
#include <memory>
#include <Windows.h>
#include <filesystem>
#include "../../ttd-bindings/TTD/TTD.hpp"

namespace ttddbg 
{
	/*!
	 * \brief	Main implementation of the Time Travel debugger
	 *			Automata for IDA
	 */
	class DebuggerManager : public IDebuggerManager
	{
		friend PositionChooser;

	public:
		/*!
		 *	\brief size of architecture
		 */
		enum Arch
		{
			ARCH_32_BITS = 4,
			ARCH_64_BITS = 8
		};

	protected:

		/*!
		 * \brief	Current debugger architecture
		 */
		Arch	m_arch;

		/*!
		 * \brief	Time travel debugger interface
		 */
		TTD::ReplayEngine m_engine;

		/*!
		 * \brief	current state of the time travel debugger
		 */
		std::shared_ptr<TTD::Cursor> m_cursor;

		/*!
		 * \brief	main logger use to print informationals messages
		 */
		std::shared_ptr<ttddbg::Logger> m_logger;

		/*!
		 * \brief	main event queue use to send message to IDA debugger thread
		 */
		EventDeque m_events;

		/*!
		 * \brief	I'm forward or backward state
		 *			Time travel debugging baby!
		 */
		bool m_isForward;

		/*!
		 * \brief	State resume mode when process is resume
		 */
		resume_mode_t m_resumeMode;

		/*!
		 * \brief	State chooser window
		 */
		PositionChooser *m_positionChooser;

		/*!
		* \brief	Traced functions chooser
		*/
		TracerTraceChooser *m_traceChooser;

		/*!
		* \brief	The next position to which the cursor should go. If it is {0, 0}, continue as normal.
		*			If it is *not* {0, 0}, divert the control flow to set the position to this one, and set 
		*			it to {0, 0} afterwards.
		*/
		TTD::Position m_nextPosition;

		/*!
		* \brief	Flag holding whether the "next action" should be a Backwards Single Step. If "true",
		*			instead of doing the normal action, force the TTD engine to go back in time for a single
		*			instruction. Then, set to "false".
		*/
		bool m_backwardsSingleStep;	

		/*!
		 * \brief	Fake process id
		 */
		pid_t m_processId;

		/*!
		 * \brief	the module of interest (the main module loaded by IDA)
		 */
		std::filesystem::path m_targetImagePath;

		/*!
		* \brief	Reference to the Plugin.
		*			Used to hide & show toolbar buttons
		*/
		Plugin *m_plugin;

		/*!
		 * \brief	use to known if the current module is the one currently reversed
		 * \param	module	module to process
		 * \return	true if the TTD module is the one loaded into IDA
		 */
		bool isTargetModule(const TTD::TTD_Replay_Module& module);

		/*!
		* \brief	Automatically adds notable events to the Timeline (position_chooser). Notable events include:
		*			thread creation, thread exit, module load, module unload
		*/
		void populatePositionChooser();

	public:

		/*!
		 * \brief	ctor
		 * \param	logger	logger use to print message
		 */
		explicit DebuggerManager(std::shared_ptr< ttddbg::Logger> logger, Arch arch, Plugin *plugin);

		/*!
		 * \brief	First state of the automata
		 * \param	hostname	use in case of remote debugging
		 * \param	portNumber	host port in case of remote debugging
		 * \param	password	remote password
		 * \param	errbuf		use to signal error
		 */
		ssize_t onInit(std::string& hostname, int portNumber, std::string& password, qstring* errBuf) override;

		/*!
		 * \brief	When terminate a debugger
		 */
		ssize_t OnTermDebugger() override;

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
		 * \brief	Set exception configuration of the debugger
		 * \param	info	exception informations
		 */
		ssize_t onSetExceptionInfo(exception_info_t* info, int qty) override;

		/*!
		 * \brief	Read memory at a special process address
		 * \param	nbytes	number of bytes read (out)
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
		 * \brief	Inform the backend that the debugger is in the suspended state
		 * \param	dllsAdded	boolean inform that new dll has been added
		 * \param	thrNames	current thread 
		 */
		ssize_t onSuspended(bool dllsAdded, thread_name_vec_t* thrNames) override;

		/*!
		 * \brief	Ask debugger to exit the debugging process
		 * \param	errbuf	error buffer
		 */
		ssize_t onExitProcess(qstring* errbuf = nullptr) override;

		/*!
		 * \brief	use to retrieve compiler source files
		 */
		ssize_t onGetSrcinfoPath(qstring* path, ea_t base) override;

		/*!
		 * \brief	use to inform new and deleted breakpoint
		 */
		ssize_t onUpdateBpts(int* nbpts, update_bpt_info_t* bpts, int nadd, int ndel, qstring* errbuf) override;

		/*!
		 * \brief	When debugger is resumed
		 */
		ssize_t onSetResumeMode(thid_t tid, resume_mode_t resmod) override;

		/*!
		 * \brief	When call stack is updated
		 */
		ssize_t onUpdateCallStack(thid_t tid, call_stack_t* trace) override;

		/*!
		 * \brief	Run steps and emit code for loaded and unloaded module
		 * \param	steps	Number of steps to run -1 to run until next breakpoint
		 */
		void applyCursor(int steps);

		/*!
		 * \brief run the cursor directly at a special positon
		 * \param	newPos	target position
		 */
		void applyCursor(TTD::Position newPos);

		/*!
		 * \brief	change the way you debug : Welcome time travel debugger !
		 */
		void switchWay() override;

		/*!
		 * \brief	Request a single step debugging but in backward way ! 
		 */
		void requestBackwardsSingleStep() override;

		/*!
		 * \brief	Open the timeline
		 */
		void openPositionChooser() override;

		/*
		* \brief	Open the traced functions window
		*/
		void openTraceChooser() override;
		void refreshTraceChooser();

		void setNextPosition(TTD::Position newPos) override;

	private:

		std::set<uint32_t> getCursorThreads();
		std::set<TTD::TTD_Replay_Module*> getCursorModules();

		/*!
		 * \brief	Compute the diff between two position (forward and backward)
		 *			And emit the appropriate debug event
		 * \param	threadsBefore	List of thread id before the change of position
		 * \param	threadsAfter	List of thread id after the change of position
		 * \param	modulesBefore	List of modules loaded before the change of position
		 * \param	modulesAfter	List of modules loaded after the change of position
		 */
		void applyDifferences(std::set<uint32_t> threadsBefore, std::set<uint32_t> threadsAfter, std::set<TTD::TTD_Replay_Module*> modulesBefore, std::set<TTD::TTD_Replay_Module*> modulesAfter);
	};
}

#endif