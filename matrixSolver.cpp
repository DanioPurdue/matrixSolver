#include <iostream>
#include <boost/asio.hpp>
using namespace boost::asio;
using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

/* send a mesage */
void send_(tcp::socket & socket, const string& message) {
    const string msg = message + "\n";
    boost::asio::write( socket, boost::asio::buffer(message) );
}

string read_(tcp::socket & socket) {
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n");
    string data = boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}

/* recieve a message */
int main() {
    std::cout<<"This is a matrix solver."<<std::endl;
    boost::asio::io_service io_service;
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 80));
    tcp::socket socket_(io_service);
    acceptor_.accept(socket_);
    while (1) {
        try {
        string msg = read_(socket_);
        cout << "server side message: " << msg << endl;
        //write operation
        send_(socket_, "hello this is a matrix solver");
        cout << "request has been sent" << endl;
        } catch (...) {
            cout << "recieved an invalid request" << endl;
        }
    }
    return 0;
}