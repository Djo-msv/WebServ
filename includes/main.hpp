#pragma once

#include <netinet/in.h>
#include <signal.h>
#include <cstring>
#include <cstdlib>
#include <sys/epoll.h>
#include <deque>
#include <stdexcept>

#define MAX_EVENTS 10
#define key first
#define value second

#include "socket_parsing.hpp"
#include "ProcessExecution.hpp"

std::map<std::string, std::string> initMimetype();

#define CATCH_AND_HANDLE(ExceptionType) catch (const ExceptionType& e) { handleError(e.what()); }

#define RETHROW(ExceptionType) catch (const ExceptionType& e) { throw e; }
