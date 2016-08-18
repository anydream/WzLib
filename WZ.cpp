
#include <assert.h>
#include <malloc.h>
#include "WZ.h"
#include "AES.h"
#include "ZLIB/zlib.h"

//////////////////////////////////////////////////////////////////////////
static const unsigned char g_AESKey1[] =
{
	0x13, 0x52, 0x2A, 0x5B,	0x08, 0x02, 0x10, 0x60,	0x06, 0x02, 0x43, 0x0F,	0xB4, 0x4B, 0x35, 0x05,
	0x1B, 0x0A, 0x5F, 0x09,	0x0F, 0x50, 0x0C, 0x1B,	0x33, 0x55, 0x01, 0x09,	0x52, 0xDE, 0xC7, 0x1E
};

static const unsigned char g_AESKey2[] =
{
	0x13, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,	0x06, 0x00, 0x00, 0x00, 0xB4, 0x00, 0x00, 0x00,
	0x1B, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,	0x33, 0x00, 0x00, 0x00, 0x52, 0x00, 0x00, 0x00
};

static const unsigned char g_ShuffleKey[] =
{
	0xEC, 0x3F, 0x77, 0xA4, 0x45, 0xD0, 0x71, 0xBF, 0xB7, 0x98, 0x20, 0xFC, 0x4B, 0xE9, 0xB3, 0xE1,
	0x5C, 0x22, 0xF7, 0x0C, 0x44, 0x1B, 0x81, 0xBD, 0x63, 0x8D, 0xD4, 0xC3, 0xF2, 0x10, 0x19, 0xE0,
	0xFB, 0xA1, 0x6E, 0x66, 0xEA, 0xAE, 0xD6, 0xCE, 0x06, 0x18, 0x4E, 0xEB, 0x78, 0x95, 0xDB, 0xBA,
	0xB6, 0x42, 0x7A, 0x2A, 0x83, 0x0B, 0x54, 0x67, 0x6D, 0xE8, 0x65, 0xE7, 0x2F, 0x07, 0xF3, 0xAA,
	0x27, 0x7B, 0x85, 0xB0, 0x26, 0xFD, 0x8B, 0xA9, 0xFA, 0xBE, 0xA8, 0xD7, 0xCB, 0xCC, 0x92, 0xDA,
	0xF9, 0x93, 0x60, 0x2D, 0xDD, 0xD2, 0xA2, 0x9B, 0x39, 0x5F, 0x82, 0x21, 0x4C, 0x69, 0xF8, 0x31,
	0x87, 0xEE, 0x8E, 0xAD, 0x8C, 0x6A, 0xBC, 0xB5, 0x6B, 0x59, 0x13, 0xF1, 0x04, 0x00, 0xF6, 0x5A,
	0x35, 0x79, 0x48, 0x8F, 0x15, 0xCD, 0x97, 0x57, 0x12, 0x3E, 0x37, 0xFF, 0x9D, 0x4F, 0x51, 0xF5,
	0xA3, 0x70, 0xBB, 0x14, 0x75, 0xC2, 0xB8, 0x72, 0xC0, 0xED, 0x7D, 0x68, 0xC9, 0x2E, 0x0D, 0x62,
	0x46, 0x17, 0x11, 0x4D, 0x6C, 0xC4, 0x7E, 0x53, 0xC1, 0x25, 0xC7, 0x9A, 0x1C, 0x88, 0x58, 0x2C,
	0x89, 0xDC, 0x02, 0x64, 0x40, 0x01, 0x5D, 0x38, 0xA5, 0xE2, 0xAF, 0x55, 0xD5, 0xEF, 0x1A, 0x7C,
	0xA7, 0x5B, 0xA6, 0x6F, 0x86, 0x9F, 0x73, 0xE6, 0x0A, 0xDE, 0x2B, 0x99, 0x4A, 0x47, 0x9C, 0xDF,
	0x09, 0x76, 0x9E, 0x30, 0x0E, 0xE4, 0xB2, 0x94, 0xA0, 0x3B, 0x34, 0x1D, 0x28, 0x0F, 0x36, 0xE3,
	0x23, 0xB4, 0x03, 0xD8, 0x90, 0xC8, 0x3C, 0xFE, 0x5E, 0x32, 0x24, 0x50, 0x1F, 0x3A, 0x43, 0x8A,
	0x96, 0x41, 0x74, 0xAC, 0x52, 0x33, 0xF0, 0xD9, 0x29, 0x80, 0xB1, 0x16, 0xD3, 0xAB, 0x91, 0xB9,
	0x84, 0x7F, 0x61, 0x1E, 0xCF, 0xC5, 0xD1, 0x56, 0x3D, 0xCA, 0xF4, 0x05, 0xC6, 0xE5, 0x08, 0x49
};

