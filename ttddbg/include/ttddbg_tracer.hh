#pragma once

#include <vector>
#include <memory>
#include <functional>

#include <pro.h>
#include <funcs.hpp>

#include "../ttd-bindings/TTD/TTD.hpp"

namespace ttddbg {
	// FunctionInvocation represents a function call event,
	// recorded by the tracer.
	struct FunctionInvocation {
		func_t* func;
		TTD::Position pos;

		std::vector<qstring> args;
	};

	class FunctionTracer {
	public:
		static FunctionTracer* getInstance();
		static void destroy();

		void setCursor(std::shared_ptr<TTD::Cursor>);
		void setEngine(TTD::ReplayEngine);
		void traceFunction(func_t*);
		void removeTrace(size_t n);
		void removeEvent(size_t n);

		bool isTraced(func_t*);
		bool isEATraced(ea_t);

		void recordCall(FunctionInvocation);

		void setNewTraceCallback(std::function<void(func_t*)>);
		void setNewEventCallback(std::function<void(FunctionInvocation)>);

		// View into the list of traced functions
		size_t countTraced();
		func_t* funcAt(int i);

		// View into the list of events
		size_t countEvents();
		FunctionInvocation eventAt(int i);
		void copyArgumentAddress(size_t nevent, size_t narg);

	private:
		void sortEvents();

		FunctionTracer();
		static FunctionTracer* c_instance;
		
		std::shared_ptr<TTD::Cursor> m_cursor;
		TTD::ReplayEngine m_engine;
		std::vector<ea_t> m_traces;

		std::vector<FunctionInvocation> m_events;

		std::function<void(func_t*)> m_cbNewTrace;
		std::function<void(FunctionInvocation)> m_cbNewEvent;
	};
}