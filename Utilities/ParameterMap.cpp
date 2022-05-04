/*	ParameterMap
	Robert Kirk DeLisle
    12 July 2006

	Purpose:	Create an associative map from a parameter file and provide conversion operations for string, int, and double.
                The general format expected is:
                <PARAMETER KEY><space delimiter><PARAMETER VALUE>
                with the <PARAMETER_KEY> being the access key to get back <PARAMETER_VALUE>
                Any line beginning with # is considered a comment and ignored.

	Modification History:
*/

#include "ParameterMap.h"
#include <iostream>
#include <fstream>

map<string, string> ParameterMap::m_ParamMap;

ParameterMap::ParameterMap(string Filename, string Delimiters)
{
    /*	Robert Kirk DeLisle
        12 July 2006

		Purpose:	Constructor to create the Parameter map directly from a param file

		Parameters:	fileName -  The parameter file from which the object will be built.
                                The general format expected is:
                                <PARAMETER KEY><delimiter><PARAMETER VALUE>
                                with the <PARAMETER_KEY> being the access key to get back <PARAMETER_VALUE>
                                Any line beginning with # is considered a comment and ignored.
                                Blank lines are ignored

                    Delimiters - The delimiters to use in the file - defaults to whitespace.
	*/

	try
	{
		ProcessFile(Filename, Delimiters);
	}
	catch(ExceptionRKD &ex)
	{
		throw ex;
	}
	catch(...)
	{
		throw ExceptionRKD(103,"ParameterMap::ParameterMap","Unknown error creating parameter map.");
	}

	return;
}

void ParameterMap::ProcessFile(string Filename, string Delimiters)
{
	/*	Robert Kirk DeLisle
        16 Sep 2006

		Purpose:	Create the Parameter map directly from a param file.
					This was originally all contained in the constructor, but I decided to add the ability
					to create an empty map and then load it with a parameter file.

		Parameters:	fileName -  The parameter file from which the object will be built.
                                The general format expected is:
                                <PARAMETER KEY><delimiter><PARAMETER VALUE>
                                with the <PARAMETER_KEY> being the access key to get back <PARAMETER_VALUE>
                                Any line beginning with # is considered a comment and ignored.
                                Blank lines are ignored

                    Delimiters - The delimiters to use in the file - defaults to whitespace.
	*/

    ifstream inFile;
    string currentLine;

    string::size_type ParamPos;
    string::size_type DelimPos;
    string::size_type ValuePos;
    string::size_type TermPos;

    string::iterator it;
    string::reverse_iterator rit;

    string errorMessage;

    inFile.open(Filename.c_str());
    if ( !inFile ) throw ExceptionRKD(101,"ParameterMap::ParameterMap","Error opening parameter file.");

    //clear out the map if there's anything there
    m_ParamMap.clear();

    while ( !inFile.eof() )
    {
        //get the next line from the parameter file
        getline(inFile, currentLine);

        //ignore blank lines and lines starting with #
        if ( currentLine.empty() || currentLine[0] == '#' )
            continue;

        //throw out anything following # - allows end of line comments
        TermPos = currentLine.find_first_of("#");
        if ( TermPos != string::npos )
        {
            it = currentLine.begin();
            currentLine.assign(it, it+(TermPos-1));
        }

        //delete any leading spaces or tabs
        for ( it = currentLine.begin(); isspace(*it); it++)
            ; //do nothing - just step over the leading spaces
        currentLine.assign(it,currentLine.end());

        //delete any trailing spaces or tabs
        for ( rit = currentLine.rbegin(); isspace(*rit); rit++ )
            ; //do nothing - step over spaces
        currentLine.assign(currentLine.begin(),rit.base());

        //check to see if all this trimming gives us an empty line
        if ( currentLine.empty() )
            continue;

        try
        {
            //find the first delimiter
            DelimPos = currentLine.find_first_of(Delimiters);
            if ( DelimPos == std::string::npos )
            {
                errorMessage = "Parameter: " + currentLine.substr(0,DelimPos-ParamPos)
                                + " not associated with a value.";
                throw ExceptionRKD( 102,"ParameterMap::ParameterMap",errorMessage.c_str() );
            }

            //move to the end of multiple delimiters to find the parameter's value
            ValuePos = currentLine.find_first_not_of(Delimiters,DelimPos);
            //m_ParamMap[currentLine.substr(0,DelimPos)]=currentLine.substr(ValuePos);
            SetParam(currentLine.substr(0,DelimPos),currentLine.substr(ValuePos));
        }
        catch(ExceptionRKD &ex)
        {
            throw ex;
        }
        catch(...)
        {
            throw ExceptionRKD(109,"ParameterMap::ProcessFile","Error creating parameter map.");
        }
    }
}

