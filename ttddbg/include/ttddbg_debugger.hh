#ifndef __TTDDBG_DEBUGGER__
#define __TTDDBG_DEBUGGER__

#include <ida.hpp>
#include <idd.hpp>

namespace ttddbg 
{
	class Debugger : public debugger_t
	{
	public:
		explicit Debugger();
	};
}

#endif