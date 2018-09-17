#pragma once

#include <cstdlib>
#include <unordered_map>
#include <atomic>

using namespace std;

class pkg_tuple_key {
public:
    int ip;
    int port;
    int proto;
};

class pkg_tuple_detail {
public:
    struct flags_type {
        uint8_t fin : 1,
            syn : 1,
            rst : 1,
            psh : 1,
            ack : 1,
            urg : 1,
            ece : 1,
            cwr : 1;
    };
    enum tcp_cur_status {
        UNKNOWN,
        SYN,
        SYN_ACK,
        ACK,
        FIN,
        RST
    };

    int srcip;
    int dstip;
    int proto;
    short sport;
    short dport;
    flags_type      flags;
    tcp_cur_status  cur_status;

    void set_cur_status(tcp_cur_status flag) { cur_status = flag; }
    tcp_cur_status get_cur_status() {
        if (flags.fin) {
            cur_status = pkg_tuple_detail::FIN;
        }
        else if (flags.rst) {
            cur_status = pkg_tuple_detail::RST;
        }
        else if (flags.syn) {
            if (flags.ack)
                cur_status = pkg_tuple_detail::SYN_ACK;
            else
                cur_status = pkg_tuple_detail::SYN;
        }
        else if (flags.ack) {
            cur_status = pkg_tuple_detail::ACK;
        }
        else{
            cur_status = pkg_tuple_detail::UNKNOWN;
        }
    }
    void change_status(pkg_tuple_detail &dtl) {
        if (this != &dtl) {
            if (cur_status == pkg_tuple_detail::UNKNOWN) {
                if (dtl.get_cur_status()) {
                }
            }
        }
    }
};



class pkg_tuple {
public:
    pkg_tuple();
    ~pkg_tuple();

private:
    class pkg_hash_fun {
    public:
        std::size_t operator()(const pkg_tuple_key &key) const {
            char buf[128] = "";
            itoa(key.ip, buf, 10);
            itoa(key.port, buf+strlen(buf), 10);
            itoa(key.proto, buf+strlen(buf), 10);
            return BKDRHash(buf);
        }
    private:
        static size_t BKDRHash(char *str)
        {
            register size_t hash = 0;
            while (size_t ch = (size_t)*str++)
            {
                // 也可以乘以31、131、1313、13131、131313..  
                // 将乘法分解为位运算及加减法可以提高效率，如将上式表达为：hash = hash << 7 + hash << 1 + hash + ch;  
                // 但其实在Intel平台上，CPU内部对二者的处理效率都是差不多的
                hash = hash * 131 + ch;
            }
            return hash;
        }
    };
    class pkg_equal_fun {
    public:
        bool operator () (const pkg_tuple_key &lhs, const pkg_tuple_key &rhs) const {
            return (0 == memcmp((void *)(&lhs), &rhs, sizeof(lhs)));
        }
    };
    typedef unordered_map<pkg_tuple_key, pkg_tuple_detail, pkg_tuple::pkg_hash_fun, pkg_tuple::pkg_equal_fun > pkg_tuple_map;
    //typedef atomic<pkg_tuple_map> atomic_pkg_tuple_map;
    typedef atomic<pkg_tuple_map*> ptr_atomic_pkg_tuple_map;
    ptr_atomic_pkg_tuple_map ptr_atoic_tuple_map;

public:
    ptr_atomic_pkg_tuple_map& get_atomic_map() {
        return ptr_atoic_tuple_map;
    }
    void add(pkg_tuple_key &key, pkg_tuple_detail &value) {
        pkg_tuple_map *pkg = ptr_atoic_tuple_map.load();
        if (pkg) {
            auto item = pkg->find(key);
            if (item != pkg->end()) {
                pkg_tuple_detail &pkg_detail = item->second;
                switch (pkg_detail.get_cur_status()) {
                case pkg_tuple_detail::SYN: {
                    switch (value.get_cur_status())
                    {
                    default:
                        break;
                    }
                    break;
                }
                case pkg_tuple_detail::SYN_ACK: {
                    break;
                }
                case pkg_tuple_detail::ACK: {
                    break;
                }
                case pkg_tuple_detail::FIN: {
                    break;
                }
                case pkg_tuple_detail::RST: {
                    break;
                }
                default: {
                    break;
                }
                }
            }
        }
    }
};