#include "common.h"

#define X_PI (3.14159265358979324 * 3000.0 / 180.0)
#define PI 3.1415926535897932384626
#define SEMI_AXIS 6378245.0  //长半轴
#define EE_SQR 0.00669342162296594323  //偏心率平方


static int comma_split(unsigned char *pstr, char *buf, int buflen, int s, int e)
{
	int i, k;
	int commaCount = 0;
	int rc = -1;
	i = k = 0;
	if (e <= s) {
		return -1;
	}

	while (pstr[i] != '\0') {
		if (pstr[i] == ',') {
			commaCount++;
		}
		if (commaCount == s) {
			k = ++i;
			break;
		}
		i++;
	}

	while (pstr[i] != '\0') {
		if (pstr[i] == ',') {
			commaCount++;
		}
		if (commaCount == e) {
			if (i - k + 1 > buflen) {
				return -1;
			}
			memset(buf, 0, i-k);
			memcpy(buf, pstr + k, i - k);

			rc = i - k;
			buf[rc] = '\0';
			//printf("%s\n", buf);
			break;
		}
		i++;
	}
	return rc;
}

// $GPGGA,,,,,,0,,,,,,,,*66
// $GPGGA,163915.00,2238.72719,N,11402.51213,E,1,05,2.5,-1626.9,M,-2.2,M,,*6E
// $GPGGA,160840.00,2239.13163,N,11402.14353,E,1,07,0.8,-263.2,M,-2.2,M,,*57
// $GPGGA,132252.00,2235.81576,N,11359.89204,E,1,06,1.2,56.9,M,-2.3,M,,*49
int GPGGA_Parse(char *pstr, struct GNRMC_Info *pGPGGAInfo)
{
	int len = 0;
	int commaCount = 0;
	double temp = 0;
	char buf[32] = {0};

	// 计算字符串长度
	while (pstr[len] != '\0') {
		if (pstr[len] == ',') {
			commaCount++;
		}
		len++;
	}

	// 逗号个数是14个
	if (commaCount != 13 && commaCount != 14) {
		return -1;
	}

	// 判断开始字符是:$GPGGA
	if (!((pstr[0] == '$') && (pstr[1] == 'G') && (pstr[2] == 'P') && (pstr[3] == 'G') && (pstr[4] == 'G') && (pstr[5] == 'A'))) {
		return -1;
	}

	// UTC时间:第1个逗号与第2个逗号之前 (eg:092846.400 hhmmss.sss )
	len = comma_split(pstr, buf, sizeof(buf), 1, 2);
	if (len <= 0) {
		return -1;
	}
	pGPGGAInfo->hour = (buf[0] - '0') * 10 + (buf[1] - '0');
	pGPGGAInfo->Minute = (buf[2] - '0') * 10 + (buf[3] - '0');
	pGPGGAInfo->second = (buf[4] - '0') * 10 + (buf[5] - '0');

	// 纬度
	len = comma_split(pstr, buf, sizeof(buf), 2, 3);
	if (len <= 0) {
		return -1;
	}
	temp = atof(buf);
	temp = temp * 10000;
	pGPGGAInfo->latitude = (unsigned int)temp;

	// 纬度半球N(北半球)或S(南半球)
	len = comma_split(pstr, buf, sizeof(buf), 3, 4);
	if (len != 1) {
		return -1;
	}
	pGPGGAInfo->latitude_suffix = buf[0];

	// 经度
	len = comma_split(pstr, buf, sizeof(buf), 4, 5);
	if (len <= 0) {
		return -1;
	}
	temp = atof(buf);
	temp = temp * 10000;
	pGPGGAInfo->longitude = (unsigned int)temp;

	// 经度半球E(东经)或W(西经)
	len = comma_split(pstr, buf, sizeof(buf), 5, 6);
	if (len != 1) {
		return -1;
	}
	pGPGGAInfo->longitude_suffix = buf[0];

	// 定位状态，0=未定位，1=非差分定位，2=差分定位，3=无效PPS，6=正在估算
	len = comma_split(pstr, buf, sizeof(buf), 6, 7);
	if (len != 1) {
		return -1;
	}
	pGPGGAInfo->position_valid = ((buf[0] == '1' || buf[0] == '2') ? 1 : 0);

	if(DEBUG) printf("GPGGA longitude:%d latitude:%d\n", pGPGGAInfo->longitude, pGPGGAInfo->latitude);
	return 0;
}

static unsigned char areyou_inchina(double lng, double lat)
{
	if (lng > 73.66 && lng < 135.05 && lat > 3.86 && lat < 53.55)
		return 1;
	else
		return 0;
}

static double transformlat(double lng, double lat)
{
	double ret = 0;

	ret = -100.0 + 2.0 * lng + 3.0 * lat + 0.2 * lat * lat +
		  0.1 * lng * lat + 0.2 * sqrt(fabs(lng));
	ret += (20.0 * sin(6.0 * lng * PI) + 20.0 *
			sin(2.0 * lng * PI)) * 2.0 / 3.0;
	ret += (20.0 * sin(lat * PI) + 40.0 *
			sin(lat / 3.0 * PI)) * 2.0 / 3.0;
	ret += (160.0 * sin(lat / 12.0 * PI) + 320 *
			sin(lat * PI / 30.0)) * 2.0 / 3.0;
	return ret;
}

