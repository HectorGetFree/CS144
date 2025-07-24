#include "byte_stream.hh"

#include <pcap/pcap.h>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity)
    : _capacity(capacity),
      _buf(capacity),  // 初始化 vector，大小为 capacity
      _nread(0),
      _nwrite(0) {

}

size_t ByteStream::write(const string &data) {
    // size_t length = data.length();
    // if (length < capacity) {
    //     copy(data.begin(), data.end(), (mem + write_pos));
    //     write_pos += length;
    //     byte_written += length;
    //     return length;
    // } else {
    //     // 由于在一定时间内mem最多只能容纳 capacity 的数据，所以多的数据应该舍弃
    //     memcpy(mem, data.data(), capacity - write_pos);
    //     write_pos = capacity;
    //     byte_written += capacity - write_pos;
    //     return capacity - write_pos;
    // }
    size_t cnt = 0;
    for (const auto &item : data) {
        if (_nwrite >= _nread + _capacity) { // 说明缓冲区已满
            break;
        }
        _buf[_nwrite % _capacity] = item;
        _nwrite++;
        cnt++;
    }
    return cnt;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    // string s = string();
    // if (write_pos - read_pos >= len) {
    //     string res(mem+read_pos, len);
    // } else {
    //     string res(mem+read_pos, write_pos - read_pos);
    // }
    // return res;

    string s = string();
    for (size_t i = _nread; (i < _nwrite) && (i < _nread + len); i++) {
        s.push_back(_buf[i % _capacity]); // 在字符串末尾追加
    }
    return s;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (_nread < _nwrite) {
            _nread++;
        } else {
            return;
        }
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string s = peek_output(len);
    pop_output(s.length());
    return s;
}

void ByteStream::end_input() {
    is_end = true;
}

bool ByteStream::input_ended() const { return is_end; }

size_t ByteStream::buffer_size() const { return _nwrite - _nread; }

bool ByteStream::buffer_empty() const { return buffer_size() == 0; }

bool ByteStream::eof() const { return is_end && buffer_empty(); }

size_t ByteStream::bytes_written() const { return _nwrite; } // 很巧妙的做法，_nwrite 和 _nread 都会一直累加
                                                             // 无需额外的变量来记录，而且获取索引也会很方便

size_t ByteStream::bytes_read() const { return _nread; }

size_t ByteStream::remaining_capacity() const { return _capacity - buffer_size(); }
