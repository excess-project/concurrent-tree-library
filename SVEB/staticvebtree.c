/*
 main.c

 (global lock-based) STATIC IMPLICIT VEB Tree

 Based on the code from:
 G. S. Brodal, R. Fagerberg, and R. Jacob, “Cache oblivious search trees via binary trees of small height,”
 in Proceedings of the thirteenth annual ACM-SIAM symposium on Discrete algorithms, ser. SODA ’02, 2002, pp. 39–48.

 Copyright belongs to the authors.

 */


# 1 "main.c"





#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <math.h>

#include <time.h>
#include <sys/times.h>
#include <sys/time.h>

#include <signal.h>
#include <pthread.h>

#include <limits.h>
#define MAXINT INT_MAX

#include "locks.h"
#include "staticvebtree.h"

# 31 "main.c"


//Added global lock
pthread_spinlock_t global_lock;


extern char *optarg;
extern int optind, opterr, optopt;









# 61 "main.c"













volatile domain res;

int n;




# 1 "implicit.c" 1

int nextvalue=0;
int depth=0;

int size;

domain *val;














# 30 "implicit.c"



















int max_dep;





# 69 "implicit.c"















typedef struct li {
    int p, ts, bs;
} levelinfo;

levelinfo *myli;
int *anc;
int bf;

int h;

void init_height(int top, int bot) {
    int me = (top+bot+1) / 2;

    if( top <= bot +1 ) return;
    myli[me].p = top;
    myli[me].ts = ( 1 << (top-me) ) -1;
    myli[me].bs = ( 1 << (me-bot) ) -1;
    init_height( top,me);
    init_height( me,bot);
}

void initialize_depth( int nn ) {
    {int hnn= nn ;max_dep=0;while((hnn=(hnn>>1)))max_dep++;max_dep++;} ;
    if(!myli) myli = malloc( 40 * sizeof( levelinfo ) );
    if( !myli) exit(23);

    myli[0].p = 0;
    myli[0].ts = 0;
    myli[0].bs = 0;

    if(!anc) anc = malloc( 40 * sizeof( int ) );
    if( !anc ) exit(23);

    init_height( max_dep,0);
}



# 141 "implicit.c"


# 188 "implicit.c"

















# 1 "implicit_h.c" 1

































































