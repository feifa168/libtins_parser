#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <list>
#include <iostream>
#include <string>

#include <windows.h>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

class my_log {
public:
    static void log(const char *fmt, ...) {
        va_list args;
        int n;
        char buf[2048];

        va_start(args, fmt);
        n = vsprintf(buf, fmt, args);
        va_end(args);

        strcat(buf, "\n");
        OutputDebugStringA(buf);
        cout << buf;
    }
};

static boost::mutex gs_bst_mtx;

class my_msg {
public:
    void push(string &msg) {
        boost::unique_lock<boost::mutex> ul(gs_bst_mtx);
        armsg.push_back(msg);
    }

    string& pop() {
        boost::unique_lock<boost::mutex> ul(gs_bst_mtx);
        string msg;
        if (!armsg.empty()) {
            msg = *armsg.begin();
            armsg.pop_front();
        }

        return std::move(msg);
    }
public:
    std::list<string> armsg;
};

class my_client {
public:
    my_client(asio::io_service &service, short pt) : sock(service), port(pt) {
        rdmsg.resize(512);
    }

    void do_start() {
        do_connect();
    }

    void do_connect() {
        my_log::log("client begin connect");
        sock.async_connect(tcp::endpoint(ip::address::from_string("127.0.0.1"), port), [this](const boost::system::error_code &ec) {
            if (!ec) {
                boost::thread t1(std::bind(&my_client::do_read, this));
                boost::thread t2(std::bind(&my_client::do_write, this));
            }
            else {
                my_log::log("async_connect error is %s", ec.message().c_str());
                return;
            }
        });
    }

    void do_read() {
        //my_log::log("    client begin read");
        //rdmsg.clear();
        sock.async_read_some(buffer(rdmsg), [this](const boost::system::error_code &ec, size_t read_len) {
            if (!ec) {
                //my_log::log("read msg is %s, len is %d", rdmsg, read_len);
                rdmsg[read_len] = 0;
                my_log::log("read msg is %s, len is %d", rdmsg.c_str(), read_len);

                //mmsg.push(rdmsg);

                do_read();
            }
            else {
                my_log::log("async_read_some error %s", ec.message().c_str());
                return;
            }
        });
    }

    void do_write() {
        //my_log::log("    client begin write");
        //cin >> wrmsg;
        getline(cin, wrmsg);
        sock.async_write_some(buffer(wrmsg, wrmsg.length() + 1), [this](const boost::system::error_code &ec, size_t write_len) {
            if (!ec) {
                my_log::log("write msg is %s, len is %d", wrmsg.c_str(), write_len);

                do_write();
            }
            else {
                my_log::log("async_write_some error %s", ec.message().c_str());
                return;
            }
        });
    }

public:
    tcp::socket sock;
private:
    string  rdmsg;
    string  wrmsg;
    short   port;
    my_msg  mmsg;
};

int main(int num, char **args) {
    asio::io_service service;
    my_client client(service, 1688);
    client.do_start();

    service.run();
}