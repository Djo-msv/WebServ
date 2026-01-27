#pragma once

# include <iostream>
# include <string>
# include <sstream>
# include <exception>
# include <vector>
# include <cmath>
# include <map>
# include <socket_utils.hpp>

#define CHUNKED -1
#define	START 0
#define HEADER 1
#define	BODY 2

class Request
{
	public:
		Request();
		~Request();
		Request(const Request &other);

		Request &operator=(const Request &other);
		Request &operator+=(const char *buffer);


		void	parse();
		void	adjust_exec();
		void	read() const;
		
		const char	**getEnv() const;
		std::string	getTarget() const;
		std::string	getMethod() const;
		
		bool	isExec() const;
		
		std::string	getBody() const;
		int 		getSize() const;

	private:
		int	_status;

		std::string 	_method;
		std::string		_target;
		std::string		*_env;
		std::string		_body;
		std::string		_request;

		const char		**c_env;
		size_t			env_size;
		bool 			exec;

		std::map<std::string, std::string> headers;
		//Server who received the request
		
		void	parse_header(std::string header);
		void	check_line(std::string line);
		void	headers_add(std::string line);
		void	startline_check(std::string line);
		void	make_env(std::string params);
		
};