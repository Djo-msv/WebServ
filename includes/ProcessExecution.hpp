#pragma once
# include <stdexcept>
# include <unistd.h>
# include <errno.h>
# include <socket_utils.hpp>

#define BUF_SIZE 1024

// This class allows the server to execute a program using a CGI script.
class ProcessExecution
{
	public :
		ProcessExecution(); /*set all value to 0 */
		~ProcessExecution(); /* close both pipes in case of crash */
	
		void startProcess (bool pipein, std::string target, char **env); /* fork Process and exec CGI, and stock execve output fd */
		
		size_t	writeBufferDataProcess(std::string buffer); /* writes buffer of request body to the child process */
		void readDataProcess(); /* Read the output data from the child process */

		bool		getStatus() const;
		std::string	getResponse() const;
		int		getFdIn() const; /* for epoll */
		int		getFdOut() const;

	private :
		int			_pipeIn[2]; /* execve input */
		int			_pipeOut[2]; /* execve output */
		int			_pid; /* Process pid */
		bool 		_status;
		std::string	_response;
		const std::string cgi;

		void		closeFds();
		
		// Error class
		class PipeError : public std::runtime_error {
			public :
				PipeError(const std::string msg) : std::runtime_error(msg) {}
		};
		class ForkError : public std::runtime_error {
			public :
				ForkError(const std::string msg) : std::runtime_error(msg) {} };
		class ExecError : public std::runtime_error {
			public :
				ExecError(const std::string msg) : std::runtime_error(msg) {}
		};
};
