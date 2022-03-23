#ifndef __TTDDBG_IDALOG__
#define __TTDDBG_IDALOG__

#include "ttddbg_logger.hh"

namespace ttddbg
{
	/*!
	 * \brief	The IDA implementation of the ttddbg logger
	 */
	class IdaLogger : public Logger
	{
	public:
		/*!
		 * \brief	Core print for IDA
		 * \param	message	message to print on console
		 */
		void print(const std::string& message) override;
	};
}

#endif