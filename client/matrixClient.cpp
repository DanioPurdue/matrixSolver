#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace boost;
using std::string;
using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "format: " << argv[0] << "< server ip address> <server port number>" << endl;
        return 1;
    }
    //obtain the server ip address and port number from the command line 
    string ipAddr = argv[1];
    short portNum = boost::lexical_cast<short>(argv[2]);
    try {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ipAddr), portNum);
        asio::io_service ios;
        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);
    } catch (boost::system::system_error &se) {
        cout << "System Error Occured | " << se.what() << endl;
        return se.code().value();
    }
    return 0;
}