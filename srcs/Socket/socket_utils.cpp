#include "socket_utils.hpp"

int throw_fcntl_error(int fcntl)
{
	if (fcntl == -1)
		throw std::runtime_error("Could not perform fcntl operation error code :" + ft_itoa(errno));
	return (fcntl);
}

void setnonblocking(int fd)
{
	try
	{
		int flags = throw_fcntl_error(fcntl(fd, F_GETFL));
		throw_fcntl_error(fcntl(fd, F_SETFL, flags | O_NONBLOCK));
	} catch (std::runtime_error &e)
	{
		close(fd);
		throw e;
	}
}

inline std::string  ft_itoa(int nb)
{
	std::stringstream ss;
	ss << nb;
	return (ss.str());
}

void	epoll_add(int epollInstance, int fd, u_int32_t flags)
{
	epoll_event ev;

	ev.events = flags;
	ev.data.fd = fd;
	if (epoll_ctl(epollInstance, EPOLL_CTL_ADD, fd, &ev) == -1)
	{
		close(fd);
		throw std::runtime_error("Could not add '" + ft_itoa(fd) + "' socket to epoll interest list. Error Code :" + ft_itoa(errno));
	}
}

void	epoll_mod(int epollInstance, int fd, u_int32_t flags)
{
	epoll_event ev;

	ev.events = flags;
	ev.data.fd = fd;
	if (epoll_ctl(epollInstance, EPOLL_CTL_MOD, fd, &ev) == -1)
	{
		close(fd);
		throw std::runtime_error("Could not add '" + ft_itoa(fd) + "' socket to epoll interest list. Error Code :" + ft_itoa(errno));
	}
}

void	epoll_del(int epollInstance, int fd, u_int32_t flags)
{
	epoll_event ev;

	ev.events = flags;
	ev.data.fd = fd;
	if (epoll_ctl(epollInstance, EPOLL_CTL_DEL, fd, &ev) == -1)
	{
		close(fd);
		throw std::runtime_error("Could not add '" + ft_itoa(fd) + "' socket to epoll interest list. Error Code :" + ft_itoa(errno));
	}
}