double ParameterMap::GetDoubleParam(string Key) const
{
    /*	Robert Kirk DeLisle
        12 July 2006

		Purpose:	Retrieve the parameter associated with Key as a double

		Parameters:	Key -   Key into the map associated with a parameter value.

        Return:     A double version of the parameter stored in the map.
                    If the value cannot be converted to a double, an exception is raised.
	*/

    string strErrorMsg;  //used to construct a meaningful error message
    string strMappedParam;  //holds the mapped parameter value for processing
    map<string,string>::iterator mapIter;

    //check to see if Key exists
    //mapIter = m_ParamMap.find("blah");
    if ( m_ParamMap.find(Key) == m_ParamMap.end() )
    {
        strErrorMsg = "Key: " + Key + " not found.";
        throw ExceptionRKD(104,"ParameterMap::GetDoubleParam",strErrorMsg.c_str());
    }

    //extract the value
    strMappedParam = (m_ParamMap.find(Key))->second;

    //verify that it contains only numbers or decimal
    for ( unsigned int x=0; x<strMappedParam.length(); x++)
    {
        if ( isdigit(strMappedParam[x]) == 0 && strMappedParam[x] != '.' && strMappedParam[x] != '-' )
        {
            strErrorMsg = "Key: " + Key + " associated to non-numeric value: " + strMappedParam;
            throw ExceptionRKD(105,"ParameterMap::GetDoubleParam",strErrorMsg.c_str());
        }
    }

    //if we get here, it must be possible to return a double from the parameter value
    return (atof(strMappedParam.c_str()));

}
long ParameterMap::GetLongParam(const string Key) const
{
    /*	Robert Kirk DeLisle
        12 July 2006

		Purpose:	Retrieve the parameter associated with Key as a long

		Parameters:	Key -   Key into the map associated with a parameter value.

        Return:     A long version of the parameter stored in the map.
                    If the value cannot be converted to a double, an exception is raised.
	*/

	string strErrorMsg;  //used to construct a meaningful error message
    string strMappedParam;  //holds the mapped parameter value for processing
    map<string,string>::iterator mapIter;

    //check to see if Key exists
    //mapIter = m_ParamMap.find("blah");
    if ( m_ParamMap.find(Key) == m_ParamMap.end() )
    {
        strErrorMsg = "Key: " + Key + " not found.";
        throw ExceptionRKD(106,"ParameterMap::GetLongParam",strErrorMsg.c_str());
    }

    //extract the value
    strMappedParam = (m_ParamMap.find(Key))->second;

    //verify that it contains only numbers
    for ( unsigned int x=0; x<strMappedParam.length(); x++)
    {
        if ( isdigit(strMappedParam[x]) == 0 && strMappedParam[x] != '-' )
        {
            strErrorMsg = "Key: " + Key + " associated to non-integer value: " + strMappedParam;
            throw ExceptionRKD(107,"ParameterMap::GetLongParam",strErrorMsg.c_str());
        }
    }

    //if we get here, it must be possible to return a long from the parameter value
    return (atol(strMappedParam.c_str()));
}

