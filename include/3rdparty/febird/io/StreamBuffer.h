/* vim: set tabstop=4 : */
#ifndef __febird_io_StreamBuffer_h__
#define __febird_io_StreamBuffer_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "../refcount.h"
#include "var_int.h"
#include <boost/mpl/bool.hpp>
#include <string.h>
#include <stdarg.h>
#include <string>

/**
 @file �� Stream �� Concept, ʵ�ֵ� Buffer

 ���Ժ� Stream һ�����ã����ṩ�˸��ٻ��塪���� C FILE �Ĵ�Լ 20 ��

 - ʹ�û��嵹�ã�
   -# һ���ʵ��(BufferStream)�Ǹ� Stream ���ӻ��幦��
   -# �����ʵ��(StreamBuffer)�Ǹ��������� Stream ����
   -# ����������� Buffer, ���� Stream

 - ��������Ҫͨ�����¼��㼼��ʵ��
   -# �õ�����Ƶ���ĺ��� inline, �����ֺ����������, �� read/ensureRead/readByte, ��
   -# ����Щ inline ������Ƶ����ִ��·����, ִ�����ٵĴ��� @see InputBuffer::readByte
   -# ֻ�м�������»�ִ�еķ�֧, ��װ��һ�����麯��(���麯���ĵ��ô�����麯��С)
   -# ���, inline ������ִ��Ч�ʻ�ǳ���, �� Visual Studio ��, ensureRead �е� memcpy �ڴ�����������ȫ���Ż�����:
     @code
      LittleEndianDataInput<InputBuffer> input(&buf);
      int x;
      input >> x;
      // ������������, input >> x;
      // ����Ƶ����֧�������Ż����˵ȼ۴���: x = *(int*)m_pos;
      // �ײ㺯�����õ� memcpy ��ȫ���Ż�����
     @endcode

 - �������� StreamBuffer: InputBuffer/OutputBuffer/SeekableInputBuffer/SeekableOutputBuffer/SeekableBuffer
   -# ÿ�� buffer �����Ը���(attach)һ��֧����Ӧ���ܵ���
   -# SeekableInputBuffer ����Ҫ�� stream ������ ISeekableInputStream,
      ֻ��Ҫ stream ͬʱʵ���� ISeekable �� IInputStream ����
   -# SeekableBuffer ����Ҫ�� stream �� ISeekableStream,
      ֻ��Ҫ stream ͬʱʵ���� ISeekable/IInputStream/IOutputStream
 */

namespace febird {

class IInputStream;
class IOutputStream;
class ISeekable;

class FEBIRD_DLL_EXPORT IOBufferBase : public RefCounter
{
private:
    // can not copy
    IOBufferBase(const IOBufferBase&);
    const IOBufferBase& operator=(const IOBufferBase&);

public:
    IOBufferBase();
    virtual ~IOBufferBase();

    //! ���� buffer �ߴ粢���� buffer �ڴ�
    //! ��������������ֻ�ܵ���һ��
    void initbuf(size_t capacity);

    //! ����� init ֮ǰ���ã������� buffer �ߴ�
    //! �������·��� buffer ��������Ӧ��ָ��
    void set_bufsize(size_t size);

    byte*  bufbeg() const { return m_beg; }
    byte*  bufcur() const { return m_pos; }
    byte*  bufend() const { return m_end; }

    size_t bufpos()  const { return m_pos-m_beg; }
    size_t bufsize() const { return m_end-m_beg; }
    size_t bufcapacity() const { return m_capacity; }

    //! only rewind buffer, not rewind the back stream
    void rewind_buf() { m_pos = m_end = m_beg; }

    //! only seek in buffer
    //!
    //! when dest stream is null, can seek and used as a memstream
    virtual void skip(ptrdiff_t diff);

    //! set buffer eof
    //!
    //! most for m_is/m_os == 0
    void set_bufeof(size_t eofpos);

    ptrdiff_t buf_remain_bytes() const { return m_end - m_pos; }

protected:
    //! �������� stream.read/write ʱ��ʹ�øú�����ͬ���ڲ� pos ����
    //!
    //! �� non-seekable stream, ��������ǿ�, SeekableBufferBase ��д�˸ú���
    //! @see SeekableBufferBase::update_pos
    virtual void update_pos(size_t inc) {} // empty for non-seekable

protected:
    // dummy, only for OutputBufferBase::attach to use
    void attach(void*) { }

protected:
    // for  InputBuffer, [m_beg, m_pos) is readed,  [m_pos, m_end) is prefetched
    // for OutputBuffer, [m_beg, m_pos) is written, [m_pos, m_end) is undefined

    byte*  m_beg;	// buffer ptr
    byte*  m_pos;	// current read/write position
    byte*  m_end;   // end mark, m_end <= m_beg + m_capacity && m_end >= m_beg
    size_t m_capacity; // buffer capacity
};

class FEBIRD_DLL_EXPORT InputBuffer : public IOBufferBase
{
public:
    typedef boost::mpl::false_ is_seekable;

