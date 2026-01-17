#pragma once

#include "ClientSocket.hpp"

// This class allows the server to execute a program using a CGI script.
class ProcessExecution
{
	public :
		ProcessExecution(); /*set all value to 0 */
		~ProcessExecution(); /* close both pape in case of crash */
	
		void startProcess(char **args); /* fork Process and exec CGI, and stock execve output fd */
		
		int sendDataProcess(char *bytes); /* send data to the child Process */
		int readDataProcess(char buffer[BUF_SIZE]); /* Read the output data from the child process, and fill the buffer passed as a parameter. */

	private :
		int _pipeIn[2]; /* execve input */
		int _pipeOut[2]; /* execve output */
		int _pid; /* Process pid */

		// Error class
		class PipeError : public std::runtime_error {
			public :
				PipeError(const std::string msg) : std::runtime_error(msg) {}
		};
		class ForkError : public std::runtime_error {
			public :
				ForkError(const std::string msg) : std::runtime_error(msg) {}
		};
		class ExecError : public std::runtime_error {
			public :
				ExecError(const std::string msg) : std::runtime_error(msg) {}
		};

		// Error Function
		void	closesFds(); /* just before throw an error to prevent leaks */
};
