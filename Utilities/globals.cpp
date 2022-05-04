/*	globals.cpp
	Robert Kirk DeLisle
	18 August 2006

	Purpose:	Contains various global structures, functions, etc.

	Modification History:

*/

#include <windows.h>

void LoadBitmapToDC(HDC *DC, HBITMAP *Bitmap, long &BitmapHeight, long &BitmapWidth)
{
	BITMAPINFO *pbmi;
	BYTE *pbits;
	BITMAPFILEHEADER *pbmfh

	//load the desired bitmap
	pbmfh = DibLoadImage((LPCTSTR)Filename.c_str());

	if (m_pBitmapHeader == NULL)
	{
		throw ExceptionRKD(301,"MapView::SetBitmap","Unable to load bitmap.");
	}

	//set the pointers to get access to details of the bitmap
	pbmi = (BITMAPINFO *) (pbmfh +1);
	pbits = (BYTE *) pbmfh + pbmfh->bfOffBits;

	//extract the height and width of tiles
	if ( pbmi->bmiHeader.biSize == sizeof(BITMAPCOREHEADER) )
	{
		BitmapHeight = ((BITMAPCOREHEADER *) pbmi)->bcHeight;
		BitmapWidth = (((BITMAPCOREHEADER *) pbmi)->bcWidth);
	}
	else
	{
		BitmapHeight = pbmi->bmiHeader.biHeight;
		BitmapWidth = (pbmi->bmiHeader.biWidth);
	}

	//target DC has to be created here and a bitmap loaded into it for drawing

	SetDIBitsToDevice(  *DC,
						0,0,
						BitmapWidth,BitmapHeight,
						0,0,
						0,BitmapHeight,
						pbits, pbmi,
						DIB_RGB_COLORS);

	//we're done with the bitmap as we now have it in memory
	free (pbmfh);

	return;
}

BITMAPFILEHEADER * DibLoadImage(LPCTSTR FileName)
{

	BOOL				bSuccess;
	DWORD				dwFileSize, dwHighSize, dwBytesRead;
	HANDLE				hFile;
	BITMAPFILEHEADER *	pbmfh;

	hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL,
						OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;

	dwFileSize = GetFileSize(hFile, &dwHighSize);

	if (dwHighSize)
	{
		CloseHandle(hFile);
		return NULL;
	}

	pbmfh = (BITMAPFILEHEADER *)malloc(dwFileSize);

	if (!pbmfh)
	{
		CloseHandle(hFile);
		return NULL;
	}

	bSuccess = ReadFile(hFile, pbmfh, dwFileSize, &dwBytesRead, NULL);
	CloseHandle(hFile);

	if (!bSuccess || (dwBytesRead != dwFileSize)
				  || (pbmfh->bfType != *(WORD *)"BM")
				  || (pbmfh->bfSize != dwFileSize))
	{
		free(pbmfh);
		return NULL;
	}

	return pbmfh;
}

