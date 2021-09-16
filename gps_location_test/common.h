#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <dirent.h>

#include <math.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define DEBUG 0

struct GNRMC_Info {
	unsigned int year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char Minute;
	unsigned char second;

	unsigned int longitude;
	unsigned char longitude_suffix; // E或W

	unsigned int latitude;
	unsigned char latitude_suffix;
	unsigned char position_valid; // 有效
};

int GPGGA_Parse(char *pstr, struct GNRMC_Info *pGPGGAInfo);
void nmea_to_wsg84(double *lng, double *lat, unsigned int lng_GNRMC, unsigned int lat_GNRMC);
void wsg84_to_bd09(double *lng, double *lat);
double real_range(double lat1, double lng1, double lat2, double lng2);

#endif //__COMMON_H__
