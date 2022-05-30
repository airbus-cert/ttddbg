#ifndef __TTDDBG_PLUGIN__
#define __TTDDBG_PLUGIN__

#include <ida.hpp>
#include <idp.hpp>
#include "ttddbg_action.hh"
#include "ttddbg_position_chooser.hh"
#include "single_step_icon.hh"
#include "resume_backwards_icon.hh"

namespace ttddbg 
{
	

	class Plugin : public plugmod_t
	{
	protected:
		/*!
		 * \brief	Backward action declaration
		 *			It's the continue in reverse way
		 *			Use by IDA to know how to print button
		 */
		BackwardStateRequest m_backwardAction;
		const action_desc_t m_backwardActionDesc = ACTION_DESC_LITERAL_PLUGMOD(
			BackwardStateRequest::actionName,
			BackwardStateRequest::actionLabel,
			&m_backwardAction,
			this,
			nullptr,
			nullptr,
			load_custom_icon(resumebackwards_png, resumebackwards_png_length, "PNG")
		);


		/*!
		 * \brief	Show the timeline GUI
		 */
		OpenPositionChooserAction m_positionChooserAction;
		const action_desc_t m_positionChooserActionDesc = ACTION_DESC_LITERAL_PLUGMOD(
			OpenPositionChooserAction::actionName,
			OpenPositionChooserAction::actionLabel,
			&m_positionChooserAction,
			this,
			nullptr,
			nullptr,
			185			// timeline Icon
		);

		/*!
		 * \brief	single instruction pointer decrement
		 */
		BackwardSingleStepRequest m_backwardSingleAction;
		const action_desc_t m_backwardSingleActionDesc = ACTION_DESC_LITERAL_PLUGMOD(
			BackwardSingleStepRequest::actionName,
			BackwardSingleStepRequest::actionLabel,
			&m_backwardSingleAction,
			this,
			nullptr,
			nullptr,
			load_custom_icon(singlestep_png, singlestep_png_length, "PNG")
		);

	public:
		/*!
		 * \brief	Constructoe
		 */
		explicit Plugin();

		/*!
		 * \brief	virtual destructor
		 */
		virtual ~Plugin();

		/*!
		 * \brief main plugin function
		 */
		virtual bool idaapi run(size_t) override;
	};
}

#endif