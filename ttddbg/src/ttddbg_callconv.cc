#include "ttddbg_callconv.hh"

#include <typeinf.hpp>

namespace ttddbg {
	void* readCursorMemory(TTD::Cursor* cursor, size_t offset, size_t len) {
		TTD::MemoryBuffer* buf = cursor->QueryMemoryBuffer(offset, len);
		return buf->data;
	}

	// Get the offset from ESP to access a specific function argument stored on the stack.
	// Information about argument sizes is fetched from the IDA database.
	// "firstStackArgNum" is the index of the first argument stored on the stack.
	size_t stackArgOffset(tinfo_t func_tinfo, size_t n, size_t firstStackArgNum) {
		size_t espOffset = 0;
		tinfo_t arg;
		for (size_t i = firstStackArgNum; i < n; i++) {
			arg = func_tinfo.get_nth_arg(i);
			espOffset += arg.get_size();
		}

		return espOffset;
	}

	DWORD32 x86_getIntArg(TTD::Cursor* cursor, tinfo_t tinfo, size_t n) {
		func_type_data_t funcdata;
		tinfo.get_func_details(&funcdata);
		cm_t cc = funcdata.get_cc();

		size_t firstStackArg = 0;

		if (cc == CM_CC_FASTCALL && n < 2) {
			DWORD32 registers[2] = { cursor->GetContextx86()->Ecx,  cursor->GetContextx86()->Edx };
			return registers[n];
		}
		else if (cc == CM_CC_FASTCALL) {
			firstStackArg = 2;
		}
		else if (cc == CM_CC_STDCALL || cc == CM_CC_CDECL) {
			// We are *before* the function prologue, and *before* the "call" instruction is actually executed so:
			// - ebp is still pointing to the caller's stack base
			// - the stack contains (no return address, because no "call"):
			//   - arg1   <--- esp
			//   - arg2
			size_t espOffset = stackArgOffset(tinfo, n, firstStackArg);
			DWORD32 esp = cursor->GetContextx86()->Esp;

			return *(DWORD32*)readCursorMemory(cursor, esp + espOffset, 4);
		}

		return -1;
	}

	DWORD64 x64_getIntArg(TTD::Cursor* cursor, tinfo_t tinfo, size_t n) {
		func_type_data_t funcdata;
		tinfo.get_func_details(&funcdata);
		cm_t cc = funcdata.get_cc();

		if (cc != CM_CC_FASTCALL) {
			msg("[ttddbg] [!!] non-fastcall functions are not supported under x64");
			return -1;
		}

		if (n < 4) {
			DWORD64 registers[4] = { cursor->GetContextx86_64()->Rcx,  cursor->GetContextx86_64()->Rdx, cursor->GetContextx86_64()->R8, cursor->GetContextx86_64()->R9 };
			return registers[n];
		}
		else {
			size_t rspOffset = stackArgOffset(tinfo, n, 4);
			DWORD64 rsp = cursor->GetContextx86_64()->Rsp;

			return *(DWORD64*)readCursorMemory(cursor, rsp + rspOffset, 8);
		}

		return -1;
	}

	M128A x64_getFloatArg(TTD::Cursor* cursor, tinfo_t tinfo, size_t n) {
		func_type_data_t funcdata;
		tinfo.get_func_details(&funcdata);
		cm_t cc = funcdata.get_cc();

		if (cc != CM_CC_FASTCALL) {
			msg("[ttddbg] [!!] non-fastcall functions are not supported under x64\n");
			return M128A{ 0 };
		}

		if (n < 4) {
			M128A registers[4] = { cursor->GetContextx86_64()->Xmm0,  cursor->GetContextx86_64()->Xmm1, cursor->GetContextx86_64()->Xmm2, cursor->GetContextx86_64()->Xmm3 };
			return registers[n];
		}
		else {
			// TODO: Need different handling depending on whether it's a float or double (different size on the stack)
		}

		return M128A{ 0 };
	}

	qstring readStringAt(TTD::Cursor* cursor, size_t offset) {
		qstring str;
		char c = *(char*)readCursorMemory(cursor, offset, 1);
		int i = 0;

		while (c != 0) {
			cat_char(&str, c);
			i++;
			c = *(char*)readCursorMemory(cursor, offset+i, 1);
		}

		return str;
	}

	qstring readWideStringAt(TTD::Cursor* cursor, size_t offset) {
		qstring str;
		wchar_t c = *(wchar_t*)readCursorMemory(cursor, offset, 2);
		int i = 0;

		while (c != 0) {
			cat_char(&str, c);
			i += 2;
			c = *(wchar_t*)readCursorMemory(cursor, offset+i, 2);
		}

		return str;
	}

	void cat_char(qstring* out, wchar_t c) {
		switch (c) {
		case '\n':
			out->cat_sprnt("\\n");
			break;
		case '\r':
			out->cat_sprnt("\\r");
			break;
		case '\t':
			out->cat_sprnt("\\t");
			break;
		default:
			if ((c >> 8) == 0) {
				// 8-bit char
				out->cat_sprnt("%c", c & 0xFF);
			}
			else {
				out->cat_sprnt("%lc", c);
			}
		}
	}
}