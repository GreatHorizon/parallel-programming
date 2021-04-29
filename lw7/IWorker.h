#pragma once
#include "ITask.h"

class IWorker
{
public: 
	virtual bool ExecuteTask(ITask *task) = 0;
	virtual bool IsBusy() = 0;

	virtual ~IWorker() {};
};