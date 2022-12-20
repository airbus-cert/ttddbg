#pragma once

#include <vector>
#include <memory>

#include <pro.h>
#include <funcs.hpp>

#include "../ttd-bindings/TTD/TTD.hpp"

namespace ttddbg {


	class FunctionTracer {
	public:
		static FunctionTracer* getInstance();

		void setCursor(std::shared_ptr<TTD::Cursor>);
		void traceFunction(func_t*);

		bool isTraced(func_t*);
		bool isEATraced(ea_t);

	private:
		FunctionTracer();
		static inline FunctionTracer* c_instance;
		
		std::shared_ptr<TTD::Cursor> m_cursor;
		std::vector<ea_t> m_traces;
	};
}