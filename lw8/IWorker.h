ó#pragma once
#include "ITask.h"

class IWorker
{
public:
	virtual void ExecuteTasks() = 0;
	virtual bool IsBusy() = 0;

	virtual ~IWorker() {};
};