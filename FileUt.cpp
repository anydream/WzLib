// 110619
// 110710

#include "FileUt.h"

void * file_read_m(const wchar_t * path, unsigned int * len, unsigned int m)
{
	if(path)
	{
		FILE * fp = 0;

		fopen_p(fp, path, L"rb");
		if(fp)
		{
			void * buf = 0;
			unsigned int flen = 0;

			fseek(fp, 0L, SEEK_END);
			flen = ftell(fp);
			fseek(fp, 0L, SEEK_SET);

			if(flen)
			{
				buf = new char[flen + m];
				if(buf) fread(buf, flen, 1, fp);
			}

			fclose(fp);

			if(len) *len = flen;

			return buf;
		}
	}

	return 0;
}

void * file_read(const wchar_t * path, unsigned int * len)
{
	return file_read_m(path, len, 0);
}

char * file_read_str(const wchar_t * path)
{
	unsigned int len = 0;
	char * buf = (char*)file_read_m(path, &len, 1);
	if(buf)
	{
		buf[len] = 0;
		return buf;
	}
	return 0;
}

bool file_save(const wchar_t * path, const void * buf, unsigned int len)
{
	if(path && buf && len)
	{
		FILE * fp = 0;

		fopen_p(fp, path, L"wb");
		if(fp)
		{
			fwrite(buf, len, 1, fp);
			fclose(fp);

			return true;
		}
	}

	return false;
}

bool file_save_a(const char * path, const void * buf, unsigned int len)
{
	if(path && buf && len)
	{
		FILE * fp = fopen(path, "wb");
		if(fp)
		{
			fwrite(buf, len, 1, fp);
			fclose(fp);

			return true;
		}
	}

	return false;
}

void file_free(void * buf)
{
	if(buf) delete buf;
}
