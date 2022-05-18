#include "ttddbg_debugger.hh"
#include "ttddbg_x86_registers.hh"
#include <dbg.hpp>
#include <winternl.h>
#include <codecvt>
#include <string>

namespace ttddbg
{
	/**********************************************************************/
	static const char* idaapi debugger_set_dbg_options(const char* keyword, int /*pri*/,
		int value_type, const void* value) {
		return IDPOPT_OK;
	}
	
	/**********************************************************************/
	ssize_t idaapi Debugger::debugger_callback(void*, int notification_code, va_list va)
	{
		Debugger* ttddbg = static_cast<Debugger*>(::dbg);

		switch (notification_code) {
			case debugger_t::ev_init_debugger: {
				const char* hostname = va_arg(va, const char*);
				int portnum = va_arg(va, int);
				const char* password = va_arg(va, const char*);
				auto errbuf = va_arg(va, qstring*);
				return ttddbg->m_manager->onInit(std::string(hostname), portnum, std::string(password), errbuf);
			}

			case debugger_t::ev_term_debugger: {
				return ttddbg->m_manager->OnTermDebugger();
			}

			case debugger_t::ev_get_processes: {
				procinfo_vec_t* procs = va_arg(va, procinfo_vec_t*);
				auto errbuf = va_arg(va, qstring*);
				return ttddbg->m_manager->onGetProcess(procs, errbuf);
			}

			case debugger_t::ev_get_debapp_attrs: {
				debapp_attrs_t* attrib = va_arg(va, debapp_attrs_t*);
				return ttddbg->m_manager->onGetDebappAttrs(attrib);
			}

			case debugger_t::ev_start_process: {
				const char* path = va_arg(va, const char*);
				const char* args = va_arg(va, const char*);
				const char* startdir = va_arg(va, const char*);
				uint32 dbg_proc_flags = va_arg(va, uint32);
				const char* input_path = va_arg(va, const char*);
				uint32 input_file_crc32 = va_arg(va, uint32);
				auto errbuf = va_arg(va, qstring*);
				return ttddbg->m_manager->onStartProcess(path, args, startdir, dbg_proc_flags, input_path, input_file_crc32, errbuf);
			}

			case debugger_t::ev_set_exception_info: {
				exception_info_t* info = va_arg(va, exception_info_t*);
				int qty = va_arg(va, int);
				return ttddbg->m_manager->onSetExceptionInfo(info, qty);
			}

			case debugger_t::ev_get_debug_event: {
				gdecode_t* code = va_arg(va, gdecode_t*);
				debug_event_t* event = va_arg(va, debug_event_t*);
				int timeout_ms = va_arg(va, int);
				return ttddbg->m_manager->onGetDebugEvent(code, event, timeout_ms);
			}

			case debugger_t::ev_get_memory_info: {
				meminfo_vec_t* ranges = va_arg(va, meminfo_vec_t*);
				auto errbuf = va_arg(va, qstring*);
				return ttddbg->m_manager->onGetMemoryInfo(ranges, errbuf);
			}

			case debugger_t::ev_read_memory: {
				size_t* nbytes = va_arg(va, size_t*);
				ea_t ea = va_arg(va, ea_t);
				void* buffer = va_arg(va, void*);
				size_t size = va_arg(va, size_t);
				auto errbuf = va_arg(va, qstring*);
				return ttddbg->m_manager->onReadMemory(nbytes, ea, buffer, size, errbuf);
			}

			case debugger_t::ev_rebase_if_required_to: {
				ea_t new_base = va_arg(va, ea_t);
				return ttddbg->m_manager->onRebaseIfRequiredTo(new_base);
			}

			case debugger_t::ev_resume: {
				debug_event_t* event = va_arg(va, debug_event_t*);
				return ttddbg->m_manager->onResume(event);
			}

			case debugger_t::ev_read_registers: {
				thid_t tid = va_argi(va, thid_t);
				int clsmask = va_arg(va, int);
				regval_t* values = va_arg(va, regval_t*);
				auto errbuf = va_arg(va, qstring*);
				return ttddbg->m_manager->onReadRegisters(tid, clsmask, values, errbuf);
			}

			case debugger_t::ev_suspended: {
				bool dlls_added = va_argi(va, bool);
				thread_name_vec_t* thr_names = va_arg(va, thread_name_vec_t*);
				return ttddbg->m_manager->onSuspended(dlls_added, thr_names);
			}
			case debugger_t::ev_exit_process:
			{
				auto errbuf = va_arg(va, qstring*);
				return ttddbg->m_manager->onExitProcess(errbuf);
			}

			case debugger_t::ev_get_srcinfo_path: {
				qstring* path = va_arg(va, qstring*);
				ea_t base = va_arg(va, ea_t);
				return ttddbg->m_manager->onGetSrcinfoPath(path, base);
			}

			case debugger_t::ev_update_bpts: {
				int* nbpts = va_arg(va, int*);
				update_bpt_info_t* bpts = va_arg(va, update_bpt_info_t*);
				int nadd = va_arg(va, int);
				int ndel = va_arg(va, int);
				auto errbuf = va_arg(va, qstring*);
				return ttddbg->m_manager->onUpdateBpts(nbpts, bpts, nadd, ndel, errbuf);
			}
			
			case debugger_t::ev_set_resume_mode: {
				thid_t tid = va_argi(va, thid_t);
				resume_mode_t resmod = va_argi(va, resume_mode_t);
				return ttddbg->m_manager->onSetResumeMode(tid, resmod);
			}

			case debugger_t::ev_update_call_stack: {
				thid_t tid = va_argi(va, thid_t);
				call_stack_t* trace = va_arg(va, call_stack_t*);
				return ttddbg->m_manager->onUpdateCallStack(tid, trace);
			}

			default:
				ttddbg->m_logger->info("unhandled code ", notification_code);
				break;
		}

		return true;
	}

	/**********************************************************************/
	Debugger::Debugger(std::shared_ptr< ttddbg::Logger> logger, std::unique_ptr<IDebuggerManager>&& manager)
		: m_logger { logger }, m_manager { std::move(manager) }
	{
		version = IDD_INTERFACE_VERSION;
		id = 0x100;

		flags =		DBG_FLAG_CAN_CONT_BPT | 
					DBG_FLAG_SAFE | 
					DBG_FLAG_DEBTHREAD | 
					DBG_FLAG_DEBUG_DLL;

		flags2 =	DBG_HAS_GET_PROCESSES |
					DBG_HAS_SET_RESUME_MODE;

		resume_modes = DBG_RESMOD_STEP_INTO;
		processor = "metapc";

		bpt_bytes = (const uchar*)"\xcc";
		bpt_size = 1;

		memory_page_size = 0x1000;

		set_dbg_options = debugger_set_dbg_options;
		callback = debugger_callback;

		filetype = inf_get_filetype();
	}

	/**********************************************************************/
	IDebuggerManager& Debugger::getManager()
	{
		return *m_manager;
	}
}