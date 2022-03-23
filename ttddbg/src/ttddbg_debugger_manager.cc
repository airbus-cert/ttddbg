#include "ttddbg_debugger_manager.hh"
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

		if (!m_engine.Initialize(L"C:\\Users\\sylvain\\Documents\\cmd01.run"))
		{
			m_logger->info("unable to load the trace ", path);
			return DRC_FAILED;
		}

		m_cursor = std::make_unique<TTD::Cursor>(*m_engine.NewCursor());
		m_cursor->SetPosition(m_engine.GetFirstPosition());

		debug_event_t start;
		start.set_eid(event_id_t::PROCESS_STARTED);
		start.pid = NO_PROCESS;
		start.tid = NO_PROCESS;
		start.modinfo().base = (ea_t)0x00007ff7da920000;
		start.modinfo().rebase_to = (ea_t)0x00007ff7da920000;
		start.modinfo().name = "cmd.exe";
		start.modinfo().size = 0x67000;
		start.handled = false;
		m_events.push_back(start);

		debug_event_t initBrk;
		initBrk.set_eid(event_id_t::BREAKPOINT);
		initBrk.ea = m_cursor->GetProgramCounter();
		initBrk.pid = 1234;
		initBrk.tid = m_cursor->GetThreadInfo()->threadid;
		initBrk.handled = true;
		initBrk.bpt().hea = m_cursor->GetProgramCounter();
		initBrk.bpt().kea = BADADDR;
		m_events.push_back(initBrk);

		return DRC_OK;
	}
	ssize_t DebuggerManager::onGetDebappAttrs(debapp_attrs_t* attrs)
	{
		m_logger->info("onGetDebappAttrs");
		attrs->addrsize = 8;
		attrs->is_be = false;
		return DRC_OK;
	}

	ssize_t DebuggerManager::onGetDebugEvent(gdecode_t* code, debug_event_t* event, int timeout_ms)
	{
		m_logger->info("onGetEventDbg");
		if (!m_events.empty())
		{
			*code = GDE_ONE_EVENT;
			auto current = m_events.front();
			m_events.pop_front();

			*event = current;
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
			info.name = "test";
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
		return DRC_OK;
	}

	ssize_t DebuggerManager::onReadRegisters(thid_t tid, int clsmask, regval_t* values, qstring* errbuf)
	{
		m_logger->info("onReadRegisters");
		auto threadInfo = m_cursor->GetCrossPlatformContext(0xdc8);

		values[16].ival = threadInfo->rip;
		
		
		return DRC_OK;
	}

	ssize_t DebuggerManager::onSuspended(bool dllsAdded, thread_name_vec_t* thrNames)
	{
		m_logger->info("onSuspended");
		for (int i = 0; i < m_cursor->GetThreadCount(); i++)
		{
			thread_name_t threadName;
			auto threadInfo = m_cursor->GetThreadInfo(i);
			threadName.tid = threadInfo->threadid;
			thrNames->push_back(threadName);
		}
		return DRC_OK;
	}
}