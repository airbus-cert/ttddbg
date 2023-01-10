#pragma once

#include <typeinf.hpp>

#include "../ttd-bindings/TTD/TTD.hpp"

namespace ttddbg {
	void* readCursorMemory(TTD::Cursor* cursor, size_t offset, size_t len);

	size_t stackArgOffset(tinfo_t func_tinfo, size_t n, size_t firstStackArgNum=0);
	DWORD32 x86_getIntArg(TTD::Cursor* cursor, tinfo_t tinfo, size_t n);
	DWORD64 x64_getIntArg(TTD::Cursor* cursor, tinfo_t tinfo, size_t n);
	M128A x64_getFloatArg(TTD::Cursor* cursor, tinfo_t tinfo, size_t n);
	qstring readStringAt(TTD::Cursor* cursor, size_t offset);
	qstring readWideStringAt(TTD::Cursor* cursor, size_t offset);
}