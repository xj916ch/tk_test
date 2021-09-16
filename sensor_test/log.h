#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

enum LOG_LEVEL {
	LOG_LEVEL_OFF,
	LOG_LEVEL_ERR,
	LOG_LEVEL_WARN,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DBG,
	LOG_LEVEL_ALL,
};

void log_init(int level);
void get_sys_time(void);

extern int log_level;

#define LOG_DBG(format, ...) \
do { \
	if (log_level >= LOG_LEVEL_DBG) { \
		get_sys_time(); \
		printf("[%10s] [%6d] [DEBUG] " format "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
	} \
} while(0)

#define LOG_INFO(format, ...) \
do { \
	if (log_level >= LOG_LEVEL_INFO) { \
		get_sys_time(); \
		printf("[%10s] [%6d] [INFO] " format "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
	} \
} while(0)

#define LOG_WARN(format, ...) \
do { \
	if (log_level >= LOG_LEVEL_WARN) { \
		get_sys_time(); \
		printf("[%10s] [%6d] [WARN] " format "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
	} \
} while(0)

#define LOG_ERR(format, ...) \
do { \
	if (log_level >= LOG_LEVEL_ERR) { \
		get_sys_time(); \
		printf("[%10s] [%6d] [ERROR] " format "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
	} \
} while(0)

#endif //_LOG_H_
