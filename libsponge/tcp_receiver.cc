#include "tcp_receiver.hh"

#include <cassert>

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    // 判断是否是syn
    const TCPHeader &header = seg.header();
    if (!_set_syn_flag) {
        if (!header.syn) {
            return;
        }
        _isn = header.seqno;
        _set_syn_flag = true;
    }
    uint64_t abs_ackno = _reassembler.stream_out().bytes_written() + 1;
    uint64_t curr_abs_seqno = unwrap(header.seqno, _isn, abs_ackno);

    uint64_t stream_index = curr_abs_seqno - 1 + (header.syn);
    _reassembler.push_substring(seg.payload().copy(), stream_index, header.fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    // 判断是否在Linsen 状态
    if (!_set_syn_flag) {
        return nullopt;
    }
    // 如果不在Linsen状态，则ackno还需要加上一个syn长度
    uint64_t abs_ack_no = _reassembler.stream_out().bytes_written() + 1;
    // 如果是 FIN_RECV 状态，则还需要加上一个FIN长度
    if (_reassembler.stream_out().input_ended()) {
        abs_ack_no++;
    }
    return WrappingInt32(_isn) + abs_ack_no;
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }





