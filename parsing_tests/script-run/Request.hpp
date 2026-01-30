#pragma once

# include <iostream>
# include <string>
# include <sstream>
# include <exception>
# include <vector>
# include <algorithm>
# include <map>
//# include <socket_utils.hpp>
inline std::string  ft_itoa(int nb)
{
	std::stringstream ss;
	ss << nb;
	return (ss.str());
}

#define CHUNKED -1

class Request
{
	public:
		Request();
		~Request();
		Request(const Request &other);

		Request &operator=(const Request &other);
		Request &operator+=(const char *buffer);


		void	parse();
		void	read() const;
		
		std::string	getTarget() const;
		std::string	getMethod() const;
		std::string	getQuery() const;
		std::string	getBody() const;
		
		char	**getEnv() const;
		
		bool	isExec() const;
		
		int 		getSize() const;

	private:
		std::string		_method;
		std::string		_target;
		std::string		_body;
		std::string		_request;
		std::string		_query;
		std::string		*_env;

		const char		**c_env;
		
		bool 			exec;

		std::map<std::string, std::string> headers;
		//Server who received the request
		
		void	parse_header(std::string header);
		void	parse_body();
		void	body_check(int diff);
		void	headers_add(std::string line);
		void	startline_check(std::string line);
		void	adjust_exec();
};
