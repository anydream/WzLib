
#ifndef __WZ_h__
#define __WZ_h__

#include "BinReader.h"
#include "WzNode.h"

//////////////////////////////////////////////////////////////////////////
struct WzNull
{

};

struct WzSubProp
{

};

struct WzConvex
{

};

struct WzUOL
{
	std::wstring	m_UOL;
};

struct WzCanvas
{
	int		m_Width;
	int		m_Height;
	int		m_Format;
	int		m_Format2;
	bool	m_Encrypted;
	int		m_Size;
	int		m_UncompSize;
	uint64	m_Offset;

	WzCanvas()
		: m_Width(0), m_Height(0),
		m_Format(0), m_Format2(0),
		m_Encrypted(false), m_Size(0), m_UncompSize(0),
		m_Offset(0) {}
};

struct WzSound
{
	int		m_TimeMS;
	int		m_Frequency;
	int		m_Size;
	uint64	m_Offset;

	WzSound() : m_TimeMS(0), m_Frequency(0), m_Size(0), m_Offset(0) {}
};

struct WzVec2D
{
	int		m_X;
	int		m_Y;

	WzVec2D() : m_X(0), m_Y(0) {}
};

//////////////////////////////////////////////////////////////////////////
struct WzFileDesc
{
	unsigned int	m_Start;
	unsigned int	m_Hash;
	short			m_FileVersion;

	WzFileDesc() : m_Start(0), m_Hash(0), m_FileVersion(0) {}
};

//////////////////////////////////////////////////////////////////////////
const unsigned char WzGMSKeyIV[4] =
{
	0x4D, 0x23, 0xC7, 0x2B
};
const unsigned char WzKMSKeyIV[4] =
{
	0xB9, 0x7D, 0x63, 0xE9
};

void WzGenKeys(const unsigned char * IV);
void WzParseListFile(BinReader & reader, std::vector<std::wstring> & strList);
bool WzParseFile(BinReader & reader, WzFileDesc & fdesc, WzNode * root);
bool WzParseImage(BinReader & reader, const WzDirectory * pimg, WzNode * root);
void * WzRebuildCanvas(FileMapping & fm, const WzCanvas & canvas, size_t & szBuf, size_t & bpp);

#endif