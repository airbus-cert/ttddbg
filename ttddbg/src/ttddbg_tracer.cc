#include "ttddbg_tracer.hh"

#include <kernwin.hpp>
#include <pro.h>
#include <funcs.hpp>


namespace ttddbg {
	void callCallback(unsigned __int64 callback_value, TTD::GuestAddress addr_func, TTD::GuestAddress addr_ret, struct TTD::TTD_Replay_IThreadView* thread_view) {
		if (addr_ret == 0) {
			return;
		}
		
		if (FunctionTracer::getInstance()->isEATraced(addr_func)) {
			qstring func_name;
			get_func_name(&func_name, addr_func);
			func_t* func = get_func(addr_func);
			TTD::Position* position = thread_view->IThreadView->GetPosition(thread_view);

			msg("[tracer] called '%s' at 0x%X\n", func_name.c_str(), addr_ret);

			FunctionTracer::getInstance()->recordCall(func, *position);
		}
	}

	FunctionTracer* FunctionTracer::c_instance = nullptr;

	FunctionTracer::FunctionTracer() {

	}

	void FunctionTracer::destroy() {
		if (FunctionTracer::c_instance != nullptr) {
			FunctionTracer::getInstance()->m_cursor->SetCallReturnCallback(NULL, 0);
			delete FunctionTracer::c_instance;
			FunctionTracer::c_instance = NULL;
		}
	}

	void FunctionTracer::setNewTraceCallback(std::function<void(func_t*)> f) {
		m_cbNewTrace = f;
	}

	FunctionTracer* FunctionTracer::getInstance() {
		if (FunctionTracer::c_instance == nullptr) {
			FunctionTracer::c_instance = new FunctionTracer();
		}

		return FunctionTracer::c_instance;
	}

	void FunctionTracer::setCursor(std::shared_ptr<TTD::Cursor> cursor) {
		m_cursor = cursor;
		m_cursor->SetCallReturnCallback(&callCallback, 0);
	}

	void FunctionTracer::traceFunction(func_t *func) {
		if (isTraced(func))
			return;
		
		msg("[tracer] Tracing function at 0x%X\n", func->start_ea);
		m_traces.push_back(func->start_ea);

		if (m_cbNewTrace) {
			m_cbNewTrace(func);
		}
	}

	void FunctionTracer::removeTrace(size_t n) {
		m_traces.erase(m_traces.begin() + n);
	}

	bool FunctionTracer::isTraced(func_t* func) {
		return isEATraced(func->start_ea);
	}

	bool FunctionTracer::isEATraced(ea_t start) {
		for (int i = 0; i < m_traces.size(); i++) {
			ea_t ea = m_traces.at(i);
			if (ea == start) {
				return true;
			}
		}
		return false;
	}

	void FunctionTracer::recordCall(func_t* func, TTD::Position pos) {
		FunctionInvocation ev{ func, pos };
		m_events.push_back(ev);
	}

	/*****************************************************************/

	size_t FunctionTracer::countTraced() {
		return m_traces.size();
	}

	func_t* FunctionTracer::funcAt(int i) {
		ea_t ea = m_traces.at(i);
		return get_func(ea);
	}

	/*****************************************************************/

	size_t FunctionTracer::countEvents() {
		return m_events.size();
	}

	FunctionInvocation FunctionTracer::eventAt(int i) {
		return m_events.at(i);
	}
}