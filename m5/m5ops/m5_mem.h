#include <sys/mman.h>
#include <fcntl.h>

void *m5_mem = NULL;

void map_m5_mem()
{
#ifdef M5OP_ADDR
    printf("Holaa!!!%p\n", m5_mem);

    int fd;

    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        fprintf(stderr, "Can't open /dev/mem\n");
        exit(1);
    }

    m5_mem = mmap(NULL, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, M5OP_ADDR);
    if (!m5_mem) {
        fprintf(stderr, "Can't mmap /dev/mem\n");
        exit(1);
    }
    printf("Holaa!!!%p\n", m5_mem);

#endif
}

