#pragma once
#include "ITask.h"
#include <iostream>
#include <string>

class CTask : public ITask
{
public:

	CTask(unsigned index)
		: m_index(index)
	{};

	CTask() {};

	void Execute() override
	{
		std::cout << "Thread number " + std::to_string(m_index) + " is working\n";
	}

private:
	unsigned m_index;
};
