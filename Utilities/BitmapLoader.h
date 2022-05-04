/*	BitmapLodaer.h
	Robert Kirk DeLisle
	28 August 2006

	Purpose:  Provide functionality to load a bitmap from a file into a DC.

	Modificaton History:

*/

#if !defined(BitmapLodaer_RKD_28Aug06)
#define BitmapLoader_RKD_28Aug06

#include <windows.h>

#include <string>
using namespace std;

class BitmapLoader
{
	private:

		BITMAPINFO *pbmi;
		BYTE *pbits;
		BITMAPFILEHEADER *pbmfh;
		BOOL				bSuccess;
		DWORD				dwFileSize, dwHighSize, dwBytesRead;
		HANDLE				hFile;

		long m_Width;
		long m_Height;

		void DibLoadImage(LPCTSTR FileName);

	public:

		BitmapLoader()
			{  bSuccess = false;  pbmfh = NULL;  };
			//set the flags to false/NULL to make sure we know whether things are working

		~BitmapLoader();
			//reclaim

		void OpenBitmap(string Filename);
		void CopyBitmapToDC(HDC *TargetDC);

		long GetBitmapHeight();
		long GetBitmapWidth();
			//return dimensions but do validation on bSuccess

};


#endif
