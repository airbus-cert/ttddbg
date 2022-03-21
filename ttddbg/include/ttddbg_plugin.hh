#ifndef __TTDDBG_PLUGIN__
#define __TTDDBG_PLUGIN__

#include <idp.hpp>

namespace ttddbg 
{
	class Plugin : public plugmod_t
	{
	public:
		virtual ~Plugin() = default;

		virtual bool idaapi run(size_t) override;
	};
}

#endif