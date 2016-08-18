// 120114

#ifndef __WzNode_h__
#define __WzNode_h__

#include <map>
#include <vector>
#include <string>

class WzNode;

typedef std::vector<WzNode*>				WzNodeList;
//! 可以使用哈希表
typedef std::map<std::wstring, WzNodeList>	WzNodeMap;

class WzNode
{
public:
	WzNode();

	virtual void AddChild(const std::wstring & name, WzNode * node);

	virtual const WzNodeMap & GetChilds() const;

	virtual void Delete();

	virtual void Free();

	virtual bool FreeChild(const std::wstring & name);

	virtual void FreeChilds();

	virtual WzNode * GetParent();

public:
	static WzNode * New();

protected:
	virtual ~WzNode();

protected:
	WzNode		*m_pParent;
	WzNodeMap	m_Childs;
};

//////////////////////////////////////////////////////////////////////////
class WzDirectory : public WzNode
{
public:
	WzDirectory();

	void Set(bool img, int size ,int checksum ,unsigned int offset);

	unsigned int GetOffset() const;

	bool IsImage() const;

public:
	static WzDirectory * New();

private:
	bool			m_bImage;
	int				m_Size;
	int				m_Checksum;
	unsigned int	m_Offset;
};

//////////////////////////////////////////////////////////////////////////
template <typename T>
class WzProperty : public WzNode
{
public:
	WzProperty() {}

	void Set(T data)
	{
		m_Data = data;
	}

	const T & Get() const
	{
		return m_Data;
	}

public:
	static WzProperty<T> * New()
	{
		return new WzProperty<T>();
	}

private:
	T	m_Data;
};

#endif