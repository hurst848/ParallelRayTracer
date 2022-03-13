#include "hurstLogger.h"
namespace hurst
{
	logger::logger()
	{
		path = "log.log";
		logStream = std::ofstream(path);
	}
	logger::logger(std::string filePath)
	{
		path = filePath;
		logStream = std::ofstream(filePath);
	}

	void logger::start(std::string filePath)
	{
		path = filePath;
		logStream = std::ofstream(filePath);
	}

	void logger::stop()
	{
		logStream.close();
	}

	void logger::log()
	{
		logStream << "/*********************** LOG ***********************\\" << std::endl;
	}
	void logger::log(std::string _log)
	{
		logStream << _log;
	}
	void logger::logLn(std::string _log)
	{
		logStream << _log << std::endl;
	}

}

