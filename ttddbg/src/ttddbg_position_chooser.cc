#include "ttddbg_position_chooser.hh"

namespace ttddbg {
	PositionChooser::PositionChooser(std::shared_ptr<TTD::Cursor> cursor)
		: chooser_t(0, 2, nullptr, new const char* [2]{"Name", "Position"}, "Position manager"), m_cursor(cursor)
	{
	}

	void PositionChooser::setCursor(std::shared_ptr<TTD::Cursor> cursor) {
		m_cursor = cursor;
	}

	size_t PositionChooser::get_count() const {
		msg("[ttddbg] get_count() = %d\n", m_positions.size());
		return m_positions.size();
	}

	void PositionChooser::get_row(qstrvec_t* out, int* out_icon, chooser_item_attrs_t* out_attrs, size_t n) const {
		msg("[ttddbg] get_count(%d)\n", n);

		auto &entry = m_positions.at(n);

		out->at(0).sprnt(entry.first.c_str());
		out->at(1).sprnt("%d %d", entry.second.Major, entry.second.Minor);
	}

	ea_t PositionChooser::get_ea(size_t n) const {
		return BADADDR;
	}

	chooser_t::cbret_t PositionChooser::ins(ssize_t n) {
		msg("[ttddbg] PositionChooser::ins()!\n");
		return NOTHING_CHANGED;
	}

	chooser_t::cbret_t PositionChooser::del(size_t n) {
		if (n >= m_positions.size()) {
			msg("[ttddbg] out-of-bounds when del()eting pos: %d (max %d)\n", n, m_positions.size());
			return NOTHING_CHANGED;
		}

		m_positions.erase(m_positions.begin() + n);
		return ALL_CHANGED;
	}
}