#include "ttddbg_logger_ida.hh"
#include <idp.hpp>
#include <kernwin.hpp>

namespace ttddbg
{
	/**********************************************************************/
	void IdaLogger::print(const std::string& message)
	{
		msg(message.c_str());
	}
}