static const unsigned char g_DefaultKey[] =
{
	0xC6, 0x50, 0x53, 0xF2, 0xA8, 0x42, 0x9D, 0x7F, 0x77, 0x09, 0x1D, 0x26, 0x42, 0x53, 0x88, 0x7C
};

static const unsigned char g_ShuffleConstant[] =
{
	0xF2, 0x53, 0x50, 0xC6
};

static const unsigned int g_OffsetKey = 0x581C3F6D;

static const unsigned int g_HeaderMagic = 0x31474B50;
//////////////////////////////////////////////////////////////////////////

static unsigned char g_WzKey[0x10000];

//////////////////////////////////////////////////////////////////////////
static void WzDecryptString(const std::wstring & stringToDecrypt, std::wstring & outputString)
{
	assert(outputString.size() >= stringToDecrypt.size());
	size_t szToDec = stringToDecrypt.size();
	for(size_t i = 0; i < szToDec; i++)
		outputString[i] = stringToDecrypt[i] ^ *(wchar_t*)(g_WzKey + 2 * i);
}

static void WzReadString(BinReader & reader, std::wstring & outputString)
{
	int smallLength = reader.ReadInt8();

	if(smallLength == 0)
	{
		return;
	}

	int length;

	if(smallLength > 0)	// Unicode
	{
		wchar_t mask = 0xAAAA;
		if(smallLength == 127)
		{
			length = reader.ReadInt32();
		}
		else
		{
			length = smallLength;
		}
		if(length <= 0)
		{
			return;
		}

		for(int i = 0; i < length; i++)
		{
			wchar_t encryptedChar = reader.ReadUInt16();
			encryptedChar ^= mask;
			encryptedChar ^= *(wchar_t*)(g_WzKey + 2 * i);
			outputString.push_back(encryptedChar);
			mask++;
		}
	}
	else	// ASCII
	{
		unsigned char mask = 0xAA;
		if(smallLength == -128)
		{
			length = reader.ReadInt32();
		}
		else
		{
			length = -smallLength;
		}
		if(length <= 0)
		{
			return;
		}

		for(int i = 0; i < length; i++)
		{
			unsigned char encryptedChar = reader.ReadUInt8();
			encryptedChar ^= mask;
			encryptedChar ^= g_WzKey[i];
			outputString.push_back((wchar_t)encryptedChar);
			mask++;
		}
	}
}

static void WzReadStringAtOffset(BinReader & reader, uint64 Offset, std::wstring & outputString)
{
	uint64 position = reader.GetPosition();
	reader.SetPosition(Offset);
	WzReadString(reader, outputString);
	reader.SetPosition(position);
}

static void WzReadStringBlock(BinReader & reader, unsigned int offset, std::wstring & outputString)
{
	switch(reader.ReadUInt8())
	{
	case 0:
	case 0x73:
		WzReadString(reader, outputString);
		break;
	case 1:
	case 0x1B:
		WzReadStringAtOffset(reader, offset + reader.ReadUInt32(), outputString);
		break;
	default:
		{
			assert(0);
		}
	}
}

static int WzReadCompressedInt(BinReader & reader)
{
	char ch = reader.ReadInt8();
	if(ch == -128) return reader.ReadInt32();
	return ch;
}

static unsigned int WzGetVersionHash(int encver, int realver)
{
	int EncryptedVersionNumber = encver;
	int VersionNumber = realver;
	int VersionHash = 0;
	int DecryptedVersionNumber = 0;
	std::wstring VersionNumberStr;
	int a = 0, b = 0, c = 0, d = 0, l = 0;

	wchar_t stmp[16];
	swprintf(stmp, L"%d", VersionNumber);
	VersionNumberStr = stmp;

	l = VersionNumberStr.size();
	for(int i = 0; i < l; i++)
	{
		VersionHash = (32 * VersionHash) + (int)VersionNumberStr[i] + 1;
	}
	a = (VersionHash >> 24) & 0xFF;
	b = (VersionHash >> 16) & 0xFF;
	c = (VersionHash >> 8) & 0xFF;
	d = VersionHash & 0xFF;
	DecryptedVersionNumber = (0xFF ^ a ^ b ^ c ^ d);

	if(EncryptedVersionNumber == DecryptedVersionNumber) return (unsigned int)VersionHash;
	else return 0;
}

