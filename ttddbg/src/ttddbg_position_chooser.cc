#include "ttddbg_position_chooser.hh"
#include "ttddbg_debugger.hh"

#include <dbg.hpp>

namespace ttddbg {
	PositionChooser::PositionChooser()
		: chooser_t(CH_CAN_INS | CH_CAN_DEL, 2, nullptr, new char* [2]{"Name", "Position"}, "Timeline"), m_cursor(nullptr)
	{
		loadPositions();
	}

	PositionChooser::PositionChooser(std::shared_ptr<TTD::Cursor> cursor) : PositionChooser() {
		setCursor(cursor);
	}

	void PositionChooser::setCursor(std::shared_ptr<TTD::Cursor> cursor) {
		m_cursor = cursor;
	}

	void PositionChooser::addNewPosition(std::string name, TTD::Position pos) {
		std::pair<std::string, TTD::Position> new_pair;
		new_pair.first = name;
		new_pair.second = pos;
		m_positions.push_back(new_pair);

		savePositions();
	}

	bool PositionChooser::init() {
		return true;
	}

	size_t PositionChooser::get_count() const {
		return m_positions.size();
	}

	void PositionChooser::get_row(qstrvec_t* out, int* out_icon, chooser_item_attrs_t* out_attrs, size_t n) const {
		auto &entry = m_positions.at(n);

		out->at(0).sprnt(entry.first.c_str());
		out->at(1).sprnt("%d %d", entry.second.Major, entry.second.Minor);
	}

	ea_t PositionChooser::get_ea(size_t n) const {
		if (n >= m_positions.size()) {
			msg("[ttddbg] out-of-bounds for PositionChooser.get_ea(): asked for %d, max %d\n", n, m_positions.size());
			return BADADDR;
		}

		auto pos = m_positions.at(n);
		msg("[ttddbg] Moving to position \"%s\" at %d %d\n", pos.first.c_str(), pos.second.Major, pos.second.Minor);

		static_cast<ttddbg::Debugger*>(dbg)->getManager().setNextPosition(pos.second);
		continue_process();
		return BADADDR;
	}

	void PositionChooser::closed() {

	}

	chooser_t::cbret_t PositionChooser::ins(ssize_t n) {
		qstring res;
		bool ok = ask_str(&res, 0, "Name of the new position");

		if (!ok) {
			return NOTHING_CHANGED;
		}
		
		std::pair<std::string, TTD::Position> new_pair;
		addNewPosition(std::string(res.c_str()), *m_cursor.get()->GetPosition());

		return ALL_CHANGED;
	}

	chooser_t::cbret_t PositionChooser::del(size_t n) {
		if (n >= m_positions.size()) {
			msg("[ttddbg] out-of-bounds when del()eting pos: %d (max %d)\n", n, m_positions.size());
			return NOTHING_CHANGED;
		}

		std::ostringstream nodename;
		nodename << "$ttddbg." << m_positions.at(n).first;
		netnode node(nodename.str().c_str() , 0, false);
		node.kill();

		m_positions.erase(m_positions.begin() + n);
		return ALL_CHANGED;
	}

	void PositionChooser::savePositions() const {
		std::for_each(m_positions.begin(), m_positions.end(),
			[](std::pair<std::string, TTD::Position> pair) {
				std::string name = pair.first;
				TTD::Position pos = pair.second;

				std::ostringstream node_name;
				node_name << "$ttddbg." << name;

				netnode node(node_name.str().c_str(), 0, true);
				node.set(&pos, sizeof(TTD::Position));
			}
		);
	}

	void PositionChooser::loadPositions() {
		netnode n;
		qstring nodename;
		qstring posname;
		TTD::Position pos;
		
		for (bool ok = n.start(); ok; ok = n.next()) {
			ssize_t sz = n.get_name(&nodename);
			if (sz == -1 || sz <= 8) {
				continue;
			}

			if (nodename.substr(0, 8) != "$ttddbg.") {
				continue;
			}

			posname = nodename.substr(8, sz);
			if (n.valobj(&pos, sizeof(TTD::Position)) == -1) {
				continue;
			}

			msg("[ttddbg] loaded position \"%s\"\n", posname.c_str());

			m_positions.push_back(std::pair<std::string, TTD::Position>(std::string(posname.c_str()), pos));
		}
	}
}