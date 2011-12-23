/* vim: set tabstop=4 : */
#ifndef DataBuffer_h__
#define DataBuffer_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
# pragma warning(disable: 4819)
#endif

#include <cstddef>
#include <boost/detail/atomic_count.hpp>
#include <boost/smart_ptr.hpp>
#include "config.h"
#include "stdtypes.h"

namespace febird {

//! forward declaration
//class FEBIRD_DLL_EXPORT DataBufferPtr;

/**
 @brief ֻ��ͨ�� DataBufferPtr ʹ�øö���
 */
class FEBIRD_DLL_EXPORT DataBuffer
{
	boost::detail::atomic_count m_refcount;
	size_t m_size;

private:
	DECLARE_NONE_COPYABLE_CLASS(DataBuffer)
	DataBuffer(size_t size);
	~DataBuffer(); // disable

	static DataBuffer* create(size_t size);
	static void destroy(DataBuffer* p);

	friend inline void intrusive_ptr_add_ref(DataBuffer* p) { ++p->m_refcount; }
	friend inline void intrusive_ptr_release(DataBuffer* p) {
		if (0 == --p->m_refcount) DataBuffer::destroy(p);
	}
	friend class DataBufferPtr;

public:
	size_t size() const { return m_size; }
	byte*  data() const { return (byte*)(this + 1); }
//	byte*  end()  const { return (byte*)(this + 1) + m_size; }
};

/**
 @brief ����ӵ���Լ��� buffer

 -# DataBufferPtr ��ռһ��ָ��Ŀռ�
 -# DataBufferPtr ָ���Ŀ����һ���������ڴ�
 */
class FEBIRD_DLL_EXPORT DataBufferPtr : public boost::intrusive_ptr<DataBuffer>
{
	typedef boost::intrusive_ptr<DataBuffer> MyBase;
public:
	DataBufferPtr() {}
	explicit DataBufferPtr(size_t size);
};


/**
 @brief ���Ը�Ч�ر���Լ�ӵ�еĻ��߷��Լ����е� buffer

 -# ������Լ�ӵ�е� buffer�������ö���ʹ�����ü��������� buffer
 -# ��������Լ�ӵ�е� buffer����������ָ��
 -# ֱ��ʹ�øö��󼴿ɣ�����ҪΪ��Ч��ʹ��ָ��ö��������ָ��
 */
class FEBIRD_DLL_EXPORT SmartBuffer
{
public:
	explicit SmartBuffer(size_t size = 0);
	SmartBuffer(void* vbuf, size_t size) {
		m_data = (byte*)vbuf;
		m_size = size;
		m_refcountp = NULL;
	}
	SmartBuffer(void* pBeg, void* pEnd) {
		m_data = (byte*)pBeg;
		m_size = (byte*)pEnd - (byte*)pBeg;
		m_refcountp = NULL;
	}
	SmartBuffer(const SmartBuffer& rhs);
	~SmartBuffer();

	const SmartBuffer& operator=(const SmartBuffer& rhs);

	void swap(SmartBuffer& y) {
		std::swap(m_data, y.m_data);
		std::swap(m_size, y.m_size);
		std::swap(m_refcountp, y.m_refcountp);
	}
	long refcount() const { return m_refcountp ? *m_refcountp : 0; }

	size_t size() const { return m_size; }
	byte*  data() const { return m_data; }

private:
	byte*  m_data;
	size_t m_size;
	boost::detail::atomic_count* m_refcountp;
};


} // namespace febird

#endif // DataBuffer_h__
