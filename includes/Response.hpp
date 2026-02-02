#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Request.hpp"
# include "ProcessExecution.hpp"
# include <sys/stat.h>
# include <fstream>

class Response
{
	private:
		std::string _target; //target file to return
		std::string msg; //full HTTP message ready to send
		std::string status; //status code
		std::string body;
		unsigned int _status;
		ProcessExecution *exec;
		
		void seekTarget(Request *req); //looking for target :: if non existent, redirects to 404
		void makeBody(); //reads the target file into a body string
		void writeExec();
		void readExec();
		void fix_error(std::string error); //takes status, adjusts target to the corresponding error_page
	public:
		Response();
		~Response();
		Response(const Response &other);
		Response &operator=(const Response &other);
		void makeResponse(Request *req); //distribution
		void actionExec();
		std::string getResponse(); //result
		int getFd() const; //fd from cgi-exec (write if write, read if read, -1 if neither)
		unsigned int getStatus() const; //status of the response ::cgi-write(1), ::cgi-read(0), ::Write(2), i guess inherited from ClientSocket
};

#endif
