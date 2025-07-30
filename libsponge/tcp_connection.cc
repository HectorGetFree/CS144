#include "tcp_connection.hh"

#include <iostream>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const { return _time_now - _last_segment_received_timestamp; }

void TCPConnection::segment_received(const TCPSegment &seg) {
    if (!_active) { // 如果TCP connection没有建立
        return;
    }
    if (seg.header().rst) { // 如果RST（reset）标志位为真，将发送端stream和接受端stream设置成error state并终止连接
        close();
        _sender.stream_in().set_error();
        _receiver.stream_out().set_error();
    }
    // 把segment传递给TCPReceiver
    _receiver.segment_received(seg);

    // 更新时间戳
    _last_segment_received_timestamp = _time_now;

    // keep-alive
    if (_receiver.ackno().has_value() && seg.length_in_sequence_space() == 0 &&
        seg.header().seqno == _receiver.ackno().value() - 1) {
        _sender.send_empty_segment();
        fill_window();  // 发了一个空包，重新整理 ackno,window—size 这些量,使得能够被ip层发送
        return;
    }

    if (seg.header().ack) {
        // 通知TCPSender有segment被确认
        _sender.ack_received(seg.header().ackno, seg.header().win);
    }

    _sender.fill_window();
    size_t cnt = fill_window();
    if (cnt == 0 && seg.length_in_sequence_space() > 0) { // 如果当前的seg有数据，那就要回复一个ack，以便远端的发送方更新ackno和window_size
        _sender.send_empty_segment();
        fill_window();
    }

    // 关闭连接机制
    if (!_sender.stream_in().eof() && _receiver.stream_out().input_ended()) {
        _linger_after_streams_finish = false;
    }
    if (_sender.fin_received() && _receiver.stream_out().input_ended()) {
        if (!_linger_after_streams_finish) {
            close();
        }
    }
}

bool TCPConnection::active() const { return _active; }

size_t TCPConnection::write(const string &data) {
    DUMMY_CODE(data);
    return {};
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) { DUMMY_CODE(ms_since_last_tick); }

void TCPConnection::end_input_stream() {}

void TCPConnection::connect() {}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";

            // Your code here: need to send a RST segment to the peer
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}

void TCPConnection::close() {
    // 将所有保存的数据弹出
    while (!_sender.segments_out().empty()) {
        _sender.segments_out().pop();
    }
    while (!_segments_out.empty()) {
        _segments_out.pop();
    }
    _active = false;
}

size_t TCPConnection::fill_window() {
    size_t cnt = _sender.segments_out().size();
    while (!_sender.segments_out().empty()) {
        auto seg = _sender.segments_out().front();
        if (_receiver.ackno().has_value()) {
            seg.header().ackno = _receiver.ackno().value();
            seg.header().ack  = true;
        }
        seg.header().win = _receiver.window_size() > UINT16_MAX ? UINT16_MAX : _receiver.window_size();
        _segments_out.push(seg);
        _sender.segments_out().pop();
    }
    return cnt;
}