#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <errno.h>
#include <sys/epoll.h>
#include <sstream>

void 		setnonblocking(int fd);
void		epoll_add(int epollInstance, int fd, u_int32_t flags);
void		epoll_mod(int epollInstance, int fd, u_int32_t flags);
void		epoll_del(int epollInstance, int fd, u_int32_t flags);
std::string ft_itoa(int nb);
