#pragma once

# include <stdexcept>
# include <unistd.h>
# include <errno.h>
# include <socket_utils.hpp>
# include <HttpErrors.hpp>
# include <cstdlib>

#ifndef BUF_SIZE
# define BUF_SIZE 1024
#endif

#define RETHROW(ExceptionType) \
	catch (const ExceptionType& e) { \
		throw e; \
	}

// This class allows the server to execute a program using a CGI script.
class ProcessExecution
{
	public :
		ProcessExecution(); /*set all value to -1 */
		~ProcessExecution(); /* close both pipes in case of crash */
	
		void setupProcess(bool pipein);//for a proper setup -> write -> exec, attempt 1
		void startProcess (bool pipein, std::string cgi, std::string target, char **env); /* fork Process and exec CGI, and stock execve input/output fds */
		
		int		getFdIn() const;
		int		getFdOut() const;
		
		void	clear(void);

	private :
		int			_pipeIn[2]; /* execve input */
		int			_pipeOut[2]; /* execve output */
		int			_pid; /* Process pid */

		void		closeFds();
};
