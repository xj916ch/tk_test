#include "common.h"

#define STRINGS_ONE_LINE 256
#define RADIUS_LIMIT 100 //100 meters

static void show_help(char *argv[]);
static int get_file_gpgga_info(FILE *fp, double lng_center, double lat_center);

int main(int argc, char *argv[])
{
	if (argc == 1) {
		show_help(argv);
		exit(-1);
	}

	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL) {
		fprintf(stderr, "open file %s failed\n", argv[1]);
		return -1;
	}

	double lng_center = 0;
	double lat_center = 0;

	if (argc == 4) {
		lng_center = atof(argv[2]);
		lat_center = atof(argv[3]);
	} else {
		printf("please input the centre longitude and latitude(in order): ");
		scanf("%lf %lf", &lng_center, &lat_center);
	}
	if (lng_center < lat_center) {
		fprintf(stderr, "IN CHINA, the longitude and latitude data is wrong\n");
		exit(-1);
	}
	printf("YOUR LONGITUDE: %lf, LATITUDE: %lf!!!\n", lng_center, lat_center);
	printf("---------------------------------------------------------------\n\n");

	get_file_gpgga_info(fp, lng_center, lat_center);

	fclose(fp);
	return 0;
}

static int get_file_gpgga_info(FILE *fp, double lng_center, double lat_center)
{
	int line = 1;

	double lon2tuya = 0;
	double lat2tuya = 0;

	struct GNRMC_Info GNRMC;
	memset(&GNRMC, 0, sizeof(struct GNRMC_Info));

	char str[STRINGS_ONE_LINE] = {0};
	while (fgets(str, STRINGS_ONE_LINE, fp) != NULL) {
		line++;

		char *substr = strstr(str, "$GPGGA");
		if (substr == NULL)
			continue;
		
		if(DEBUG) printf("%s", substr);

		if (GPGGA_Parse(substr, &GNRMC) == 0) {
			if (GNRMC.position_valid) {
				nmea_to_wsg84(&lon2tuya, &lat2tuya, GNRMC.longitude, GNRMC.latitude);

				wsg84_to_bd09(&lon2tuya, &lat2tuya);
			}


			double gap = real_range(lat2tuya, lon2tuya, lat_center, lng_center); //ATTENTION!! lng!! lat!!
			fprintf(stderr, "gap: %lf\n", gap);
			if(DEBUG) printf("gap is %lf\n\n", gap);

			if (gap > 100) {
				fprintf(stderr, "line:%d, %s", line, str);
				fprintf(stderr, "bd09 longitude:%lf latitude:%lf\n", lon2tuya, lat2tuya);
			}
		}

		memset(str, 0, STRINGS_ONE_LINE);
	}
}

static void show_help(char *argv[])
{
	fprintf(stderr, "run mode:\n"
			"%s file_name\n"
			"or\n"
			"%s file_name longitude latitude\n",
			argv[0], argv[0]);
}
