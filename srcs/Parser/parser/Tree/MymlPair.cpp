#include "MymlPair.hpp"

MymlPair::MymlPair(std::string &key, std::string &value) : _pair(std::pair<std::string, std::string>(key, value))
{}

MymlPair::~MymlPair(void)
{}

