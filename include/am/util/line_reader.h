/*
 * =====================================================================================
 *
 *       Filename:  line_reader.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013年05月22日 09时29分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kevin Hu (), kevin.hu@b5m.com
 *        Company:  B5M.com
 *
 * =====================================================================================
 */

#ifndef _IZENELIB_AM_UTIL_LINE_READER_H_
#define _IZENELIB_AM_UTIL_LINE_READER_H_

#include "types.h"
#include "am/hashtable/khash_table.hpp"

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
NS_IZENELIB_AM_BEGIN

namespace util{

class LineReader
{
	FILE* f_;
	char* mem_;
	uint32_t bytes_;

	bool next_block_()
	{
		if (::feof(f_))
		  return false;
		memset(mem_, 0, bytes_);
		uint64_t p = ftell(f_);
		fread(mem_, bytes_, 1, f_);
		//std::cout<<mem_<<"PPPP\n";
		char* m = mem_;
		char* la_n = NULL;
		while(*m != 0 && m - mem_ < bytes_)
		{
  			if (*m == '\n')
			  *m = 0, la_n = m;
			m++;
		}

		if (m == mem_ && ::feof(f_))
		  return false;

		if (m-mem_<bytes_ || ::feof(f_))
		  return true;

		IASSERT(la_n != NULL);
		fseek(f_, p + (la_n - mem_+1), SEEK_SET);
		IASSERT((la_n - mem_+1) <= bytes_);
		memset(la_n+1, 0, bytes_ -(la_n - mem_+1));
		return true;
	}
	public:
		LineReader(const std::string& nm, uint32_t buf_size = 1000000)
		{
			f_ = fopen(nm.c_str(), "r");
			if (!f_)
			  throw std::runtime_error("can't open file.");

			bytes_ = buf_size;
			mem_ = new char[buf_size];
		}

		~LineReader()
		{
			if(f_)fclose(f_);
			if (mem_)delete mem_;
		}

		char* line(char* prev_line = NULL)
		{
			if (!prev_line)
			{
				fseek(f_, 0, SEEK_SET);
				if(next_block_())return mem_;
				return NULL;
			}

			IASSERT(prev_line >= mem_);
			IASSERT(prev_line < mem_+bytes_);
			//std::cout<<prev_line<<"LLLL\n";
			//std::cout<<prev_line-mem_<<std::endl;
			while(*prev_line != 0)++prev_line;
			IASSERT(prev_line < mem_+bytes_);

			prev_line++;
			if (*prev_line  == 0 || prev_line >= mem_+bytes_)
			{
				if(next_block_())return mem_;
				return NULL;
			}
			return prev_line;
		}
};

}

NS_IZENELIB_AM_END
#endif
