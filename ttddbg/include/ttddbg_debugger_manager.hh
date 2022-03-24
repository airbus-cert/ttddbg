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
	class DebuggerManager : public IDebuggerManager
	{
	protected:
		TTD::ReplayEngine m_engine;
		std::unique_ptr<TTD::Cursor> m_cursor;
		std::shared_ptr<ttddbg::Logger> m_logger;
		std::unique_ptr<TTD::Position> m_currentPosition;
		EventDeque m_events;

	public:
		explicit DebuggerManager(std::shared_ptr< ttddbg::Logger> logger);

		ssize_t onInit(std::string& hostname, int portNumber, std::string& password, qstring* errBuf) override;
		ssize_t onGetProcess(procinfo_vec_t* infos, qstring* errBuf) override;
		ssize_t onStartProcess(const char* path, const char* args, const char* startdir, uint32 flags, const char* inputPath, uint32 inputFileCRC32, qstring* errbuf = nullptr) override;
		ssize_t onGetDebappAttrs(debapp_attrs_t* attrs) override;
		ssize_t onGetDebugEvent(gdecode_t* code, debug_event_t* event, int timeout_ms) override;
		ssize_t onGetMemoryInfo(meminfo_vec_t* infos, qstring* errbuf = nullptr) override;

		ssize_t onReadMemory(size_t* nbytes, ea_t ea, void* buffer, size_t size, qstring* errbuf = nullptr) override;
		ssize_t onRebaseIfRequiredTo(ea_t newBase) override;
		ssize_t onResume(debug_event_t* event) override;
		ssize_t onReadRegisters(thid_t tid, int clsmask, regval_t* values, qstring* errbuf) override;
		ssize_t onSuspended(bool dllsAdded, thread_name_vec_t* thrNames) override;
		ssize_t onExitProcess(qstring* errbuf = nullptr) override;
	};
}

#endif