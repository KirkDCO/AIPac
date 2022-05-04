/*	ParameterMap
	Robert Kirk DeLisle
    12 July 2006

	Purpose:	Create an associative map from a parameter file and provide conversion operations for string, int, and double.
                The general format expected is:
                <PARAMETER KEY><space delimiter><PARAMETER VALUE>
                with the <PARAMETER_KEY> being the access key to get back <PARAMETER_VALUE>
                Any line beginning with # is considered a comment and ignored.

	Modification History:
                16 January 2008:
                    Added the ability to deal with terminal comments.  Anything following an internal
                    "#" symbol is ignored.
                    <parameter name> <parameter value> #ignored text
*/

#if !defined(ParameterMap_12Jul2006_INCLUDED)
#define ParameterMap_12Jul2006_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <string>
#include <map>
using namespace std;

#include "ExceptionRKD.h"

class ParameterMap
{
private:

    static map<string, string> m_ParamMap;

	void ProcessFile(string Filename, string Delimiters);
		/*	Workhorse of the class - extracts parameters/key duples from Filename using Delimiters  */

public:

    ParameterMap() {};
        /*  Empty constructor allows in process construction of parameters  */

    ParameterMap(string fileName, string Delimiters="\t ");
        /*  Constructor to create the Parameter map directly from a param file  */

	void SetParamFile(const string Filename, string Delimiters="\t ");
		/*  Specify a file containg the specific parameters.  */

    double GetDoubleParam(string Key) const;
        /*  Retrieve the parameter associated with Key as a double  */

    long GetIntParam(const string Key) const;
        /*  Retrieve the parameter associated with Key as an int  */

    long GetLongParam(string Key) const;
        /*  Retrieve the parameter associated with Key as a long  */

    string GetStringParam(string Key) const;
        /*  Retrieve the parameter associated with Key as a string  */

    string SetParam(const string Key, const string Param);
        /*  Set a parameter with the Key - Param tuple  */

    ~ParameterMap() {  }
        /*  Destructor only needs to clear out the map to make sure references are cleared  */
};

#endif
