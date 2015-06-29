#include "./intelpcm/cpucounters.h"

PCM * m;

std::vector<CoreCounterState> cstates1, cstates2;
std::vector<SocketCounterState> sktstate1, sktstate2;
SystemCounterState sstate1, sstate2;

uint64 TimeAfter, TimeBefore;

void pcm_bench_start()
{
	m = PCM::getInstance();
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
        	std::cerr << "Access to Intel(r) Performance Counter Monitor has denied (Performance Monitoring Unit is occupied by other application). Try to stop the application that uses PMU." << std::endl;
        	std::cerr << "Alternatively you can try to reset PMU configuration at your own risk. Try to reset? (y/n)" << std::endl;
        	char yn;
        	std::cin >> yn;
        	if ('y' == yn)
        	{
            		m->resetPMU();
            		std::cout << "PMU configuration has been reset. Try to rerun the program again." << std::endl;
        	}
       		exit(-1);
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
	for(uint32 socket=0;socket<m->getNumSockets();++socket){
	    std::cout << "\nS:"<<socket
              << ","<< getConsumedJoules(sktstate1[socket],sktstate2[socket])
              << ","<< getDRAMConsumedJoules(sktstate1[socket],sktstate2[socket])
	      << ","<< getBytesReadFromMC(sktstate1[socket], sktstate2[socket]) / double(1024ULL * 1024ULL * 1024ULL)
              << ","<< getBytesWrittenToMC(sktstate1[socket], sktstate2[socket]) / double(1024ULL * 1024ULL * 1024ULL)
              << "\n";
	}
	std::cout << "T:" << TimeBefore << "," << TimeAfter << "\n";
	m->cleanup();
	m->resetPMU();
}