static unsigned int WzReadOffset(BinReader & reader, const WzFileDesc & fdesc)
{
	unsigned int offset = (unsigned int)reader.GetPosition();
	offset = ~(offset - fdesc.m_Start);// ^ 0xFFFFFFFF;
	offset *= fdesc.m_Hash;
	offset -= g_OffsetKey;
	offset = (offset << (offset & 0x1F)) | (offset >> (32 - (offset & 0x1F)));
	unsigned int encryptedOffset = reader.ReadUInt32();
	offset ^= encryptedOffset;
	offset += fdesc.m_Start * 2;
	return offset;
}

static bool WzIsImage(BinReader & reader)
{
	unsigned char tcode = reader.ReadUInt8();

	if(tcode != 0x73) return false;
	else
	{
		std::wstring tstr;
		WzReadString(reader, tstr);
		if(tstr != L"Property") return false;
		else if(reader.ReadUInt16() != 0) return false;
	}

	return true;
}

static WzCanvas WzParseCanvasProperty(BinReader & reader)
{
	WzCanvas canvas;
	canvas.m_Width = WzReadCompressedInt(reader);
	canvas.m_Height = WzReadCompressedInt(reader);
	canvas.m_Format = WzReadCompressedInt(reader);
	canvas.m_Format2 = reader.ReadUInt8();
	reader.ReadUInt32();
	canvas.m_Size = reader.ReadInt32() - 1;
	reader.ReadUInt8();

	canvas.m_Offset = reader.GetPosition();

	unsigned short header = reader.ReadUInt16();
	// 这两个是 zlib 压缩数据的魔数
	if(header != 0x9C78 && header != 0xDA78)
	{
		canvas.m_Encrypted = true;
	}

	switch(canvas.m_Format + canvas.m_Format2)
	{
	case 1:
		{
			canvas.m_UncompSize = canvas.m_Width * canvas.m_Height * 2;
		}
		break;
	case 2:
		{
			canvas.m_UncompSize = canvas.m_Width * canvas.m_Height * 4;
		}
		break;
	case 513:	// Format16bppRgb565
		{
			canvas.m_UncompSize = canvas.m_Width * canvas.m_Height * 2;
		}
		break;
	case 517:
		{
			canvas.m_UncompSize = canvas.m_Width * canvas.m_Height / 128;
		}
		break;
	}

	reader.SetPosition(canvas.m_Offset + canvas.m_Size);

	return canvas;
}

static void * WzDeflateCanvas(FileMapping & fm, const WzCanvas & canvas)
{
	if(canvas.m_Encrypted)
	{
		BinReader reader(fm);
		reader.SetPosition(canvas.m_Offset);
		uint64 endOfCanvas = canvas.m_Offset + canvas.m_Size;

		int blockSize = 0;
		std::vector<unsigned char> dataStream;

		while(reader.GetPosition() < endOfCanvas)
		{
			blockSize = reader.ReadInt32();
			for(int i = 0; i < blockSize; i++)
			{
				dataStream.push_back(reader.ReadUInt8() ^ g_WzKey[i]);
			}
		}

		size_t dstLen = canvas.m_UncompSize;
		void * pBufDst = malloc(dstLen);
		uncompress((Bytef*)pBufDst, (uLongf*)&dstLen, (Bytef*)&dataStream[0], dataStream.size());
		return pBufDst;
	}
	else
	{
		size_t revlen = 0;
		void * pBufSrc = fm.Read(canvas.m_Offset, canvas.m_Size, &revlen);
		if(pBufSrc)
		{
			size_t dstLen = canvas.m_UncompSize;
			void * pBufDst = malloc(dstLen);
			uncompress((Bytef*)pBufDst, (uLongf*)&dstLen, (Bytef*)pBufSrc, revlen);
			return pBufDst;
		}
	}
	return 0;
}

