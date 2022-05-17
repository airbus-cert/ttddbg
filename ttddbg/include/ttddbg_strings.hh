#ifndef __TTDDBG_STRINGS__
#define __TTDDBG_STRINGS__

#include <string>

namespace ttddbg 
{
	/*!
	 * \brief	Sring toolbox
	 */
	class Strings
	{
	public:
		/*!
		 * \brief	Convert string from utf8 standard c++ into utf16 windows styme string
		 */
		static std::wstring to_wstring(const char* cStr);

		/*!
		 * \brief	Convert string from utf16 windows styme string into utf8 standard c++
		 */
		static std::string to_string(const wchar_t* cStr);

		/*!
		 * \brief	Extract the filename to isolate the module name
		 */
		static std::string find_module_name(const wchar_t* path);
	};
}

#endif