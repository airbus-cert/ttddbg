#include "ttddbg_debugger_manager.hh"
#include "ttddbg_strings.hh"
#include <idp.hpp>
#include <ida.hpp>

namespace ttddbg
{
	DebuggerManager::DebuggerManager(std::shared_ptr<ttddbg::Logger> logger)
		: m_logger(logger)
	{

	}

	ssize_t DebuggerManager::onInit(std::string& hostname, int portNumber, std::string& password, qstring* errBuf)
	{
		m_logger->info("OnInit");
		return DRC_OK;
	}
	ssize_t DebuggerManager::onGetProcess(procinfo_vec_t* infos, qstring* errBuf)
	{
		m_logger->info("onGetProcess");
		process_info_t info;
		info.name = "test";
		info.pid = 1234;
		infos->push_back(info);
		return DRC_OK;
	}

	ssize_t DebuggerManager::onStartProcess(const char* path, const char* args, const char* startdir, uint32 dbg_proc_flags, const char* input_path, uint32 input_file_crc32, qstring* errbuf)
	{
		m_logger->info("onStartProcess");
		
		if (!m_engine.Initialize(Strings::to_wstring(path).c_str()))
		{
			m_logger->info("unable to load the trace ", path);
			return DRC_FAILED;
		}

		m_cursor = std::make_unique<TTD::Cursor>(*m_engine.NewCursor());
		
		// Init cursor at the first position
		m_cursor->SetPosition(m_engine.GetFirstPosition());

		m_events.addProcessStartEvent(
			1234,
			m_cursor->GetThreadInfo()->threadid, 
			Strings::find_module_name(m_engine.GetModuleList()[0].path), 
			m_engine.GetModuleList()[0].base_addr,
			0x1000, 
			m_engine.GetModuleList()[0].imageSize
		);
		
		for (int i = 1; i < m_cursor->GetThreadCount(); i++)
		{
			auto threadId = m_cursor->GetThreadList()[i].info->threadid;
			m_events.addThreadStartEvent(1234, threadId);
		}

		for (int i = 1; i < m_engine.GetModuleCount(); i++)
		{
			auto moduleInfo = m_engine.GetModuleList()[i];
			m_events.addLibLoadEvent(Strings::find_module_name(moduleInfo.path), moduleInfo.base_addr, moduleInfo.imageSize);
		}

		m_events.addBreakPointEvent(
			1234,
			m_cursor->GetThreadInfo()->threadid,
			m_cursor->GetProgramCounter()
		);
		
		return DRC_OK;
	}
	ssize_t DebuggerManager::onGetDebappAttrs(debapp_attrs_t* attrs)
	{
		m_logger->info("onGetDebappAttrs");
		return DRC_OK;
	}

	ssize_t DebuggerManager::onGetDebugEvent(gdecode_t* code, debug_event_t* event, int timeout_ms)
	{
		m_logger->info("onGetEventDbg");
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

	ssize_t DebuggerManager::onGetMemoryInfo(meminfo_vec_t* infos, qstring* errbuf)
	{
		m_logger->info("onGetMemoryInfo");
		auto moduleList = m_engine.GetModuleList();
		for (int i = 0; i < m_engine.GetModuleCount(); i++)
		{
			auto moduleInfo = moduleList[i];
			memory_info_t info;
			info.start_ea = moduleInfo.base_addr;
			info.end_ea = moduleInfo.base_addr + moduleInfo.imageSize;
			info.name = Strings::find_module_name(moduleInfo.path).c_str();
			info.bitness = 2;
			infos->push_back(info);
		}
		return DRC_OK;
	}

	ssize_t DebuggerManager::onReadMemory(size_t* nbytes, ea_t ea, void* buffer, size_t size, qstring* errbuf)
	{
		m_logger->info("onReadMemory ", ea);
		auto memory = m_cursor->QueryMemoryBuffer(ea, size);
		*nbytes = memory->size;
		if (memory->size > 0)
		{
			memcpy(buffer, memory->data, size);
		}

		return DRC_OK;
	}

	ssize_t DebuggerManager::onRebaseIfRequiredTo(ea_t newBase)
	{
		m_logger->info("onRebaseIfRequiredTo ", newBase);
		return DRC_OK;
	}

	ssize_t DebuggerManager::onResume(debug_event_t* event)
	{
		m_logger->info("onResume");
		if (event->eid() == event_id_t::BREAKPOINT)
		{
			TTD::TTD_Replay_ICursorView_ReplayResult replayrez;
			m_cursor->ReplayForward(&replayrez, m_engine.GetLastPosition(), 1);
			m_events.addBreakPointEvent(event->pid, event->tid, m_cursor->GetProgramCounter());
		}
		return DRC_OK;
	}

	ssize_t DebuggerManager::onReadRegisters(thid_t tid, int clsmask, regval_t* values, qstring* errbuf)
	{
		m_logger->info("onReadRegisters");
		auto threadInfo = m_cursor->GetCrossPlatformContext(tid);

		values[0].ival = threadInfo->rax;
		values[1].ival = threadInfo->rcx;
		values[2].ival = threadInfo->rdx;
		values[3].ival = threadInfo->rbx;
		values[4].ival = threadInfo->rsp;
		values[5].ival = threadInfo->rbp;
		values[6].ival = threadInfo->rsi;
		values[7].ival = threadInfo->rdi;
		values[8].ival = threadInfo->r8;
		values[9].ival = threadInfo->r9;
		values[10].ival = threadInfo->r10;
		values[11].ival = threadInfo->r11;
		values[12].ival = threadInfo->r12;
		values[13].ival = threadInfo->r13;
		values[14].ival = threadInfo->r14;
		values[15].ival = threadInfo->r15;
		values[16].ival = threadInfo->rip;
		
		return DRC_OK;
	}

	ssize_t DebuggerManager::onSuspended(bool dllsAdded, thread_name_vec_t* thrNames)
	{
		m_logger->info("onSuspended");
		return DRC_OK;
	}

	ssize_t DebuggerManager::onExitProcess(qstring* errbuf)
	{
		m_logger->info("onExitProcess");
		m_events.addProcessExitEvent(1234);
		return DRC_OK;
	}
}