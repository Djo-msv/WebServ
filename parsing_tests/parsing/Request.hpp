#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <iostream>
# include <string>
# include <sstream>
# include <exception>
# include <vector>

const char **make_env(std::string params);

class Request
{
	private:
		std::string _method;
		std::string _target;
		const char **_env;
		//clientServer who sent the request ?
	public:
		Request();
		~Request();
		Request(const std::string &request);
		Request(const Request &other);
		Request &operator=(const Request &other);
		void startline_check(std::string line);//, ServerSocket *server);
		void read();
};

#endif
