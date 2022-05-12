#ifndef __TTDDBG_X86_REGISTERS__
#define __TTDDBG_X86_REGISTERS__

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
}

#endif