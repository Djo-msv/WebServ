#include "ProcessExecution.hpp"

ProcessExecution::ProcessExecution(void) : _pid(0), _status(0)
{
	_pipeOut[0] = -1;
	_pipeOut[1] = -1;
} 

ProcessExecution::~ProcessExecution(void)
{
	if (_pipeOut[0] != -1)
		close(_pipeOut[0]);
	if (_pipeOut[1] != -1)
		close(_pipeOut[1]);
}

void ProcessExecution::startProcess(char **args, char **env) /* fork Process and exec CGI, and stock execve output fd */
{
	if (pipe(_pipeOut)) {
		closesFds();
		throw PipeError("An error occurred during the creation of the pipes, Error code : " + ft_itoa(errno)); 
	}
	if ((_pid = fork()) < 0) {
		closesFds();
		throw ForkError("An error occurred during the creation of the fork, Error code : " + ft_itoa(errno)); 
	}
	if (_pid == 0) {
		close(_pipeOut[0]);
		dup2(_pipeOut[1], STDOUT_FILENO);
		close(_pipeOut[1]);
		execve(args[0], args, env); /* This allows a new "args" process to be executed, takes data via the input pipe, and outputs it via the output pipe. */
		closesFds();
		throw ExecError("An error occurred in the children's process, Error code : " + ft_itoa(errno));
	}
	close(_pipeOut[1]);
}

void	ProcessExecution::readDataProcess(void) /* Read the output data from the child process, and fill the buffer passed as a parameter. */
{
	char	buffer[BUF_SIZE];

	ssize_t size = read(_pipeOut[0], buffer, BUF_SIZE);
	if (size < 0)
		return ;
	if (size == 0) {
		_status = true;
		return ;
	}
	_response += buffer;
}


void	ProcessExecution::closesFds(void) /* just before throw an error to prevent leaks */
{
	if (_pipeOut[0] != -1)
		close(_pipeOut[0]);
	if (_pipeOut[1] != -1)
		close(_pipeOut[1]);
}

bool		ProcessExecution::getStatus()
{
	return (_status);
}

std::string	ProcessExecution::getResponse()
{
	return (_response);
}
