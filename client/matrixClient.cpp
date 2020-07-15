#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace boost;
using std::string;
using std::cout;
using std::endl;
using std::size_t;

void writeToSocket(asio::ip::tcp::socket& sock, char * msgPtr, size_t msgSize) {
    std::unique_ptr<char[]> charPtr = std::make_unique<char []>(msgSize+1);
    memcpy(charPtr.get(), msgPtr, msgSize);
    *(charPtr.get()+msgSize) = '\n';
    asio::write(sock, asio::buffer((void * ) (charPtr.get()), msgSize+1));
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "format: " << argv[0] << "< server ip address> <server port number>" << endl;
        return 1;
    }
    //obtain the server ip address and port number from the command line 
    string ipAddr = argv[1];
    short portNum = boost::lexical_cast<short>(argv[2]);
    int x[3][4] = {0, 1 ,2 ,3 ,4 , 5 , 6 , 7 , 8 , 9 , 10 , 11};
    size_t arraySize_inByte = sizeof(x);
    string hello("HelloWorld\n");
    try {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ipAddr), portNum);
        asio::io_service ios;
        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);
        writeToSocket(sock, (char *) x, arraySize_inByte);
    } catch (boost::system::system_error &se) {
        cout << "System Error Occured | " << se.what() << endl;
        return se.code().value();
    }
    return 0;
}