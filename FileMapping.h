// 120111

#ifndef __FileMapping_h__
#define __FileMapping_h__

#include <windows.h>

#ifdef _MSC_VER
	typedef __int64 int64;
	typedef unsigned __int64 uint64;
#else
	typedef long long int64;
	typedef unsigned long long uint64;
#endif

class FileMapping
{
public:
	enum ReadHint
	{
		RANDOM = 0,
		SEQUENTIAL,
	};
public:
	FileMapping();
	~FileMapping();

	bool Open(const wchar_t * path, ReadHint rhint = RANDOM);
	void Close();
	void Unmap();

	void * Read(uint64 addr, size_t len, size_t * revlen);

	size_t GetBlockBytes() const;
	uint64 GetFileLen() const;

private:
	HANDLE	m_hFileMap;
	uint64	m_qwFileSize;

	size_t	m_dwGran;
	size_t	m_dwBlockBytes;

	void	*m_pMapped;
	uint64	m_qwMappedOffset;
	uint64	m_qwMappedBlockSize;

#ifdef _DEBUG
public:
	size_t	m_ReadCount;
#endif
};

#endif