static double transformlng(double lng, double lat)
{
	double ret = 0;

	ret = 300.0 + lng + 2.0 * lat + 0.1 * lng * lng + \
		  0.1 * lng * lat + 0.1 * sqrt(fabs(lng));
	ret += (20.0 * sin(6.0 * lng * PI) + 20.0 *
			sin(2.0 * lng * PI)) * 2.0 / 3.0;
	ret += (20.0 * sin(lng * PI) + 40.0 *
			sin(lng / 3.0 * PI)) * 2.0 / 3.0;
	ret += (150.0 * sin(lng / 12.0 * PI) + 300.0 *
			sin(lng / 30.0 * PI)) * 2.0 / 3.0;
	return ret;
}

static void wgs84_to_gcj02(double *lng, double *lat)
{
	double lng_tmp = *lng, lat_tmp = *lat;

	double dlng = transformlng(lng_tmp - 105.0, lat_tmp - 35.0);
	double dlat = transformlat(lng_tmp - 105.0, lat_tmp - 35.0);

	double radlat = lat_tmp / 180.0 * PI;
	double magic = sin(radlat);
	magic = 1 - EE_SQR * magic * magic;
	double sqrtmagic = sqrt(magic);
	dlat = (dlat * 180.0) / ((SEMI_AXIS * (1 - EE_SQR)) / (magic * sqrtmagic) * PI);
	dlng = (dlng * 180.0) / (SEMI_AXIS / sqrtmagic * cos(radlat) * PI);
	double mglat = lat_tmp + dlat;
	double mglng = lng_tmp + dlng;

	*lng = mglng;
	*lat = mglat;
}

static void gcj02_to_bd09(double *lng, double *lat)
{
	double lng_tmp = *lng, lat_tmp = *lat;
	double z = sqrt(lng_tmp * lng_tmp + lat_tmp * lat_tmp) + 0.00002 * sin(lat_tmp * X_PI);
	double theta = atan2(lat_tmp, lng_tmp) + 0.000003 * cos(lng_tmp * X_PI);
	double bd_lng = z * cos(theta) + 0.0065;
	double bd_lat = z * sin(theta) + 0.006;

	if(DEBUG) printf("bd09 longitude:%lf latitude:%lf\n", bd_lng, bd_lat);

	*lng = bd_lng;
	*lat = bd_lat;
}

void nmea_to_wsg84(double *lng, double *lat, unsigned int lng_GNRMC, unsigned int lat_GNRMC)
{
	double lontmp = 0;
	double lontmp_100 = 0;
	double lattmp = 0;
	double lattmp_100 = 0;
	unsigned int lontmp_int = 0;
	unsigned int lattmp_int = 0;
	double lontmp2unsig_double = 0;
	double lattmp2unsig_double = 0;

	//MOD(n, d) = n - d*INT(n/d)
	lontmp = (double)lng_GNRMC/10000;
	lontmp_int = lng_GNRMC/1000000;
	lontmp2unsig_double = lontmp - (double)lontmp_int*100;
	lontmp_100 = (double)lontmp_int + lontmp2unsig_double/60;


	lattmp = (double)lat_GNRMC/10000;
	lattmp_int = lat_GNRMC/1000000;
	lattmp2unsig_double =  lattmp - (double)lattmp_int*100;
	lattmp_100 = (double)lattmp_int + lattmp2unsig_double/60;

	if(DEBUG) printf("WSG84 longitude:%lf latitude:%lf\n", lontmp_100, lattmp_100);

	*lng = lontmp_100;
	*lat = lattmp_100;
}

void wsg84_to_bd09(double *lng, double *lat)
{
	wgs84_to_gcj02(lng, lat);
	gcj02_to_bd09(lng, lat);
}

#if _TEST_
int main()
{
	struct GNRMC_Info GNRMC;

	double lontmp = 0;
	double lontmp_100 = 0;
	double lattmp = 0;
	double lattmp_100 = 0;
	double lontmp_double = 0;
	double lattmp_double = 0;
	unsigned int lontmp_int = 0;
	unsigned int lattmp_int = 0;
	double lontmp2unsig_double = 0;
	double lattmp2unsig_double = 0;

	double lontmp = 0;
	double lattmp = 0;

	//unsigned char *str = "$GPGGA,,,,,,0,,,,,,,,*66";
	//unsigned char *str = "$GPGGA,163915.00,2238.72719,N,11402.51213,E,1,05,2.5,-1626.9,M,-2.2,M,,*6E";
	unsigned char *str = "$GPGGA,160840.00,2239.13163,N,11402.14353,E,1,07,0.8,-263.2,M,-2.2,M,,*57";
	//unsigned char *str = "$GPGGA,132252.00,2235.81576,N,11359.89204,E,1,06,1.2,56.9,M,-2.3,M,,*49";
	if (GPGGA_Parse(str, &GNRMC) == 0) {
		printf("nmea data, lng:%lf lat:%lf\n", (double)GNRMC.longitude/1000000, (double)GNRMC.latitude/1000000);
		nmea_to_wsg84(&lontmp, &lattmp, GNRMC.longitude, GNRMC.latitude);

		wsg84_to_bd09(&lontmp, &lattmp);

		printf("bd09, lng:%lf lat:%lf\n", lontmp, lattmp);
	}
}
#endif
