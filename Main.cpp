
#include "MemLeaks.h"
#include <assert.h>
#include <stdio.h>
#include <conio.h>
#include <locale.h>
#include "FileUt.h"
#include "WZ.h"

void DisplayData(WzNode * root)
{
	if(WzProperty<WzNull> * pProp = dynamic_cast<WzProperty<WzNull>*>(root))
	{
		wprintf(L"** null\n");
	}
	else if(WzProperty<unsigned short> * pProp = dynamic_cast<WzProperty<unsigned short>*>(root))
	{
		wprintf(L"** ushort: [%u]\n", pProp->Get());
	}
	else if(WzProperty<int> * pProp = dynamic_cast<WzProperty<int>*>(root))
	{
		wprintf(L"** int: [%d]\n", pProp->Get());
	}
	else if(WzProperty<float> * pProp = dynamic_cast<WzProperty<float>*>(root))
	{
		wprintf(L"** float: [%f]\n", pProp->Get());
	}
	else if(WzProperty<double> * pProp = dynamic_cast<WzProperty<double>*>(root))
	{
		wprintf(L"** double: [%lf]\n", pProp->Get());
	}
	else if(WzProperty<std::wstring> * pProp = dynamic_cast<WzProperty<std::wstring>*>(root))
	{
		wprintf(L"** string: [%s]\n", pProp->Get().c_str());
	}
	else if(WzProperty<WzSubProp> * pProp = dynamic_cast<WzProperty<WzSubProp>*>(root))
	{
		wprintf(L"** sub property\n");
	}
	else if(WzProperty<WzCanvas> * pProp = dynamic_cast<WzProperty<WzCanvas>*>(root))
	{
		wprintf(L"** canvas: [%d, %d], fmt(%d) %s\n",
			pProp->Get().m_Width, pProp->Get().m_Height,
			pProp->Get().m_Format + pProp->Get().m_Format2,
			pProp->Get().m_Encrypted ? L"Encrypted" : L"");
	}
	else if(WzProperty<WzVec2D> * pProp = dynamic_cast<WzProperty<WzVec2D>*>(root))
	{
		wprintf(L"** vector 2d: [%d, %d]\n", pProp->Get().m_X, pProp->Get().m_Y);
	}
	else if(WzProperty<WzConvex> * pProp = dynamic_cast<WzProperty<WzConvex>*>(root))
	{
		wprintf(L"** convex\n");
	}
	else if(WzProperty<WzSound> * pProp = dynamic_cast<WzProperty<WzSound>*>(root))
	{
		wprintf(L"** sound: [%d]\n", pProp->Get().m_Size);
	}
	else if(WzProperty<WzUOL> * pProp = dynamic_cast<WzProperty<WzUOL>*>(root))
	{
		wprintf(L"** link: [%s]\n", pProp->Get().m_UOL.c_str());
	}
	else if(WzDirectory * pProp = dynamic_cast<WzDirectory*>(root))
	{
		wprintf(L"** %s\n", pProp->IsImage() ? L"image" : L"directory");
	}
	else wprintf(L"** root\n");
}

void Win32_DrawPixels(HDC hDC,
					  const RECT & dstRC,
					  const POINT & srcPt,
					  void * pSrc, size_t len,
					  const POINT & szSrc,
					  WORD bitCount)
{
	static BITMAPINFO s_origBMI =
	{
		{
			sizeof(BITMAPINFO),
				0, 0, 1, bitCount,
				BI_RGB, 0, 0, 0, 0, 0
		},
		0
	};

	// 如果带 Alpha 通道, 则计算半透明值
	if(bitCount == 32)
	{
		unsigned char * pBuf = (unsigned char*)pSrc;
		for(size_t i = 0; i < len; i += 4)
		{
			// BGRA
			float alp = pBuf[i + 3] / 255.f;
			float inva = 1.0f - alp;

			pBuf[i] *= alp;
			pBuf[i + 1] *= alp;
			pBuf[i + 2] *= alp;

			// 混色
			/*pBuf[i] += inva * 216.f;
			pBuf[i + 1] += inva * 233.f;
			pBuf[i + 2] += inva * 236.f;*/
		}
	}

	BITMAPINFO bmi;
	memmove(&bmi, &s_origBMI, sizeof(BITMAPINFO));
	memmove(&bmi.bmiHeader.biWidth, &szSrc, sizeof(POINT));

	SetDIBitsToDevice(hDC,
		dstRC.left, dstRC.top, dstRC.right, dstRC.bottom,
		srcPt.x, srcPt.y, 0, dstRC.bottom,
		pSrc,
		&bmi,
		DIB_RGB_COLORS);
}

