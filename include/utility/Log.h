#ifndef __LOG_H__
#define __LOG_H__

int logInit();
void logDebug(char *format, ...);
void logInfo(char *format, ...);
void logWarn(char *format, ...);
void logError(char *format, ...);

#endif // __LOG_H__