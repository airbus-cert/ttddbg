#include "ttddbg_debugger.hh"
#include <dbg.hpp>

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

	static const char* idaapi debugger_set_dbg_options(const char* keyword, int /*pri*/,
		int value_type, const void* value) {
		if (value_type == IDPOPT_STR) {
			msg("   option value: %s\n", (char*)value);
		}
		return IDPOPT_OK;
	}
	
	static ssize_t idaapi debugger_callback(void*, int notification_code, va_list va) 
	{
		Debugger* ttddbg = static_cast<Debugger*>(::dbg);
		return true;
	}

	Debugger::Debugger() : debugger_t()
	{
		version = IDD_INTERFACE_VERSION;
		name = "ttddbg";
		id = 0x100;

		flags =		DBG_FLAG_CAN_CONT_BPT | 
					DBG_FLAG_SAFE | 
					DBG_FLAG_DEBTHREAD | 
					DBG_FLAG_DEBUG_DLL;

		flags2 =	DBG_HAS_GET_PROCESSES | 
					DBG_HAS_DETACH_PROCESS | 
					DBG_HAS_REQUEST_PAUSE | 
					DBG_HAS_SET_EXCEPTION_INFO | 
					DBG_HAS_THREAD_SUSPEND | 
					DBG_HAS_THREAD_CONTINUE |
					DBG_HAS_SET_RESUME_MODE | 
					DBG_HAS_CHECK_BPT;

		resume_modes = DBG_RESMOD_STEP_INTO | DBG_RESMOD_STEP_OVER | DBG_RESMOD_STEP_OUT;
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

}