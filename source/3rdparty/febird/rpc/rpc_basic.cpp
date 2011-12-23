/* vim: set tabstop=4 : */
#include <febird/rpc/rpc_basic.h>

namespace febird { namespace rpc {

	RoCreator::~RoCreator()
	{
	}

	remote_object::~remote_object()
	{

	}

	rpc_exception::rpc_exception(const char* szMsg)
		: m_message(szMsg)
	{ }
	rpc_exception::rpc_exception(const std::string& szMsg)
		: m_message(szMsg)
	{ }
	rpc_exception::~rpc_exception() throw() {}

	const char* rpc_exception::what() const throw() { return m_message.c_str(); }


	//! ʹ�� classID ��������
	remote_object* ObjectFactoryBase::create(unsigned classID) const
	{
		const RoCreator* meta = m_map.get_byid(classID);
		remote_object* obj = meta->create();
		obj->m_classMeta = meta;
		return obj;
	}
	//! ʹ�� className ��������
	remote_object* ObjectFactoryBase::create(const std::string& className) const
	{
		const RoCreator* meta = m_map.get_byname(className);
		if (meta)
		{
			remote_object* obj = meta->create();
			obj->m_classMeta = meta;
			return obj;
		}
		std::ostringstream oss;
		oss << "ObjectFactory::create: can not find class: " << className;
		throw std::runtime_error(oss.str());
	}
	//! @brief ������Ϣ���빤��
	//! ���������Ϊ meta ����һ�� id���� id �����������ڶ���Ч
	bool ObjectFactoryBase::add(RoCreator* meta)
	{
		assert(meta);
		RoCreator* meta2;
		long classID = m_map.add_ptr(meta, meta->className, &meta2);
		if (meta2 == meta)
		{
			return false;
		}
		else if (NULL != meta2)
		{
			throw std::runtime_error("class existed");
		}
		meta->classID = classID;
		return true;
	}
	const RoCreator* ObjectFactoryBase::FindMetaByName(const std::string& className) const
	{
		const RoCreator* meta = m_map.get_byname(className);
		return meta;
	}

} } // namespace febird::rpc

