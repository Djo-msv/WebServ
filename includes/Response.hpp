#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Request.hpp"
# include "ProcessExecution.hpp"
# include <HttpErrors.hpp>
# include <sys/stat.h>
# include <fstream>
# include <vector>
# include <fcntl.h>
# include <unistd.h>

class Response
{
	public:
		Response();
		~Response();
		Response(const Response &other);
		
		Response &operator=(const Response &other);
		
		void add(const unsigned char *buffer, size_t size); //adds the buffer to the body (cgi read response)
		bool makeResponse(Request *req); //distribution
		void makeErrorResponse(HttpError &error, ServerConfig &s); //for immediate error return, adjusts status + body
		void makeMsg();//response is ready for sending, make the final message
		void clear();

		//getters
		unsigned char *getResponse(std::map<std::string, std::string> &mime); //result
		size_t getSize() const;
	
	private:
		std::string _target; //target file to return
		std::string _status; //status code
		std::string _headers; //HTTP headers
		
		ustring _body;
		
		unsigned char *_msg;
		
		bool exec; //is there an exec to run ? (makeResponse return)
		size_t sizer;
		
		void readFile(); //if target, reads the target file into a body string
		void chunkBody(); //chunks body, by BUF_SIZE
};

#endif
