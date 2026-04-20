#pragma once

#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Parser/Lexer/Parser.hpp"
#include "Parser/Tree/MymlDictionary.hpp"
#include "Parser/Tree/MymlList.hpp"
#include "Parser/Tree/MymlPair.hpp"

#define EXCLUSIVE_RULE_ERROR "logic error : the rules alias, root and redirect are exclusive and can't be defined at the same time"

ServerConfig *initServerConfig(MymlDictionary *serverRepertory);

#define IGNORE(ExceptionType) catch (const ExceptionType& ignored) {}