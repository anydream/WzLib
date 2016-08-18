// 120112

#include "BinReader.h"

BinReader::BinReader(FileMapping & fm)
	: m_FileMapping(fm), m_Pointer(0), m_bError(false)
{
#ifdef _DEBUG
	m_ReadCount = 0;
#endif
}

#ifdef _DEBUG
#define DefReadFunc(_name, _type)\
_type BinReader::Read##_name()\
{\
	if(!m_bError)\
	{\
		m_ReadCount++;\
		size_t szType = sizeof(_type);\
		size_t realSz = 0;\
		void * pData = m_FileMapping.Read(m_Pointer, szType, &realSz);\
		if(!pData || realSz != szType)\
		{\
			m_bError = true;\
			return 0;\
		}\
		m_Pointer += szType;\
		return *(_type*)pData;\
	}\
	return 0;\
}
#else
#define DefReadFunc(_name, _type)\
_type BinReader::Read##_name()\
{\
	if(!m_bError)\
	{\
		size_t szType = sizeof(_type);\
		size_t realSz = 0;\
		void * pData = m_FileMapping.Read(m_Pointer, szType, &realSz);\
		if(!pData || realSz != szType)\
		{\
			m_bError = true;\
			return 0;\
		}\
		m_Pointer += szType;\
		return *(_type*)pData;\
	}\
	return 0;\
}
#endif

DefReadFunc(Int8, char);
DefReadFunc(Int16, short);
DefReadFunc(Int32, int);
DefReadFunc(Int64, int64);

DefReadFunc(UInt8, unsigned char);
DefReadFunc(UInt16, unsigned short);
DefReadFunc(UInt32, unsigned int);
DefReadFunc(UInt64, uint64);

DefReadFunc(Single, float);
DefReadFunc(Double, double);

void BinReader::ReadWstring(std::wstring & str)
{
	while(!IsEOF())
	{
		wchar_t ch = ReadUInt16();
		if(!ch) break;
		str.push_back(ch);
	}
}

void BinReader::ReadAstring(std::string & str)
{
	while(!IsEOF())
	{
		char ch = ReadInt8();
 		if(!ch) break;
		str.push_back(ch);
	}
}

bool BinReader::IsEOF() const
{
	return m_bError;
}

uint64 BinReader::GetPosition() const
{
	return m_Pointer;
}

void BinReader::SetPosition(uint64 ptr)
{
	m_Pointer = ptr;
	m_bError = false;
}

void BinReader::Reset()
{
	m_FileMapping.Unmap();

	m_Pointer = 0;
	m_bError = false;
}

FileMapping & BinReader::GetFileMapping() const
{
	return m_FileMapping;
}