void * WzRebuildCanvas(FileMapping & fm, const WzCanvas & canvas, size_t & szBuf, size_t & bpp)
{
	unsigned char * decBuf = (unsigned char*)WzDeflateCanvas(fm, canvas);
	unsigned char * argb = 0;
	if(decBuf)
	{
		switch(canvas.m_Format + canvas.m_Format2)
		{
		case 1:
			{
				int b, g;
				bpp = 32;
				szBuf = canvas.m_UncompSize * 2;
				argb = (unsigned char*)malloc(szBuf);
				for(int i = 0; i < canvas.m_UncompSize; i++)
				{
					b = decBuf[i] & 0x0F;
					b |= (b << 4);
					argb[i * 2] = (unsigned char)b;

					g = decBuf[i] & 0xF0;
					g |= (g >> 4);
					argb[i * 2 + 1] = (unsigned char)g;
				}
			}
			break;
		case 2:		// 32bpp ARGB8
			{
				bpp = 32;
				szBuf = canvas.m_UncompSize;
				// 直接克隆一份
				argb = (unsigned char*)malloc(szBuf);
				memmove(argb, decBuf, szBuf);
			}
			break;
		case 513:	// 16bpp RGB565
			{
				bpp = 16;
				szBuf = canvas.m_UncompSize;
				// 直接克隆一份
				argb = (unsigned char*)malloc(szBuf);
				memmove(argb, decBuf, szBuf);
			}
			break;
		case 517:	// 二值图
			{
				bpp = 8;
				szBuf = canvas.m_Width * canvas.m_Height;
				argb = (unsigned char*)malloc(szBuf);
				int x = 0, y = 0;
				unsigned char iB = 0;
				for(int i = 0; i < canvas.m_UncompSize; i++)
				{
					for(unsigned char j = 0; j < 8; j++)
					{
						iB = ((decBuf[i] & (0x01 << (7 - j))) >> (7 - j)) * 0xFF;
						for(int k = 0; k < 16; k++)
						{
							// 跳到下一行
							if(x == canvas.m_Width)
							{
								x = 0;
								y++;
							}
							argb[y * canvas.m_Width + x] = iB;
							x++;
						}
					}
				}
			}
			break;
		}

		free(decBuf);
	}

	return argb;
}

static WzSound WzParseSoundProperty(BinReader & reader)
{
	WzSound sound;
	reader.ReadUInt8();
	sound.m_Size = WzReadCompressedInt(reader);
	sound.m_TimeMS = WzReadCompressedInt(reader);
	reader.SetPosition(reader.GetPosition() + 56);
	sound.m_Frequency = reader.ReadInt32();
	reader.SetPosition(reader.GetPosition() + 22);

	sound.m_Offset = reader.GetPosition();

	reader.SetPosition(sound.m_Offset + sound.m_Size);

	return sound;
}

static bool WzParsePropertyList(BinReader & reader, unsigned int offset, WzNode * root);

static void WzParseExtendedProp(BinReader & reader, unsigned int offset,
								const std::wstring & name, WzNode * root)
{
	std::wstring strPropName;
	WzReadStringBlock(reader, offset, strPropName);

	if(strPropName == L"Property")
	{
		WzProperty<WzSubProp> * pProp = WzProperty<WzSubProp>::New();
		reader.ReadUInt16();
		WzParsePropertyList(reader, offset, pProp);
		root->AddChild(name, pProp);
	}
	else if(strPropName == L"Canvas")
	{
		WzProperty<WzCanvas> * pProp = WzProperty<WzCanvas>::New();
		reader.ReadUInt8();
		if(reader.ReadUInt8() == 1)
		{
			reader.ReadUInt16();
			WzParsePropertyList(reader, offset, pProp);
		}

		pProp->Set(WzParseCanvasProperty(reader));

		root->AddChild(name, pProp);
	}
	else if(strPropName == L"Shape2D#Vector2D")
	{
		WzProperty<WzVec2D> * pProp = WzProperty<WzVec2D>::New();

		WzVec2D vec2;
		vec2.m_X = WzReadCompressedInt(reader);
		vec2.m_Y = WzReadCompressedInt(reader);
		pProp->Set(vec2);

		root->AddChild(name, pProp);
	}
	else if(strPropName == L"Shape2D#Convex2D")
	{
		WzProperty<WzConvex> * pProp = WzProperty<WzConvex>::New();

		int convexEntryCount = WzReadCompressedInt(reader);
		for(int i = 0; i < convexEntryCount; i++)
		{
			WzParseExtendedProp(reader, offset, name, pProp);
		}

		root->AddChild(name, pProp);
	}
	else if(strPropName == L"Sound_DX8")
	{
		WzProperty<WzSound> * pProp = WzProperty<WzSound>::New();

		pProp->Set(WzParseSoundProperty(reader));

		root->AddChild(name, pProp);
	}
	else if(strPropName == L"UOL")
	{
		reader.ReadUInt8();
		WzProperty<WzUOL> * pProp = WzProperty<WzUOL>::New();
		WzUOL uol;
		WzReadStringBlock(reader, offset, uol.m_UOL);
		pProp->Set(uol);
		root->AddChild(name, pProp);
	}
	else
	{
		assert(0);
	}
}

