#pragma once


class ITask
{
public:
	virtual void Execute() = 0;
	virtual ~ITask() {};
};