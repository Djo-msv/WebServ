#include "ProcessExecution.hpp"

ProcessExecution::ProcessExecution(void) : _pid(0), _status(0), cgi("/usr/bin/python3")
{
	_pipeOut[0] = -1;
	_pipeOut[1] = -1;
	_pipeIn[0] = -1;
	_pipeIn[1] = -1;
} 

ProcessExecution::~ProcessExecution(void)
{
	/*if (_pipeIn[0] != -1)
		close(_pipeIn[0]);
	if (_pipeIn[1] != -1)
		close(_pipeIn[1]);
	if (_pipeOut[0] != -1)
		close(_pipeOut[0]);
	if (_pipeOut[1] != -1)
		close(_pipeOut[1]);*/
	//these closes seem redundant to me as :: if error, closes are handled :: if !error, closes are also handled
}

void ProcessExecution::startProcess(bool pipein, std::string target, char **env) /* fork Process and exec CGI, and stock execve output fd */
{
	const char **args = new const char*[3];
	args[0] = cgi.c_str();
	args[1] = target.c_str();
	args[2] = NULL;
	if (pipe(_pipeOut))
		throw PipeError("An error occurred during the creation of the pipes, Error code : " + ft_itoa(errno));
	if (pipein && pipe(_pipeIn))
	{
		this->closeFds();
		throw PipeError("An error occurred during the creation of the pipes, Error code : " + ft_itoa(errno));
	}
	if ((_pid = fork()) < 0) {
		this->closeFds();
		throw ForkError("An error occurred during the creation of the fork, Error code : " + ft_itoa(errno)); 
	}
	if (_pid == 0) {
		//unsure if we need this ? ive run a couple and so far no fds issues, which somewhat presumes that this does actually work
		/*close(STDOUT_FILENO);
		close(STDIN_FILENO);*/
		dup2(_pipeOut[1], STDOUT_FILENO);
		if (pipein)
			dup2(_pipeIn[0], STDIN_FILENO);
		this->closeFds();
		execve(args[0], (char **)args, env); /* This allows a new "args" process to be executed and output the result via the output pipe. */
		delete[] args;
		throw ExecError("An error occurred in the children's process, Error code : " + ft_itoa(errno));
	}
	delete[] args;
	close(_pipeOut[1]);
	if (pipein)
		close(_pipeIn[0]);
}

//Write the Request body to the child process
ssize_t	ProcessExecution::writeBufferDataProcess(std::string buffer)
{
	return (write(_pipeIn[1], buffer.c_str(), buffer.length()));
}

void	ProcessExecution::readDataProcess(void) /* Read the output data from the child process, and fill the buffer passed as a parameter. */
{
	char	buffer[BUF_SIZE];

	ssize_t size = read(_pipeOut[0], buffer, BUF_SIZE);
	buffer[size] = '\0';
	if (size < 0)
		return ;
	if (size == 0) {
		_status = true;
		close(_pipeOut[0]);
		return ;
	}
	_response += buffer;
}

void		ProcessExecution::closeFds()
{
	close(_pipeOut[0]);
	close(_pipeOut[1]);
	if (_pipeIn[0] != -1)
		close(_pipeIn[0]);
	if (_pipeIn[1] != -1)
		close(_pipeIn[1]);
}

bool		ProcessExecution::getStatus() const
{
	return (_status);
}

std::string	ProcessExecution::getResponse() const
{
	return (_response);
}

int		ProcessExecution::getFdIn() const { return _pipeIn[1]; }

int		ProcessExecution::getFdOut() const { return _pipeOut[0]; }
