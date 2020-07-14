#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;
#define MAXCONNSIZE 10
/* send a mesage */
void send_(tcp::socket & socket, const string& message) {
    const string msg = message + "\n";
    boost::asio::write( socket, boost::asio::buffer(message) );
}

string read_(tcp::socket & socket) {
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n ｜ ");
    string data = boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}

/* recieve a message */
int main() {
    std::cout<<"This is a matrix solver."<<std::endl;
    boost::asio::io_service io_service;
    // the alternative approach to the following part to instantiate first and then bind the port
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v6(), 80)); //acceptor represents acceptor socket which is pasisve
    while (1) {
        try {
            tcp::socket socket_(io_service); //instantiate asocket that used for teh connection
            acceptor_.listen(MAXCONNSIZE);
            acceptor_.accept(socket_); //switched to the active socket and start the connection
            string msg = read_(socket_);
            cout << "server side message: " << msg << endl;
            //write operation
            send_(socket_, "hello this is a matrix solver");
            cout << "request has been sent" << endl;
        } catch(boost::system::system_error & sysError) {
            cout << "System Error | Error Code: " <<sysError.code() << " | Error Message: " << sysError.what()<<endl;
        } catch (...) {
            cout << "recieved an invalid request" << endl;
        }
    }
    return 0;
}