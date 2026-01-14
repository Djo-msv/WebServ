#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <sys/epoll.h>
#include <sstream>

void 		setnonblocking(int fd);
void		epoll_add(int epollInstance, int fd, u_int32_t flags);
std::string ft_itoa(int nb);