long ParameterMap::GetIntParam(const string Key) const
{
    /*	Robert Kirk DeLisle
        12 July 2006

		Purpose:	Retrieve the parameter associated with Key as a int

		Parameters:	Key -   Key into the map associated with a parameter value.

        Return:     An INT version of the parameter stored in the map.
                    If the value cannot be converted to a INT, an exception is raised.
	*/

	string strErrorMsg;  //used to construct a meaningful error message
    string strMappedParam;  //holds the mapped parameter value for processing
    map<string,string>::iterator mapIter;

    //check to see if Key exists
    //mapIter = m_ParamMap.find("blah");
    if ( m_ParamMap.find(Key) == m_ParamMap.end() )
    {
        strErrorMsg = "Key: " + Key + " not found.";
        throw ExceptionRKD(107,"ParameterMap::GetIntParam",strErrorMsg.c_str());
    }

    //extract the value
    strMappedParam = (m_ParamMap.find(Key))->second;

    //verify that it contains only numbers
    for ( unsigned int x=0; x<strMappedParam.length(); x++)
    {
        if ( isdigit(strMappedParam[x]) == 0 )
        {
            strErrorMsg = "Key: " + Key + " associated to non-integer value: " + strMappedParam;
            throw ExceptionRKD(107,"ParameterMap::GetIntParam",strErrorMsg.c_str());
        }
    }

    //if we get here, it must be possible to return a long from the parameter value
    return (atoi(strMappedParam.c_str()));
}


string ParameterMap::GetStringParam(const string Key) const
{
    /*	Robert Kirk DeLisle
        12 July 2006

		Purpose:	Retrieve the parameter associated with Key as a string

		Parameters:	Key -   Key into the map associated with a parameter value.

        Return:     A string version of the parameter stored in the map.
                    If the value cannot be converted to a double, an exception is raised.
	*/

	string strErrorMsg;  //used to construct a meaningful error message
    string strMappedParam;  //holds the mapped parameter value for processing
    map<string,string>::iterator mapIter;

    //check to see if Key exists
    //mapIter = m_ParamMap.find("blah");
    if ( m_ParamMap.find(Key) == m_ParamMap.end() )
    {
        strErrorMsg = "Key: " + Key + " not found.";
        throw ExceptionRKD(108,"ParameterMap::GetStringParam",strErrorMsg.c_str());
    }

    //extract the value
    return (m_ParamMap.find(Key))->second;
}

string ParameterMap::SetParam(const string Key, const string Param)
{
    /*	Robert Kirk DeLisle
        12 July 2006

		Purpose:	Set a parameter with the Key - Param tuple

		Parameters:	Key -   Key into the map associated with a parameter value.
                    Param - The parameter to store.

        Return:     the old parameter value if this Key exists
                    an empty string if the key does not exist
	*/

    string returnString;

    //check for the value already existing
    if ( m_ParamMap.find(Key) == m_ParamMap.end() )
    {
        returnString.erase(); //ensure an empty return
    }
    else
    {
        returnString = (m_ParamMap.find(Key))->second;
    }

    //plug in the new value
	m_ParamMap[Key] = Param;

	return returnString;
}

void ParameterMap::SetParamFile(const string Filename, string Delimiters)
{
	/*	Robert Kirk DeLisle
        16 Sep 2006

		Purpose:	Specify a file containg the specific parameters.
					This will clear the existing parameters.

		Parameters:	Filename	-   File containing the key/parameter pairs
                    Delimiters	-	Characters to treat as delimiters - default is whitespace

        Return:     none
	*/

	try
	{
		ProcessFile(Filename, Delimiters);
	}
	catch(ExceptionRKD &ex)
	{
		throw ex;
	}
	catch(...)
	{
		throw ExceptionRKD(110,"ParameterMap::SetParamFile","Unknown error constructing ParameterMap.");
	}

	return;
}





