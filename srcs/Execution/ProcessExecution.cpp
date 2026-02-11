#include "ProcessExecution.hpp"

ProcessExecution::ProcessExecution(void) : _pid(0)
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

void ProcessExecution::setupProcess(bool pipein)
{
	if (pipe(_pipeOut))
		throw PipeError("An error occurred during the creation of the pipes, Error code : " + ft_itoa(errno));
	if (pipein && pipe(_pipeIn))
	{
		this->closeFds();
		throw PipeError("An error occurred during the creation of the pipes, Error code : " + ft_itoa(errno));
	}
}
#include <cstdlib>
void ProcessExecution::startProcess(bool pipein, std::string cgi, std::string target, char **env) /* fork Process and exec CGI, and stock execve in/out fds */
{
	const char **args = new const char*[3];
	args[0] = cgi.c_str();
	args[1] = target.c_str();
	args[2] = NULL;

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
		if (execve(args[0], (char **)args, env) == -1) { //execution
			delete[] args;
			exit(EXIT_FAILURE);
			throw ExecError("An error occurred in the children's process, Error code : " + ft_itoa(errno));
		}
		delete[] args;
		//exit(EXIT_SUCCESS);
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
