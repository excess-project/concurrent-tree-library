/*
 papicounters.c

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
#include <string.h>
#include <math.h>
#include <pthread.h>

#include "papi.h"
#include "papicounters.h"



#if (defined (__x86_64__) && !defined(__KNC__))

#if defined (__MULTIPLEX)

#define NUM_EVENTS 16

char papi_events[NUM_EVENTS][PAPI_MAX_STR_LEN]={
	"PAPI_TOT_INS",
	"PAPI_TOT_CYC",
	"PAPI_REF_CYC",
	"PAPI_LST_INS",
	"PAPI_L1_DCM",
	"PAPI_L2_DCM",
	"PAPI_L2_DCA",
	"PAPI_L3_TCM",
	"PAPI_L3_TCA",
	"PAPI_L3_LDM",
	"PAPI_BR_CN",
	"PAPI_BR_MSP",
	"DTLB_LOAD_MISSES:WALK_COMPLETED",
	"DTLB_STORE_MISSES:WALK_COMPLETED",
	"DTLB_LOAD_MISSES:WALK_DURATION",
	"DTLB_STORE_MISSES:WALK_DURATION",
};

#endif

# if defined (__PRESET_ONE)

#define NUM_EVENTS 5

char papi_events[NUM_EVENTS][PAPI_MAX_STR_LEN]={
	"PAPI_L1_DCM",
	"PAPI_L2_DCM",
	"PAPI_L2_DCA",
	"PAPI_L3_TCM",
	"PAPI_L3_TCA",
};

/*
  
#define NUM_EVENTS 10

char papi_events[NUM_EVENTS][PAPI_MAX_STR_LEN]={
	"INST_RETIRED:ANY_P",
	"ix86arch::UNHALTED_CORE_CYCLES",
	"ix86arch::UNHALTED_REFERENCE_CYCLES",
	"MEM_LOAD_UOPS_RETIRED:L3_HIT",
	"MEM_LOAD_UOPS_RETIRED:L3_MISS",
	"UOPS_RETIRED:ALL",
	"L2_TRANS:ALL_REQUESTS",
	"L2_RQSTS:MISS",
	"BR_INST_RETIRED:ALL_BRANCHES",
	"BR_MISP_RETIRED:ALL_BRANCHES",
};
*/
# endif /*__PRESET_ONE*/

# if defined (__PRESET_TWO)

#define NUM_EVENTS 9

char papi_events[NUM_EVENTS][PAPI_MAX_STR_LEN]={
	"INST_RETIRED:ANY_P",
	"ix86arch::UNHALTED_CORE_CYCLES",
	"ix86arch::UNHALTED_REFERENCE_CYCLES",
	"MEM_UOPS_RETIRED:ALL_LOADS",
	"MEM_UOPS_RETIRED:ALL_STORES",
	"DTLB_LOAD_MISSES:MISS_CAUSES_A_WALK",
	"DTLB_STORE_MISSES:MISS_CAUSES_A_WALK",
	"DTLB_LOAD_MISSES:WALK_DURATION",
	"DTLB_STORE_MISSES:WALK_DURATION",
};

# endif /*__PRESET_TWO*/

#endif /*__x86_64__*/

#ifdef __arm__

#define NUM_EVENTS 11

char papi_events[NUM_EVENTS][PAPI_MAX_STR_LEN]={
	"PAPI_TOT_INS",
	"PAPI_TOT_CYC",
	"PAPI_L1_DCM",
	"PAPI_L1_DCA",
	"PAPI_L2_DCM",
	"PAPI_L2_DCH",
	"perf::PERF_COUNT_HW_CACHE_DTLB:READ",
	"perf::PERF_COUNT_HW_CACHE_DTLB:WRITE",
	"perf::PERF_COUNT_HW_CACHE_DTLB:MISS",
	"perf::PERF_COUNT_HW_BRANCH_INSTRUCTIONS",
	"perf::PERF_COUNT_HW_BRANCH_MISSES",
};

#endif /*__arm__*/

#ifdef __KNC__

#define NUM_EVENTS 13

