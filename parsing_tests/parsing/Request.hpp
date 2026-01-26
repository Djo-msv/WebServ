#pragma once

# include <iostream>
# include <string>
# include <sstream>
# include <exception>
# include <vector>
# include <cmath>
# include <map>

#define CHUNKED -1

class Request
{
	private:
		int status;
		std::string _method;
		std::string _target;
		std::string *_env;
		std::string body;
		const char **c_env;
		size_t env_size;
		std::map<std::string, std::string> headers;
		bool exec;
		//Server who received the request
		void	headers_add(std::string line);
		void	startline_check(std::string line);
		void	make_env(std::string params);
	public:
		Request();
		~Request();
		Request(const Request &other);
		Request &operator=(const Request &other);
		
		int		check_header(std::string header);
		int		check_line(std::string line);
		void	adjust_exec();
		void	read() const;

		const char	**getEnv() const;
		std::string	getTarget() const;
		std::string	getMethod() const;
		
		bool	isExec() const;
		
		std::string	getBody() const;
		int 		getSize() const;
};