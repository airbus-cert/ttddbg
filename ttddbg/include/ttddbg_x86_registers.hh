#ifndef __TTDDBG_X86_REGISTERS__
#define __TTDDBG_X86_REGISTERS__

namespace ttddbg 
{
	/*!
	 * \brief	Declare x86 arch class register
	 *			This code is inspired from https://github.com/cseagle/sk3wldbg
	 */
	static const char* x86_register_classes[] = {
		"General registers",
		"Segment registers",
		"FPU registers",
		"MMX registers",
		"XMM registers",
		NULL
	};

	/*!
	 * \brief	Declare x86 arch class register enum
	 *			This code is inspired from https://github.com/cseagle/sk3wldbg
	 */
	enum X86RegClass {
		X86_GENERAL = 1,
		X86_SEGMENT = 2,
		X86_FPU = 4,
		X86_MMX = 8,
		X86_XMM = 16
	};

	/*!
	 * \brief	Bit flags
	 *			This code is inspired from https://github.com/cseagle/sk3wldbg
	 */
	static const char* flag_bits[32] = {
		"CF", NULL, "PF", NULL, "AF", NULL, "ZF", "SF", "TF", "IF", "DF", "OF",
		"IOPL", "IOPL", "NT", NULL, "RF", "VM", "AC", "VIF", "VIP", "ID"
	};
}

#endif