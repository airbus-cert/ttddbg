#ifndef __TTDDBG_ILOGGER__
#define __TTDDBG_ILOGGER__

#include <string>
#include <sstream>
#include <ostream>

namespace ttddbg
{
	/*!
	 * \brief	SFINAE override to manage multiple type
	 *			Use to convert integer type into string
	 */
	template<typename T, typename std::enable_if<std::is_integral<T>::value, T>::type = 0>
	std::string to_string(T p)
	{
		std::stringstream ss;
		ss << p;
		return ss.str();
	}

	/*!
	 * \brief	SFINAE override to manage multiple type
	 *			Use to convert c++ std::string (of it's easy)
	 */
	template<typename T, typename std::enable_if<std::is_same_v<T, std::string>, T>::type = 0>
	std::string to_string(T p)
	{
		return p;
	}

	/*!
	 * \brief	SFINAE override to manage multiple type
	 *			Use to convert C char* string
	 */
	template<typename T, typename std::enable_if<std::is_same_v<T, const char*>, T>::type = 0>
	std::string to_string(T p)
	{
		return std::string(p);
	}

	/*!
	 * \brief	A base class for every logger 
	 */
	class Logger
	{
	private:
		/*!
		 * \brief	Format any message from logger implementation 
		 */
		template<typename... Params>
		void format(const std::string& level, const std::string message, Params... parameters)
		{
			std::stringstream ss;

			ss << "[ttddbg] " << level << " : ";
			for (auto& val : { message, to_string(parameters) ... })
			{
				ss << " " << val;
			}

			ss << std::endl;
			print(ss.str());
		}

		/*!
		 * \brief	Core implementation of the logger
		 *			Must be reimplemented by any subclasses
		 */
		virtual void print(const std::string& message) = 0;

	public:
		
		/*!
		 *	\brief	An error message
		 *			This will be prefixed with the ERROR keyword
		 *  \param	message main message
		 *  \param	parameters	convenient format parameters
		 */
		template<typename... Params>
		void error(const std::string& message, Params... parameters) {
			this->format("ERROR", message, parameters...);
		}

		/*!
		 * \brief	Write an informations log
		 *			This will prefix all message with the INFI prefix
		 */
		template<typename... Params>
		void info(const std::string message, Params... parameters) {
			this->format("INFO", message, parameters...);
		}
	};
}
#endif