char papi_events[NUM_EVENTS][PAPI_MAX_STR_LEN]={
		"CPU_CLK_UNHALTED:mg=0:mh=0",
		"DATA_READ_MISS_OR_WRITE_MISS:mg=0:mh=0",
		"L1_DATA_HIT_INFLIGHT_PF1:mg=0:mh=0",
		"DATA_READ_OR_WRITE:mg=0:mh=0",
		"L2_DATA_READ_MISS_CACHE_FILL:mg=0:mh=0",
		"L2_DATA_READ_MISS_MEM_FILL:mg=0:mh=0",
		"L2_DATA_WRITE_MISS_CACHE_FILL:mg=0:mh=0",
		"L2_DATA_WRITE_MISS_MEM_FILL:mg=0:mh=0",
		"EXEC_STAGE_CYCLES:mg=0:mh=0",
		"L2_VICTIM_REQ_WITH_DATA:mg=0:mh=0",
		"INSTRUCTIONS_EXECUTED:mg=0:mh=0",
		"SNP_HITM_L2:mg=0:mh=0",
		"L2_DATA_PF1_MISS:mg=0:mh=0",
};

#endif /*__KNC__*/


//long long _papivalues[NUM_EVENTS];
//int papiEventSet = PAPI_NULL;
//long long papi_start_time, papi_end_time;

int check_papi_rapl()
{
	int cid,rapl_cid=-1,numcmp;

	const PAPI_component_info_t *cmpinfo = NULL;

	numcmp = PAPI_num_components();

	for(cid=0; cid<numcmp; cid++) {

		if ( (cmpinfo = PAPI_get_component_info(cid)) == NULL) {
			fprintf(stderr,"PAPI_get_component_info failed\n");
			exit(1);
		}
		if (strstr(cmpinfo->name,"rapl")) {
			rapl_cid=cid;
			printf("Found rapl component at cid %d\n", rapl_cid);

			if (cmpinfo->num_native_events==0) {
				fprintf(stderr,"No rapl events found\n");
				exit(1);
			}
			break;
		}
	}

	/* Component not found */
	if (cid==numcmp) {
		fprintf(stderr,"No rapl component found\n");
		exit(1);
	}
	return 0;
}

struct localcounters *prof_prepare(int master)
{
	int i, retval;

	int EventCode;

	struct localcounters *cnt_local;

	cnt_local= (struct localcounters*) malloc(sizeof(struct localcounters));
	cnt_local->values = (long long *) malloc(sizeof(long long) * NUM_EVENTS);
	cnt_local->papiEventSet = PAPI_NULL;
	cnt_local->start_time = 0;
	cnt_local->end_time = 0;

