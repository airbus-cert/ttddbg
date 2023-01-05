#include "ttddbg_tracer_choosers.hh"
#include "ttddbg_tracer.hh"
#include "ttddbg_debugger.hh"

namespace ttddbg {
	// Function trace list
	TracerTraceChooser::TracerTraceChooser()
		: chooser_t(CH_CAN_DEL | CH_KEEP, 2, nullptr, new char* [2] {"Function name", "Offset"}, "Traced functions")
	{}

	size_t TracerTraceChooser::get_count() const {
		return FunctionTracer::getInstance()->countTraced();
	}

	void TracerTraceChooser::get_row(qstrvec_t* out, int* out_icon, chooser_item_attrs_t* out_attrs, size_t n) const {
		func_t* func = FunctionTracer::getInstance()->funcAt(n);
		qstring fname;
		get_func_name(&fname, func->start_ea);

		out->at(0).sprnt(fname.c_str());
		out->at(1).sprnt("0x%X", func->start_ea);
	}

	chooser_t::cbret_t TracerTraceChooser::del(size_t n) {
		FunctionTracer::getInstance()->removeTrace(n);

		return ALL_CHANGED;
	}
}