//////////////////////////////////////////////////////////////////////////
static bool WzParsePropertyList(BinReader & reader, unsigned int offset, WzNode * root)
{
	int entryCount = WzReadCompressedInt(reader);

	for(int i = 0; i < entryCount; i++)
	{
		std::wstring name;
		WzReadStringBlock(reader, offset, name);

		unsigned char utype = reader.ReadUInt8();
		switch(utype)
		{
		case 0:
			{
				WzProperty<WzNull> * pProp = WzProperty<WzNull>::New();
				root->AddChild(name, pProp);
			}
			break;
		case 0x0B:
		case 2:
			{
				WzProperty<unsigned short> * pProp = WzProperty<unsigned short>::New();
				pProp->Set(reader.ReadUInt16());
				root->AddChild(name, pProp);
			}
			break;
		case 3:
			{
				WzProperty<int> * pProp = WzProperty<int>::New();
				pProp->Set(WzReadCompressedInt(reader));
				root->AddChild(name, pProp);
			}
			break;
		case 4:
			{
				unsigned char type = reader.ReadUInt8();
				if(type == 0x80)
				{
					WzProperty<float> * pProp = WzProperty<float>::New();
					pProp->Set(reader.ReadSingle());
					root->AddChild(name, pProp);
				}
				else if(type == 0)
				{
					WzProperty<float> * pProp = WzProperty<float>::New();
					pProp->Set(0.0f);
					root->AddChild(name, pProp);
				}
			}
			break;
		case 5:
			{
				WzProperty<double> * pProp = WzProperty<double>::New();
				pProp->Set(reader.ReadDouble());
				root->AddChild(name, pProp);
			}
			break;
		case 8:
			{
				WzProperty<std::wstring> * pProp = WzProperty<std::wstring>::New();
				std::wstring str;
				WzReadStringBlock(reader, offset, str);
				pProp->Set(str);
				root->AddChild(name, pProp);
			}
			break;
		case 9:
			{
				unsigned int ofs = reader.ReadUInt32();
				uint64 eob = reader.GetPosition() + ofs;
				WzParseExtendedProp(reader, offset, name, root);
				if(reader.GetPosition() != eob) reader.SetPosition(eob);
			}
			break;
		default:
			{
				assert(0);
				return false;
			}
		}
	}

	return true;
}

