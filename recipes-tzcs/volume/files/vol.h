#ifndef _vol_h
#define _vol_h
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SET_MIN_VOL "\
amixer set Master 0 > /dev/null\n\
"

#define SET_MAX_VOL "\
amixer set Master 100% > /dev/null\n\
"

#define GET_VOL_LIMIT "\
amixer sget Master | grep Limits | awk 'END{printf $5}'\n\
"

#define GET_VOL "\
amixer sget Master | awk 'END{print $5}' | sed \"s/\\[\\(.*\\)dB\\]/\\1/\"\n\
"

#define GET_CUR_VOL "\
cat /etc/volume.conf\n\
"

//Get volume infomation
double get_info(const char *cmd) {
        double result;
        FILE *fp;
        char path[100];

        /* Open the command for reading. */
        fp = popen(cmd, "r");
        if (fp == NULL) {
                printf("Failed to run command\n");
                return 0;
        }
        /* Read the output a line at a time - output it. */
        while (fgets(path, sizeof (path) - 1, fp) != NULL) {
                                result = atof(path);
        }
        /* close */
        pclose(fp);
        return result;
}

/* Write information to text file */
void  write_info(double cmd) {
        FILE *fp;
        /* Open the file for writing. */
        fp = fopen("/etc/volume.conf", "w+");
        if (fp == NULL) {
                printf("Failed to run command\n");
        }
        /* Write information to the text file */
	fprintf(fp,"%f",cmd);   
	/* close */
        fclose(fp);
}


// Convert volume from db to W
double db_to_pow(double volume_db) {
	double index;
	double volume_w;
	index = volume_db/30;
	volume_w = pow(10,index);
	return volume_w;
}

// Convert volume from W to db
double pow_to_db(double volume_w) {
        double volume_db;
        volume_db = log10(volume_w);
	volume_db = volume_db*30;
        return volume_db;
}

// Get volume step
double get_vol_step() {
	double max_vol_db;
	double max_vol_w;
	double vol_step_w;
	double vol_max_w;
	system(SET_MAX_VOL);	
	max_vol_db = get_info(GET_VOL);
	max_vol_w = db_to_pow(max_vol_db);
	vol_step_w = max_vol_w/100;
	return vol_step_w;
}

// Get range of volume
//double get_vol_ran()
//{
//	double min_vol_db;
//      double max_vol_db;
//      double vol_ran;
//	system(SET_MIN_VOL);
//      min_vol_db = get_info(GET_VOL);
//	system(SET_MAX_VOL);
//      max_vol_db = get_info(GET_VOL);
//      vol_ran = max_vol_db - min_vol_db;
//      return vol_ran;
//}

extern void  write_info(double cmd);
extern double db_to_pow(double volume_db);
extern double pow_to_db(double volume_w);
extern double get_vol_step();
//extern double get_vol_ran();
#endif
