#include "Socket.hpp"

Socket::Socket() : _socketFd(createSocket()) {}

Socket::~Socket() { close(_socketFd); }

const int Socket::getSocketFd(void) { return (_socketFd); }
