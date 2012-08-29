#ifndef _FM_INDEX_WAVELET_TREE_NODE_H
#define _FM_INDEX_WAVELET_TREE_NODE_H

#include "rrr_bit_vector.hpp"


NS_IZENELIB_AM_BEGIN

namespace succinct
{
namespace fm_index
{

class WaveletTreeNode
{
public:
    WaveletTreeNode();
    WaveletTreeNode(uint64_t c, size_t freq);
    WaveletTreeNode(WaveletTreeNode *left, WaveletTreeNode *right);

    void resize(size_t len);

    void setBit(size_t pos);
    void unsetBit(size_t pos);
    void changeBit(bool bit, size_t pos);

    void append0();
    void append1();
    void appendBit(bool bit);

    void build();

    size_t length() const;
    size_t getSize() const;

    void save(std::ostream &ostr) const;
    void load(std::istream &istr);

    bool operator>(const WaveletTreeNode &rhs) const;

public:
    WaveletTreeNode *left_;
    WaveletTreeNode *right_;
    WaveletTreeNode *parent_;

    size_t freq_;
    uint64_t c0_;
    uint64_t c1_;

    size_t len_;
    size_t ones_;

    std::vector<uint32_t> raw_array_;
    RRRBitVector bit_vector_;
};

}
}

NS_IZENELIB_AM_END

namespace std
{

template <>
struct greater<izenelib::am::succinct::fm_index::WaveletTreeNode *>
{
    bool operator()(izenelib::am::succinct::fm_index::WaveletTreeNode *p1, izenelib::am::succinct::fm_index::WaveletTreeNode *p2)
    {
        if (!p1) return false;
        if (!p2) return true;
        return *p1 > *p2;
    }
};

}

#endif