#ifndef WAT_ARRAY_BIT_TRIE_HPP_
#define WAT_ARRAY_BIT_TRIE_HPP_
//Use only in tdt WikiGraph

#include <stdint.h>
#include <vector>
#include <iostream>

namespace wat_array
{

class BitNode
{
public:
    int grade_;
    BitNode* ZNext_;
    BitNode* ONext_;
public:
    BitNode(int grade);
    ~BitNode();
};

class BitTrie
{
    int height_;
public:
    BitNode* Root_;
    BitTrie(int alphbatNum);
    ~BitTrie();
    void insert(int val);
    bool exist(int val);

};


};
#endif
