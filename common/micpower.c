/*
 
 Credits: Qingpeng N. (https://software.intel.com/en-us/forums/intel-many-integrated-core/topic/507670)
 
 */

// -sh-4.2$ cat /sys/class/micras/power
// 104000000  //Total power, win 0 uW
// 104000000  //Total power, win 1 uW
// 106000000  //PCI-E connector power
// 217000000  //Instantaneous power uW
// 32000000   //Max Instantaneous power
// 26000000   //2x3 connector power
// 48000000   //2x4 connector power
// 1) Core rail; Power reading(uW)
// 2) Core rail; Current(uA)
// 3) Core rail; Voltage(uV)
// 24000000 0 995000 
// 33000000 0 1000000 //Uncore rail; Power reading, Current, Voltage
// 32000000 0 1501000 //Memory subsystem rail; Power reading, Current, Voltage

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define MICPOWER_MAX_COUNTERS 16
typedef struct MICPOWER_control_state {
    long long counts[MICPOWER_MAX_COUNTERS];    // used for caching
    long long lastupdate;
} MICPOWER_control_state_t;

static int read_sysfs_file(long long* counts) {
        FILE* fp = NULL;
        int i;
        int retval = 1;
        fp = fopen("/sys/class/micras/power", "r");
        if (!fp)
            return 0;
        for (i=0; i < MICPOWER_MAX_COUNTERS-9; i++) {
                retval &= fscanf(fp, "%lld", &counts[i]);
        }
        for (i = MICPOWER_MAX_COUNTERS - 9; i < MICPOWER_MAX_COUNTERS; i += 3) {
                retval &= fscanf(fp, "%lld %lld %lld", &counts[i], &counts[i+1], &counts[i+2]);
        }
        fclose(fp);
        return retval;
}

volatile unsigned keepAlive = 1;
double passEnergy = 0.0;
void* recordEnergy(void *arg) {

   struct timeval t1, t2;
   double elapsedTime;

   gettimeofday(&t1, NULL);


  int retval = 0;
    long long counts[MICPOWER_MAX_COUNTERS];    // used for caching
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 50000000L; // 50 milliseconds
  double energy = 0.0;
  while (keepAlive) {
    retval = read_sysfs_file(counts);
    energy += counts[0];
    nanosleep(&ts, NULL);
  }

  gettimeofday(&t2, NULL);

  keepAlive = 1;
  
  elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
  elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms

  passEnergy = energy;
  printf("#D,ENERGY_MIC,%lf\n", energy * 50.0 / 1000 / 1000 / 1000);
  printf("#D,TIME,%lf\n", elapsedTime);

  energy = 0.0;

  return arg;
}

pthread_t micPthread;
void micpower_start() {
  int iret1 = pthread_create(&micPthread, NULL, recordEnergy, (void*)NULL);
}

double micpower_finalize() {
  keepAlive = 0;
  pthread_join(micPthread, NULL);
  return passEnergy * 50.0 / 1000 / 1000 / 1000;
}
