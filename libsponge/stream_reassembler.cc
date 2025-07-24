#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    :
        _output(capacity),
        _capacity(capacity){

}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // 更新eof_index
    if (eof) {
        _eof = true;
        _eof_index = index + data.length();
    }
    // 数据过老
    if (index < first_unassembled()) {
        if (index + data.length() > first_unassembled()) {
            push_substring(data.substr(first_unassembled() - index), first_unassembled(), eof);
        }
        return;
    }

    size_t old_first_unassembled = first_unassembled();
    size_t old_first_unaccepted = first_unaccepted();
    // 处理剩下的情况
    if (index == first_unassembled()) {
        _output.write(data);
        reassemble(old_first_unassembled, old_first_unaccepted);
    } else { // 乱序到达就放进缓冲区
        put_in_buf(data, index);
    }

    // 处理碰到eof的情况
    if (_eof && first_unassembled() == _eof_index) {
        _buf.clear();
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return _buf.size(); }

bool StreamReassembler::empty() const { return _buf.empty(); }
size_t StreamReassembler::first_unread() { return _output.bytes_read(); }
size_t StreamReassembler::first_unassembled() { return _output.bytes_written(); }
size_t StreamReassembler::first_unaccepted() { return _output.bytes_read() + _capacity; }
void StreamReassembler::reassemble(size_t old_first_unassembled, size_t old_first_unaccepted) {
    string s = string();
    for (size_t i = old_first_unassembled; i < old_first_unaccepted; i++) {
        if (_buf.find(i) != _buf.end()) {
            if (i < first_unassembled()) { // i 比 unassembled 小，说明数据已经被写入上层字节流中了，所以我们erase
                _buf.erase(i);             // 为什么会有这种可能，是因为我们在调用该函数之前已经调用了 write(),所以相应的
                continue;                  // first_unassembled 会改变，我们这里进行检查
            }
            if (i == s.length() + first_unassembled()) { // 这其实就是一个字符串拼接的过程
                s.push_back(_buf.at(i));
                _buf.erase(i);
            } else {
                break;
            }
        }
    }
    _output.write(s);
}

void StreamReassembler::put_in_buf(const std::string &data, const size_t index) {
    for (size_t i = 0; i < data.length(); i++) {
        if (index + i < first_unaccepted()) {
            _buf.insert(std::pair<size_t, char>(index + i, data[i]));
        }
    }
}


