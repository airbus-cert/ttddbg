#ifndef USE_DANGEROUS_FUNCTIONS
#define USE_DANGEROUS_FUNCTIONS 1
#endif  // USE_DANGEROUS_FUNCTIONS

#ifndef USE_STANDARD_FILE_FUNCTIONS
#define USE_STANDARD_FILE_FUNCTIONS
#endif

#ifndef NO_OBSOLETE_FUNCS
#define NO_OBSOLETE_FUNCS
#endif

#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <dbg.hpp>

#include "ttddbg_debugger.hh"
#include "ttddbg_plugin.hh"

bool idaapi ttddbg::Plugin::run(size_t)
{
	return true;
}

static plugmod_t* idaapi ttddbg_init(void)
{
	dbg = new ttddbg::Debugger();
	return new ttddbg::Plugin();
}

/*!
 * \brief	This is an export present into loader.hpp of IDA SDK
 */
plugin_t PLUGIN =
{
	IDP_INTERFACE_VERSION,
	PLUGIN_DBG | PLUGIN_HIDE | PLUGIN_MULTI,
	ttddbg_init,
	nullptr,
	nullptr,
	"Time Travel Debugger",
	"Airbus CERT and Camille Mougey\n",
	"ttddbg",
	""
};
