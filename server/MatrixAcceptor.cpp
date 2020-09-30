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
    // this is an async operation and it will return immediately.
    std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(m_ios));
    std::cout << "test | before m_acceptor.async_accet" << std::endl;
    m_acceptor.async_accept(*sock.get(),
        [this, sock](
        const boost::system::error_code& error)
    {
        std::cout << "acception has been recieved" << std::endl;
        onAccept(error, sock);
    });
    std::cout << "test | after m_acceptor.async_accet" << std::endl;
}


//the function calls in the onAccept are non-blocking, and you can always accept new connection
void MatrixAcceptor::onAccept(const boost::system::error_code& ec,
    std::shared_ptr<asio::ip::tcp::socket> sock)
{
    if (ec.value() == 0) {
        //once this is done it will cause the self deletion. 
        //since this function is async it will return immediately
        (new MatrixService(sock))->StartHandling(); 
        
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