	if (master == 1 ){
		if((retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT ){
			fprintf(stderr, "Error PAPI_Init: %d\n",retval);
			exit(1);
		}
#ifdef __MULTIPLEX
		retval = PAPI_multiplex_init();
		if ( retval == PAPI_ENOSUPP) {
			fprintf(stderr, "Error Multiplex is not supported : %d\n",retval);
		}
		else if ( retval != PAPI_OK ) {
			fprintf(stderr, "Error Init Multiplex: %d\n",retval);
			exit(1); 
		}
#endif
		if ( ( retval = PAPI_thread_init( ( unsigned long ( * )( void ) ) ( pthread_self ) ) ) != PAPI_OK ){
			fprintf(stderr, "Error PAPI_thread_init: %d\n",retval);
			exit(1);
		}
	}else{
		retval = PAPI_register_thread();
		if ( retval != PAPI_OK ) {
			fprintf(stderr, "Error PAPI_register_thread: %d\n",retval);
		}
	}

	/* Create EventSet */
	retval = PAPI_create_eventset( &(cnt_local->papiEventSet) );
	if (retval != PAPI_OK) {
		fprintf(stderr,"Error creating eventset!\n");
	}

	retval = PAPI_assign_eventset_component( cnt_local->papiEventSet, 0 );
	if (retval != PAPI_OK) {
		fprintf(stderr,"PAPI_assign_eventset_component\n");
	}

#ifdef __MULTIPLEX
	/* Convert the ''EventSet'' to a multiplexed event set */
	retval = PAPI_set_multiplex(cnt_local->papiEventSet);
	if (retval < 0) {
			fprintf(stderr,"Error converting multiplex!\n");
	}
#endif

	for(i=0;i<NUM_EVENTS;i++) {
		retval = PAPI_event_name_to_code(papi_events[i], &EventCode);
		retval = PAPI_add_event(cnt_local->papiEventSet, EventCode);
		if (retval != PAPI_OK) {
			fprintf(stderr,"Error adding event %s (%d)\n", papi_events[i], retval);
			exit(99);
		}
	}
	return cnt_local;
}


int prof_start(struct localcounters *cnt)
{
	int retval;
	cnt->start_time=PAPI_get_real_nsec();
	retval = PAPI_start( cnt->papiEventSet);
	if (retval != PAPI_OK) {
		fprintf(stderr,"PAPI_start() failed\n");
		exit(1);
	}
	return 0;
}


int prof_end(struct localcounters *cnt)
{
	int retval;
	cnt->end_time=PAPI_get_real_nsec();
	retval = PAPI_stop( cnt->papiEventSet, cnt->values);
	if (retval != PAPI_OK) {
		fprintf(stderr, "PAPI_stop() failed\n");
	}
	return 0;
}


int prof_print(struct localcounters *cnt)
{
	int i;
	double total_time =((double)(cnt->end_time-cnt->start_time))/1.0e9;

	printf("\n\nTIME,%f\n", total_time);

	for(i=0;i<NUM_EVENTS;i++) {
		printf("#P,%s,%lld\n", papi_events[i], cnt->values[i]);	
	}
	/*
	 printf("PACKAGE_ENERGY:PACKAGE0: %f J\n", (double) _papivalues[1]/1.0e9);
	 printf("PACKAGE_ENERGY:PACKAGE1: %f\n",  (double)_papivalues[2]/1.0e9);
	 printf("DRAM_ENERGY:PACKAGE0: %f\n",  (double)_papivalues[3]/1.0e9);
	 printf("DRAM_ENERGY:PACKAGE1: %f\n", (double) _papivalues[4]/1.0e9);
	 printf("PP0_ENERGY:PACKAGE0: %f\n", (double)_papivalues[5]/1.0e9);
	 printf("PP0_ENERGY:PACKAGE1: %f\n",(double) _papivalues[6]/1.0e9);
	 */
	return 0;
}

int prof_print_all_threads(int threads, long long** values)
{

	long long temp [NUM_EVENTS];
	int i, j;
	for(i=0;i<NUM_EVENTS;i++) {
		temp[i] = 0;
		for(j=0;j<threads;j++) {
			temp[i] += values[j][i];
		}
		printf("#P,%s,%lld\n", papi_events[i], temp[i]);
	}
#if (defined (__x86_64__) && !defined(__KNC__))
	printf("#D,CPI,%f\n", (double)temp[0]/temp[1]);

	printf("#D,L1_MISS_RATIO,%f\n", (double)temp[4]/temp[3]);

	printf("#D,L2_MISS_RATIO,%f\n", (double)temp[5]/temp[6]);

	printf("#D,L3_MISS_RATIO,%f\n", (double)temp[7]/temp[8]);

	printf("#D,L3_LOAD_MISS_RATIO,%f\n", (double)temp[9]/temp[7]);

	printf("#D,BR_MISPREDICTED,%f\n", (double)temp[11]/temp[10]);


	printf("#D,DTLB_WALK_RATIO,%f\n", (double)(temp[14]+temp[15])/temp[1]);

	printf("#D,DTLB_LOAD_WALK_DURATION,%f\n", (double)temp[14]/temp[12]);

	printf("#D,DTLB_STORE_WALK_DURATION,%f\n", (double)temp[15]/temp[13]);

#endif

#if defined (__arm__)
	printf("#D,CPI,%f\n", (double)temp[0]/temp[1]);

	printf("#D,L1_MISS_RATIO,%f\n", (double)temp[2]/temp[3]);

	printf("#D,L2_MISS_RATIO,%f\n", (double)temp[4]/(temp[4]+temp[5]));

	printf("#D,DTLB_MISS_RATIO,%f\n", (double)temp[8]/(temp[6]+temp[7]));

	printf("#D,BR_MISPREDICTED,%f\n", (double)temp[10]/temp[9]);
#endif

#if defined (__KNC__)
	printf("#D,CPI,%f\n", (double)temp[0]/temp[1]);

	printf("#D,L1_MISS_RATIO,%f\n", (double)temp[2]/temp[3]);

	printf("#D,L2_MISS_RATIO,%f\n", (double)(temp[4]+temp[5]+temp[6]+temp[7])/temp[2]);

	printf("#D,L2_MISS_RATIO_CACHE_FILL,%f\n", (double)(temp[4]+temp[6])/temp[2]);

	printf("#D,L2_MISS_RATIO_MEM_FILL,%f\n", (double)(temp[5]+temp[7])/temp[2]);

	printf("#D,DTLB_MISS_RATIO,%f\n", (double)temp[8]/temp[9]);

	printf("#D,BR_MISPREDICTED,%f\n", (double)temp[10]/temp[11]);

	printf("#D,TOT_L2_RW_MISS,%lld\n", temp[4]+temp[5]+temp[6]+temp[7]);
#endif

	return 0;
}
