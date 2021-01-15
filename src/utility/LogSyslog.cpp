
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "LogSyslog.h"
#include <stdarg.h>
#include <syslog.h>

using namespace std;

// #define	LOG_EMERG	0	/* system is unusable */
// #define	LOG_ALERT	1	/* action must be taken immediately */
// #define	LOG_CRIT	2	/* critical conditions */
// #define	LOG_ERR		3	/* error conditions */
// #define	LOG_WARNING	4	/* warning conditions */
// #define	LOG_NOTICE	5	/* normal but significant condition */
// #define	LOG_INFO	6	/* informational */
// #define	LOG_DEBUG	7	/* debug-level messages */

SysLog::SysLog(int num)
{
	mLogLevel = num + LOG_ERR;
	if(mLogLevel > LOG_WARNING) mLogLevel++; // Don`t have LOG_NOTICE
	openlog("[MW][MediaPlayer]", LOG_PID | LOG_CONS, LOG_LOCAL3);
	setlogmask(LOG_UPTO(mLogLevel));
}
SysLog::~SysLog()
{
	closelog();
}

void SysLog::ERROR(std::string message)
{
	syslog(LOG_ERR, "%s\n", message.data());
}

void SysLog::WARN(std::string message)
{
	syslog(LOG_WARNING, "%s\n", message.data());
}

void SysLog::INFO(std::string message)
{
	syslog(LOG_INFO, "%s\n", message.data());
}

void SysLog::DEBUG(std::string message)
{
	syslog(LOG_DEBUG, "%s\n", message.data());
}
