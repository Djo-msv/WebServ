#include "Socket.hpp"

Socket::Socket(int fd) : _socketFd(fd) {}

Socket::~Socket() { close(_socketFd); }

int Socket::getSocketFd(void) { return (_socketFd); }