    explicit InputBuffer(IInputStream* stream = 0)
        : m_is(stream)
    {
    }

    void attach(IInputStream* stream)
    {
        m_is = stream;
    }

    bool eof() { return m_pos == m_end && test_eof(); }

    size_t read(void* vbuf, size_t length)
    {
        if (febird_likely(m_pos+length <= m_end)) {
            memcpy(vbuf, m_pos, length);
            m_pos += length;
            return length;
        } else
            return fill_and_read(vbuf, length);
    }
    void ensureRead(void* vbuf, size_t length)
    {
        // Ϊ��Ч�ʣ���ôʵ�ֿ����ñ��������õ� inline �������
        // inline ��ĺ����岢������С
        if (febird_likely(m_pos+length <= m_end)) {
            memcpy(vbuf, m_pos, length);
            m_pos += length;
        } else
            fill_and_ensureRead(vbuf, length);
    }

    byte readByte()
    {
        if (febird_likely(m_pos < m_end))
            return *m_pos++;
        else
            return fill_and_read_byte();
    }
    int getByte()
    {
        if (febird_likely(m_pos < m_end))
            return *m_pos++;
        else
            return this->fill_and_get_byte();
    }

    void getline(std::string& line, size_t maxlen = UINT_MAX);

    template<class OutputStream>
    void to_output(OutputStream& output, size_t length)
    {
        size_t total = 0;
        while (total < length)
        {
            using namespace std; // for min
            if (febird_unlikely(m_pos == m_end))
                this->fill_and_read(m_beg, m_end-m_beg);
            size_t nWrite = min(size_t(m_end-m_pos), size_t(length-total));
            output.ensureWrite(m_pos, nWrite);
            total += nWrite;
            m_pos += nWrite;
        }
    }

    FEBIRD_DECL_FAST_VAR_INT_READER()

protected:
    size_t fill_and_read(void* vbuf, size_t length);
    void   fill_and_ensureRead(void* vbuf, size_t length);
    byte   fill_and_read_byte();
    int    fill_and_get_byte();
    size_t read_min_max(void* vbuf, size_t min_length, size_t max_length);
    int    test_eof();

    virtual size_t do_fill_and_read(void* vbuf, size_t length);

protected:
    IInputStream* m_is;
};

template<class BaseClass>
class FEBIRD_DLL_EXPORT OutputBufferBase : public BaseClass
{
public:
    typedef boost::mpl::false_ is_seekable;

    explicit OutputBufferBase(IOutputStream* os = 0) : m_os(os)
    {
    }
    virtual ~OutputBufferBase();

    template<class Stream>
    void attach(Stream* stream)
    {
        BaseClass::attach(stream);
        m_os = stream;
    }

    void flush();

    size_t write(const void* vbuf, size_t length)
    {
        if (febird_likely(m_pos+length <= m_end)) {
            memcpy(m_pos, vbuf, length);
            m_pos += length;
            return length;
        } else
            return flush_and_write(vbuf, length);
    }

    void ensureWrite(const void* vbuf, size_t length)
    {
        // Ϊ��Ч�ʣ���ôʵ�ֿ����ñ��������õ� inline �������
        // inline ��ĺ����岢������С
        if (febird_likely(m_pos+length <= m_end)) {
            memcpy(m_pos, vbuf, length);
            m_pos += length;
        } else
            flush_and_ensureWrite(vbuf, length);
    }

    void writeByte(byte b)
    {
        if (febird_likely(m_pos < m_end))
            *m_pos++ = b;
        else
            flush_and_write_byte(b);
    }

    template<class InputStream>
    void from_input(InputStream& input, size_t length)
    {
        size_t total = 0;
        while (total < length)
        {
            using namespace std; // for min
            if (febird_unlikely(m_pos == m_end))
                flush_buffer();
            size_t nRead = min(size_t(m_end-m_pos), size_t(length-total));
            input.ensureRead(m_pos, nRead);
            total += nRead;
            m_pos += nRead;
        }
    }

    size_t printf(const char* format, ...)
#ifdef __GNUC__
    __attribute__ ((__format__ (__printf__, 2, 3)))
#endif
    ;

    size_t vprintf(const char* format, va_list ap)
#ifdef __GNUC__
    __attribute__ ((__format__ (__printf__, 2, 0)))
#endif
    ;

protected:
    size_t flush_and_write(const void* vbuf, size_t length);
    void   flush_and_ensureWrite(const void* vbuf, size_t length);
    void   flush_and_write_byte(byte b);

    virtual size_t do_flush_and_write(const void* vbuf, size_t length);

    virtual void flush_buffer(); // only write to m_os, not flush m_os

protected:
    IOutputStream* m_os;
    using BaseClass::m_pos;
    using BaseClass::m_beg;
    using BaseClass::m_end;
    using BaseClass::m_capacity;
};

class FEBIRD_DLL_EXPORT OutputBuffer : public OutputBufferBase<IOBufferBase>
{
public:
    explicit OutputBuffer(IOutputStream* os = 0)
       	: OutputBufferBase<IOBufferBase> (os)
    { }

