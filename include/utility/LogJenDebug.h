#ifndef __LOGJENDEBUG_H__
#define __LOGJENDEBUG_H__
#include"ILog.h"

class Jen : public ILog
{
public:
	Jen(int num);
	~Jen(){};
	void DEBUG(const std::string message);
	void INFO(const std::string message);
	void WARN(const std::string message);
	void ERROR(const std::string message);

private:
	int mLogLevel;
};

#endif // __LOGJENDEBUG_H__