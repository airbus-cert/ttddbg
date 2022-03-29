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
#include "ttddbg_logger_ida.hh"
#include "ttddbg_debugger_manager.hh"

/**********************************************************************/
bool idaapi ttddbg::Plugin::run(size_t)
{
	return true;
}

/**********************************************************************/
static plugmod_t* idaapi ttddbg_init(void)
{
	auto logger = std::make_shared<ttddbg::IdaLogger>();
	try
	{
		auto manager = std::make_unique<ttddbg::DebuggerManager>(logger);
		dbg = new ttddbg::Debugger(logger, std::move(manager));
		return new ttddbg::Plugin();
	}
	catch (std::exception& e)
	{
		logger->error(e.what());
		return nullptr;
	}
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
