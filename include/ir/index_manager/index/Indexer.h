/**
* @file        Indexer.h
* @author     Yingfeng Zhang
* @version     SF1 v5.0
* @brief The interface class of IndexManager component in SF1v5.0
*/

#ifndef INDEXER_H
#define INDEXER_H

#include <ir/index_manager/utility/IndexManagerConfig.h>

#include <ir/index_manager/index/CommonItem.h>
#include <ir/index_manager/index/IndexWriter.h>
#include <ir/index_manager/index/IndexReader.h>
#include <ir/index_manager/index/BTreeIndex.h>
#include <ir/index_manager/index/IndexerDocument.h>
#include <ir/index_manager/store/Directory.h>

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <deque>
#include <string>

NS_IZENELIB_IR_BEGIN

namespace indexmanager{

enum ManagerType
{
    MANAGER_TYPE_LOCAL,       /// Deployed in a single machine
    MANAGER_TYPE_CLIENTPROCESS, /// Deployed as the client indexer
    MANAGER_TYPE_SERVERPROCESS, /// Deployed as the server indexer
    MANAGER_TYPE_FORWARDREADER_AND_MERGER ///Only read forwardindex and merge it if necessary
};

typedef int32_t ACCESS_MODE;

const static int32_t ACCESS_CREATE = 0x10;   /// can add or delete files but can't search over it,will delete existed index database

const static int32_t ACCESS_APPEND = 0x20;   /// will append index data do existed index database.


class BarrelsInfo;
class UDTFSAgent;
class BTreeIndexerClient;
class BTreeIndexerServer;
/**
*The interface class of IndexManager component in SF1v5.0
 * @brief It is the interface component of the IndexManager.
*/
class Indexer: private boost::noncopyable
{
public:

    Indexer(bool indexingForward = false, ManagerType managerType = MANAGER_TYPE_LOCAL);

    virtual ~Indexer();
public:
    int insertDocumentWithNoBatch(IndexerDocument* pDoc);

    int insertDocument(IndexerDocument* pDoc);

    int removeDocument(IndexerDocument* pDoc);

    int removeCollection(collectionid_t colID);

    int updateDocument(IndexerDocument* pDoc);

    void flush();
    
    bool getDocsByTermInProperties(termid_t termID, collectionid_t colID, std::vector<std::string> properties, std::deque<docid_t>& docIds);

    bool getDocsByTermInProperties(termid_t termID, collectionid_t colID, std::vector<std::string> properties, std::deque<CommonItem>& commonSet);

    bool getWordOffsetListOfQueryByDocumentProperty (const std::vector<termid_t>& queryTermIdList, collectionid_t colId, docid_t docId,  string propertyName, std::deque<std::deque<std::pair<unsigned int, unsigned int> > >& wordOffsetListOfQuery );
  
    bool getForwardIndexByDocumentProperty(collectionid_t colId, docid_t docId, string propertyName, ForwardIndex& forwardIndex);	
    
    bool getTermFrequencyInCollectionByTermId ( const std::vector<termid_t>& termIdList, const unsigned int collectionId, const std::vector<std::string>& propertyList, std::vector<unsigned int>& termFrequencyList );

    bool getDocsByPropertyValue(collectionid_t colID, std::string property, PropertyType value, std::vector<docid_t>& docs);

    bool getDocsByPropertyValueRange(collectionid_t colID, std::string property, PropertyType value1, PropertyType value2, std::vector<docid_t>& docs);

    bool getDocsByPropertyValueLessThan(collectionid_t colID, std::string property, PropertyType value, std::vector<docid_t>&docList);

    bool getDocsByPropertyValueLessThanOrEqual(collectionid_t colID, std::string property, PropertyType value, std::vector<docid_t>&docList);

    bool getDocsByPropertyValueGreaterThan(collectionid_t colID, std::string property, PropertyType value, std::vector<docid_t>&docList);