void impl_fill_al_rec ( int ad) {
    if( ! (  ad  < size && h > 0)  ) return;


    h--; ; ;
    impl_fill_al_rec ( (bf=bf*2,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ; ;


    ;val[ ad ] = nextvalue++; ;

    h--; ; ;
    impl_fill_al_rec ( (bf=2*bf+1,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ; ;



    return;
}

void impl_fill () {
    nextvalue=0;size=n;fprintf(stderr,"starting fill %d\n", n); ;
    h=max_dep;bf=1;anc[max_dep]=0; ;
    impl_fill_al_rec ( 0  );
    ;
}




# 168 "implicit_h.c"




# 199 "implicit_h.c"


# 239 "implicit_h.c"









# 205 "implicit.c" 2







# 1 "implicit_h.c" 1

































































void impl_acc_al_rec ( int ad) {
    if( ! (  ad  < size && h > 0)  ) return;


    h--; ; ;
    impl_acc_al_rec ( (bf=bf*2,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ; ;


    ;val[ ad ]++; ;

    h--; ; ;
    impl_acc_al_rec ( (bf=2*bf+1,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ; ;



    return;
}

void impl_acc_all () {
    ;
    h=max_dep;bf=1;anc[max_dep]=0; ;
    impl_acc_al_rec ( 0  );
    ;
}




# 168 "implicit_h.c"




# 199 "implicit_h.c"


# 239 "implicit_h.c"









# 212 "implicit.c" 2




int search( domain key ) {

    int ad= 0 ;

    int last_right= 0 ;

    h=max_dep;bf=1;anc[max_dep]=0; ;

# 274 "implicit.c"

    while( (  ad  < size && h > 0)  ) {

        ;
        if( ( key <val[ ad ])  ) {
            h--; ;
            ad = (bf=bf*2,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) ;
            continue;
        }
        else {

            last_right=ad;
            h--; ;
            ad= (bf=2*bf+1,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) ;
        }
    }



    ;
    res = val[last_right];


    return last_right;

}


# 329 "implicit.c"


void test_all_searches() {
    int i,it;


    for(i=0;i<n;i++) {


        it  = search(i);

        ;
        if( val[it] != i ) fprintf(stderr,"bailing out with code %d\n", 36 ),printf("# error %d\n", 36 ),exit( 36 ) ;







    }
    fprintf(stderr,"passed all searches\n");
}


int lh,rh;
int keys=0;
int *helper;
int key;
int was_in;





void rebuild( int node ) {
    int m,x;
    if( ! (  node  < size && h > 0)  ) {
        if( lh <= rh ) fprintf(stderr, "loosing things in rebuild!\n");
        return;
    }
    if( lh > rh ) {
        h--; ;
        rebuild( (bf=bf*2,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p])  );
        h++;bf=bf>>1; ;


        val[node] = MAXINT ;
        h--; ;
        rebuild( (bf=2*bf+1,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p])  );
        h++;bf=bf>>1; ;
        return;
    }

    m = (lh+rh) /2;
    x=rh;

    rh=m-1;
    h--; ;
    rebuild( (bf=bf*2,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p])  );
    h++;bf=bf>>1; ;
    rh=x;

    ; ;
    val[node] = helper[m];
    lh=m+1;
    h--; ;
    rebuild( (bf=2*bf+1,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p])  );
    h++;bf=bf>>1; ;

}



int level;
int l_count(int node) {
    int c;

    if( ! (  node  < size && h > 0)  ) return 0;
    ;
    if( MAXINT  == val[node] ) return 0;

    h--; ;
    c=l_count((bf=2*bf+1,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ;
    lh--;

    ; ;
    helper[lh] = val[node];
    val[node] = -3;
    c++;
    h--; ;
    c += l_count((bf=bf*2,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ;
    return c;
}
int r_count(int node) {
    int c;

    if( ! (  node  < size && h > 0)  ) return 0;
    if( MAXINT  == val[node] ) return 0;

    h--; ;
    c=r_count((bf=bf*2,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ;
    rh++;
    ; ;
    helper[rh] = val[node];
    val[node] = -2;

    c++;
    h--; ;
    c += r_count((bf=2*bf+1,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ;
    return c;
}
int insert_rec(int node ) {

    if( ! (  node  < size && h > 0)  ) {
        lh=rh=size;
        ;
        helper[size] = key;

        return 1;
    }
    ;
    if( MAXINT  == val[node] ) {
        val[node]=key;

        return 0;
    }

    if( key < val[node] ) {
        int c;
        h--; ;
        level--;
        c = insert_rec((bf=bf*2,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
        level++;
        h++;bf=bf>>1; ;
        if( 0 == c ) return 0;

        ; ;
        rh++;helper[rh] = val[node];
        val[node] = -4;


        h--; ;
        c += r_count( (bf=2*bf+1,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p])  );
        h++;bf=bf>>1; ;
        c++;



        if( c <= ((float) (1<<level)-1)*(1.0-((((float)level)/max_dep)*0.6)) ) {
            rebuild( node );
            return 0;
        }
        return c;
    }
    ;
    if( val[node] < key ) {
        int c;
        h--; ;
        level--;
        c = insert_rec((bf=2*bf+1,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
        level++;
        h++;bf=bf>>1; ;
        if( 0 == c ) return 0;

        ; ;
        lh--;helper[lh] = val[node];
        val[node] = -5;



        h--; ;
        c += l_count( (bf=bf*2,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p])  );
        h++;bf=bf>>1; ;
        c++;



        if( c <= ((float) (1<<level)-1)*(1.0-((((float)level)/max_dep)*0.6)) ) {
            rebuild( node );
            return 0;
        }
        return c;
    }


    was_in = -1;
    keys--;
    return 0;
}

int insert(int ky) {

    pthread_spin_lock(&global_lock);

    int c;
    h=max_dep;bf=1;anc[max_dep]=0; ;
    key=ky;
    level=max_dep;
    was_in = 0;
    keys++;


    c =insert_rec( 0  );

    if( was_in ) {
        pthread_spin_unlock(&global_lock);
        return 0;
    }
    if( c ) {

        size=(size+1)*2 -1;
        initialize_depth(size);


        //fprintf(stderr,"resize to: %d max_depth %d, members: %d(%d) density %f\n", size,max_dep,c,keys, ((float)c)/n);


        if(val) free(val);
        val = malloc( (size+2)* sizeof ( domain ));




        if( !val ) fprintf(stderr,"bailing out with code %d\n", 21 ),printf("# error %d\n", 21 ),exit( 21 ) ;
        {int i; for(i=0;i<size+2;i++) val[i]= MAXINT ;}

        h=max_dep;bf=1;anc[max_dep]=0; ;
        rebuild( 0  );



        if(helper) free(helper);
        helper = malloc( 2* size* sizeof ( domain ));
        if( !helper ) fprintf(stderr,"bailing out with code %d\n", 211 ),printf("# error %d\n", 211 ),exit( 211 ) ;
    }

    pthread_spin_unlock(&global_lock);

    return 1;
}







int e;










# 1 "implicit_h.c" 1

































































void impl_report_al_rec ( int ad) {
    if( ! (  ad  < size && h > 0)  ) return;


    h--; ;depth++; ;
    impl_report_al_rec ( (bf=bf*2,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ;depth--; ;


    {fprintf(stderr,"<%2d>",depth);for(e=0;e<depth;e++) fprintf(stderr," .");if(val[ ad ]== MAXINT ) fprintf(stderr,"  +"); else fprintf(stderr,"%3d",val[ ad ]);for(;e<16;e++) fprintf(stderr," -");fprintf(stderr,"<%d>\n",  ad  );} ;

    h--; ;depth++; ;
    impl_report_al_rec ( (bf=2*bf+1,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ;depth--; ;



    return;
}

void impl_report_al () {
    ;
    h=max_dep;bf=1;anc[max_dep]=0; ;
    impl_report_al_rec ( 0  );
    ;
}




# 168 "implicit_h.c"




# 199 "implicit_h.c"


# 239 "implicit_h.c"









# 582 "implicit.c" 2




int maxd=0;
int sum_path=0;





# 1 "implicit_h.c" 1

































































void impl_report_depth_rec ( int ad) {
    if( ! (  ad  < size && h > 0)  ) return;


    h--; ;depth++; ;
    impl_report_depth_rec ( (bf=bf*2,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ;depth--; ;


    {sum_path=sum_path+depth;if(depth>maxd)maxd=depth;} ;

    h--; ;depth++; ;
    impl_report_depth_rec ( (bf=2*bf+1,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ;depth--; ;



    return;
}

void impl_report_depth () {
    ;
    h=max_dep;bf=1;anc[max_dep]=0; ;
    impl_report_depth_rec ( 0  );
    ;
}




# 168 "implicit_h.c"




# 199 "implicit_h.c"


# 239 "implicit_h.c"









# 593 "implicit.c" 2























# 1 "implicit_h.c" 1

































































void test_walk_rec ( int ad) {
    if( ! (  ad  < size && h > 0)  ) return;


    h--; ; ;
    test_walk_rec ( (bf=bf*2,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ; ;


    { ;if(val[ ad ]!= MAXINT ){if(nextvalue!=val[ ad ])fprintf(stderr,"bailing out with code %d\n", 345 ),printf("# error %d\n", 345 ),exit( 345 ) ;nextvalue++;}} ;

    h--; ; ;
    test_walk_rec ( (bf=2*bf+1,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) );
    h++;bf=bf>>1; ; ;



    return;
}

void test_rec_walk () {
    nextvalue=0; ;
    h=max_dep;bf=1;anc[max_dep]=0; ;
    test_walk_rec ( 0  );
    if(nextvalue!=keys) fprintf(stderr,"bailing out with code %d\n", 335 ),printf("# error %d\n", 335 ),exit( 335 ) ;fprintf(stderr,"passed recursive walk test\n"); ;
}




# 168 "implicit_h.c"




# 199 "implicit_h.c"


# 239 "implicit_h.c"









# 616 "implicit.c" 2





















# 1 "implicit_h.c" 1































































# 93 "implicit_h.c"













void test_it_walk () {
    int ad = 0 ;

    nextvalue=0; ;
    h=max_dep;bf=1;anc[max_dep]=0; ;





descent:








    while( (  ad  < size && h > 0)  ) {
        h--; ; ;
        ad = (bf=bf*2,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) ;
    }

    h++;bf=bf>>1; ; ;
    ad = (anc[h]) ;

doit:

    { ;if(val[ ad ]!= MAXINT ){if(nextvalue!=val[ ad ])fprintf(stderr,"bailing out with code %d\n", 348 ),printf("# error %d\n", 348 ),exit( 348 ) ;nextvalue++;}} ;

    h--; ; ;
    ad = (bf=2*bf+1,anc[h]=(myli[h].bs*(bf&myli[h].ts))+myli[h].ts+anc[myli[h].p]) ;
    if( (  ad  < size && h > 0)  )
        goto descent;






    h++;bf=bf>>1; ; ;
    ad = (anc[h]) ;


    while( (bf&1)  ) {

        h++;bf=bf>>1; ; ;
        ad = (anc[h]) ;
        if( ad <= 0  ) {
            if(nextvalue!=keys) fprintf(stderr,"bailing out with code %d\n", 936 ),printf("# error %d\n", 936 ),exit( 936 ) ;  fprintf(stderr,"passed iterative walk\n"); ;
            return;
        }
    }
    h++;bf=bf>>1; ; ;
    ad = (anc[h]) ;



    goto doit;

}




# 199 "implicit_h.c"


# 239 "implicit_h.c"









# 637 "implicit.c" 2













void init_memory() {
    int i;

    size = 7;



    val=malloc((size+2)*sizeof(domain));if(!val)fprintf(stderr,"bailing out with code %d\n", 131 ),printf("# error %d\n", 131 ),exit( 131 ) ;
    for(i=0;i<size+2;i++) val[i]= MAXINT ;

    helper = malloc( 2* size * sizeof ( domain ));
    if( !helper ) exit(21);

    initialize_depth(size);

}













# 85 "main.c" 2






















# 1 "timer.c" 1




char *caption="";
float handler_interv,handler_elapsed=0;
volatile int run;
void timer_handler(int sig){
    run = 0;
    handler_elapsed += handler_interv;
}
volatile int cont_exp;

int exitvalue=0;

void int_handler(int sig){
    if( cont_exp < 0 ) {
        if(run==0 ) {
            fprintf(stderr,"bailing out on ctrl-C\n");
            printf(" %s # bail out\n",caption);
            fprintf(stderr,"bailing out with code %d\n", 1 ),printf("# error %d\n", 1 ),exit( 1 ) ;
        }
        run = 0;
        caption = "# interrupted experiment";
        fprintf(stderr,"stoping running experiment\n");
    } else {
        cont_exp=-1;
        caption = "# forced limit on repetitions";
        fprintf(stderr,"stoping experiments\n");
    }
    exitvalue=1;
}

# 73 "timer.c"

# 107 "main.c" 2


float max_fr=1.01, min_fr=.99;
int stabil_exps=3,duration=10;

int curr_entries;

char *myname;

















void time_searches() {
    long int r=0;
    float it_time;
    float time_per_action=0.0,max_tpa=0.0,min_tpa=-1.0;
    int expct;

    expct=1;
    cont_exp=100;
    printf("%s:search: %d %d", myname, curr_entries, size);
    while( cont_exp > 0 ) {
        fprintf(stderr,"starting experiment %d (%d)\n", expct, cont_exp);
        run=1;
        r=0;

# 1 "timer.c" 1


# 34 "timer.c"

        {
            struct tms buf;
            struct itimerval myitval,mygtval;
            time_t startt;
            time_t searcht;

            signal(SIGALRM,timer_handler);
            signal(SIGINT,int_handler);

            myitval.it_interval.tv_sec = myitval.it_value.tv_sec = duration;
            myitval.it_interval.tv_usec= myitval.it_value.tv_usec= 0;

            mygtval.it_interval.tv_sec=0;  mygtval.it_interval.tv_usec=0;
            mygtval.it_value.tv_sec=0;     mygtval.it_value.tv_usec=0;


            handler_interv=.001*.001*myitval.it_value.tv_usec+myitval.it_value.tv_sec;
            handler_elapsed=0.0;

            setitimer(ITIMER_REAL, &myitval, 0 );

            startt = times(&buf);

            while( run>0 && r>=0 ) {search( MAXINT * drand48() ); ; r++;   } ;

            getitimer(ITIMER_REAL, &mygtval);
            searcht = times(&buf) - startt;

            it_time = 0.001*0.001*(myitval.it_value.tv_usec- mygtval.it_value.tv_usec)
            + (myitval.it_value.tv_sec - mygtval.it_value.tv_sec)
            + handler_elapsed;

            fprintf(stderr,"t: t%ld (%f)\n",
                    searcht,it_time);

        }



# 146 "main.c" 2

        if(r<0) r--;
        time_per_action = it_time/r;
        fprintf(stderr,"s: #%ld  %e\n",
                (long int) r, time_per_action);

        printf(" %e", time_per_action);

        if( max_tpa > 0  && cont_exp > stabil_exps
           && time_per_action < max_fr * max_tpa
           && time_per_action > min_fr * min_tpa ) cont_exp = stabil_exps;

        if( max_tpa < time_per_action ) max_tpa = time_per_action;
        if( min_tpa > time_per_action ||  min_tpa < 0 ) min_tpa=time_per_action;
        cont_exp--;
        expct++;
    }
    printf("\n");
    fflush(stdout);
}


void time_inserts(int k) {
    float time_per_action;
    int i;
    float it_time;


# 1 "timer.c" 1


# 34 "timer.c"

    {
        struct tms buf;
        struct itimerval myitval,mygtval;
        time_t startt;
        time_t searcht;

        signal(SIGALRM,timer_handler);
        signal(SIGINT,int_handler);
        
        myitval.it_interval.tv_sec = myitval.it_value.tv_sec = duration;
        myitval.it_interval.tv_usec= myitval.it_value.tv_usec= 0;
        
        mygtval.it_interval.tv_sec=0;  mygtval.it_interval.tv_usec=0;
        mygtval.it_value.tv_sec=0;     mygtval.it_value.tv_usec=0;
        
        
        handler_interv=.001*.001*myitval.it_value.tv_usec+myitval.it_value.tv_sec;
        handler_elapsed=0.0;
        
        setitimer(ITIMER_REAL, &myitval, 0 );
        
        startt = times(&buf);
        
        for(i=0;i<k;i++) {if(insert((int)(drand48()*(double)MAXINT)))curr_entries++;} ;
        
        getitimer(ITIMER_REAL, &mygtval);
        searcht = times(&buf) - startt;
        
        it_time = 0.001*0.001*(myitval.it_value.tv_usec- mygtval.it_value.tv_usec)
        + (myitval.it_value.tv_sec - mygtval.it_value.tv_sec)
        + handler_elapsed;
        
        fprintf(stderr,"t: t%ld (%f)\n",
                searcht,it_time); 
        
    }
    
    
    
# 174 "main.c" 2
    
    time_per_action = it_time/k;
    fprintf(stderr,"ins: %d #%d (%d)  %e\n",curr_entries, k,size, time_per_action); 
    printf("%s:insert: %d %d %d %e %e %e\n", myname, curr_entries, size,k, time_per_action, time_per_action, time_per_action); 
    fflush(stdout);
}

int delete_node(int key) {
    int it;
    
    pthread_spin_lock(&global_lock);
    
    it  = search(key);
    
    pthread_spin_unlock(&global_lock);
    
    
    if(val[it] == key) return 1;
    else return 0;
    
};

int search_test(domain key) {
    int it;
    
    pthread_spin_lock(&global_lock);
    
    it  = search(key);
    
    pthread_spin_unlock(&global_lock);
    
    if(val[it] == key) return 1;
    else return 0;
}

int init_tree( int t) {
    
    
    n=t;
    
    fprintf(stderr,"Initial tree size n:%d\n", n);
    
    
    init_memory();       
    fprintf(stderr,"finished init memory\n");
    
    pthread_spin_init(&global_lock, PTHREAD_PROCESS_PRIVATE);
    
    
    return 1;
}

void initial_add (int num, int range) {
    int i = 0, j = 0;
    
    while(i < num){
        j = (rand()%range) + 1;
        i += insert(j);
        
    }
}


