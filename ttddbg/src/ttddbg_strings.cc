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
		return tmpString;
	}

	/**********************************************************************/
	std::string Strings::find_module_name(const wchar_t* path)
	{
		std::filesystem::path modulePath(to_string(path));
		auto fileName = modulePath.filename();
		return fileName.string();
	}
}