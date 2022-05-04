/*	ExceptionRKD implementation
	Robert Kirk DeLisle

	Purpose:	Class to be used in exception handling.

	Modification History:
*/

#include "ExceptionRKD.h"

ostream& operator<<(ostream &s, const ExceptionRKD &ex)
{
	/*	Robert Kirk DeLisle

		Purpose:	Allow streaming output of exception contents.

		Parameters:	s	-	reference to the ostream object
					ex	-	reference to the ExceptionRKD object

		Return:
	*/

	s << ex.m_lErrCode << " : " << ex.m_cFunction << " : " << ex.m_cMessage;

	return s;
}

long ExceptionRKD::ErrorCode() const
{
    /*	Robert Kirk DeLisle

		Purpose:	Return the ErrorCode value

		Parameters:	none

		Return:     The error code value currently in the exception.
	*/

    return m_lErrCode;	//contains an error code value
}

string ExceptionRKD::ErrorMessage() const
{
    /*	Robert Kirk DeLisle

		Purpose:	return the error message currently in the exception

		Parameters:	none

		Return:     The error message currently in the exception.
	*/

    return string(m_cMessage);
}

string ExceptionRKD::ErrorFunction() const
{
    /*	Robert Kirk DeLisle

		Purpose:	return the name of the function currently in the exception

		Parameters:	none

		Return:     The name of the function currently in the exception.
	*/

    return string(m_cFunction);
}

