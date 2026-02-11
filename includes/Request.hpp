#pragma once

# include <iostream>
# include <string>
# include <sstream>
# include <exception>
# include <vector>
# include <algorithm>
# include <map>
# include <sys/stat.h>
# include <HttpError.hpp>
# include <ServerSocket.hpp>
# include <socket_utils.hpp>
# include <request_utils.hpp>
# include <HttpErrors.hpp>

#define CHUNKED -1

class Request
{
	public:
		Request(ServerConfig &s);
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
		std::string	getCgi() const;
		
		char	**getEnv() const;
		
		bool	isExec() const;
		
		int 		getSize() const;
		
		class MissingData : public std::out_of_range {
			public:
				MissingData();
		};
	
	private:
		ServerConfig &	_config;
		std::string		_method;
		std::string		_cgi;
		std::string		_target;
		std::string		_body;
		std::string		_request;
		std::string		_query;
		std::string		*_env;
		int				_status;

		const char		**c_env;
		
		bool 			exec;

		std::map<std::string, std::string> headers;
		//Server who received the request
		
		void	parse_header(std::string header);
		void	parse_body();
		void	body_check(size_t size_told, size_t real_size);
		void	headers_add(std::string line);
		void	startline_check(std::string line);
		void	adjust_exec();

};
