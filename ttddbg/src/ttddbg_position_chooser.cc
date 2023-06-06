#include "ttddbg_position_chooser.hh"
#include "ttddbg_debugger.hh"

#include <dbg.hpp>

namespace ttddbg {

	/**********************************************************************/
	PositionChooser::PositionChooser(std::shared_ptr<Logger> logger)
		: chooser_t(CH_CAN_INS | CH_CAN_DEL | CH_KEEP, 2, nullptr, new char* [2]{"Name", "Position"}, "Timeline"), m_cursor(nullptr), m_logger{ logger },m_isClosed(true)
	{
		loadPositions();
		this->icon = 185;
	}

	/**********************************************************************/
	PositionChooser::PositionChooser(std::shared_ptr<TTD::Cursor> cursor, std::shared_ptr<Logger> logger) : PositionChooser(logger) {
		setCursor(cursor);
	}

	/**********************************************************************/
	void PositionChooser::setCursor(std::shared_ptr<TTD::Cursor> cursor) {
		m_cursor = cursor;
	}

	/**********************************************************************/
	void PositionChooser::addNewPosition(std::string name, TTD::Position pos) {
		std::pair<std::string, TTD::Position> new_pair;
		new_pair.first = name;
		new_pair.second = pos;
		m_positions.push_back(new_pair);

		sortPositions();
		savePositions();
	}

	/**********************************************************************/
	bool PositionChooser::isClosed() {
		return m_isClosed;
	}

	/**********************************************************************/
	ssize_t PositionChooser::choose(ssize_t d) {
		if (isClosed()) {
			m_isClosed = false;
			return chooser_t::choose(d);
		}
		return 0;
	}

	/**********************************************************************/
	bool PositionChooser::init() {
		return true;
	}

	/**********************************************************************/
	size_t PositionChooser::get_count() const {
		return m_positions.size();
	}

	/**********************************************************************/
	void PositionChooser::get_row(qstrvec_t* out, int* out_icon, chooser_item_attrs_t* out_attrs, size_t n) const {
		auto &entry = m_positions.at(n);

		out->at(0).sprnt(entry.first.c_str());
		out->at(1).sprnt("0x%X:0x%X", entry.second.Major, entry.second.Minor);
	}

	/**********************************************************************/
	ea_t PositionChooser::get_ea(size_t n) const {
		if (n >= m_positions.size()) {
			m_logger->info("out-of-bounds for PositionChooser.get_ea(): asked for ", n, ", max", m_positions.size());
			return BADADDR;
		}

		auto pos = m_positions.at(n);
		m_logger->info("Moving to position ", pos.first.c_str(), " at ", pos.second.Major, ":", pos.second.Minor);
		static_cast<ttddbg::Debugger*>(dbg)->getManager().setNextPosition(pos.second);
		continue_process();
		return BADADDR;
	}

	/**********************************************************************/
	void PositionChooser::closed() {
		m_isClosed = true;
	}

	/**********************************************************************/
	chooser_t::cbret_t PositionChooser::ins(ssize_t n) {
		if (m_cursor == NULL) {
			warning("Cannot insert a new position when not in debug mode");
			return NOTHING_CHANGED;
		}

		qstring res;
		bool ok = ask_str(&res, 0, "Name of the new position");

		if (!ok) {
			return NOTHING_CHANGED;
		}
		
		std::pair<std::string, TTD::Position> new_pair;
		addNewPosition(std::string(res.c_str()), *m_cursor.get()->GetPosition());

		return ALL_CHANGED;
	}

	/**********************************************************************/
	chooser_t::cbret_t PositionChooser::del(size_t n) {
		if (n >= m_positions.size()) {
			m_logger->info("out-of-bounds when del()eting pos: ", n, "max ", m_positions.size());
			return NOTHING_CHANGED;
		}

		std::ostringstream nodename;
		nodename << "$ttddbg." << m_positions.at(n).first;
		netnode node(nodename.str().c_str() , 0, false);
		node.kill();

		m_positions.erase(m_positions.begin() + n);
		return ALL_CHANGED;
	}

	/**********************************************************************/
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

	/**********************************************************************/
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

			m_logger->info("loaded position ", posname.c_str());

			m_positions.push_back(std::pair<std::string, TTD::Position>(std::string(posname.c_str()), pos));
		}

		sortPositions();
	}

	/**********************************************************************/
	void PositionChooser::sortPositions() {
		sort(m_positions.begin(), m_positions.end(), [](auto p1, auto p2) -> bool {
			return (p1.second < p2.second);
		});
	}
}