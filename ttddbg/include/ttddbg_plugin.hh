#ifndef __TTDDBG_PLUGIN__
#define __TTDDBG_PLUGIN__

#include <ida.hpp>
#include <idp.hpp>
#include "ttddbg_action.hh"
#include "ttddbg_position_chooser.hh"
#include "ttddbg_hooks.hh"

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
		const action_desc_t m_backwardActionDesc;


		/*!
		 * \brief	Show the timeline GUI
		 */
		OpenPositionChooserAction m_positionChooserAction;
		const action_desc_t m_positionChooserActionDesc;

		/*!
		 * \brief	single instruction pointer decrement
		 */
		BackwardSingleStepRequest m_backwardSingleAction;
		const action_desc_t m_backwardSingleActionDesc;

		/*!
		* \brief	Show the traced function GUI
		*/
		OpenTraceChooserAction m_traceChooserAction;
		const action_desc_t m_traceChooserActionDesc;

		/*!
		* \brief	Hooks manager (used to add action to context menus)
		*/
		Hooks hooks;

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

		/*!
		* \brief	add the actions to the Debug toolbar
		*/
		virtual void showActions();

		/*!
		* \brief	removes the actions from the Debug toolbar
		*/
		virtual void hideActions();
	};
}

#endif