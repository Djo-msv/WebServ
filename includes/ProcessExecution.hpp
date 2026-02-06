#pragma once
# include <stdexcept>
# include <unistd.h>
# include <errno.h>
# include <socket_utils.hpp>
#ifndef BUF_SIZE
# define BUF_SIZE 1024
#endif

// This class allows the server to execute a program using a CGI script.
class ProcessExecution
{
	public :
		ProcessExecution(); /*set all value to -1 */
		~ProcessExecution(); /* close both pipes in case of crash */
	
		void setupProcess(bool pipein);//for a proper setup -> write -> exec, attempt 1
		void startProcess (bool pipein, std::string target, char **env); /* fork Process and exec CGI, and stock execve input/output fds */
		
		int		getFdIn() const;
		int		getFdOut() const;

	private :
		int			_pipeIn[2]; /* execve input */
		int			_pipeOut[2]; /* execve output */
		int			_pid; /* Process pid */
		const std::string cgi; //future this will be sent to startProcess() directly

		void		closeFds();
		
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
};
