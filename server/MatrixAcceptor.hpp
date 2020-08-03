#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <iostream>
#pragma once

using namespace boost;
class MatrixAcceptor {
public:
	MatrixAcceptor(asio::io_service& ios, unsigned short port_num);
	// Start accepting incoming connection requests.
	void Start();
	// Stop accepting incoming connection requests.
	void Stop();
private:
	void InitAccept();
	//the function calls in the onAccept are non-blocking, and you can always accept new connection
	void onAccept(const boost::system::error_code& ec, std::shared_ptr<asio::ip::tcp::socket> sock);
private:
	asio::io_service& m_ios;
	asio::ip::tcp::acceptor m_acceptor;
	std::atomic<bool> m_isStopped;
};