void DirLoop(BinReader & reader, WzNode * root)
{
	bool bShowIdx = false;
	int index = 0;
	wchar_t strInput[256];
	std::vector<std::wstring> strDirs;

	strDirs.push_back(L"root");

	while(1)
	{
		system("cls");

		wprintf(L"-- [Dir]: ");
		for(std::vector<std::wstring>::const_iterator dit = strDirs.begin();
			dit != strDirs.end();
			dit++)
		{
			wprintf(L"%s/", dit->c_str());
		}
		if(bShowIdx) wprintf(L"\tIndex: [%d]", index);
		bShowIdx = false;

		wprintf(L"\n");

		const WzNodeMap & childs = root->GetChilds();

		if(!childs.size())
		{
			WzDirectory * pdir = dynamic_cast<WzDirectory*>(root);
			if(pdir && pdir->IsImage())
			{
				WzParseImage(reader, pdir, root);
			}
		}

		DisplayData(root);

		std::map<int, std::wstring> strIDmap;
		int nid = 0;

		if(!childs.size())
		{
			wprintf(L"-- [No child data, input \'/\' to back]\n");
		}
		else
		{
			for(WzNodeMap::const_iterator it = childs.begin();
				it != childs.end();
				it++)
			{
				wprintf(L"%d\t%s\n", nid, it->first.c_str());
				strIDmap[nid] = it->first.c_str();
				nid++;
			}
		}

		wprintf(L"-- [Input]: ");
		wscanf(L"%s", strInput);

_InputOK:
		WzNodeMap::const_iterator fit = childs.find(strInput);
		if(fit != childs.end())
		{
			if(!fit->second.size())
			{
				wprintf(L"-- [No any data]\n");
				getch();
				continue;
			}
			else if(fit->second.size() > 1)
			{
				wprintf(L"-- [Index (0~%d)]: ", fit->second.size() - 1);
				wscanf(L"%d", &index);

				if(index >= fit->second.size())
					index = fit->second.size() - 1;

				bShowIdx = true;
			}

			root = fit->second[index];
			strDirs.push_back(strInput);
		}
		else if(!wcsicmp(strInput, L"/"))
		{
			WzNode * pnode = root->GetParent();
			if(pnode)
			{
				WzDirectory * pdir = dynamic_cast<WzDirectory*>(root);
				if(pdir && pdir->IsImage())
				{
					pdir->FreeChilds();
				}

				root = pnode;
				strDirs.pop_back();
			}
		}
		else if(!wcsicmp(strInput, L"/exit"))
		{
#ifdef _DEBUG
			printf("BinReader: %d, FileMapping: %d\n",
				reader.m_ReadCount,
				reader.GetFileMapping().m_ReadCount);
			getch();
#endif
			break;
		}
		else if(!wcsicmp(strInput, L"/d"))
		{
			WzProperty<WzCanvas> * pCanvas = dynamic_cast<WzProperty<WzCanvas>*>(root);
			if(pCanvas)
			{
				FileMapping & fm = reader.GetFileMapping();

				size_t szBuf = 0;
				size_t bpp = 0;
				void * pBuf = WzRebuildCanvas(fm, pCanvas->Get(), szBuf, bpp);

				if(pBuf)
				{
					/*std::wstring strPath = L"./";
					strPath += *strDirs.rbegin();
					strPath += L".png";
					file_save(strPath.c_str(), pBuf, szBuf);*/

					POINT szSrc = { pCanvas->Get().m_Width, pCanvas->Get().m_Height };
					RECT dstRc = { 0, 30, szSrc.x, szSrc.y };
					POINT srcPt = { 0, 0 };
					// 翻转图像
					szSrc.y = -szSrc.y;

					HWND hConWnd = GetConsoleWindow();
					HDC hDC = GetDC(hConWnd);
					
					system("cls");
					printf("Press any key to back.");
					Win32_DrawPixels(hDC, dstRc, srcPt, pBuf, szBuf, szSrc, bpp);

					ReleaseDC(hConWnd, hDC);

					free(pBuf);

					getch();
				}
			}
			else
			{
				WzProperty<WzSound> * pSound = dynamic_cast<WzProperty<WzSound>*>(root);
				if(pSound)
				{
					FileMapping & fm = reader.GetFileMapping();

					size_t rsize = 0;
					void * pBuf = fm.Read(pSound->Get().m_Offset, pSound->Get().m_Size, &rsize);
					if(pBuf)
					{
						std::wstring strPath = L"./";
						strPath += *strDirs.rbegin();
						strPath += L".mp3";
						file_save(strPath.c_str(), pBuf, rsize);
					}
				}
			}
		}
		else if(strInput[0] == L'/' && nid)
		{
			int tid = 0;
			swscanf(strInput + 1, L"%d", &tid);
			if(tid >= nid || tid < 0) goto _Error;

			wcscpy(strInput, strIDmap[tid].c_str());
			goto _InputOK;
		}
		else
		{
_Error:
			wprintf(L"-- [Error command]\n");
			getch();
		}
	}
}

int wmain(int argc, wchar_t ** argv)
{
	CHECK_MEM_LEAKS;
	setlocale(LC_CTYPE, "chs");

	wchar_t currDir[MAX_PATH];
	size_t strDirLen = wcslen(argv[0]);
	memmove(currDir, argv[0], (strDirLen + 1) * sizeof(wchar_t));
	for(size_t i = strDirLen; i != 0; i--)
	{
		if(currDir[i] == L'\\' || currDir[i] == L'/')
		{
			currDir[i] = 0;
			break;
		}
	}
	SetCurrentDirectoryW(currDir);

#ifdef _DEBUG
	GetCurrentDirectoryW(MAX_PATH, currDir);
	wprintf(L"Current Dir: %s\n", currDir);
	getch();
#endif

	FileMapping fm;
	BinReader reader(fm);

	WzGenKeys(WzKMSKeyIV);

	if(argc > 1)	// 传入参数
	{
		if(fm.Open(argv[1]))
		{
			WzFileDesc fdesc;
			WzNode * root = WzNode::New();

			if(!WzParseFile(reader, fdesc, root))
			{
				std::vector<std::wstring> strList;

				reader.Reset();
				WzParseListFile(reader, strList);

				for(std::vector<std::wstring>::const_iterator it = strList.begin();
					it != strList.end();
					it++)
				{
					wprintf(L"%s\n", it->c_str());
				}
				getch();
			}
			else
			{
				DirLoop(reader, root);
			}

			root->Free();

			fm.Close();
		}
	}

	return 0;
}