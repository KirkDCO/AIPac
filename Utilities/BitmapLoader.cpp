/*	BitmapLodaer.cpp
	Robert Kirk DeLisle
	28 August 2006

	Purpose:  Provide functionality to load a bitmap from a file into a DC.
				Mostly stolen from Petzold 5th ed.

	Modificaton History:

*/

#include "BitmapLoader.h"
#include "ExceptionRKD.h"

void BitmapLoader::DibLoadImage(LPCTSTR FileName)
{
	/*	Purpose:	Load the DIB from the specified file

		Parameters:	FileName - DIB to load

		Return:		none - all internal object settings

		Exceptions:

	*/

	hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL,
						OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return;

	dwFileSize = GetFileSize(hFile, &dwHighSize);

	if (dwHighSize)
	{
		CloseHandle(hFile);
		return;
	}

	pbmfh = (BITMAPFILEHEADER *)malloc(dwFileSize);

	if (!pbmfh)
	{
		CloseHandle(hFile);
		return;
	}

	bSuccess = ReadFile(hFile, pbmfh, dwFileSize, &dwBytesRead, NULL);
	CloseHandle(hFile);

	if (!bSuccess || (dwBytesRead != dwFileSize)
				  || (pbmfh->bfType != *(WORD *)"BM")
				  || (pbmfh->bfSize != dwFileSize))
	{
		free(pbmfh);
		return;
	}

	return ;
}

BitmapLoader::~BitmapLoader()
{
	/*	Purpose:

		Parameters:

		Return:

		Exceptions:

	*/

	//we're done with the bitmap as we now have it in memory
	free (pbmfh);
}

void BitmapLoader::OpenBitmap(string Filename)
{
	/*	Purpose:

		Parameters:

		Return:

		Exceptions:

	*/

	//load the desired bitmap
	DibLoadImage((LPCTSTR)Filename.c_str());

	if (pbmfh == NULL)
	{
		throw ExceptionRKD(401,"BitmapLoader::LoadBitmap","Unable to load bitmap.");
	}

	//set the pointers to get access to details of the bitmap
	pbmi = (BITMAPINFO *) (pbmfh +1);
	pbits = (BYTE *) pbmfh + pbmfh->bfOffBits;

	//extract the height and width of tiles
	if ( pbmi->bmiHeader.biSize == sizeof(BITMAPCOREHEADER) )
	{
		m_Height = ((BITMAPCOREHEADER *) pbmi)->bcHeight;
		m_Width = (((BITMAPCOREHEADER *) pbmi)->bcWidth);
	}
	else
	{
		m_Height = pbmi->bmiHeader.biHeight;
		m_Width = (pbmi->bmiHeader.biWidth);
	}

	return;
}

void BitmapLoader::CopyBitmapToDC(HDC *TargetDC)
{
	/*	Purpose:

		Parameters:

		Return:

		Exceptions:

	*/

	SetDIBitsToDevice(  *TargetDC,
						0,0,
						m_Width,m_Height,
						0,0,
						0,m_Height,
						pbits, pbmi,
						DIB_RGB_COLORS);

	return;
}

long BitmapLoader::GetBitmapHeight()
{
	/*	Purpose:

		Parameters:

		Return:

		Exceptions:

	*/

	if (pbmfh==NULL)
		throw ExceptionRKD(402,"BitmapLoader::GetBitmapHeight","Bitmap not loaded.");

	//extract the height and width of tiles
	return m_Height;
}

long BitmapLoader::GetBitmapWidth()
{
	/*	Purpose:

		Parameters:

		Return:

		Exceptions:

	*/

	if (pbmfh==NULL)
		throw ExceptionRKD(403,"BitmapLoader::GetBitmapWidth","Bitmap not loaded.");

	return m_Width;
}
