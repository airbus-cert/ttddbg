#include "ttddbg_debugger_x86.hh"
#include "ttddbg_x86_registers.hh"

namespace ttddbg
{
	/**********************************************************************/
	// x86 registers declaration
	static struct register_info_t x86_regs[] = {
		{"EAX", REGISTER_ADDRESS, X86_GENERAL, dt_dword, NULL, 0},
		{"ECX", REGISTER_ADDRESS, X86_GENERAL, dt_dword, NULL, 0},
		{"EDX", REGISTER_ADDRESS, X86_GENERAL, dt_dword, NULL, 0},
		{"EBX", REGISTER_ADDRESS, X86_GENERAL, dt_dword, NULL, 0},
		{"ESP", REGISTER_SP | REGISTER_ADDRESS, X86_GENERAL, dt_dword, NULL, 0},
		{"EBP", REGISTER_FP | REGISTER_ADDRESS, X86_GENERAL, dt_dword, NULL, 0},
		{"ESI", REGISTER_ADDRESS, X86_GENERAL, dt_dword, NULL, 0},
		{"EDI", REGISTER_ADDRESS, X86_GENERAL, dt_dword, NULL, 0},
		{"EIP", REGISTER_IP | REGISTER_ADDRESS, X86_GENERAL, dt_dword, NULL, 0},
		{"EFL", 0, X86_GENERAL, dt_dword, flag_bits, 0xdd5},
		{"CS", REGISTER_CS, X86_SEGMENT, dt_word, NULL, 0},
		{"DS", 0, X86_SEGMENT, dt_word, NULL, 0},
		{"SS", REGISTER_SS, X86_SEGMENT, dt_word, NULL, 0},
		{"ES", 0, X86_SEGMENT, dt_word, NULL, 0},
		{"FS", 0, X86_SEGMENT, dt_word, NULL, 0},
		{"GS", 0, X86_SEGMENT, dt_word, NULL, 0}
	};

	/**********************************************************************/
	DebuggerManagerX86::DebuggerManagerX86(std::shared_ptr<ttddbg::Logger> logger, std::shared_ptr<Plugin> plugin)
		: DebuggerManager(logger, Arch::ARCH_32_BITS, plugin)
	{

	}

	/**********************************************************************/
	ssize_t DebuggerManagerX86::onReadRegisters(thid_t tid, int clsmask, regval_t* values, qstring* errbuf)
	{
		auto threadInfo = m_cursor->GetContextx86(tid);

		values[0].ival = threadInfo->Eax;
		values[1].ival = threadInfo->Ecx;
		values[2].ival = threadInfo->Edx;
		values[3].ival = threadInfo->Ebx;
		values[4].ival = threadInfo->Esp;
		values[5].ival = threadInfo->Ebp;
		values[6].ival = threadInfo->Esi;
		values[7].ival = threadInfo->Edi;
		values[8].ival = threadInfo->Eip;
		values[9].ival = threadInfo->EFlags;
		values[10].ival = threadInfo->SegCs;
		values[11].ival = threadInfo->SegDs;
		values[12].ival = threadInfo->SegSs;
		values[13].ival = threadInfo->SegEs;
		values[14].ival = threadInfo->SegFs;
		values[15].ival = threadInfo->SegGs;

		return DRC_OK;
	}

	/**********************************************************************/
	DebuggerX86::DebuggerX86(std::shared_ptr< ttddbg::Logger> logger, std::shared_ptr<Plugin> plugin)
		: Debugger(logger, std::make_unique<DebuggerManagerX86>(logger, plugin))
	{
		name = "ttddbg_x86";
		
		regclasses = x86_register_classes;
		default_regclasses = X86_GENERAL;
		registers = x86_regs;

		nregs = 16;
	}
}