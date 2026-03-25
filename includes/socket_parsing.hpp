#pragma once

#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Parser/Lexer/Parser.hpp"
#include "Parser/Tree/MymlDictionary.hpp"
#include "Parser/Tree/MymlList.hpp"
#include "Parser/Tree/MymlPair.hpp"

ServerConfig initServerConfig(MymlDictionary *serverRepertory);

#define IGNORE(ExceptionType) catch (const ExceptionType& ignored) {}