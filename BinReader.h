// 120111
// 120112
// 120806

#ifndef __BinReader_h__
#define __BinReader_h__

#include <string>
#include "FileMapping.h"

class BinReader
{
public:
	BinReader(FileMapping & fm);

	char ReadInt8();
	short ReadInt16();
	int ReadInt32();
	int64 ReadInt64();

	unsigned char ReadUInt8();
	unsigned short ReadUInt16();
	unsigned int ReadUInt32();
	uint64 ReadUInt64();

	float ReadSingle();
	double ReadDouble();

	void ReadWstring(std::wstring & str);
	void ReadAstring(std::string & str);

	bool IsEOF() const;

	uint64 GetPosition() const;
	void SetPosition(uint64 ptr);
	void Reset();

	FileMapping & GetFileMapping() const;

private:
	FileMapping	&m_FileMapping;
	uint64		m_Pointer;
	bool		m_bError;

#ifdef _DEBUG
public:
	size_t		m_ReadCount;
#endif
};

#endif