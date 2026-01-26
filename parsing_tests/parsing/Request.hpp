#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <iostream>
# include <string>
# include <sstream>
# include <exception>
# include <vector>
# include <cmath>
# include <map>

inline std::string  ft_itoa(int nb)
{
	std::stringstream ss;
	ss << nb;
	return (ss.str());
}

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
	public:
		Request();
		~Request();
		Request(const Request &other);
		Request &operator=(const Request &other);
		void make_env(std::string params);
		int check_line(std::string line);
		void adjust_exec();
		void startline_check(std::string line);
		void headers_add(std::string line);
		void read() const;
		const char **getEnv() const;
		std::string getTarget() const;
		std::string getMethod() const;
		bool isExec() const;
		std::string getBody() const;
		int getSize() const;
};

#endif
