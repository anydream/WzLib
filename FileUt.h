// 110619

#ifndef __FileUt_h__
#define __FileUt_h__

#include <stdio.h>
#include <malloc.h>

//////////////////////////////////////////////////////////////////////////
// 操作内存中的数据的辅助宏
#define AS_UCHAR(_p)			(((unsigned char*)_p) += sizeof(unsigned char), *(unsigned char*)(((unsigned char*)_p) - sizeof(unsigned char)))
#define AS_USHORT(_p)			(((unsigned char*)_p) += sizeof(unsigned short), *(unsigned short*)(((unsigned char*)_p) - sizeof(unsigned short)))
#define AS_SHORT(_p)			(((unsigned char*)_p) += sizeof(short), *(short*)(((unsigned char*)_p) - sizeof(short)))
#define AS_UINT(_p)				(((unsigned char*)_p) += sizeof(unsigned int), *(unsigned int*)(((unsigned char*)_p) - sizeof(unsigned int)))
#define AS_CHAR(_p)				(((unsigned char*)_p) += sizeof(char), *(char*)(((unsigned char*)_p) - sizeof(char)))
#define AS_INT(_p)				(((unsigned char*)_p) += sizeof(int), *(int*)(((unsigned char*)_p) - sizeof(int)))
#define AS_FLOAT(_p)			(((unsigned char*)_p) += sizeof(float), *(float*)(((unsigned char*)_p) - sizeof(float)))

#define READ_CHAR3(_o, _p)		((_o)[0] = AS_CHAR(_p), (_o)[1] = AS_CHAR(_p), (_o)[2] = AS_CHAR(_p))
#define READ_UCHAR3(_o, _p)		((_o)[0] = AS_UCHAR(_p), (_o)[1] = AS_UCHAR(_p), (_o)[2] = AS_UCHAR(_p))
#define READ_UCHAR4(_o, _p)		((_o)[0] = AS_UCHAR(_p), (_o)[1] = AS_UCHAR(_p), (_o)[2] = AS_UCHAR(_p), (_o)[3] = AS_UCHAR(_p))
#define READ_USHORT2(_o, _p)	((_o)[0] = AS_USHORT(_p), (_o)[1] = AS_USHORT(_p))
#define READ_USHORT3(_o, _p)	((_o)[0] = AS_USHORT(_p), (_o)[1] = AS_USHORT(_p), (_o)[2] = AS_USHORT(_p))
#define READ_USHORT4(_o, _p)	((_o)[0] = AS_USHORT(_p), (_o)[1] = AS_USHORT(_p), (_o)[2] = AS_USHORT(_p), (_o)[3] = AS_USHORT(_p))
#define READ_FLOAT3(_o, _p)		((_o)[0] = AS_FLOAT(_p), (_o)[1] = AS_FLOAT(_p), (_o)[2] = AS_FLOAT(_p))
#define READ_FLOAT4(_o, _p)		((_o)[0] = AS_FLOAT(_p), (_o)[1] = AS_FLOAT(_p), (_o)[2] = AS_FLOAT(_p), (_o)[3] = AS_FLOAT(_p))

#define WRITE_CHAR3(_i, _p)		(AS_CHAR(_p) = (_i)[0], AS_CHAR(_p) = (_i)[1], AS_CHAR(_p) = (_i)[2])
#define WRITE_UCHAR3(_i, _p)	(AS_UCHAR(_p) = (_i)[0], AS_UCHAR(_p) = (_i)[1], AS_UCHAR(_p) = (_i)[2])
#define WRITE_USHORT3(_i, _p)	(AS_USHORT(_p) = (_i)[0], AS_USHORT(_p) = (_i)[1], AS_USHORT(_p) = (_i)[2])
#define WRITE_USHORT4(_i, _p)	(AS_USHORT(_p) = (_i)[0], AS_USHORT(_p) = (_i)[1], AS_USHORT(_p) = (_i)[2], AS_USHORT(_p) = (_i)[3])
#define WRITE_FLOAT3(_i, _p)	(AS_FLOAT(_p) = (_i)[0], AS_FLOAT(_p) = (_i)[1], AS_FLOAT(_p) = (_i)[2])
#define WRITE_FLOAT4(_i, _p)	(AS_FLOAT(_p) = (_i)[0], AS_FLOAT(_p) = (_i)[1], AS_FLOAT(_p) = (_i)[2], AS_FLOAT(_p) = (_i)[3])

//////////////////////////////////////////////////////////////////////////
// 文件函数的定义
#if (!defined _MSC_VER) || (_MSC_VER <= 1200)
#  define fopen_p(_fptr, _path, _mode)	((_fptr) = _wfopen(_path, _mode))
#else
#  define fopen_p(_fptr, _path, _mode)	(_wfopen_s(&(_fptr), _path, _mode))
#endif

inline void * ut_alloc(size_t n)
{
	if(n) return malloc(n);
	return 0;
}

inline void * ut_realloc(void * p, size_t n)
{
	if(n) return realloc(p, n);
	else if(p) free(p);
	return 0;
}

void * file_read_m(const wchar_t * path, unsigned int * len, unsigned int m);
void * file_read(const wchar_t * path, unsigned int * len);
char * file_read_str(const wchar_t * path);
bool file_save(const wchar_t * path, const void * buf, unsigned int len);
bool file_save_a(const char * path, const void * buf, unsigned int len);
void file_free(void * buf);


#endif