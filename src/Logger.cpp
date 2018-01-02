#include "Logger.h"

void Logger::init(int debug, std::string name) {
	if (debug) {
		boost::log::add_console_log(
			std::cout,
			boost::log::keywords::format = "[%TimeStamp%] *%Severity%* %Message%",
			boost::log::keywords::auto_flush = true
			)->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
		boost::log::add_file_log(
			boost::log::keywords::file_name = name + "_Debug.log",
			boost::log::keywords::rotation_size = 1 * 1024 * 1024,
			boost::log::keywords::max_size = 20 * 1024 * 1024,
			boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
			boost::log::keywords::format = "[%TimeStamp%] *%Severity%* %Message%",
			boost::log::keywords::auto_flush = true
			);
	} else {
		boost::log::add_console_log(
			std::cout,
			boost::log::keywords::format = "[%TimeStamp%] *%Severity%* %Message%",
			boost::log::keywords::auto_flush = true
			)->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
	}
	boost::log::add_common_attributes();
	boost::log::core::get()->set_filter(
		boost::log::trivial::severity >= boost::log::trivial::trace
		);
}
