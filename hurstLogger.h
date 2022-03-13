#include <iostream>
#include <fstream>

namespace hurst 
{
	struct logger
	{
	public:
		logger();
		logger(std::string filePath);

		void start(std::string filePath);
		void stop();

		void log();
		void log(std::string _log);
		void logLn(std::string _log);

	private:
		std::ofstream logStream;
		std::string path;
		
	};
}
