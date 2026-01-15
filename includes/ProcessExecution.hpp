#pragma once


// This class allows the server to execute a program using a CGI script.
class ProcessExecution
{
	public :
		ProcessExecution(); /*set all value to 0 */
		~ProcessExecution(); /* close both pape in case of crash */

		void	startProcess(char **args); /* fork Process and exec CGI, and stock execve output fd */

		int		getProcessFd(int *pid); /* return fd of execve output and set the pid to the actual pid of Process */
		
	private :
		int _pipeIn[2]; /* execve input */
		int	_pipeOut[2]; /* execve output */
		int	_pid; /* Process pid */
}
