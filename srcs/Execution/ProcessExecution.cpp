#include "ProcessExecution.hpp"

ProcessExecution::ProcessExecution(void) : _pid(0)
{
	_pipeOut[0] = -1;
	_pipeOut[1] = -1;
	_pipeIn[0] = -1;
	_pipeIn[1] = -1;
} 

ProcessExecution::~ProcessExecution(void) {}

void ProcessExecution::clear(void)
{
	_pipeOut[0] = -1;
	_pipeOut[1] = -1;
	_pipeIn[0] = -1;
	_pipeIn[1] = -1;
	_pid = 0;
}

void ProcessExecution::setupProcess(bool pipein)
{
	if (pipe(_pipeOut))
		throw InternalServerError();
	if (pipein && pipe(_pipeIn))
	{
		this->closeFds();
		throw InternalServerError();
	}
}

void ProcessExecution::startProcess(bool pipein, std::string cgi, std::string target, char **env) /* fork Process and exec CGI, and stock execve in/out fds */
{
	const char **args;
	
	try { args = new const char*[3]; }
	RETHROW(std::bad_alloc)

	args[0] = cgi.c_str();
	args[1] = target.c_str();
	args[2] = NULL;

	if ((_pid = fork()) < 0) {
		this->closeFds();
		throw InternalServerError();
	}
	if (_pid == 0) {
		if (dup2(_pipeOut[1], STDOUT_FILENO) == -1) {
			this->closeFds();
			throw InternalServerError();
		}
		if (pipein && dup2(_pipeIn[0], STDIN_FILENO) == -1) {
			this->closeFds();
			throw InternalServerError();
		}
		this->closeFds();
		if (execve(args[0], (char **)args, env) == -1) {
			delete[] args;
			exit(EXIT_FAILURE);
			throw InternalServerError();
		}
	}
	delete[] args;
	close(_pipeOut[1]);
	if (pipein)
		close(_pipeIn[0]);
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

int		ProcessExecution::getFdIn() const { return _pipeIn[1]; }

int		ProcessExecution::getFdOut() const { return _pipeOut[0]; }
