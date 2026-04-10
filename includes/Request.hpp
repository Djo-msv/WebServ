#pragma once

# include <string>
typedef std::basic_string<unsigned char> ustring;

# include <iostream>
# include <sstream>
# include <exception>
# include <vector>
# include <algorithm>
# include <map>
# include <list>
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

		void	add(const unsigned char *buffer, size_t size);
		void	parse(std::map<std::string, std::string> &mime);
		void	clear();
		
		std::string	getTarget() const;
		std::string	getMethod() const;
		std::string	getQuery() const;
		unsigned char	*getBody() const;
		std::string	getCgi() const;
		std::string	getPathInfo() const;
		
		char	**getEnv() const;
		
		bool	isExec() const;
		bool	isPost() const;
		bool	keepAlive() const;
		
		ssize_t 		getSize() const;
		
		class MissingData : public std::out_of_range {
			public:
				MissingData();
		};
		
		class ChunkParsing : public std::out_of_range {
			public:
				ChunkParsing();
		};
		
		class DeleteRequest : public std::out_of_range {
			public:
				DeleteRequest(std::string target);
		};
	
	private:
		ServerConfig &	_config;
		
		ustring			_request;
		ustring			_body;
		ustring			_parse_body;

		std::string		_method;
		std::string		_cgi;
		std::string		_target;
		std::string		_query;
		std::string		path_info;
		std::string		*_env;
		int				_status;

		const char		**c_env;
		unsigned char *c_body;

		bool 			exec;

		std::map<std::string, std::string> headers;

		void	parse_header(std::string header);
		void	startline_check(std::string line);
		void	target_work();
		bool	needsIndex(std::string full_target);
		void	adjust_exec(std::string path_info, std::string script_name);
		void	headers_add(std::string line);
		void	mime_check(std::map<std::string, std::string> &mime);
		void	parse_body();
		void	body_check(size_t size_told, size_t real_size);
		void	create_env();
};
