#include "tcp_receiver.hh"

#include <cassert>

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    if (_isn.has_value() && seg.header().syn && _isn == seg.header().seqno) {
        printf("重复设置syn");
        assert(0);
    }
    if (!_isn.has_value()) {
        // 如果isn没有被设置
        if (seg.header().syn) {
        _isn = seg.header().seqno; // 设置isn
        _reassembler.push_substring(seg.payload().copy(), 0, seg.header().fin); // 将数据传送给reassembler处理
        update_ackno();
        }
        return;
    }
    auto index = unwrap(seg.header().seqno, _isn.value(), _reassembler.first_unassembled());
    _reassembler.push_substring(seg.payload().copy(), index - 1, seg.header().fin); // 减1是因为要在abs_seqno 和 stream index之间转化
    update_ackno();
}

optional<WrappingInt32> TCPReceiver::ackno() const { return _ackno; }

size_t TCPReceiver::window_size() const { return _reassembler.first_unaccepted() - _reassembler.first_unassembled(); }

void TCPReceiver::update_ackno() {
    // 遇到syn + 1 byte
    // 遇到fin + 2 byte
    if (_reassembler.stream_out().input_ended()) {
        _ackno = wrap(_reassembler.first_unassembled() + 2, _isn.value());
    } else {
        _ackno = wrap(_reassembler.first_unassembled() + 1, _isn.value());
    }
}



