#include "tcp_sender.hh"

#include "tcp_config.hh"

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
    , _stream(capacity) {
    _retransmission_timeout = retx_timeout;
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
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) { DUMMY_CODE(ackno, window_size); }

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) { DUMMY_CODE(ms_since_last_tick); }

unsigned int TCPSender::consecutive_retransmissions() const { return {}; }

void TCPSender::send_empty_segment() {}
