/*	ParameterSingleton
	Robert Kirk DeLisle
    13 March 2007

	Purpose:	Extend the to become a singleton

	Modification History:
*/

#if !defined(ParameterSingleton_12Jul2006_INCLUDED)
#define ParameterSingleton_12Jul2006_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ParameterMap.h"

class ParameterSingleton
{
private:

    static ParameterMap m_ParamMap;

public:

    ParameterSingleton() {};
        /*  Empty constructor allows in process construction of parameters  */

    ParameterSingleton(string fileName, string Delimiters="\t ")
    {  m_ParamMap.SetParamFile(fileName, Delimiters);  };
        /*  Constructor to create the Parameter map directly from a param file  */

	void SetParamFile(const string Filename, string Delimiters="\t ")
	{  m_ParamMap.SetParamFile(Filename, Delimiters);  };
		/*  Specify a file containg the specific parameters.  */

    double GetDoubleParam(string Key) const
    {  return m_ParamMap.GetDoubleParam(Key);  };
        /*  Retrieve the parameter associated with Key as a double  */

    long GetLongParam(string Key) const
    {  return m_ParamMap.GetLongParam(Key);  };
        /*  Retrieve the parameter associated with Key as a long  */

    string GetStringParam(string Key) const
    {  return m_ParamMap.GetStringParam(Key);  };
        /*  Retrieve the parameter associated with Key as a string  */

    string SetParam(const string Key, const string Param)
    {  return m_ParamMap.SetParam(Key, Param);  };
        /*  Set a parameter with the Key - Param tuple  */

    ~ParameterSingleton() {  };
};

#endif
