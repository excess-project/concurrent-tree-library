/*
 armpower.c

 This is part of the tree library

 Copyright 2015 Ibrahim Umar (UiT the Arctic University of Norway)

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "armpower.h"

#define ARMPOWER_MAX_COUNTERS 4

volatile unsigned msrKeepAlive;
pthread_t msrPthread;

struct timeval _arm_t0, _arm_t1;


int read_sysfs_file(double* counts) {
        FILE* fp_a7 = NULL;
        FILE* fp_a15 = NULL;
        FILE* fp_mem = NULL;
        FILE* fp_gpu = NULL;

        int retval = 1;
        fp_a7 = fopen("/sys/bus/i2c/drivers/INA231/4-0045/sensor_W","r");
    	fp_a15 = fopen("/sys/bus/i2c/drivers/INA231/4-0040/sensor_W","r");
	fp_mem = fopen("/sys/bus/i2c/drivers/INA231/4-0041/sensor_W","r");
	fp_gpu = fopen("/sys/bus/i2c/drivers/INA231/4-0044/sensor_W","r");

        if (!fp_a7 || !fp_a15 || !fp_mem || !fp_gpu){
	    fprintf(stderr, "Error reading the ARM energy sensors!\n");
            exit(10);
	}

	retval &= fscanf(fp_a7,  "%lf", &counts[0]);
	retval &= fscanf(fp_a15, "%lf", &counts[1]);
	retval &= fscanf(fp_mem, "%lf", &counts[2]);
	retval &= fscanf(fp_gpu, "%lf", &counts[3]);

        fclose(fp_a7);
        fclose(fp_a15);
        fclose(fp_mem);
        fclose(fp_gpu);

        return retval;
}

void* recordEnergy(void *arg) {
  int i = 0;
  double counts[ARMPOWER_MAX_COUNTERS];    // used for caching
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 300000000L; // 300 milliseconds

  double energy[4] = {0.0};

  while (msrKeepAlive) {
    read_sysfs_file(counts);
    for(i = 0; i<ARMPOWER_MAX_COUNTERS; i++)
    	energy[i] += counts[i];
    nanosleep(&ts, NULL);
  }

  msrKeepAlive = 1;

  printf("\n#D,Energy_A7,%lf\n", energy[0] * 300 / 1000);
  printf("#D,Energy_A15,%lf\n", energy[1] * 300 / 1000);
  printf("#D,Energy_MEM,%lf\n", energy[2] * 300 / 1000);
  printf("#D,Energy_GPU,%lf\n", energy[3] * 300 / 1000);

  return arg;

}


void armpower_start() {
	FILE* fp_a7 = NULL;
        FILE* fp_a15 = NULL;
        FILE* fp_mem = NULL;
        FILE* fp_gpu = NULL;

        fp_a7 = fopen("/sys/bus/i2c/drivers/INA231/4-0045/enable","w");
        fp_a15 = fopen("/sys/bus/i2c/drivers/INA231/4-0040/enable","w");
        fp_mem = fopen("/sys/bus/i2c/drivers/INA231/4-0041/enable","w");
        fp_gpu = fopen("/sys/bus/i2c/drivers/INA231/4-0044/enable","w");

        if (!fp_a7 || !fp_a15 || !fp_mem || !fp_gpu){
            fprintf(stderr, "Error initializing ARM sensors!\n");
            exit(10);
        }

        fputc('0', fp_a7);
        fputc('0', fp_a15);
        fputc('0', fp_mem);
        fputc('0', fp_gpu);


        fputc('1', fp_a7);
        fputc('1', fp_a15);
        fputc('1', fp_mem);
        fputc('1', fp_gpu);

        fclose(fp_a7);
        fclose(fp_a15);
        fclose(fp_mem);
        fclose(fp_gpu);

  	msrKeepAlive = 1;

  	pthread_create(&msrPthread, NULL, recordEnergy, (void*)NULL);
  	gettimeofday(&_arm_t0, NULL);

}

void armpower_finalize() {

  	gettimeofday(&_arm_t1, NULL);
  	msrKeepAlive = 0;
  	pthread_join(msrPthread, NULL);

        float elapsed = (_arm_t1.tv_sec-_arm_t0.tv_sec)*1000 + (_arm_t1.tv_usec-_arm_t0.tv_usec)/1000;
        printf("#D,Time,%f\n", elapsed);

}
