#include "MymlObject.hpp"

MymlObject::MymlObject(void)
{}

// for value string only, canot stock any list or dictionary
MymlObject::MymlObject(const std::string value) : _value(value)
{}

MymlObject::~MymlObject(void)
{}

void MymlObject::insert(MymlObject *value)
{
	(void) value;
}

void MymlObject::insert(const std::pair<std::string, MymlObject *> value)
{
	(void) value;
}

std::string	MymlObject::getAsString(void)
{
	return (_value);
}

int	MymlObject::getAsInt(void)
{
	std::string::iterator it = _value.begin();

	if (*it == '-')
		it++;
	for (; it != _value.end(); it++) {
		if (!isdigit(*it))
			throw BadCast("the value [" + _value + "] is not an integer !");
	}
	return (std::atoi(_value.c_str()));
}

double	MymlObject::getAsFloat(void)
{
	int dot = 0;
	std::string::iterator it = _value.begin();

	if (*it == '-')
		it++;
	for (; it != _value.end(); it++) {
		if (!isdigit(*it) || !(*it == '.' && dot++ == 0))
			throw BadCast("the value [" + _value + "] is not an integer !");
	}
	return (atof(_value.c_str()));
}

bool	MymlObject::getAsBool(void)
{
	if (_value == "true" || _value == "1")
		return (1);
	else if (_value == "false" || _value == "0")
		return (0);
	else {
		throw BadCast("the value [" + _value + "] is not an boolean !");
	}
}
