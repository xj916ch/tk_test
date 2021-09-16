#include "common.h"

#define PI 3.1415926535897932384626433832795
#define EARTH_RADIUS 6378.137 //地球半径 KM

//https://www.cxyzjd.com/article/weixin_41519463/88999339
static double rad(double d)
{
	return (d * PI/180.0);
}

double real_range(double lat1, double lng1, double lat2, double lng2)
{

	double a;
	double b;
	double radLat1 = rad(lat1);
	double radLat2 = rad(lat2);
	a = radLat1 - radLat2;
	b = rad(lng1) - rad(lng2);
	double s = 2 * asin(sqrt(pow(sin(a/2),2) + cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
	s = s * EARTH_RADIUS;
	s = s * 1000;

	return s;
}

#ifdef _TEST_
// the same spot, the caculated dis differs lot
/*
[xinjian@virtualbox: ~/tk_test]$ ./a.out 
input the one: 114.048806 22.656206
input the other one: 114.050064 22.655476
the data you input: 114.048806,22.656206 114.050064,22.655476
dis: 152.660015
[xinjian@virtualbox: ~/tk_test]$ ./a.out 
input the one: 114.049731 22.660604
input the other one: 114.050072 22.655485
the data you input: 114.049731,22.660604 114.050072,22.655485
dis: 570.920169
*/
int main()
{
	double radius = 10;
	/*
	double lat = 114.045231;
	double lng = 22.660792;
	double lat_center = 114.045828;
	double lng_center = 22.660725;
	*/
	double lat = 0;
	double lng = 0;
	double lat_center = 0;
	double lng_center = 0;

	printf("input the one: ");
	scanf("%lf %lf", &lng, &lat);
	printf("input the other one: ");
	scanf("%lf %lf", &lng_center, &lat_center);
	printf("the data you input: %lf,%lf %lf,%lf\n", lng, lat, lng_center, lat_center);

	double ret = real_range(lat, lng, lat_center, lng_center);
	printf("dis: %lf\n", ret);
}
#endif
