#pragma once

#include <vector>
#include <functional>

#include <pro.h>
#include <kernwin.hpp>

#include <Windows.h>
#include "ttddbg_logger.hh"
#include "../../ttd-bindings/TTD/TTD.hpp"

namespace ttddbg {
	class PositionChooser : public chooser_t {
	public:
		PositionChooser(std::shared_ptr<Logger> logger);
		PositionChooser(std::shared_ptr<TTD::Cursor>, std::shared_ptr<Logger> logger);

		void setCursor(std::shared_ptr<TTD::Cursor>);
		void addNewPosition(std::string name, TTD::Position pos);

		bool isClosed();
		
		// Overrides
		bool init() override;
		size_t get_count() const override;
		void get_row(qstrvec_t* out, int* out_icon, chooser_item_attrs_t* out_attrs, size_t n) const override;
		ea_t get_ea(size_t) const override;
		void closed() override;
		cbret_t ins(ssize_t) override;
		cbret_t del(size_t) override;
		ssize_t choose(ssize_t = 0);

	private:
		void savePositions() const;
		void loadPositions();
		void sortPositions();

		/* List of all positions saved by the user. The pair is <position name, TTD position struct> */
		std::vector<std::pair<std::string, TTD::Position>> m_positions;

		/* A pointer to the current TTD::Cursor used by the debugger. Used when adding a new position to get the current position. */
		std::shared_ptr<TTD::Cursor> m_cursor;

		/* A pointer to the shared logger object. */
		std::shared_ptr<Logger> m_logger;

		/* This boolean holds the internal state of the Chooser: is it visible or not. Used to prevent calling choose() twice. */
		bool m_isClosed;
	};
}