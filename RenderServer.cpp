#include "RenderServer.h"
#include <thread>
#include <sstream>
#include <chrono>


void RenderServer::StartServer(std::string _data, std::shared_ptr<JobController> _controller)
{
	RunServer(std::make_shared<std::string>(_data), _controller);
}

void RenderServer::RunServer(std::shared_ptr<std::string> _serializedData, std::shared_ptr<JobController> _controller)
{

	try
	{
		tcpServer server(context, *_serializedData, _controller);
		context.run();
	}
	catch (std::exception& e)
	{

	}


}

std::vector<NetworkResult> RenderServer::interpretResults(std::string _data)
{
	std::vector<NetworkResult> rtrn;

	std::istringstream f(_data);
	std::string line;

	NetworkResult r;
	int itr = 0;
	while (std::getline(f, line))
	{
		if (line.find("//// JOB_RESULT ////") != std::string::npos)
		{
			r = NetworkResult(); 
			itr++;
		}
		if (itr == 1)
		{
			r.coords = readIvec2(line);
			itr++;
		}
		else  if (itr == 2)
		{
			r.colours = readVec3(line);
			itr = 0;
			rtrn.push_back(r);
		}
		else
		{
			std::cout << "ITR OUT OF RANGE" << std::endl;
		}
	}
	return rtrn;
}

glm::ivec2 RenderServer::readIvec2(std::string _input)
{
	glm::ivec2 rtrn = glm::ivec2(0, 0);
	int length = _input.find(",");

	std::string x = _input.substr(0, length);
	rtrn.x = std::stoi(x);

	std::string y = _input.substr(length + (size_t)2);
	rtrn.y = std::stoi(y);

	return rtrn;
}

glm::vec3 RenderServer::readVec3(std::string _input)
{
	glm::vec3 rtrn = glm::vec3(0.0f, 0.0f, 0.0f);
	int length = _input.find(",");

	std::string x = _input.substr(0, _input.find(","));
	rtrn.x = std::stof(x);

	std::string y = _input.substr(length + (size_t)2, length);
	rtrn.y = std::stof(y);

	std::string z = _input.substr(_input.find_last_of(",") + 1);
	rtrn.z = std::stof(z);

	return rtrn;
}

std::string RenderServer::serializeJob(std::shared_ptr<Job> j)
{
	std::string rtrn = "//// RENDER JOB ////\n";
	for (int i = 0; i < j->jobCoords.size(); i++)
	{
		rtrn += std::to_string(j->jobCoords.at(i).x) + ", " + std::to_string(j->jobCoords.at(i).y) + "\n";
	}
	return rtrn;
}

std::string RenderServer::readSocket(asio::ip::tcp::socket& _socket)
{
	asio::error_code ec;
	asio::streambuf buffer;
	asio::read_until(_socket, buffer, "\\\\END", ec);
	if (ec)
	{
		std::cout << "FAILED TO READ MESSAGE: " << ec.message() << std::endl;
	}
	std::string rtrn = asio::buffer_cast<const char*>(buffer.data());
	return rtrn;
}
void RenderServer::writeSocket(asio::ip::tcp::socket& _socket, const std::string& _message)
{
	asio::error_code ec;
	const std::string msg = _message + " \\\\END";
	asio::write(_socket, asio::buffer(msg), ec);
	if (ec)
	{
		std::cout << "FAILED TO SEND MESSAGE: " << ec.message() << std::endl;
	}
}



