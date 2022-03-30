#include "ttddbg_event_deque.hh"
namespace ttddbg
{
	/**********************************************************************/
	void EventDeque::pushEvent(debug_event_t& newEvent)
	{
		std::lock_guard<std::mutex> guard(m_safeQueue);
		m_events.push_back(newEvent);
	}

	/**********************************************************************/
	debug_event_t EventDeque::popEvent()
	{
		std::lock_guard<std::mutex> guard(m_safeQueue);
		auto result = m_events.front();
		m_events.pop_front();
		return result;
	}

	/**********************************************************************/
	bool EventDeque::isEmpty()
	{
		std::lock_guard<std::mutex> guard(m_safeQueue);
		return m_events.empty();
	}

	/**********************************************************************/
	void EventDeque::addProcessStartEvent(pid_t processId, tid_t threadId, std::string& moduleName, ea_t base, ea_t rebase_to, asize_t moduleSize)
	{
		debug_event_t event;
		event.set_eid(event_id_t::PROCESS_STARTED);
		event.pid = processId;
		event.tid = threadId;
		event.ea = base;
		event.modinfo().base = base;
		event.modinfo().rebase_to = rebase_to;
		event.modinfo().name = moduleName.c_str();
		event.modinfo().size = moduleSize;
		event.handled = false;
		this->pushEvent(event);
	}

	/**********************************************************************/
	void EventDeque::addThreadStartEvent(pid_t processId, tid_t threadId)
	{
		debug_event_t event;
		event.set_eid(event_id_t::THREAD_STARTED);
		event.pid = processId;
		event.tid = threadId;
		event.ea = BADADDR;
		event.handled = false;
		this->pushEvent(event);
	}

	/**********************************************************************/
	void EventDeque::addThreadExitEvent(pid_t processId, tid_t threadId)
	{
		debug_event_t event;
		event.set_eid(event_id_t::THREAD_EXITED);
		event.pid = processId;
		event.tid = threadId;
		event.ea = BADADDR;
		event.handled = false;
		this->pushEvent(event);
	}

	/**********************************************************************/
	void EventDeque::addBreakPointEvent(pid_t processId, tid_t threadId, ea_t bpk)
	{
		debug_event_t event;
		event.set_eid(event_id_t::BREAKPOINT);
		event.pid = processId;
		event.tid = threadId;
		event.ea = bpk;
		event.handled = true;
		this->pushEvent(event);
	}

	/**********************************************************************/
	void EventDeque::addLibLoadEvent(std::string& moduleName, ea_t base, ea_t rebase_to, asize_t moduleSize)
	{
		debug_event_t event;
		event.set_eid(event_id_t::LIB_LOADED);
		event.ea = base;
		event.handled = false;
		event.modinfo().base = base;
		event.modinfo().rebase_to = rebase_to;
		event.modinfo().name = moduleName.c_str();
		event.modinfo().size = moduleSize;
		this->pushEvent(event);
	}

	/**********************************************************************/
	void EventDeque::addLibUnloadEvent(std::string& moduleName, ea_t base, asize_t moduleSize)
	{
		debug_event_t event;
		event.set_eid(event_id_t::LIB_UNLOADED);
		event.ea = base;
		event.handled = false;
		event.modinfo().base = base;
		event.modinfo().rebase_to = BADADDR;
		event.modinfo().name = moduleName.c_str();
		event.modinfo().size = moduleSize;
		this->pushEvent(event);
	}

	/**********************************************************************/
	void EventDeque::addProcessExitEvent(pid_t processId)
	{
		debug_event_t event;
		event.set_eid(event_id_t::PROCESS_EXITED);
		event.pid = processId;
		event.ea = BADADDR;
		event.handled = false;
		this->pushEvent(event);
	}

	/**********************************************************************/
	void EventDeque::addStepEvent(pid_t processId, tid_t threadId)
	{
		debug_event_t event;
		event.set_eid(event_id_t::STEP);
		event.pid = processId;
		event.tid = threadId;
		event.ea = BADADDR;
		event.handled = true;
		this->pushEvent(event);
	}
}