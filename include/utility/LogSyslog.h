#ifndef __LOGSYSLOG_H__
#define __LOGSYSLOG_H__
#include "ILog.h"

/* syslog:日志等级 */
// #define	LOG_EMERG	0	/* system is unusable */
// #define	LOG_ALERT	1	/* action must be taken immediately */
// #define	LOG_CRIT	2	/* critical conditions */
// #define	LOG_ERR		3	/* error conditions */
// #define	LOG_WARNING	4	/* warning conditions */
// #define	LOG_NOTICE	5	/* normal but significant condition */
// #define	LOG_INFO	6	/* informational */
// #define	LOG_DEBUG	7	/* debug-level messages */

class SysLog : public ILog
{
public:
	SysLog(int num);
	~SysLog();
	void DEBUG(const std::string message);
	void INFO(const std::string message);
	void WARN(const std::string message);
	void ERROR(const std::string message);

private:
	int mLogLevel;
};

#endif // __LOGSYSLOG_H__