static bool WzParseDirectory(BinReader & reader, const WzFileDesc & fdesc, WzNode * root)
{
	int entryCount = WzReadCompressedInt(reader);

	for(int i = 0; i < entryCount; i++)
	{
		unsigned char type = reader.ReadUInt8();
		std::wstring fname;
		int fsize = 0;
		int checksum = 0;
		unsigned int offset = 0;

		uint64 rememberPos = 0;

		if(type == 1)
		{
			int unknown = reader.ReadInt32();
			reader.ReadInt16();
			unsigned int offs = WzReadOffset(reader, fdesc);
			continue;
		}
		else if(type == 2)
		{
			int stringOffset = reader.ReadInt32();
			rememberPos = reader.GetPosition();
			reader.SetPosition(fdesc.m_Start + stringOffset);
			type = reader.ReadUInt8();
			WzReadString(reader, fname);
		}
		else if(type == 3 || type == 4)
		{
			WzReadString(reader, fname);
			rememberPos = reader.GetPosition();
		}
		else
		{
			assert(0);
		}

		reader.SetPosition(rememberPos);
		fsize = WzReadCompressedInt(reader);
		checksum = WzReadCompressedInt(reader);
		offset = WzReadOffset(reader, fdesc);

		if(!root && (offset >= reader.GetFileMapping().GetFileLen())) return false;

		if(type == 3)
		{
			if(root)
			{
				WzDirectory * pDir = WzDirectory::New();
				pDir->Set(false, fsize, checksum, offset);
				root->AddChild(fname, pDir);
			}
		}
		else
		{
			if(root)
			{
				WzDirectory * pDir = WzDirectory::New();
				pDir->Set(true, fsize, checksum, offset);
				root->AddChild(fname, pDir);
			}
			else
			{
				rememberPos = reader.GetPosition();

				reader.SetPosition(offset);
				if(!WzIsImage(reader)) return false;

				reader.SetPosition(rememberPos);
			}
		}
	}

	if(root)
	{
		WzNodeMap nmap = root->GetChilds();
		for(WzNodeMap::const_iterator it = nmap.begin();
			it != nmap.end();
			it++)
		{
			for(WzNodeList::const_iterator cit = it->second.begin();
				cit != it->second.end();
				cit++)
			{
				WzNode * pnode = *cit;
				WzDirectory * pdir = dynamic_cast<WzDirectory*>(pnode);

				if(pdir)
				{
					if(pdir->IsImage())
					{

					}
					else
					{
						reader.SetPosition(pdir->GetOffset());
						WzParseDirectory(reader, fdesc, pdir);
					}
				}
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void WzGenKeys(const unsigned char * IV)
{
	AES AESGen;
	unsigned char BigIV[16];

	for(int i = 0; i < 16; i += 4)
	{
		memmove(BigIV + i, IV, 4);
	}

	AESGen.SetParameters(256, 128);
	AESGen.StartEncryption(g_AESKey2);
	AESGen.EncryptBlock(BigIV, g_WzKey);

	for(int i = 16; i < 0x10000; i += 16)
	{
		AESGen.EncryptBlock(g_WzKey + i - 16, g_WzKey + i);
	}
}

void WzParseListFile(BinReader & reader, std::vector<std::wstring> & strList)
{
	while(!reader.IsEOF())
	{
		int len = reader.ReadInt32();
		std::wstring str, dstr;
		str.resize(len);
		dstr.resize(len);

		for(int i = 0; i < len; i++)
		{
			str[i] = reader.ReadUInt16();
		}

		reader.ReadUInt16();

		WzDecryptString(str, dstr);
		strList.push_back(dstr);
	}
}

bool WzParseFile(BinReader & reader, WzFileDesc & fdesc, WzNode * root)
{
	unsigned int hdr = reader.ReadUInt32();
	if(hdr != g_HeaderMagic) return false;
	uint64 fsize = reader.ReadUInt64();
	unsigned int fstart = reader.ReadUInt32();

	std::string copyright;
	reader.ReadAstring(copyright);

	reader.SetPosition(fstart);

	short encVersion = reader.ReadInt16();

	short fileVersion = -1;
	unsigned int versionHash = 0;

	for(int j = 0; j < 0x7FFF; j++)
	{
		fileVersion = (short)j;
		versionHash = WzGetVersionHash(encVersion, fileVersion);
		if(versionHash != 0)
		{
			fdesc.m_Start = fstart;
			fdesc.m_Hash = versionHash;
			fdesc.m_FileVersion = fileVersion;
			
			uint64 position = reader.GetPosition();
			if(!WzParseDirectory(reader, fdesc, 0))
			{
				reader.SetPosition(position);
				continue;
			}
			else
			{
				if(root)
				{
					reader.SetPosition(position);
					WzParseDirectory(reader, fdesc, root);
				}
				return true;
			}
		}
	}

	return false;
}

bool WzParseImage(BinReader & reader, const WzDirectory * pimg, WzNode * root)
{
	if(pimg && pimg->IsImage())
	{
		reader.SetPosition(pimg->GetOffset());
		if(WzIsImage(reader))
		{
			return WzParsePropertyList(reader, pimg->GetOffset(), root);
		}
	}
	return false;
}