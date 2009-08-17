/**
* @file        MultiTermPositions.h
* @author     Yingfeng Zhang
* @version     SF1 v5.0
* @brief   Iterate the index of position posting (*.pop) in multi barrels
*/
#ifndef MULTITERMPOSITIONS_H
#define MULTITERMPOSITIONS_H

#include <ir/index_manager/index/TermPositions.h>
#include <ir/index_manager/index/BarrelInfo.h>
#include <ir/index_manager/utility/PriorityQueue.h>
#include <list>

NS_IZENELIB_IR_BEGIN

namespace indexmanager{

class BarrelTermPositionsEntry
{
public:
    BarrelTermPositionsEntry(BarrelInfo* barrelInfo_,TermPositions* termPositions_)
    {
        barrelInfo = new BarrelInfo(*barrelInfo_);
        termPositions = termPositions_;
    }
    ~BarrelTermPositionsEntry()
    {
        delete barrelInfo;
        delete termPositions;
    }
protected:
    BarrelTermPositionsEntry() {}
public:
    BarrelInfo* barrelInfo;
    TermPositions* termPositions;

    friend class MultiTermPositions;
};

/**
* Iterate the index of position posting (*.pop) in multi barrels
*/

class MultiTermPositions : public TermPositions
{
    class TermPositionQueue:public PriorityQueue<BarrelTermPositionsEntry*>
    {
    public:
        TermPositionQueue(size_t size)
        {
            initialize(size,false);
        }
    protected:
        bool lessThan(BarrelTermPositionsEntry* o1, BarrelTermPositionsEntry* o2)
        {
            return (o1->termPositions->doc()) < (o2->termPositions->doc());
        }
    };
public:
    MultiTermPositions(void);
    ~MultiTermPositions(void);
public:
    docid_t doc();

    count_t freq();

    bool next();

    count_t next(docid_t*& docs, count_t*& freqs);

    freq_t docFreq();

    int64_t getCTF();

    void  close();

    loc_t nextPosition();

    int32_t nextPositions(loc_t*& positions);

    void add(BarrelInfo* pBarrelInfo,TermPositions* pTermPositions);
protected:

    void initQueue();
protected:

    list<BarrelTermPositionsEntry*>	termPositionsList;

    BarrelTermPositionsEntry* current;

    TermPositionQueue* pTermPositionQueue;
};

}

NS_IZENELIB_IR_END

#endif
