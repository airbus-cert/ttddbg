#include <filesystem>
#include <fstream>
#include <iostream>
#include "ttddbg_debugger_manager.hh"
#include "ttddbg_strings.hh"
#include <idp.hpp>
#include <ida.hpp>
#include <segment.hpp>

namespace ttddbg
{
	/**********************************************************************/
	static int from_idabptype(bpttype_t bp_type)
	{
		switch (bp_type)
		{
		case BPT_WRITE:
			return TTD::BP_FLAGS::WRITE;
		case BPT_READ:
			return TTD::BP_FLAGS::READ;
		case BPT_RDWR:
			return TTD::BP_FLAGS::READ | TTD::BP_FLAGS::READ;
		case BPT_EXEC:
			return TTD::BP_FLAGS::EXEC;
		case BPT_DEFAULT:
			return TTD::BP_FLAGS::EXEC;
		default:
			return TTD::BP_FLAGS::EXEC;
		} 
	}

	/**********************************************************************/
	bool DebuggerManager::isTargetModule(const TTD::TTD_Replay_Module& module)
	{
		if (m_targetImagePath.string() == Strings::to_string(module.path))
		{
			return true;
		}
		else if (m_targetImagePath.filename() == Strings::find_module_name(module.path))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/**********************************************************************/
	DebuggerManager::DebuggerManager(std::shared_ptr<ttddbg::Logger> logger, Arch arch)
		: m_logger(logger), m_arch{ arch }, m_isForward{ true }, m_resumeMode{ resume_mode_t::RESMOD_NONE }, m_positionChooser(new PositionChooser(m_logger)), m_nextPosition{ 0 }, m_processId(1234), m_backwardsSingleStep(false)
	{
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onInit(std::string& hostname, int portNumber, std::string& password, qstring* errBuf)
	{
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onGetProcess(procinfo_vec_t* infos, qstring* errBuf)
	{
		process_info_t info;
		info.name = "test";
		info.pid = m_processId;
		infos->push_back(info);
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onStartProcess(const char* path, const char* args, const char* startdir, uint32 dbg_proc_flags, const char* input_path, uint32 input_file_crc32, qstring* errbuf)
	{	
		m_isForward = true;
		m_targetImagePath = input_path;

		// check if the file exist
		if (!std::filesystem::exists(path))
		{
			m_logger->error("unable to find trace file : ", path);
			return DRC_FAILED;
		}

		std::ifstream traceFile(path, std::ios::out | std::ios::binary);
		if (!traceFile.is_open())
		{
			m_logger->error("unable to open the trace : ", path);
			return DRC_FAILED;
		}

		std::vector<char> magic(6);
		traceFile.read(magic.data(), magic.size());
		traceFile.close();

		if (magic != std::vector<char>({ 'T', 'T', 'D', 'L', 'o', 'g'}))
		{
			m_logger->error("invalid trace file (wrong magic) : ", path);
			return DRC_FAILED;
		}

		// Initialize engine
		if (!m_engine.Initialize(Strings::to_wstring(path).c_str()))
		{
			m_logger->error("unable to load the trace : ", path);
			return DRC_FAILED;
		}

		// init step mode
		m_resumeMode = resume_mode_t::RESMOD_NONE;

		m_cursor = std::make_shared<TTD::Cursor>(m_engine.NewCursor());
		m_positionChooser->setCursor(m_cursor);

		// Populate position chooser (timeline)
		populatePositionChooser();
		
		// Init cursor at the first position
		m_cursor->SetPosition(m_engine.GetFirstPosition());

		m_events.addProcessStartEvent(
			m_processId,
			m_cursor->GetThreadInfo()->threadid, 
			Strings::to_string(m_engine.GetModuleList()[0].path), 
			m_engine.GetModuleList()[0].base_addr,
			isTargetModule(m_engine.GetModuleList()[0]) ? m_engine.GetModuleList()[0].base_addr : BADADDR,
			m_engine.GetModuleList()[0].imageSize
		);

		// if the target module is not the main one
		// we will simulate load program to force rebase
		// PDB load and unload symbol
		// The main symbol must not be unload !!!
		if (!isTargetModule(m_engine.GetModuleList()[0]))
		{
			for (int i = 1; i < m_engine.GetModuleCount(); i++)
			{
				auto moduleInfo = m_engine.GetModuleList()[i];
				if (isTargetModule(moduleInfo))
				{
					m_events.addLibLoadEvent(
						Strings::to_string(moduleInfo.path),
						moduleInfo.base_addr,
						moduleInfo.base_addr,
						moduleInfo.imageSize
					);
				}
			}
		}
		
		for (int i = 1; i < m_cursor->GetThreadCount(); i++)
		{
			auto threadId = m_cursor->GetThreadList()[i].info->threadid;
			m_events.addThreadStartEvent(m_processId, threadId);
		}

		for (int i = 1; i < m_cursor->GetModuleCount(); i++)
		{
			auto moduleInfo = m_cursor->GetModuleList()[i];
			m_events.addLibLoadEvent(
				Strings::to_string(moduleInfo.module->path), 
				moduleInfo.module->base_addr,
				isTargetModule(*moduleInfo.module) ? moduleInfo.module->base_addr : BADADDR,
				moduleInfo.module->imageSize
			);
		}

		m_events.addBreakPointEvent(
			m_processId,
			m_cursor->GetThreadInfo()->threadid,
			m_cursor->GetProgramCounter()
		);
		
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onGetDebappAttrs(debapp_attrs_t* attrs)
	{
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onGetDebugEvent(gdecode_t* code, debug_event_t* event, int timeout_ms)
	{
		if (!m_events.isEmpty())
		{
			*code = GDE_ONE_EVENT;
			*event = m_events.popEvent();
		}
		else
		{
			*code = GDE_NO_EVENT;
		}
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onGetMemoryInfo(meminfo_vec_t* infos, qstring* errbuf)
	{
		memory_info_t other;
		other.start_ea = 0;
		other.end_ea = (ea_t)0x7FFFFFFFFFFF; // Userland process on windows
		other.bitness = (m_arch == ARCH_64_BITS)?2:1;
		infos->push_back(other);
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onReadMemory(size_t* nbytes, ea_t ea, void* buffer, size_t size, qstring* errbuf)
	{
		auto memory = m_cursor->QueryMemoryBuffer(ea, size);
		*nbytes = memory->size;
		if (memory->size > 0)
		{
			memcpy(buffer, memory->data, size);
		}
		free(memory->data);
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onRebaseIfRequiredTo(ea_t newBase)
	{
		rebase_program(newBase - get_imagebase(), MSF_FIXONCE);
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onResume(debug_event_t* event)
	{
		if (event->eid() == event_id_t::BREAKPOINT || event->eid() == event_id_t::STEP)
		{
			if (m_nextPosition.Major != 0 || m_nextPosition.Minor != 0) {
				// Special case: instead of stepping or resuming, if there is a "next position" saved,
				// go to this position instead
				m_logger->info("special case: next position: ", m_nextPosition.Major, " ", m_nextPosition.Minor);
				this->applyCursor(m_nextPosition);
				m_events.addBreakPointEvent(m_processId, m_cursor->GetThreadInfo()[0].threadid, m_cursor->GetProgramCounter());
				m_nextPosition = { 0 };
				return DRC_OK;
			}

			if (m_backwardsSingleStep) {
				// Special case: if "m_backwardsSingleStep" is true, simulate a "single-step"
				// back in time: we force m_isForward to false and "applyCursor(1)", which effectively
				// moves back in time of 1 unit
				bool old_isForward = m_isForward;
				m_isForward = false;
				applyCursor(1);
				m_isForward = old_isForward;

				m_events.addBreakPointEvent(m_processId, m_cursor->GetThreadInfo()[0].threadid, m_cursor->GetProgramCounter());

				m_backwardsSingleStep = false;

				return DRC_OK;
			}

			switch (m_resumeMode)
			{
			case resume_mode_t::RESMOD_NONE:
			{
				this->applyCursor(-1);
				m_events.addBreakPointEvent(m_processId, m_cursor->GetThreadInfo()[0].threadid, m_cursor->GetProgramCounter());
				break;
			}
			case resume_mode_t::RESMOD_INTO:
			{
				this->applyCursor(1);
				m_events.addStepEvent(m_processId, m_cursor->GetThreadInfo()[0].threadid);
				break;
			}
			default:
				m_logger->info("unsupported resume mode ", (int)m_resumeMode);
				break;
			}
			m_resumeMode = resume_mode_t::RESMOD_NONE;
		}
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onSuspended(bool dllsAdded, thread_name_vec_t* thrNames)
	{
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onExitProcess(qstring* errbuf)
	{
		m_events.addProcessExitEvent(m_processId);
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onGetSrcinfoPath(qstring* path, ea_t base)
	{
		for (int i = 0; i < m_cursor->GetModuleCount(); i++)
		{
			auto module = m_cursor->GetModuleList()[i].module;
			if (module->base_addr == base)
			{
				*path = Strings::to_string(module->path).c_str();
				break;
			}
		}
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onUpdateBpts(int* nbpts, update_bpt_info_t* bpts, int nadd, int ndel, qstring* errbuf)
	{
		int i = 0;
		*nbpts = 0;
		for (; i < nadd; i++)
		{
			TTD::TTD_Replay_MemoryWatchpointData data;
			data.addr = bpts[i].ea;
			data.size = m_arch;
			data.flags = from_idabptype(bpts->type);
			m_cursor->AddMemoryWatchpoint(&data);
			(*nbpts)++;
		}

		for (; i < ndel; i++)
		{
			TTD::TTD_Replay_MemoryWatchpointData data;
			data.addr = bpts[i].ea;
			data.size = m_arch;
			data.flags = from_idabptype(bpts->type);
			m_cursor->RemoveMemoryWatchpoint(&data);
			(*nbpts)++;
		}
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onSetResumeMode(thid_t tid, resume_mode_t resmod)
	{
		m_resumeMode = resmod;
		return DRC_OK;
	}

	/**********************************************************************/
	ssize_t DebuggerManager::onUpdateCallStack(thid_t tid, call_stack_t* trace)
	{
		return DRC_NONE;
	}

	/**********************************************************************/
	void DebuggerManager::applyCursor(int steps)
	{
		// compute current list of thread
		std::set<uint32_t> threadBefore = getCursorThreads();
		std::set<TTD::TTD_Replay_Module*> moduleBefore = getCursorModules();

		TTD::TTD_Replay_ICursorView_ReplayResult replayrez;
		
		if (m_isForward)
		{
			m_cursor->ReplayForward(&replayrez, m_engine.GetLastPosition(), steps);
		}
		else
		{
			m_cursor->ReplayBackward(&replayrez, m_engine.GetFirstPosition(), steps);
		}

		std::set<uint32_t> threadAfter = getCursorThreads();
		std::set<TTD::TTD_Replay_Module*> moduleAfter = getCursorModules();

		applyDifferences(threadBefore, threadAfter, moduleBefore, moduleAfter);

		m_logger->info("Now at position ", m_cursor->GetPosition()->Major, " ", m_cursor->GetPosition()->Minor);
	}

	/**********************************************************************/
	void DebuggerManager::applyCursor(TTD::Position newPos) {
		std::set<uint32_t> threadBefore = getCursorThreads();
		std::set<TTD::TTD_Replay_Module*> moduleBefore = getCursorModules();

		m_cursor->SetPosition(&newPos);

		std::set<uint32_t> threadAfter = getCursorThreads();
		std::set<TTD::TTD_Replay_Module*> moduleAfter = getCursorModules();

		applyDifferences(threadBefore, threadAfter, moduleBefore, moduleAfter);

		m_logger->info("Now at position ", m_cursor->GetPosition()->Major, " ", m_cursor->GetPosition()->Minor);
	}

	/**********************************************************************/
	std::set<uint32_t> DebuggerManager::getCursorThreads() {
		std::set<uint32_t> threads;
		for (int i = 0; i < m_cursor->GetThreadCount(); i++)
		{
			threads.insert(m_cursor->GetThreadList()[i].info->threadid);
		}
		return threads;
	}

	/**********************************************************************/
	std::set<TTD::TTD_Replay_Module*> DebuggerManager::getCursorModules() {
		std::set<TTD::TTD_Replay_Module*> modules;
		for (int i = 0; i < m_cursor->GetModuleCount(); i++)
		{
			modules.insert(m_cursor->GetModuleList()[i].module);
		}
		return modules;
	}

	/**********************************************************************/
	void DebuggerManager::applyDifferences(std::set<uint32_t> threadBefore, std::set<uint32_t> threadAfter, std::set<TTD::TTD_Replay_Module*> moduleBefore, std::set<TTD::TTD_Replay_Module*> moduleAfter) {
		// Check created and exited thread between two states
		std::vector<uint32_t> threadExited, threadStarted;

		std::set_difference(threadBefore.begin(), threadBefore.end(), threadAfter.begin(), threadAfter.end(), std::inserter(threadExited, threadExited.begin()));
		std::set_difference(threadAfter.begin(), threadAfter.end(), threadBefore.begin(), threadBefore.end(), std::inserter(threadStarted, threadStarted.begin()));

		std::for_each(threadExited.begin(), threadExited.end(),
			[this](uint32_t threadId) {
				m_events.addThreadExitEvent(m_processId, threadId);
			}
		);
		std::for_each(threadStarted.begin(), threadStarted.end(),
			[this](uint32_t threadId) {
				m_events.addThreadStartEvent(m_processId, threadId);
			}
		);

		// Check loaded and unloaded modules
		std::vector<TTD::TTD_Replay_Module*> moduleUnloaded, moduleLoaded;

		std::set_difference(moduleBefore.begin(), moduleBefore.end(), moduleAfter.begin(), moduleAfter.end(), std::inserter(moduleUnloaded, moduleUnloaded.begin()));
		std::set_difference(moduleAfter.begin(), moduleAfter.end(), moduleBefore.begin(), moduleBefore.end(), std::inserter(moduleLoaded, moduleLoaded.begin()));

		std::for_each(moduleUnloaded.begin(), moduleUnloaded.end(),
			[this](TTD::TTD_Replay_Module* module) {
				if (!isTargetModule(*module))
				{
					m_events.addLibUnloadEvent(
						Strings::to_string(module->path),
						module->base_addr
					);
				}
			}
		);

		std::for_each(moduleLoaded.begin(), moduleLoaded.end(),
			[this](TTD::TTD_Replay_Module* module) {
				m_events.addLibLoadEvent(
					Strings::to_string(module->path),
					module->base_addr,
					isTargetModule(*module) ? module->base_addr : BADADDR,
					module->imageSize
				);
			}
		);
	}

	/**********************************************************************/
	void DebuggerManager::switchWay()
	{
		m_isForward = !m_isForward;
	}

	/**********************************************************************/
	void DebuggerManager::requestBackwardsSingleStep()
	{
		m_backwardsSingleStep = true;
	}

	/**********************************************************************/
	void DebuggerManager::openPositionChooser() {
		if (m_positionChooser != nullptr) {
			m_positionChooser->choose();
		}
	}

	/**********************************************************************/
	void DebuggerManager::setNextPosition(TTD::Position newPos) {
		m_nextPosition = newPos;
	}

	/**********************************************************************/
	void DebuggerManager::populatePositionChooser() {
		// TODO: use m_engine methods to add timeline positions for each:
		// - Thread creation / exit
		// - Module load / unload

		if (m_positionChooser->get_count() > 0) {
			// If the position count is > 0 even before populating, it means that
			// a list of positions was already loaded. In this case, we do not populate it further.
			return;
		}

		auto threadCreatedEvents = m_engine.GetThreadCreatedEvents();
		auto threadExitedEvents = m_engine.GetThreadTerminatedEvents();
		auto moduleLoadedEvents = m_engine.GetModuleLoadedEvents();
		auto moduleUnloadedEvents = m_engine.GetModuleUnloadedEvents();

		auto itModuleLoaded = moduleLoadedEvents.begin();
		auto itModuleUnloaded = moduleUnloadedEvents.begin();
		auto itThreadCreate = threadCreatedEvents.begin();
		auto itThreadTerminate = threadExitedEvents.begin();

		std::ostringstream oss;
		TTD::Position pos = {0};

		while (itModuleLoaded != moduleLoadedEvents.end() || itModuleUnloaded != moduleUnloadedEvents.end() || itThreadCreate != threadCreatedEvents.end() || itThreadTerminate != threadExitedEvents.end())
		{
			// For each iterator, get the current event position
			TTD::Position moduleLoadedPosition = (itModuleLoaded == moduleLoadedEvents.end()) ? TTD::POSITION_MAX : itModuleLoaded->pos;
			TTD::Position moduleUnloadedPosition = (itModuleUnloaded == moduleUnloadedEvents.end()) ? TTD::POSITION_MAX : itModuleUnloaded->pos;
			TTD::Position threadCreatePosition = (itThreadCreate == threadCreatedEvents.end()) ? TTD::POSITION_MAX : itThreadCreate->pos;
			TTD::Position threadTerminatePosition = (itThreadTerminate == threadExitedEvents.end()) ? TTD::POSITION_MAX : itThreadTerminate->pos;

			// Now, we look for the smallest position and add the event to the timeline
			// This way, we add the events in the order they happen
			oss.str("");

			if (moduleLoadedPosition < moduleUnloadedPosition && moduleLoadedPosition < threadCreatePosition && moduleLoadedPosition < threadTerminatePosition)
			{
				oss << "Module loaded: " << Strings::to_string(itModuleLoaded->info->path);
				pos = moduleLoadedPosition;
				itModuleLoaded++;
			}
			else if (moduleUnloadedPosition < moduleLoadedPosition && moduleUnloadedPosition < threadCreatePosition && moduleUnloadedPosition < threadTerminatePosition)
			{
				oss << "Module unloaded: " << Strings::to_string(itModuleUnloaded->info->path);
				pos = moduleUnloadedPosition;
				itModuleUnloaded++;
			}
			else if (threadCreatePosition < moduleLoadedPosition && threadCreatePosition < moduleLoadedPosition && threadCreatePosition < threadTerminatePosition)
			{
				oss << "Thread created: " << itThreadCreate->info->threadid;
				pos = threadCreatePosition;
				itThreadCreate++;
			}
			else if (threadTerminatePosition < moduleLoadedPosition && threadTerminatePosition < moduleLoadedPosition && threadTerminatePosition < threadCreatePosition)
			{
				oss << "Thread exited: " << itThreadTerminate->info->threadid;
				pos = threadTerminatePosition;
				itThreadTerminate++;
			}
			else
			{
				oss << "Unknown iterator";
				pos = { 0 };
			}

			m_positionChooser->addNewPosition(oss.str(), pos);
		}
	}
}