    bool getDocsByPropertyValueGreaterThanOrEqual(collectionid_t colID, std::string property, PropertyType value, std::vector<docid_t>&docList);

    bool getDocsByPropertyValueIn(collectionid_t colID, std::string property, std::vector<PropertyType> values, std::vector<docid_t>&docList);

    bool getDocsByPropertyValueNotIn(collectionid_t colID, std::string property, std::vector<PropertyType> values, std::vector<docid_t>&docList);

    bool getDocsByPropertyValueNotEqual(collectionid_t colID, std::string property, PropertyType value, std::vector<docid_t>&docList);

    bool getDocsByPropertyValueStart(collectionid_t colID, std::string property, PropertyType value, std::vector<docid_t>&docList);

    bool getDocsByPropertyValueEnd(collectionid_t colID, std::string property, PropertyType value, std::vector<docid_t>&docList);

    bool getDocsByPropertyValueSubString(collectionid_t colID, std::string property, PropertyType value, std::vector<docid_t>&docList);

public:
    void setIndexManagerConfig(IndexManagerConfig* pConfigManager,const std::map<std::string, uint32_t>& collectionIdMapping);

    IndexManagerConfig* getIndexManagerConfig()
    {
        return pConfigurationManager_;
    }

    ManagerType getIndexerType()
    {
        return managerType_;
    }

    void add_index_process_node(string ip, string batchport, string rpcport);

    pair<string,pair<string, string> >& get_curr_index_process();

    bool change_curr_index_process();

    bool destroy_connection(pair<string,pair<string, string> >& node);

    bool initialize_connection(pair<string,pair<string, string> >& node, bool wait=false);

    const std::string& getVersionString() const
    {
        return version_;
    }

    void optimizeIndex();

    void set_property_name_id_map(const std::map<std::string, IndexerCollectionMeta>& collections);

    const std::map<std::string, IndexerCollectionMeta>& getCollectionsMeta();

    BarrelsInfo* getBarrelsInfo()
    {
        return pBarrelsInfo_;
    }

    Directory* getDirectory()
    {
        return pDirectory_;
    }

    void setBasePath(std::string basePath);

    void setDirty(bool bDirty);

    IndexWriter* getIndexWriter()
    {
        return pIndexWriter_;
    }

    BTreeIndexerInterface* getBTreeIndexer();

    fieldid_t getPropertyIDByName(collectionid_t colID, string property);

    std::string getBasePath();

protected:
    void initIndexManager();

    void openDirectory();

    void close();

protected:
    ManagerType managerType_;

    bool indexingForward_;

    Directory* pDirectory_;

    ACCESS_MODE accessMode_;

    bool dirty_;

    BarrelsInfo* pBarrelsInfo_;

    IndexWriter* pIndexWriter_;

    IndexReader* pIndexReader_;

    IndexManagerConfig* pConfigurationManager_;

    boost::mutex mutex_;

    BTreeIndexer* pBTreeIndexer_;

    BTreeIndexerClient* pBTreeIndexerClient_;

    BTreeIndexerServer* pBTreeIndexerServer_;

    std::map<collectionid_t, std::map<string, fieldid_t> > property_name_id_map_;

    std::deque<pair<string, pair<string, string> > > index_process_address_;

    UDTFSAgent* pAgent_;

    friend class IndexWriter;

    friend class IndexReader;

    friend class IndexBarrelWriter;	

    friend class IndexBarrelReader;

    std::string version_;
};

class IndexerFactory
{
public:
    IndexerFactory():pIndexer_(NULL) {}
    ~IndexerFactory()
    {
        if (pIndexer_)
            delete pIndexer_;
    }
public:
    Indexer* getIndexer()
    {
        if (NULL == pIndexer_)
            pIndexer_ = new Indexer();
        return pIndexer_;
    }
private:
    Indexer* pIndexer_;
};
///extern Indexer indexer;
extern IndexerFactory indexerFactory;
}

NS_IZENELIB_IR_END

#endif
