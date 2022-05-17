#ifndef __TTDDBG_EVENT_DEQUE__
#define __TTDDBG_EVENT_DEQUE__

#include <mutex>
#include <deque>
#include <ida.hpp>
#include <idd.hpp>

namespace ttddbg 
{
	/*!
	 * \brief	A thread safe queue use to manage Debugger events
	 */
	class EventDeque
	{
	protected:
		/*!
		 * \brief	Current event list queue
		 */
		std::deque<debug_event_t> m_events;

		/*!
		 * \brief	Insert and remove mutex
		 *			The thread safe part
		 */
		std::mutex m_safeQueue;

	public:

		/*!
		 * \brief	Push a new event into the queue
		 * \param	newEvent	new event to push
		 */
		void pushEvent(debug_event_t& newEvent);

		/*!
		 * \brief	Pop a pending event
		 *			Be sure you checked with the isEmpty function
		 */
		debug_event_t popEvent();

		/*!
		 * \brief	Check if the event queue is empty
		 * \return	true if empty
		 */
		bool isEmpty();

		/*!
		 * \brief	Create a Process start event and must be the first in the queue
		 * \param	processId	process Id
		 * \param	threadId	Thread id is the first thread created by the process
		 * \param	moduleName	name of the exe module (the main one)
		 * \param	base		base of the main module
		 * \param	rebase_to	if the main module is one loaded into IDA we need to ask for rebase
		 * \param	moduleSize	Size of the main module
		 */
		void addProcessStartEvent(pid_t processId, tid_t threadId, std::string& moduleName, ea_t base, ea_t rebase_to, asize_t moduleSize);
		
		/*!
		 * \brief	New thread star
		 * \param	processId	Process ID
		 * \param	threadId	Id of the new thread
		 */
		void addThreadStartEvent(pid_t processId, tid_t threadId);

		/*!
		 * \brief	Enfd of a particular thread
		 * \param	processId Process ID
		 * \param	threadId	Id of the exited one
		 */
		void addThreadExitEvent(pid_t processId, tid_t threadId);

		/*!
		 * \brief	Inform the debugger that a debugger breakpoint was reached
		 * \param	processID	process ID Id of the current debug process
		 * \param	threadId	thread id where breakpoint is reached
		 * \param	bpk			breakpoint infos
		 */
		void addBreakPointEvent(pid_t processId, tid_t threadId, ea_t bpk);

		/*!
		 * \brief	Inform debuger that a newly module was loaded
		 * \param	base	base of the newly module
		 * \param	rebase_to	if the module is the one loaded into IDA, you must rebase the database
		 * \param	moduleSize	size of the newly module
		 */
		void addLibLoadEvent(std::string& moduleName, ea_t base, ea_t rebase_to, asize_t moduleSize);

		/*!
		 * \brief	Inform the debugger of the unload of a particular moduler
		 * \param	moduleName	name of the unloaded module
		 * \param	base		base of the unloaded module
		 */
		void addLibUnloadEvent(std::string& moduleName, ea_t base);

		/*!
		 * \brief	Inform the debugger of the end of the current debugged process
		 * \param	processID	process ID
		 */
		void addProcessExitEvent(pid_t processId);

		/*!
		 * \brief	Inform debugger of the single step debug event
		 * \param	processId	id of the process
		 * \param	threadId	Id of the main thread
		 */
		void addStepEvent(pid_t processId, tid_t threadId);
	};
}

#endif