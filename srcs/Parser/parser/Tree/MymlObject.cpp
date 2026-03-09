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

std::string	MymlObject::getValue(void)
{
	return (_value);
}
