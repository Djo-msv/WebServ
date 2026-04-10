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
# include <dirent.h>
# ifndef BUF_SIZE
#  define BUF_SIZE 1024
# endif

#define RETHROW(ExceptionType) \
	catch (const ExceptionType& e) { \
		throw e; \
	}

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
		bool _post; //is the no exec request a POST request ?
		size_t sizer;
		
		void handleExec(); //if exec, check the cgi return for 1)empty body 2)status in "status: []" form
		void readFile(); //if GET target, reads the target file into a body string
		void postFile(unsigned char *body, size_t length, std::string path_info); //if POST target, appends content to file
		void chunkBody();
};

#endif
