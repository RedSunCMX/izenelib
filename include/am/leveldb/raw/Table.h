#ifndef AM_LEVELDB_RAW_TABLE_H
#define AM_LEVELDB_RAW_TABLE_H
/**
 * @file am/leveldb/raw/Table.h
 * @author Ian Yang
 * @date Created <2009-09-10 09:37:25>
 * @date Updated <2009-09-10 10:46:40>
 */

#include <am/concept/DataType.h>
#include <am/raw/Buffer.h>

#include <3rdparty/am/leveldb/db.h>
#include <iostream>
namespace izenelib {
namespace am {
namespace leveldb {
namespace raw {

using izenelib::am::raw::Buffer;

class Table
    : boost::noncopyable
{
public:
    typedef Buffer key_type;
    typedef Buffer value_type;
    typedef DataType<Buffer, Buffer> data_type;
    typedef int size_type;

    explicit Table(const std::string& file = "")
    : db_(NULL), isOpened_(false), file_(file)
    {
        open(file);
    }

    ~Table()
    {
        close();
    }

    bool open(const std::string& file)
    {
        file_ = file;
        close(); // close first if opened

        ::leveldb::Options options;
        options.create_if_missing = true;

        ::leveldb::Status status;

        status = ::leveldb::DB::Open(options, file_, &db_);

        if(status.ok())
            isOpened_ = true;

        return isOpened_;
    }
    std::string getFileName() const
    {
        return file_;
    }
    void close()
    {
        if (db_ && isOpened_)
        {
            flush();
            delete db_;
            db_ = NULL;
            isOpened_ = false;
        }
    }

    bool isOpened() const
    {
        return isOpened_;
    }
    /**
     * @deprecated
     */
    bool is_open() const
    {
        return isOpened_;
    }

    bool flush()
    {
        return checkHandle_(db_) && isOpened();
    }

    size_type size() const
    {
        return db_ ? 0 : 0;
    }
    /**
     * @deprecated
     */
    size_type num_items() const
    {
        return size();
    }
    bool empty() const
    {
        return size() == 0;
    }
    bool clear()
    {
        return checkHandle_(db_) && isOpened();
    }

    /**
     * @brief Insert new data into database.
     *
     * If a record with the same key exists in the database, this function has no effect.
     *
     * @param key Key Buffer
     * @param value Value Buffer
     *
     * @return If successful, the return value is @c true, else, it is @c false.
     */
    bool insert(const Buffer& key, const Buffer& value)
    {
        return checkHandle_(db_) && isOpened() && (db_->Put(
            ::leveldb::WriteOptions(),
            ::leveldb::Slice(key.data(),key.size()),
            ::leveldb::Slice(value.data(),value.size())).ok()
        );
    }
    /**
     * @brief Insert new data into database.
     *
     * If a record with the same key exists in the database, this function has no effect.
     *
     * @param data data record
     *
     * @return If successful, the return value is @c true, else, it is @c false.
     */
    bool insert(const data_type& data)
    {
        return insert(data.get_key(), data.get_value());
    }

    /**
     * @brief Insert new data into database or update the existing record.
     *
     * @param key Key Buffer
     * @param value Value Buffer
     *
     * @return If successful, the return value is @c true, else, it is @c false.
     */
    bool update(const Buffer& key, const Buffer& value)
    {
        return checkHandle_(db_) && isOpened() && (db_->Put(
            ::leveldb::WriteOptions(),
            ::leveldb::Slice(key.data(),key.size()),
            ::leveldb::Slice(value.data(),value.size())).ok()
        );
    }
    /**
     * @brief Insert new data into database or update the existing record.
     *
     * @param data data record
     *
     * @return If successful, the return value is @c true, else, it is @c false.
     */
    bool update(const data_type& data)
    {
        return update(data.get_key(), data.get_value());
    }

    bool get(const Buffer& key, Buffer& value) const
    {
        if (checkHandle_(db_) && isOpened())
        {
            ::leveldb::Status s = db_->Get(::leveldb::ReadOptions(), ::leveldb::Slice(key.data(),key.size()), &(value.strbuffer()));
            if(s.ok())
            {
                value.attach();
                return true;
            }
        }

        return false;
    }


    bool del(const Buffer& key)
    {
        return checkHandle_(db_) && isOpened() &&
            (db_->Delete(
            ::leveldb::WriteOptions(),
            ::leveldb::Slice(key.data(),key.size())).ok());
    }


private:

    static bool checkHandle_(::leveldb::DB* h)
    {
        //BOOST_ASSERT(h);
        // if (!h) // todo: add logs
        return h;
    }

    ::leveldb::DB* db_;
    bool isOpened_;
    std::string file_;
};

}}}} // namespace izenelib::am::leveldb::raw

#endif // AM_LEVELDB_RAW_TABLE_H