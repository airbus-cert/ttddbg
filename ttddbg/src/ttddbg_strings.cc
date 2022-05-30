#include "ttddbg_strings.hh"
#include <Windows.h>
#include <iostream>
#include <filesystem>

namespace ttddbg
{
	/**********************************************************************/
	std::wstring Strings::to_wstring(const char* cStr)
	{
		auto stringSize = strnlen_s(cStr, MAX_PATH);
		std::wstring ws(L"", stringSize + 1);
		size_t tmp;
		mbstowcs_s(&tmp, &ws[0], stringSize + 1, cStr, stringSize);
		return ws;
	}

	/**********************************************************************/
	std::string Strings::to_string(const wchar_t* cStr)
	{
		auto stringSize = wcsnlen_s(cStr, MAX_PATH);
		std::string tmpString("", stringSize + 1);
		size_t tmp;
		wcstombs_s(&tmp, &tmpString[0], stringSize + 1, cStr, stringSize);
		// remove las \0 char
		tmpString.resize(stringSize);
		return tmpString;
	}

	/**********************************************************************/
	std::string Strings::find_module_name(const wchar_t* path)
	{
		std::filesystem::path modulePath(to_string(path));
		auto fileName = modulePath.filename();
		return fileName.string();
	}

	/**********************************************************************/
	std::string Strings::exception_name(uint32_t exceptionCode)
	{
		switch (exceptionCode)
		{
		case ('msc' | 0xE0000000): // c++ exception
			return "c++";
		case EXCEPTION_ACCESS_VIOLATION:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_BREAKPOINT:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_SINGLE_STEP:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_FLT_INEXACT_RESULT:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_FLT_INVALID_OPERATION:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_FLT_OVERFLOW:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_FLT_STACK_CHECK:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_FLT_UNDERFLOW:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_INT_OVERFLOW:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_PRIV_INSTRUCTION:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_IN_PAGE_ERROR:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_STACK_OVERFLOW:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_INVALID_DISPOSITION:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_GUARD_PAGE:
			return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_INVALID_HANDLE:
			return "EXCEPTION_ACCESS_VIOLATION";
		default:
			return "unknown exception code";
		}
	}
}