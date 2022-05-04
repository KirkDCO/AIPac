/*	globals.h
	Robert Kirk DeLisle
	18 August 2006

	Purpose:	Contains various global structures, functions, etc.

	Modification History:

*/

#if !defined(globals_RKD_18Aug06)
#define globals_RKD_18Aug06


BITMAPFILEHEADER * DibLoadImage(LPCTSTR FileName);
	//loads a bitmap from the passed file name

void LoadBitmapToDC(HDC *DC);
	//loads a bitmap into the passed DC

#endif