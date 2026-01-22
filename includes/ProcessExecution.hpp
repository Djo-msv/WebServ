#pragma once

#include "ClientSocket.hpp"

class ClientSocket ;

// This class allows the server to execute a program using a CGI script.
class ProcessExecution
{
	public :
		ProcessExecution(); /*set all value to 0 */
		~ProcessExecution(); /* close both pape in case of crash */
	
		void startProcess(char **args, char **env); /* fork Process and exec CGI, and stock execve output fd */
		
		void readDataProcess(); /* Read the output data from the child process, and fill the buffer passed as a parameter. */

		bool		getStatus();
		std::string	getResponse();

	private :
		int			_pipeOut[2]; /* execve output */
		int			_pid; /* Process pid */
		bool 		_status;
		std::string	_response;

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
