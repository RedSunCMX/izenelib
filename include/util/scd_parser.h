#ifndef IZENE_UTIL_SCDPARSER_H_
#define IZENE_UTIL_SCDPARSER_H_

#include <types.h>

#include <util/ustring/UString.h>
#include <util/streambuf.h>

#include <am/3rdparty/rde_hash.h>

#include <boost/shared_ptr.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
namespace izenelib{
namespace util{

typedef std::pair<izenelib::util::UString, izenelib::util::UString> FieldPair;
typedef vector< FieldPair > SCDDoc;
typedef boost::shared_ptr< SCDDoc > SCDDocPtr;
typedef std::pair<izenelib::util::UString, uint32_t> DocIdPair;

enum SCD_TYPE
{
    NOT_SCD = 0,
    INSERT_SCD,
    UPDATE_SCD,
    DELETE_SCD
};


class ScdParser
{
        /// @brief	Reads a document from the loaded SCD file, when given a DOCID value. 
        //			prerequisites: SCD file must be loaded by load(), and getDocIdList() must be called.
        bool getDoc( const izenelib::util::UString & docId, SCDDoc& doc );

    public:
        ScdParser();
        ScdParser(const izenelib::util::UString::EncodingType & encodingType);
        virtual ~ScdParser();
        
        static bool checkSCDFormat( const string & file );
        static unsigned checkSCDDate( const string& file);
        static unsigned checkSCDTime( const string& file);
        static SCD_TYPE checkSCDType( const string & file );
        static bool compareSCD (const string & file1, const string & file2);


        /// @brief Release allocated memory and make it as initial state.
        void clear();

        /// @brief Read a SCD file and load data into memory.
        bool load(const std::string& path);
        
        long getFileSize(){
        	return size_;        	
        }

        /// @brief  A utility function to get all the DOCID values from an SCD
        bool getDocIdList( std::vector<izenelib::util::UString> & list );

        bool getDocIdList( std::vector<DocIdPair > & list );
        

        /// @brief gets the encoding type from the config
        inline izenelib::util::UString::EncodingType& getEncodingType() {
            return encodingType_;
        };


        class iterator
        {
            public:
                iterator(long offset);

                iterator(ScdParser* pScdParser, unsigned int start_doc);

                iterator(const iterator& other);

                ~iterator();

                iterator& operator=(const iterator& other);

                bool operator==(const iterator& other) const;

                bool operator!=(const iterator& other) const;

                void operator++();

                void operator++(int);

                SCDDocPtr operator*();

                long getOffset();

            private:
                SCDDoc* getDoc();

            private:
                std::ifstream* pfs_;

                std::size_t readLength_;

                long prevOffset_;

                long offset_;

                SCDDocPtr doc_;

                izenelib::util::UString::EncodingType codingType_;

                boost::shared_ptr<izenelib::util::izene_streambuf> buffer_;
        };  // class iterator

        iterator begin(unsigned int start_doc = 0);
        
        iterator end();
        
    private:

        ifstream fs_; 
        
        long size_;

        izenelib::util::UString::EncodingType encodingType_;

        izenelib::am::rde_hash<izenelib::util::UString, long> docOffsetList_;
        //izenelib::am::cccr_hash<izenelib::util::UString, long> * docOffsetList_;
        //izenelib::am::LinearHashTable<izenelib::util::UString, long> docOffsetList_;
};

}
}


#endif

