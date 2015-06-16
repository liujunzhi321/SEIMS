/*!
 * \file ModelException.h
 * \brief Define ModelException class
 *
 *
 *
 * 
 */
#pragma once

#include <exception>
#include <string>

using namespace std;
/*!
 * \ingroup Util
 * \class ModelException
 *
 * \brief Print the exception message
 *
 *
 *
 */
class ModelException :
	public exception
{
public:
	ModelException(string, string, string);
	~ModelException(void) throw();

	string toString();

    const char* what() const throw()
    {
        string descri = "\n";
        descri = "Class:" + m_className + "\n";
        descri += "Function:" + m_functionName + "\n";
        descri += "Message:" + m_msg;

        return descri.c_str();
    }
private:
	string m_className;
	string m_functionName;
	string m_msg;

};

