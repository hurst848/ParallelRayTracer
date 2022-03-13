#define ASIO_STANDALONE

#include "MutexRenderer.h"

#include <asio.hpp>

#include "include/glm/ext.hpp"
#include <asio/placeholders.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <string>

struct JobController;
struct Job;

struct NetworkResult
{
public:
	glm::ivec2 coords;
	glm::vec3 colours;
};

struct RenderServer
{
public:
	void StartServer(std::string _data, std::shared_ptr<JobController> _controller);
	void RunServer(std::shared_ptr<std::string> _serializedData, std::shared_ptr<JobController> _controller);
	std::string readSocket(asio::ip::tcp::socket &_socket);
	void writeSocket(asio::ip::tcp::socket& _socket, const std::string & _message);
	std::string serializeJob(std::shared_ptr<Job> j);
	std::vector<NetworkResult> interpretResults(std::string _data);
	glm::ivec2 readIvec2(std::string _input);
	glm::vec3 readVec3(std::string _input);
private:
	asio::io_context context;
};

struct ConnectionHandler : std::enable_shared_from_this<ConnectionHandler>
{
public:
	typedef std::shared_ptr<ConnectionHandler> pointer;

	static pointer create(asio::io_service& _context, std::string _data, std::shared_ptr<JobController> _controller)
	{
		return pointer(new ConnectionHandler(_context, _data, _controller));
	}

	asio::ip::tcp::socket& socket()
	{
		return _socket;
	}

	void Start()
	{
		asio::async_read
		(
			_socket, asio::buffer(message), "\\\\END",
			bind
			(
				&ConnectionHandler::handle_read, shared_from_this(),
				std::placeholders::_1,	std::placeholders::_2
			)
		);
		asio::async_write
		(
			_socket,asio::buffer(message),
			bind
			(
				&ConnectionHandler::handle_write, shared_from_this(),
				std::placeholders::_1,	std::placeholders::_2
			)
		);
	}


private:
	ConnectionHandler(asio::io_service& _context, std::string _data, std::shared_ptr<JobController> _controller) : 
		_socket(_context),	scene(_data),	controller(_controller)
	{}

	void handle_read(const asio::error_code& err/*error*/,	size_t bt/*bytes_transferred*/)
	{
		if (!err)
		{
			RenderServer serv;
			std::string clientMsg = serv.readSocket(_socket);
			if (clientMsg.find("WELCOME") != std::string::npos)
			{
				// Get ip out of message
				std::cout << clientMsg << std::endl;
				clientAddress = _socket.remote_endpoint().address().to_string();
				std::cout << clientAddress << " Saved!" << std::endl << std::endl;
				// send serialized scene data
				std::cout << "Sending " + clientAddress + " Scene Data" << std::endl << std::endl;
				serv.writeSocket(_socket, scene);
			}
			else if (clientMsg.find("REQUEST_JOB") != std::string::npos)
			{
				std::cout << "JOB REQUEST RECIVED\n\n";
				if (controller->Lock(0))
				{
					std::shared_ptr<Job> j = std::make_shared<Job>(controller->RequestJob());
					controller->Unlock();
					if (j->jobCoords.size() != 0)
					{
						std::string jobMsg = serv.serializeJob(j);
						std::cout << "JOB REQUESTED FROM (" << clientAddress << ")\n" << jobMsg;
						serv.writeSocket(_socket, jobMsg);
					}
					else 
					{
						std::cout << "Job requested does not contain any data, assuming completion\n\n";
					}
				}
			}
			else if (clientMsg.find("JOB_RESULT") != std::string::npos)
			{
				std::vector<NetworkResult> results = serv.interpretResults(clientMsg);
				// read results and update frame
				if (controller->Lock(0))
				{
					for (int i = 0; i < results.size(); i++)
					{
						controller->PushPixelData(results.at(i).colours, results.at(i).coords);
					}
					serv.writeSocket(_socket, "//// RECIVED ////\n\\\\END");
					controller->Unlock();
				}
			}
			else
			{
				std::cout << "Bad Request from " << clientAddress << ":\n" << clientMsg << std::endl << std::endl;
			}
		}
		else
		{
			std::cout << "ERROR READING: " << err.message() << std::endl << std::endl;
		}
	}

	void handle_write(const asio::error_code& err/*error*/, size_t bt/*bytes_transferred*/)
	{
		if (!err)
		{

		}
		else
		{
			std::cout << "ERROR WRITING: " << err.message() << std::endl << std::endl;
		}
	}

	asio::ip::tcp::socket _socket;
	std::string message;
	std::string scene;
	std::shared_ptr<JobController> controller;
	std::string clientAddress;

};

struct tcpServer
{
public: 
	tcpServer(asio::io_context& _context, std::string _data, std::shared_ptr<JobController> _controller) :
		context(_context), acceptor(context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 1134)),
		data(_data), controller(_controller)
	{ StartAccept(); }

private:

	void StartAccept()
	{
		ConnectionHandler::pointer newConnection = ConnectionHandler::create(context, data, controller);
		acceptor.async_accept
		(
			newConnection->socket(),
			bind
			(
				&tcpServer::handle_accept,	this,
				newConnection,	std::placeholders::_1
			)
		);
	}

	void handle_accept(ConnectionHandler::pointer new_connection, const asio::error_code& error)
	{
		if (!error)	{ new_connection->Start(); }
		StartAccept();
	}

	asio::io_context& context;
	asio::ip::tcp::acceptor acceptor;
	std::string data;
	std::shared_ptr<JobController> controller;
};