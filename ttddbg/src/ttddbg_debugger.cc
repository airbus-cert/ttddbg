#include "ttddbg_debugger.hh"
#include <dbg.hpp>
#include <winternl.h>
#include <codecvt>
#include <string>

namespace ttddbg
{
	static const char* x86_register_classes[] = {
	   "General registers",
	   "Segment registers",
	   "FPU registers",
	   "MMX registers",
	   "XMM registers",
	   NULL
	};

	enum X86RegClass {
		X86_GENERAL = 1,
		X86_SEGMENT = 2,
		X86_FPU = 4,
		X86_MMX = 8,
		X86_XMM = 16
	};

	static const char* flag_bits[32] = {
		"CF", NULL, "PF", NULL, "AF", NULL, "ZF", "SF", "TF", "IF", "DF", "OF",
		"IOPL", "IOPL", "NT", NULL, "RF", "VM", "AC", "VIF", "VIP", "ID"
	};

	static struct register_info_t x64_regs[] = {
		{"RAX", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"RCX", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"RDX", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"RBX", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"RSP", REGISTER_SP | REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"RBP", REGISTER_FP | REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"RSI", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"RDI", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"R8", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"R9", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"R10", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"R11", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"R12", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"R13", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"R14", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"R15", REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"RIP", REGISTER_IP | REGISTER_ADDRESS, X86_GENERAL, dt_qword, NULL, 0},
		{"EFL", 0, X86_GENERAL, dt_dword, flag_bits, 0xdd5},
		{"CS", REGISTER_CS, X86_SEGMENT, dt_word, NULL, 0},
		{"DS", 0, X86_SEGMENT, dt_word, NULL, 0},
		{"SS", REGISTER_SS, X86_SEGMENT, dt_word, NULL, 0},
		{"ES", 0, X86_SEGMENT, dt_word, NULL, 0},
		{"FS", 0, X86_SEGMENT, dt_word, NULL, 0},
		{"GS", 0, X86_SEGMENT, dt_word, NULL, 0},
	};

	/**********************************************************************/
	static const char* idaapi debugger_set_dbg_options(const char* keyword, int /*pri*/,
		int value_type, const void* value) {
		if (value_type == IDPOPT_STR) {
			msg("   option value: %s\n", (char*)value);
		}
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
				ttddbg->m_logger->info("ev_set_exception_info");
				return DRC_OK;
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
		name = "ttddbg";
		id = 0x100;

		flags =		DBG_FLAG_CAN_CONT_BPT | 
					DBG_FLAG_SAFE | 
					DBG_FLAG_DEBTHREAD | 
					DBG_FLAG_DEBUG_DLL;

		flags2 = DBG_HAS_GET_PROCESSES |
			DBG_HAS_SET_RESUME_MODE;

		resume_modes = DBG_RESMOD_STEP_INTO;
		processor = "metapc";

		regclasses = x86_register_classes;
		default_regclasses = X86_GENERAL;
		registers = x64_regs;

		nregs = 24;
		bpt_bytes = (const uchar*)"\xcc";
		bpt_size = 1;

		memory_page_size = 0x1000;

		set_dbg_options = debugger_set_dbg_options;
		callback = debugger_callback;

		filetype = inf_get_filetype();
	}

	/**********************************************************************/
	void Debugger::switchWay()
	{
		m_manager->switchWay();
	}
}