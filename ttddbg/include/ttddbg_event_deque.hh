#ifndef __TTDDBG_EVENT_DEQUE__
#define __TTDDBG_EVENT_DEQUE__

#include <mutex>
#include <deque>
#include <ida.hpp>
#include <idd.hpp>

namespace ttddbg 
{
	class EventDeque
	{
	protected:
		std::deque<debug_event_t> m_events;
		std::mutex m_safeQueue;

	public:

		void pushEvent(debug_event_t& newEvent);
		debug_event_t popEvent();

		bool isEmpty();

		/*!
		 * Thread id is the first thread created by the process
		 */
		void addProcessStartEvent(pid_t processId, tid_t threadId, std::string& moduleName, ea_t base, ea_t rebase_to, asize_t moduleSize);
		void addThreadStartEvent(pid_t processId, tid_t threadId);
		void addThreadExitEvent(pid_t processId, tid_t threadId);
		void addBreakPointEvent(pid_t processId, tid_t threadId, ea_t bpk);
		void addLibLoadEvent(std::string& moduleName, ea_t base, ea_t rebase_to, asize_t moduleSize);
		void addLibUnloadEvent(std::string& moduleName, ea_t base, asize_t moduleSize);
		void addProcessExitEvent(pid_t processId);
		void addStepEvent(pid_t processId, tid_t threadId);
	};
}

#endif