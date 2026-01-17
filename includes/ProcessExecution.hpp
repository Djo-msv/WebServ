#pragma once


// This class allows the server to execute a program using a CGI script.
class ProcessExecution
{
	public :
		ProcessExecution(); /*set all value to 0 */
		~ProcessExecution(); /* close both pape in case of crash */
	
		void startProcess(char **args); /* fork Process and exec CGI, and stock execve output fd */
		
		int sendDataProcess(char *bytes); /* send data to the child Process */
		int readDataProcess(char buffer[BUF_SIZE]); /* Read the output data from the child process, and fill the buffer passed as a parameter. */
		int getProcessData(int *input, int *output); /* return the pid of the Process and set the fd input and output value */
	private :
		int _pipeIn[2]; /* execve input */
		int _pipeOut[2]; /* execve output */
		int _pid; /* Process pid */
};
