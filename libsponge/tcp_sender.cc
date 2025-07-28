#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <cassert>
#include <random>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _retransmission_timeout(retx_timeout)
    , _stream(capacity){
}

size_t TCPSender::bytes_in_flight() const { return _next_seqno - _abs_ackno; }

void TCPSender::fill_window() {
    if (_fin_sent) {
        return; // 如果已经发送过fin了，那么直接返回，不需要处理了
    }
    while (bytes_in_flight() < _window_size) {
        auto seg = TCPSegment();
        if (!_syn_sent) {
            seg.header().syn = true;
            seg.header().seqno = _isn;
            _syn_sent = true;
        }
        seg.header().seqno = wrap(_next_seqno, _isn);
        // 从 stream 中读取数据, 写入到 payload 中
        size_t len = min(_window_size - bytes_in_flight() - seg.length_in_sequence_space(),
            TCPConfig::MAX_PAYLOAD_SIZE);
        seg.payload() = _stream.read(len);

        if (!_fin_sent && _stream.eof()) { // 如果还没有添加fin，并且stream已经遇到eof了
            if (seg.length_in_sequence_space() + bytes_in_flight() < _window_size) { // 如果添加fin不会超过_window_size
                seg.header().fin = true;
                _fin_sent = true;
            }
        }

        if (seg.length_in_sequence_space() > 0) {
            _segments_vector.push_back(seg);
            _segments_out.push(seg);
            _next_seqno += seg.length_in_sequence_space();
        } else {
            break;
        }

    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    if (!_syn_sent) {
        return; // 还没发送过syn，当然无法接受ack
    }

    if (window_size == 0) {
        _window_size = 1;
        _zero_window = true;
    } else {
        _window_size = window_size;
        _zero_window = false;
    }

    // 更新 ackno
    // 但是更新之前先要判断是否合理
    uint64_t abs_ackno = unwrap(ackno, _isn, _abs_ackno);
    if (abs_ackno > _abs_ackno && abs_ackno <= _next_seqno) {
        _ackno = ackno;
        _abs_ackno = abs_ackno;
    } else {
        return;
    }

    // 更新计时器--用于维护超时重传
    _retransmission_timeout = _initial_retransmission_timeout;
    _ticks = 0;
    _consecutive_retransmissions = 0;

    // 更新一些状态
    if (!_syn_received && _syn_sent && _abs_ackno > 0) {
        _syn_received = true;
    }

    if (!_fin_received && _fin_sent && _stream.input_ended() && _abs_ackno >= _stream.bytes_read() + 2) {
        _fin_received = true;
        _segments_vector.clear();
    }

    // 仅更新 ackno 还不够
    // 还要更新 缓存的segments_vector
    std::vector<TCPSegment> segs = vector<TCPSegment>();
    for (const auto &seg: _segments_vector) {
        uint64_t seqno = unwrap(seg.header().seqno, _isn, _abs_ackno);
        if (seqno + seg.length_in_sequence_space() > _abs_ackno) {
            segs.push_back(seg);
        }
    }
    _segments_vector = segs;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    if (bytes_in_flight() == 0) {
        if (!_segments_vector.empty()) {
            printf("!_segments_vector.empty()");
            assert(0);
        }
        _ticks = 0;
        return;
    }
    if (_segments_vector.empty()) {
        printf("_segments_vector.empty()");
        assert(0);
    }
    _ticks += ms_since_last_tick;
    if (_ticks >= _retransmission_timeout) { // 超时了
        _segments_out.push(_segments_vector[0]);
        _ticks = 0;
        if (!_zero_window) {
            _retransmission_timeout *= 2; // RTO 重传策略
        }
        _consecutive_retransmissions++;
    }

}

unsigned int TCPSender::consecutive_retransmissions() const { return _consecutive_retransmissions; }

void TCPSender::send_empty_segment() {
    TCPSegment seg = TCPSegment();
    seg.header().seqno = wrap(_next_seqno, _isn);
    _segments_out.push(seg);
}
