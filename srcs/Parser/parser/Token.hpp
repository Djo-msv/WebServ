#pragma once

#include <iostream>

// Comment token
#define HASH				0

// basic token
#define SPACE				1
#define INDENTATION			2
#define DASH				3
#define DOUBLE_QUOTE		4
#define QUOTE				5
#define COLON				6
#define COMMA				7
#define OPEN_BRACE			8
#define CLOSE_BRACE			9
#define OPEN_BRACKET		10
#define CLOSE_BRACKET		11

// type of value
#define STRING				12
#define INTEGER				13
#define FLOAT				14
#define BOOL				15
#define NULL_VALUE			16

// cast of value
#define STRING_CAST			17
#define INTEGER_CAST		18
#define FLOAT_CAST			19
#define DOUBLE_CAST			20
#define BOOL_CAST			21

// alias
#define AMPERSTAND			22
#define STAR				23

// escape value
#define BACK_SLASH			24

// special value
#define END_OF_LINE			25
#define END_OF_FILE			26

struct Token
{
	std::string		_token;
	uint8_t			_type;
	unsigned int	_level;
};
