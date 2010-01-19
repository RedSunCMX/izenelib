/**
* @file        SkipListReader.h
* @author     Yingfeng Zhang
* @version     SF1 v5.0
* @brief Reading skiplist data
*/

#ifndef SKIPLIST_MERGER_H
#define SKIPLIST_MERGER_H

#include <ir/index_manager/index/SkipListWriter.h>
#include <ir/index_manager/index/SkipListReader.h>

NS_IZENELIB_IR_BEGIN

namespace indexmanager{

/**
 * @brief SkipListMerger
 */
class SkipListMerger : public SkipListWriter
{
public:
    SkipListMerger(int skipInterval, int maxLevel, MemCache* pMemCache);

    virtual ~SkipListMerger();

public:
    void setBasePoint(docid_t baseDocID,fileoffset_t baseOffset,fileoffset_t basePOffset)
    {
        baseDocID_ = baseDocID;
        baseOffset_ = baseOffset;
        basePOffset_ = basePOffset;
    }
	
    bool addToMerge(SkipListReader* pSkipReader,docid_t lastDoc,int skipInterval);

    void addSkipPoint(docid_t docId,fileoffset_t offset,fileoffset_t pOffset);

    void reset();
private:
    void addSkipPoint(SkipListReader* pSkipReader,int skipInterval);

private:
    docid_t baseDocID_;
    fileoffset_t baseOffset_;
    fileoffset_t basePOffset_;		

    int* pSkipInterval_; ///current skip interval of each skip level

};

}
NS_IZENELIB_IR_END

#endif


