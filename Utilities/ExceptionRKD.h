/*	ExceptionRKD
	Robert Kirk DeLisle

	Purpose:	Class to be used in exception handling.

	Modification History:
*/

#if !defined(ExceptionRKD_06May2004_INCLUDED)
#define ExceptionRKD_06May2004_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ostream>
#include <string>
using namespace std;

class ExceptionRKD
{
private:

	long m_lErrCode;	//contains an error code value
	const char *m_cMessage;	//an error message
	const char *m_cFunction;	//will contain the name of the function which generated the error

	ExceptionRKD();	//force a construction method

public:

	ExceptionRKD(long ErrCode, const char *FunctionName, const char *Message)
	{  m_lErrCode=ErrCode; m_cMessage=Message; m_cFunction=FunctionName; }

    long ErrorCode() const;
        //return the error code currently in the exception

    string ErrorMessage() const;
        //return the error message currently in the exception

    string ErrorFunction() const;
        //return the name of the function currently in the exception

	~ExceptionRKD() {}

friend ostream& operator<<(ostream& s, const ExceptionRKD &ex);
};

#endif
