/*
 pcmpower.c

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


#include "cpucounters.h"

PCM * m;

std::vector<CoreCounterState> cstates1, cstates2;
std::vector<SocketCounterState> sktstate1, sktstate2;
SystemCounterState sstate1, sstate2;

uint64 TimeAfter, TimeBefore;

void pcm_bench_start()
{
	m = PCM::getInstance();
        m->resetPMU();
    	m->disableJKTWorkaround();
    	PCM::ErrorCode status = m->program();
    	switch (status)
   	{
    	  case PCM::Success:
        	break;
    	  case PCM::MSRAccessDenied:
        	std::cerr << "Access to Intel(r) Performance Counter Monitor has denied (no MSR or PCI CFG space access)." << std::endl;
        	exit(-1);
    	  case PCM::PMUBusy:
            	m->resetPMU();
    	  default:
        	std::cerr << "Access to Intel(r) Performance Counter Monitor has denied (Unknown error)." << std::endl;
        	exit(-1);
    	}
	m->getAllCounterStates(sstate1, sktstate1, cstates1);
	TimeBefore = m->getTickCount();
}

void pcm_bench_end()
{
	TimeAfter = m->getTickCount();
	m->getAllCounterStates(sstate2, sktstate2, cstates2);
}

void pcm_bench_print()
{
	std::cout << "\n#D,TIME," << TimeAfter - TimeBefore;

	for(uint32 socket=0;socket<m->getNumSockets();++socket){
	    std::cout
              << "\n#D,ENERGY_CONSUMED_"<< socket << "," << getConsumedJoules(sktstate1[socket],sktstate2[socket])
              << "\n#D,ENERGY_DRAM_"<< socket << "," << getDRAMConsumedJoules(sktstate1[socket],sktstate2[socket])
	      << "\n#D,BYTES_READ_"<< socket << "," << getBytesReadFromMC(sktstate1[socket], sktstate2[socket]) / double(1024ULL * 1024ULL * 1024ULL)
              << "\n#D,BYTES_WRITE_"<< socket << "," << getBytesWrittenToMC(sktstate1[socket], sktstate2[socket]) / double(1024ULL * 1024ULL * 1024ULL);
//	   if(socket+1 < m->getNumSockets()) std::cout << ",";
	}
	std::cout << "\n";
	m->cleanup();
	m->resetPMU();
}

