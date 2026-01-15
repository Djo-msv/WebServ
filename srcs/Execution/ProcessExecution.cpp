#include "ProcessExecution.hpp"

ProcessExecution::ProcessExecution(void) : _pipeIn({-1, -1}), _pipeOut({-1, -1}, _pid(0))
{}

ProcessExecution::~ProcessExecution(void)
{

	close(_pipeIn[0]);
	close(_pipeIn[1]);
	close(_pipeOut[0]);
	close(_pipeOut[1]);
}
