#include "ttddbg_tracer.hh"

#include <kernwin.hpp>
#include <pro.h>
#include <funcs.hpp>


namespace ttddbg {
	void callCallback(unsigned __int64 callback_value, TTD::GuestAddress addr_func, TTD::GuestAddress addr_ret, struct TTD::TTD_Replay_IThreadView* thread_view) {
		if (FunctionTracer::getInstance()->isEATraced(addr_func)) {
			qstring func_name;
			get_func_name(&func_name, addr_func);

			msg("[tracer] called '%s' at 0x%X\n", func_name.c_str(), addr_ret);
		}
	}

	FunctionTracer::FunctionTracer() {

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
}