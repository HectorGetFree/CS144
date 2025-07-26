#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    return isn + n;
}


uint64_t distance(uint64_t a, uint64_t b) {
    if (a > b) {
        return a - b;
    }
    return b - a;
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    auto _checkpoint = wrap(checkpoint, isn);
    auto diff = n - _checkpoint;
    //! method 1
    // 采用数学公式进行运算 -- 做差 ，找差的范围
    // 三种情形下k = 0, -1, 1 时的值
    uint64_t r1 = checkpoint + diff;
    uint64_t r2 = checkpoint + diff + (1ll << 32);
    uint64_t r3 = checkpoint + diff - (1ll << 32);

    uint64_t d1 = distance(r1, checkpoint);
    uint64_t d2 = distance(r2, checkpoint);
    uint64_t d3 = distance(r3, checkpoint);

    // 比较 -- 得到最小的来解决
    if (d1 < d3) {
        if (d1 < d2) {
            return r1;
        }
        return r2;
    } else { // d3 < d1
        if (d3 < d2) {
            return r3;
        }
        return r2;
    }

    //! method 2
    // 没有办法直接返回，因为要考虑到32->64的拓展，可能导致结果出现很大的偏差
    // 所以特殊的溢出情况
    // if (diff < 0 && checkpoint + diff > checkpoint) {
    //     return checkpoint + uint32_t(diff);
    // }
    // // 我不认为这种情况会存在，因为diff>0,它转换为64位后还是正的，按理说溢出的可能很小
    // // if (diff > 0 && checkpoint + diff < checkpoint) {
    // // }
    // return checkpoint + diff;
}
