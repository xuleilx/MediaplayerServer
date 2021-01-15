
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Log.h"
#include "ILog.h"
#include "LogJenDebug.h"
#include "LogSyslog.h"

static ILog *pLog = NULL;

int logInit()
{
	int logLevel = DEBUG_LEVEL;

	char *temp = getenv("MEDIA_LOG_LEVEL");
	if (temp != NULL)
	{
		logLevel = atoi(temp);
	}

	pLog = new Jen(logLevel);

	return 0;
}

void logInfo(char *format, ...)
{
	char buffer[256 + 1] = {0};
	va_list arg;
	va_start(arg, format);
	vsnprintf(buffer, 256 + 1, format, arg);
	va_end(arg);
	pLog->INFO(buffer);
}
void logWarn(char *format, ...)
{
	char buffer[256 + 1] = {0};
	va_list arg;
	va_start(arg, format);
	vsnprintf(buffer, 256 + 1, format, arg);
	va_end(arg);
	pLog->WARN(buffer);
}
void logError(char *format, ...)
{
	char buffer[256 + 1] = {0};
	va_list arg;
	va_start(arg, format);
	vsnprintf(buffer, 256 + 1, format, arg);
	va_end(arg);
	pLog->ERROR(buffer);
}
void logDebug(char *format, ...)
{
	char buffer[256 + 1] = {0};
	va_list arg;
	va_start(arg, format);
	vsnprintf(buffer, 256 + 1, format, arg);
	va_end(arg);
	pLog->DEBUG(buffer);
}
