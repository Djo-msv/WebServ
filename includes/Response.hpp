#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Request.hpp"
# include "ProcessExecution.hpp"
# include <HttpErrors.hpp>
# include <sys/stat.h>
# include <fstream>

class Response
{
	private:
		std::string _target; //target file to return
		std::string msg; //full HTTP message ready to send
		std::string status; //status code
		std::string body;
		bool exec; //is there an exec to run ? (makeResponse return)
		
		void readFile(); //if target, reads the target file into a body string
		void chunkBody(); //chunks body, by BUF_SIZE
	public:
		Response();
		~Response();
		Response(const Response &other);
		Response &operator=(const Response &other);
		Response &operator+=(const char *buffer);
		void fix_error(HttpError &error, ServerConfig &s); //for error return, adjusts status + body
		bool makeResponse(Request *req); //distribution
		std::string getResponse(); //result
		void clear();
};

#endif
