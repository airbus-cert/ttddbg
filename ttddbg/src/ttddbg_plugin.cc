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
#include "ttddbg_debugger_x86.hh"
#include "ttddbg_debugger_x86_64.hh"
#include "single_step_icon.hh"
#include "resume_backwards_icon.hh"
#include "ttddbg_hooks.hh"

/**********************************************************************/
ttddbg::Plugin::Plugin() :
	m_backwardActionDesc(ACTION_DESC_LITERAL_PLUGMOD(
		BackwardStateRequest::actionName,
		BackwardStateRequest::actionLabel,
		&m_backwardAction,
		this,
		BackwardStateRequest::actionHotkey,
		nullptr,
		load_custom_icon(resumebackwards_png, resumebackwards_png_length, "PNG")
	)),
	m_fullRunActionDesc(ACTION_DESC_LITERAL_PLUGMOD(
		FullRunActionRequest::actionName,
		FullRunActionRequest::actionLabel,
		&m_fullRunAction,
		this,
		FullRunActionRequest::actionHotkey,
		nullptr,
		203
	)),
	m_positionChooserActionDesc(ACTION_DESC_LITERAL_PLUGMOD(
		OpenPositionChooserAction::actionName,
		OpenPositionChooserAction::actionLabel,
		&m_positionChooserAction,
		this,
		OpenPositionChooserAction::actionHotkey,
		nullptr,
		185			// timeline Icon
	)),
	m_backwardSingleActionDesc(ACTION_DESC_LITERAL_PLUGMOD(
		BackwardSingleStepRequest::actionName,
		BackwardSingleStepRequest::actionLabel,
		&m_backwardSingleAction,
		this,
		BackwardSingleStepRequest::actionHotkey,
		nullptr,
		load_custom_icon(singlestep_png, singlestep_png_length, "PNG")
	)),
	m_traceChooserActionDesc(ACTION_DESC_LITERAL_PLUGMOD(
		OpenTraceChooserAction::actionName,
		OpenTraceChooserAction::actionLabel,
		&m_traceChooserAction,
		this,
		OpenTraceChooserAction::actionHotkey,
		nullptr,
		128
	)),
	m_traceEventChooserActionDesc(ACTION_DESC_LITERAL_PLUGMOD(
		OpenTraceEventChooserAction::actionName,
		OpenTraceEventChooserAction::actionLabel,
		&m_traceEventChooserAction,
		this,
		OpenTraceEventChooserAction::actionHotkey,
		nullptr,
		73
	)),
	m_gotoPositionActionDesc(ACTION_DESC_LITERAL_PLUGMOD(
		GotoPositionAction::actionName,
		GotoPositionAction::actionLabel,
		&m_gotoPositionAction,
		this,
		GotoPositionAction::actionHotkey,
		nullptr,
		-1
	))
{
	register_action(m_backwardActionDesc);
	register_action(m_fullRunActionDesc);
	register_action(m_positionChooserActionDesc);
	register_action(m_backwardSingleActionDesc);
	register_action(m_traceChooserActionDesc);
	register_action(m_traceEventChooserActionDesc);
	register_action(m_gotoPositionActionDesc);

	hooks.registerHooks();
	
	//showActions();
}

/**********************************************************************/
ttddbg::Plugin::~Plugin()
{
	unregister_action(m_backwardAction.actionName);
	unregister_action(m_fullRunAction.actionName);
	unregister_action(m_positionChooserAction.actionName);
	unregister_action(m_backwardSingleAction.actionName);
	unregister_action(m_traceChooserAction.actionName);
	unregister_action(m_traceEventChooserAction.actionName);
	unregister_action(m_gotoPositionAction.actionName);

	hooks.unregisterHooks();
}

void ttddbg::Plugin::showActions()
{
	attach_action_to_toolbar("DebugToolBar", m_backwardActionDesc.name);
	attach_action_to_toolbar("DebugToolBar", m_fullRunActionDesc.name);
	attach_action_to_toolbar("DebugToolBar", m_backwardSingleActionDesc.name);
	attach_action_to_toolbar("DebugToolBar", m_positionChooserActionDesc.name);
	attach_action_to_toolbar("DebugToolBar", m_traceChooserActionDesc.name);
	attach_action_to_toolbar("DebugToolBar", m_traceEventChooserActionDesc.name);
}

void ttddbg::Plugin::hideActions()
{
	detach_action_from_toolbar("DebugToolBar", m_backwardActionDesc.name);
	detach_action_from_toolbar("DebugToolBar", m_fullRunActionDesc.name);
	detach_action_from_toolbar("DebugToolBar", m_backwardSingleActionDesc.name);
	detach_action_from_toolbar("DebugToolBar", m_positionChooserActionDesc.name);
	detach_action_from_toolbar("DebugToolBar", m_traceChooserActionDesc.name);
	detach_action_from_toolbar("DebugToolBar", m_traceEventChooserActionDesc.name);
}

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
		auto plg = new ttddbg::Plugin();
		if (inf_is_64bit())
		{
			dbg = new ttddbg::DebuggerX86_64(logger, plg);
		}
		else
		{
			dbg = new ttddbg::DebuggerX86(logger, plg);
		}
		return plg;
	}
	catch (std::exception& e)
	{
		logger->error(e.what());
		return nullptr;
	}
}

/**********************************************************************/
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
	"Airbus CERT\n",
	"ttddbg",
	"F3"
};
