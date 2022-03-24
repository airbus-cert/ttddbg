#ifndef __TTDDBG_STRINGS__
#define __TTDDBG_STRINGS__

#include <string>

namespace ttddbg 
{
	class Strings
	{
	public:
		static std::wstring to_wstring(const char* cStr);
		static std::string to_string(const wchar_t* cStr);
		static std::string find_module_name(const wchar_t* path);
	};
}

#endif