#include <ir/index_manager/index/SkipListMerger.h>

NS_IZENELIB_IR_BEGIN

namespace indexmanager{

SkipListMerger::SkipListMerger(int skipInterval, int maxLevel, MemCache* pMemCache)
    :SkipListWriter(skipInterval, maxLevel, pMemCache)
    ,baseDocID_(0)
    ,baseOffset_(0)
    ,basePOffset_(0)		
{
    pSkipInterval_ = new int[maxLevel];
    memset(pSkipInterval_,0,maxLevel * sizeof(int));
}

SkipListMerger::~SkipListMerger()
{
    if(pSkipInterval_)
    {
        delete[] pSkipInterval_;
        pSkipInterval_ = NULL;
    }
}

void SkipListMerger::addSkipPoint(SkipListReader* pSkipReader,int skipInterval)
{
    int curSkipped = (pSkipReader->getCurSkipInterval() + skipInterval);
    for(int i = 0;i < maxSkipLevel_;i++)
        pSkipInterval_[i] += curSkipped;

    if(pSkipInterval_[0] <= 1)
        return;

    SkipListWriter::addSkipPoint( pSkipReader->getDoc() + baseDocID_, 
                        pSkipReader->getOffset() + baseOffset_,
                        pSkipReader->getPOffset() + basePOffset_
                      );
}

void SkipListMerger::addSkipPoint(docid_t docId,fileoffset_t offset,fileoffset_t pOffset)
{
    SkipListWriter::addSkipPoint( docId + baseDocID_, 
                        offset + baseOffset_,
                        pOffset + basePOffset_
                      );
}

bool SkipListMerger::addToMerge(SkipListReader* pSkipReader,docid_t lastDoc,int skipInterval)
{			
    bool ret = false;
    while(pSkipReader->nextSkip(lastDoc))
    {				
        addSkipPoint(pSkipReader,skipInterval);
        ret = true;
        skipInterval = 0;
    }
    return ret;
}

void SkipListMerger::reset()
{
    SkipListWriter::reset();
    for(int i = 0;i < maxSkipLevel_;i++)
        pSkipInterval_[i] = 0;
}

}
NS_IZENELIB_IR_END


