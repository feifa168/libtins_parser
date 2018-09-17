#include <iostream>
#include <vector>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace std;
using namespace boost;


class SniffServerClient {
public:
    SniffServerClient(boost::asio::io_service &ioservice) : socket_(ioservice), read_buf(""){}

    void start() {
        do_read();
        //do_write();
    }

    asio::ip::tcp::socket& get_socket() { return socket_; }

private:
    void handle_read(const boost::system::error_code &ec) {
        if (!ec) {
            asio::ip::tcp::endpoint ep = socket_.remote_endpoint();
            cout << ep.address().to_v4().to_string() << ":" << ep.port()  << " <== " << read_buf << endl;
            read_buf[0] = '\0';
            do_read();
        }
        else {
            cout << ec.message() << endl;
        }
    }
    void do_read() {
        socket_.async_read_some(asio::buffer(read_buf), boost::bind(&SniffServerClient::handle_read, this, boost::asio::placeholders::error));
        //socket_.async_receive(asio::buffer(read_buf), boost::bind(&SniffServerClient::handle_read, this, boost::asio::placeholders::error));
    }

    void handle_write(const boost::system::error_code &ec) {
        if (!ec) {
            asio::ip::tcp::endpoint ep = socket_.remote_endpoint();
            cout << ep.address().to_v4().to_string() << ":" << ep.port() << " ==> " << write_buf << endl;

            boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

            do_write();
        }
        else {
            cout << ec.message() << endl;
        }
    }
    void do_write() {
        static int i = 0;
        write_buf = read_buf;
        char buf[32];
        itoa(++i, buf, 10);
        write_buf += buf;
        socket_.async_write_some(asio::buffer(write_buf), boost::bind(&SniffServerClient::handle_write, this, boost::asio::placeholders::error));
    }
private:
    asio::ip::tcp::socket socket_;
    char read_buf[1024];
    std::string write_buf;
};

class SniffServer {
public:
    SniffServer(boost::asio::io_service &ioservice, unsigned short port) :
        ioservice_(ioservice),
        accept_(ioservice, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)){
    }

    void start() {
        do_accept();

        ioservice_.run();
    }

    void handle_accept(const boost::system::error_code &ec, SniffServerClient *pclient) {
        using namespace std;
        if (!ec) {
            asio::ip::tcp::endpoint ep = pclient->get_socket().remote_endpoint();
            cout << "client is connect " << ep.address().to_v4().to_string() << " port is " << ep.port() << endl;
            pclient->start();
        }
        else {
            cout << ec.message() << endl;
        }

        do_accept();
    }

    void do_accept() {
        //asio::ip::tcp::socket *pclient = new asio::ip::tcp::socket(ioservice_);
        SniffServerClient *pclient = new SniffServerClient(ioservice_);
        vt_clients.push_back(pclient);
        accept_.async_accept(pclient->get_socket(), boost::bind(&SniffServer::handle_accept, this, boost::asio::placeholders::error, pclient));
    }

private:
    boost::asio::io_service &ioservice_;
    boost::asio::ip::tcp::acceptor accept_;
    std::vector<SniffServerClient*> vt_clients;

};
int main(int argc, char **argvs) {

    boost::asio::io_service ioservice;
//     boost::asio::io_service ioservice2(ioservice);
//     boost::asio::io_service ioservice2 = ioservice;
    SniffServer server(ioservice, 1688);
    server.start();

    return 0;
}