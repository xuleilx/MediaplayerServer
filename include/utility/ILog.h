#ifndef __ILOG_H__
#define __ILOG_H__

#include<string>

enum
{
    ERROR_LEVEL,
    WARN_LEVEL,
    INFO_LEVEL,
    DEBUG_LEVEL
};

class ILog
{
public:
    virtual ~ILog(){};
    virtual void DEBUG(const std::string message) = 0;
    virtual void INFO(const std::string message) = 0;
    virtual void WARN(const std::string message) = 0;
    virtual void ERROR(const std::string message) = 0;
};

#endif // __ILOG_H__