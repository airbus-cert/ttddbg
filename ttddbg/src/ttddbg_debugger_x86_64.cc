#include "ttddbg_debugger_x86_64.hh"
#include "ttddbg_x86_registers.hh"

namespace ttddbg
{
	/**********************************************************************/
	// Register information for x86_64 arch
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
	DebuggerManagerX86_64::DebuggerManagerX86_64(std::shared_ptr<ttddbg::Logger> logger, std::shared_ptr<Plugin> plugin)
		: DebuggerManager(logger, Arch::ARCH_64_BITS, plugin)
	{

	}

	/**********************************************************************/
	ssize_t DebuggerManagerX86_64::onReadRegisters(thid_t tid, int clsmask, regval_t* values, qstring* errbuf)
	{
		auto threadInfo = m_cursor->GetContextx86_64(tid);

		values[0].ival = threadInfo->Rax;
		values[1].ival = threadInfo->Rcx;
		values[2].ival = threadInfo->Rdx;
		values[3].ival = threadInfo->Rbx;
		values[4].ival = threadInfo->Rsp;
		values[5].ival = threadInfo->Rbp;
		values[6].ival = threadInfo->Rsi;
		values[7].ival = threadInfo->Rdi;
		values[8].ival = threadInfo->R8;
		values[9].ival = threadInfo->R9;
		values[10].ival = threadInfo->R10;
		values[11].ival = threadInfo->R11;
		values[12].ival = threadInfo->R12;
		values[13].ival = threadInfo->R13;
		values[14].ival = threadInfo->R14;
		values[15].ival = threadInfo->R15;
		values[16].ival = threadInfo->Rip;
		values[17].ival = threadInfo->EFlags;
		values[18].ival = threadInfo->SegCs;
		values[19].ival = threadInfo->SegDs;
		values[20].ival = threadInfo->SegSs;
		values[21].ival = threadInfo->SegEs;
		values[22].ival = threadInfo->SegFs;
		values[23].ival = threadInfo->SegGs;

		return DRC_OK;
	}


	/**********************************************************************/
	DebuggerX86_64::DebuggerX86_64(std::shared_ptr< ttddbg::Logger> logger, std::shared_ptr<Plugin> plugin)
		: Debugger(logger, std::make_unique<DebuggerManagerX86_64>(logger, plugin))
	{
		name = "ttddbg_x86_64";

		regclasses = x86_register_classes;
		default_regclasses = X86_GENERAL;
		registers = x64_regs;

		nregs = 24;
		bpt_bytes = (const uchar*)"\xcc";
	}

}