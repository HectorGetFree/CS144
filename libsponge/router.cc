#include "router.hh"

#include <iostream>

using namespace std;

// Dummy implementation of an IP router

// Given an incoming Internet datagram, the router decides
// (1) which interface to send it out on, and
// (2) what next hop address to send it to.

// For Lab 6, please replace with a real implementation that passes the
// automated checks run by `make check_lab6`.

// You will need to add private members to the class declaration in `router.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

//! \param[in] route_prefix The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
//! \param[in] prefix_length For this route to be applicable, how many high-order (most-significant) bits of the route_prefix will need to match the corresponding bits of the datagram's destination address?
//! \param[in] next_hop The IP address of the next hop. Will be empty if the network is directly attached to the router (in which case, the next hop address should be the datagram's final destination).
//! \param[in] interface_num The index of the interface to send the datagram out on.
void Router::add_route(const uint32_t route_prefix,
                       const uint8_t prefix_length,
                       const optional<Address> next_hop,
                       const size_t interface_num) {
    cerr << "DEBUG: adding route " << Address::from_ipv4_numeric(route_prefix).ip() << "/" << int(prefix_length)
         << " => " << (next_hop.has_value() ? next_hop->ip() : "(direct)") << " on interface " << interface_num << "\n";

    _router_table.push_back({route_prefix, prefix_length, next_hop, interface_num});
}

//! \param[in] dgram The datagram to be routed
void Router::route_one_datagram(InternetDatagram &dgram) {
    // 先查找最优匹配
    auto max_length_prefix_match = _router_table.end(); // 指向的是最后一个对象的下一个，表示超出容器末尾
    auto dst_ip_addr = dgram.header().dst;
    for (auto router_table_iter = _router_table.begin(); router_table_iter != _router_table.end();
        router_table_iter++) {
        // 如果匹配长度为0，或者前缀匹配相同
        if (router_table_iter->prefix_length == 0 ||
            (router_table_iter->route_prefix ^ dst_ip_addr) >> (32 - router_table_iter->prefix_length) == 0) {
            // 如果符合条件，更新max_length_prefix_match
            if (max_length_prefix_match == _router_table.end() ||
                max_length_prefix_match->prefix_length <= router_table_iter->prefix_length) {
                max_length_prefix_match = router_table_iter;
            }
        }
    }

    // 为数据包的TTL - 1
    dgram.header().ttl--;
    // 如果存在最优匹配就转发
    if (max_length_prefix_match != _router_table.end() && dgram.header().ttl > 0) {
        const optional<Address> next_hop = max_length_prefix_match->next_hop;
        AsyncNetworkInterface interface = _interfaces[max_length_prefix_match->interface_num];
        if (next_hop.has_value()) {
            // 如果路由器通过其他路由器连接到相关网络，则下一跳将包含路径上下一个路由器的 IP 地址
            interface.send_datagram(dgram, next_hop.value());
        } else {
            // 如果路由器直接连接到相关网络，则下一跳将为空的可选项。在这种情况下，下一跳是数据报的目标地址
            interface.send_datagram(dgram, Address::from_ipv4_numeric(dst_ip_addr));
        }
    }
    // 其他情况就丢弃
}

void Router::route() {
    // Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
    for (auto &interface : _interfaces) {
        auto &queue = interface.datagrams_out();
        while (not queue.empty()) {
            route_one_datagram(queue.front());
            queue.pop();
        }
    }
}
