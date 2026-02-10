#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Request.hpp"
# include "ProcessExecution.hpp"
# include <fstream>

class Response
{
	private:
		std::string _target; //target file to return
		std::string msg; //full HTTP message ready to send
		std::string status; //status code
		std::string body;
		bool exec; //is there an exec to run ? (makeResponse return)
		
		// void seekTarget(Request *req); //looking for target :: if non existent, redirects to 404
		void makeBody(); //reads the target file into a body string
		void fix_error(std::string error); //takes status, adjusts target to the corresponding error_page
	public:
		Response();
		~Response();
		Response(const Response &other);
		Response &operator=(const Response &other);
		Response &operator+=(const char *buffer);
		bool makeResponse(Request *req); //distribution
		std::string getResponse(); //result
};

#endif
