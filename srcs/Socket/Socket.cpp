#include "Socket.hpp"

Socket::Socket() : _socketFd(createSocket()) {}

Socket::~Socket() { close(_socketFd); }

int Socket::getSocketFd(void) { return (_socketFd); }
