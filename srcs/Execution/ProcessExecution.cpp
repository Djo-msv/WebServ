#include "ProcessExecution.hpp"

ProcessExecution::ProcessExecution(void) : _pipeIn({-1, -1}), _pipeOut({-1, -1}, _pid(0))
{}

ProcessExecution::~ProcessExecution(void)
{
	if (_pipeIn[0] != -1)
		close(_pipeIn[0]);
	if (_pipeIn[1] != -1)
		close(_pipeIn[1]);
	if (_pipeOut[0] != -1)
		close(_pipeOut[0]);
	if (_pipeOut[1] != -1)
		close(_pipeOut[1]);
}

void ProcessExecution::startProcess(char **args) /* fork Process and exec CGI, and stock execve output fd */
{
	if (pipe(_pipeIn) || pipe(_pipeOut)) {
		closesFds();
		throw PipeError("An error occurred during the creation of the pipes, Error code : " + ft_itoa(errno)); 
	}
	if ((_pid = fork()) < 0) {
		closesFds();
		throw ForkError("An error occurred during the creation of the fork, Error code : " + ft_itoa(errno)); 
	}
	if (_pid == 0) {
		close(_pipeIn[1]); /* close unused fd */
		close(_pipeOut[0]);
		dup2(_pipeIn[0], STDIN_FILENO); /* allows you to change the standard input and output of the child process */
		dup2(_pipeOut[1], STDOUT_FILENO);
		close(_pipeIn[0]);
		close(_pipeOut[1]);
		execve(args[0], args, NULL); /* This allows a new "args" process to be executed, takes data via the input pipe, and outputs it via the output pipe. */
		closesFds();
		throw ExecError("An error occurred in the children's process, Error code : " + ft_itoa(errno));
	}
	close(_pipeIn[0]);
	close(_pipeOut[1]);
}

int ProcessExecution::sendDataProcess(char *bytes) /* send data to the child Process */
{
	size_t	i;

	if (!bytes)
		return (1);
	for (i = 0; bytes[i]; i++);
	write (_pipeIn[1], bytes, i);
	return (0);
}


int ProcessExecution::readDataProcess(char buffer[BUF_SIZE]) /* Read the output data from the child process, and fill the buffer passed as a parameter. */
{
	return (read(_pipeOut[0], buffer, BUF_SIZE));
}


void	closesFds(void) /* just before throw an error to prevent leaks */
{
	if (_pipeIn[0] != -1)
		close(_pipeIn[0]);
	if (_pipeIn[1] != -1)
		close(_pipeIn[1]);
	if (_pipeOut[0] != -1)
		close(_pipeOut[0]);
	if (_pipeOut[1] != -1)
		close(_pipeOut[1]);
}
