#include "server/MatrixAcceptor.hpp"
#include "server/MatrixService.hpp"
#include <atomic>
#include <memory>
#include <iostream>
using namespace boost;
MatrixAcceptor::MatrixAcceptor(asio::io_service& ios, unsigned short port_num) :
		m_ios(ios),
		m_acceptor(m_ios,
		asio::ip::tcp::endpoint(
		asio::ip::address_v4::any(),
		port_num)),
		m_isStopped(false)
{}

// Start accepting incoming connection requests.
void MatrixAcceptor::Start() {
    m_acceptor.listen();
    InitAccept();
}

// Stop accepting incoming connection requests.
void MatrixAcceptor::Stop() {
    m_isStopped.store(true);
}

void MatrixAcceptor::InitAccept() {//you should create a new socket when you start the accept
    std::shared_ptr<asio::ip::tcp::socket>
        sock(new asio::ip::tcp::socket(m_ios));

    m_acceptor.async_accept(*sock.get(),
        [this, sock](
        const boost::system::error_code& error)
    {
        onAccept(error, sock);
    });
}


//the function calls in the onAccept are non-blocking, and you can always accept new connection
void MatrixAcceptor::onAccept(const boost::system::error_code& ec,
    std::shared_ptr<asio::ip::tcp::socket> sock)
{
    if (ec.value() == 0) {
        (new MatrixService(sock))->StartHandling(); //once this is done it will cause teh self deletion
    }
    else {
        std::cout << "Error occured! Error code = "
            << ec.value()
            << ". Message: " << ec.message();
    }

    // Init next async accept operation if
    // acceptor has not been stopped yet.
    if (!m_isStopped.load()) {
        InitAccept();
    }
    else {
        // Stop accepting incoming connections
        // and free allocated resources.
        m_acceptor.close();
    }
}