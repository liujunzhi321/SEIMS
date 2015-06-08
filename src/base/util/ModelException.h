#pragma once

#include <exception>
#include <string>

using namespace std;

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

