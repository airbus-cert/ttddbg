#ifndef __TTDDBG_PLUGIN__
#define __TTDDBG_PLUGIN__

#include <ida.hpp>
#include <idp.hpp>
#include "ttddbg_action.hh"

namespace ttddbg 
{
	

	class Plugin : public plugmod_t
	{
	protected:
		BackwardStateRequest m_backwardAction;

		const action_desc_t m_backwardActionDesc = ACTION_DESC_LITERAL_PLUGMOD(
			BackwardStateRequest::actionName,
			BackwardStateRequest::actionLabel,
			&m_backwardAction,
			this,
			nullptr,
			nullptr,
			-1
		);

	public:

		explicit Plugin();

		virtual ~Plugin();

		virtual bool idaapi run(size_t) override;
	};
}

#endif