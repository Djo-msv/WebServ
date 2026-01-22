#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <iostream>
# include <string>
# include <sstream>
# include <exception>
# include <vector>
#include <cmath>
# include <map>

class Request
{
	private:
		std::string _request;
		std::string _method;
		std::string _target;
		std::string *_env;
		const char **c_env;
		size_t env_size;
		std::map<std::string, std::string> headers;
		//Server who received the request
	public:
		Request();
		~Request();
		Request(const std::string &request);
		Request(const Request &other);
		Request &operator=(const Request &other);
		void make_env(std::string params);
		void check_request();
		void startline_check(std::string line);
		void headers_add(std::string line);
		void read() const;
		const char **getEnv() const;
		std::string getTarget() const;
		std::string getMethod() const;
		int getSize() const;
};

#endif
