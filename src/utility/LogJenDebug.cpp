#include "LogJenDebug.h"
#include "ILog.h"

#include <stdarg.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>

Jen::Jen(int num)
{
    mLogLevel = num;
}

void Jen::ERROR(std::string message)
{
    if (mLogLevel >= ERROR_LEVEL)
    {
        struct tm *__now;
        struct timeb __tb;
        ftime(&__tb);
        __now = localtime(&__tb.time);
        fprintf(stdout, "\e[31m%02d:%02d:%02d:%03d [MW][MediaPlayer][ERROR]:\e[0m %s",
                __now->tm_hour, __now->tm_min, __now->tm_sec, __tb.millitm,
                message.data());
        fflush(stdout);
    }
}
void Jen::WARN(std::string message)
{
    if (mLogLevel >= WARN_LEVEL)
    {
        struct tm *__now;
        struct timeb __tb;
        ftime(&__tb);
        __now = localtime(&__tb.time);
        fprintf(stdout, "\e[33m%02d:%02d:%02d:%03d [MW][MediaPlayer][WARN]:\e[0m %s",
                __now->tm_hour, __now->tm_min, __now->tm_sec, __tb.millitm,
                message.data());
        fflush(stdout);
    }
}
void Jen::INFO(std::string message)
{
    if (mLogLevel >= INFO_LEVEL)
    {
        struct tm *__now;
        struct timeb __tb;
        ftime(&__tb);
        __now = localtime(&__tb.time);
        fprintf(stdout, "\e[36m%02d:%02d:%02d:%03d [MW][MediaPlayer][INFO]:\e[0m %s",
                __now->tm_hour, __now->tm_min, __now->tm_sec, __tb.millitm,
                message.data());
        fflush(stdout);
    }
}

void Jen::DEBUG(std::string message)
{
    if (mLogLevel >= DEBUG_LEVEL)
    {
        struct tm *__now;
        struct timeb __tb;
        ftime(&__tb);
        __now = localtime(&__tb.time);
        fprintf(stdout, "\e[32m%02d:%02d:%02d:%03d [MW][MediaPlayer][DEBUG]:\e[0m %s",
                __now->tm_hour, __now->tm_min, __now->tm_sec, __tb.millitm,
                message.data());
        fflush(stdout);
    }
}
