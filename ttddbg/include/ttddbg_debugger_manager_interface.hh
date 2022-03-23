#ifndef __TTDDBG_DEBUGGER_MANAGER_INTERFACE__
#define __TTDDBG_DEBUGGER_MANAGER_INTERFACE__

#include <ida.hpp>
#include <idd.hpp>

namespace ttddbg 
{
	class IDebuggerManager
	{
	public:
		virtual ssize_t onInit(std::string& hostname, int portNumber, std::string& password, qstring* errBuf) = 0;
		virtual ssize_t onGetProcess(procinfo_vec_t* infos, qstring* errBuf) = 0;
		virtual ssize_t onStartProcess(const char* path, const char* args, const char* startdir, uint32 dbg_proc_flags, const char* input_path, uint32 input_file_crc32, qstring* errbuf = nullptr) = 0;
		virtual ssize_t onGetDebappAttrs(debapp_attrs_t* out_pattrs) = 0;
		virtual ssize_t onGetDebugEvent(gdecode_t* code, debug_event_t* event, int timeout_ms) = 0;
		virtual ssize_t onGetMemoryInfo(meminfo_vec_t* infos, qstring* errbuf = nullptr) = 0;
		virtual ssize_t onReadMemory(size_t* nbytes, ea_t ea, void* buffer, size_t size, qstring* errbuf = nullptr) = 0;
		virtual ssize_t onRebaseIfRequiredTo(ea_t newBase) = 0;
		virtual ssize_t onResume(debug_event_t* event) = 0;
		virtual ssize_t onReadRegisters(thid_t tid, int clsmask, regval_t* values, qstring* errbuf) = 0;
		virtual ssize_t onSuspended(bool dllsAdded, thread_name_vec_t* thrNames) = 0;
	};
}

#endif