    FEBIRD_DECL_FAST_VAR_INT_WRITER()
};

template<class BaseClass>
class FEBIRD_DLL_EXPORT SeekableBufferBase : public BaseClass
{
protected:
    using BaseClass::m_beg;
    using BaseClass::m_pos;
    using BaseClass::m_end;
    using BaseClass::m_capacity;

public:
    typedef boost::mpl::true_ is_seekable;

    //! constructor
    //!
    //! ����� append ��ʽ��������� m_stream_pos �ǲ��Ե�
    //! ����һ����������º��ٻ���� seek/tell
    //! �������ô�����ᵼ��δ������Ϊ
    explicit SeekableBufferBase()
    {
        m_seekable = 0;
        m_stream_pos = 0;
    }

    template<class Stream>
    void attach(Stream* stream)
    {
        BaseClass::attach(stream);
        m_seekable = stream;
    }

    void seek(stream_position_t pos);
    void seek(stream_offset_t offset, int origin);

    void skip(ptrdiff_t diff);

    stream_position_t tell() const;
    stream_position_t size() const;

protected:
    virtual void update_pos(size_t inc); //!< override
    virtual void invalidate_buffer() = 0;

    //! �����Ԥȡ��m_stream_pos ��Ӧ������ĩβ m_end
    //! ���� m_stream_pos ��Ӧ��������ʼ
    virtual int is_prefetched() const = 0;

protected:
    ISeekable* m_seekable;
    stream_position_t m_stream_pos;
};

class FEBIRD_DLL_EXPORT SeekableInputBuffer : public SeekableBufferBase<InputBuffer>
{
    typedef SeekableBufferBase<InputBuffer> super;
public:
    SeekableInputBuffer() { }
protected:
    virtual void invalidate_buffer();
    virtual int is_prefetched() const;
};

class FEBIRD_DLL_EXPORT SeekableOutputBuffer : public SeekableBufferBase<OutputBuffer>
{
    typedef SeekableBufferBase<OutputBuffer> super;

public:
//	typedef boost::mpl::true_ is_seekable;

    //! constructor
    //!
    //! ����� append ��ʽ��������� m_stream_pos �ǲ��Ե�
    //! ����һ����������º��ٻ���� seek/tell
    //! �������ô�����ᵼ��δ������Ϊ
    SeekableOutputBuffer() {}

protected:
    virtual void invalidate_buffer();
    virtual int is_prefetched() const;
};

class FEBIRD_DLL_EXPORT SeekableBuffer :
    public SeekableBufferBase<OutputBufferBase<InputBuffer> >
{
    typedef SeekableBufferBase<OutputBufferBase<InputBuffer> > super;

public:
    SeekableBuffer();
    ~SeekableBuffer();

    size_t read(void* vbuf, size_t length)
    {
        if (febird_likely(m_pos+length <= m_end && m_prefetched)) {
            memcpy(vbuf, m_pos, length);
            m_pos += length;
            return length;
        } else
            return fill_and_read(vbuf, length);
    }
    void ensureRead(void* vbuf, size_t length)
    {
        // Ϊ��Ч�ʣ���ôʵ�ֿ����ñ��������õ� inline �������
        // inline ��ĺ����岢������С
        if (febird_likely(m_pos+length <= m_end && m_prefetched)) {
            memcpy(vbuf, m_pos, length);
            m_pos += length;
        } else
            fill_and_ensureRead(vbuf, length);
    }

    byte readByte()
    {
        if (febird_likely(m_pos < m_end && m_prefetched))
            return *m_pos++;
        else
            return fill_and_read_byte();
    }
    int getByte()
    {
        if (febird_likely(m_pos < m_end && m_prefetched))
            return *m_pos++;
        else
            return fill_and_get_byte();
    }

    size_t write(const void* vbuf, size_t length)
    {
        m_dirty = true;
        return super::write(vbuf, length);
    }

    void ensureWrite(const void* vbuf, size_t length)
    {
        m_dirty = true;
        super::ensureWrite(vbuf, length);
    }

    void writeByte(byte b)
    {
        m_dirty = true;
        super::writeByte(b);
    }

protected:
    virtual size_t do_fill_and_read(void* vbuf, size_t length) ; //!< override
    virtual size_t do_flush_and_write(const void* vbuf, size_t length) ; //!< override

    virtual void flush_buffer(); //!< override
    virtual void invalidate_buffer(); //!< override
    virtual int is_prefetched() const;

private:
    int m_dirty;
    int m_prefetched;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class FEBIRD_DLL_EXPORT FileStreamBuffer : public SeekableBuffer
{
public:
    explicit FileStreamBuffer(const char* fname, const char* mode, size_t capacity = 8*1024);
    ~FileStreamBuffer();
};

} // febird

#endif // __febird_io_StreamBuffer_h__
