
#include <assert.h>
#include "WzNode.h"

//////////////////////////////////////////////////////////////////////////
WzNode::WzNode() : m_pParent(0) {}

WzNode::~WzNode() {}

void WzNode::AddChild(const std::wstring & name, WzNode * node)
{
	assert(node);
	m_Childs[name].push_back(node);
	node->m_pParent = this;
}

const WzNodeMap & WzNode::GetChilds() const
{
	return m_Childs;
}

void WzNode::Delete()
{
	delete this;
}

void WzNode::Free()
{
	for(WzNodeMap::iterator it = m_Childs.begin();
		it != m_Childs.end();
		it++)
	{
		for(WzNodeList::iterator cit = it->second.begin();
			cit != it->second.end();
			cit++)
		{
			(*cit)->Free();
		}
	}

	Delete();
}

bool WzNode::FreeChild(const std::wstring & name)
{
	WzNodeMap::iterator it = m_Childs.find(name);
	if(it != m_Childs.end())
	{
		for(WzNodeList::iterator cit = it->second.begin();
			cit != it->second.end();
			cit++)
		{
			(*cit)->Free();
		}

		m_Childs.erase(it);
		return true;
	}
	return false;
}

void WzNode::FreeChilds()
{
	for(WzNodeMap::iterator it = m_Childs.begin();
		it != m_Childs.end();
		it++)
	{
		for(WzNodeList::iterator cit = it->second.begin();
			cit != it->second.end();
			cit++)
		{
			(*cit)->Free();
		}
	}
	m_Childs.clear();
}

WzNode * WzNode::GetParent()
{
	return m_pParent;
}

WzNode * WzNode::New()
{
	return new WzNode();
}

//////////////////////////////////////////////////////////////////////////
WzDirectory::WzDirectory() : m_bImage(false), m_Size(0), m_Checksum(0), m_Offset(0) {}

void WzDirectory::Set(bool img, int size ,int checksum ,unsigned int offset)
{
	m_bImage = img;
	m_Size = size;
	m_Checksum = checksum;
	m_Offset = offset;
}

unsigned int WzDirectory::GetOffset() const
{
	return m_Offset;
}

bool WzDirectory::IsImage() const
{
	return m_bImage;
}

WzDirectory * WzDirectory::New()
{
	return new WzDirectory();
}