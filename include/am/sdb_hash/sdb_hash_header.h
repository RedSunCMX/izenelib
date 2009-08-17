#ifndef SDB_HASH_HEADER_H_
#define SDB_HASH_HEADER_H_

#include <iostream>

using namespace std;

NS_IZENELIB_AM_BEGIN

/**
 * \brief FileHeader of sdb_hash
 *  
 */
struct ShFileHeader {
	int magic; //set it as 0x061561, check consistence.			
	size_t bucketSize;
	//size_t directorySize;
	size_t dpow;
	size_t cacheSize;
	size_t numItems;
	size_t nBlock; //the number of bucket allocated, it indicates the file size: sizeof(ShFileHeader) + nblock*bucketSize

	ShFileHeader()
	{
		magic = 0x061561;
		bucketSize = 2048;
		//directorySize =8192*8;
		dpow = 16;
		cacheSize = 150000;
		numItems = 0;
		nBlock = 0;
	}

	void display(std::ostream& os = std::cout) {
		os<<"magic: "<<magic<<endl;
		os<<"bucketSize: "<<bucketSize<<endl;
		os<<"dpow: "<<dpow<<endl;
		os<<"directorySize: "<<(1<<dpow)<<endl;
		os<<"cacheSize: "<<cacheSize<<endl;
		os<<"numItem: "<<numItems<<endl;
		os<<"nBlock: "<<nBlock<<endl;
		
		os<<endl;
		os<<"file size: "<<nBlock*bucketSize+sizeof(ShFileHeader)<<"bytes"<<endl;
		if(nBlock != 0) {
			os<<"average items number in bucket: "<<double(numItems)/double(nBlock)<<endl;		
			os<<"average length of bucket chain: "<< double(nBlock)/double(2<<dpow)<<endl;
		}
	}

	bool toFile(FILE* f)
	{
		if ( 0 != fseek(f, 0, SEEK_SET) )
		return false;

		fwrite(this, sizeof(ShFileHeader), 1, f);
		return true;

	}

	bool fromFile(FILE* f)
	{
		if ( 0 != fseek(f, 0, SEEK_SET) )
		return false;
		fread(this, sizeof(ShFileHeader), 1, f);
		return true;
	}
};

NS_IZENELIB_AM_END

#endif /*SDB_HASH_HEADER_H_*/
