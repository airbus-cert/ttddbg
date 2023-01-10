#pragma once

#include <pro.h>
#include <kernwin.hpp>

namespace ttddbg {
	class TracerTraceChooser : public chooser_t {
	public:
		TracerTraceChooser();

		// Overrides
		size_t get_count() const override;
		void get_row(qstrvec_t* out, int* out_icon, chooser_item_attrs_t* out_attrs, size_t n) const override;
		//ea_t get_ea(size_t) const override;
		cbret_t del(size_t) override;
	};

	class TracerEventChooser : public chooser_t {
	public:
		TracerEventChooser();

		// Overrides
		size_t get_count() const override;
		void get_row(qstrvec_t* out, int* out_icon, chooser_item_attrs_t* out_attrs, size_t n) const override;
		ea_t get_ea(size_t n) const override;
		chooser_t::cbret_t del(size_t) override;
	};
}