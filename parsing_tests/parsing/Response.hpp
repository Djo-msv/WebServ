#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include "Request.hpp"
# include <sys/stat.h>

class Response
{
	private:
		std::string _target; //target file to return
		std::string msg; //full HTTP message ready to send
		std::string status; //status code
		std::string body;
	public:
		Response();
		~Response();
		Response(const std::string &name);
		Response(const Response &other);
		Response &operator=(const Response &other);
		void makeResponse(Request *req);
		void seekTarget(Request *req);
		void makeBody();
		void fix_error(std::string error);
};

#endif
