#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

//#define _TEST_

int log_level = LOG_LEVEL_DBG;

void get_sys_time(void)
{
	struct timeval tv;
	struct tm *tm_ptr;

	gettimeofday(&tv, NULL);

	tm_ptr = localtime(&tv.tv_sec);
	printf("[%d-%02d-%02d %02d:%02d:%02d.%ld] ", 1900+tm_ptr->tm_year, 1+tm_ptr->tm_mon,
			tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec, tv.tv_usec/1000);
}

void log_init(int level)
{
	if (level >= LOG_LEVEL_OFF && level <= LOG_LEVEL_ALL)
		log_level = level;
}

#ifdef _TEST_
int main(int argc, char **argv)
{
	log_init(LOG_LEVEL_INFO);
	LOG_DBG("xxxx\n");
}
#endif
