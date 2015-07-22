#define _GNU_SOURCE
#include <sys/mman.h>
#include <stdio.h>
#undef _GNU_SOURCE
#include <stdlib.h>
#include <inttypes.h>
#include <dlfcn.h>
#include <omp.h>

#include <miclord.h>

int main (int argc, char **argv)
{
    int nelem = 50000000;
    int offload = argc > 1 ? atoi(argv[1]) : 1;

    void (*voodoo)(double*, double*, double*, int, int);
    void* handle;
    char* error;

    printf("Loading library..\n");

    handle = dlopen("/others/safl/snippets/libvoodoo.so", RTLD_LAZY);
    if (!handle) {
        fputs (dlerror(), stderr);
        exit(1);
    }

    printf("Loading voodoo..\n");
    voodoo = dlsym(handle, "voodoo");
    if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }


    printf("Started...\n");

    double* a = (double*)mmap(0, nelem*sizeof(double), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    double* b = (double*)mmap(0, nelem*sizeof(double), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    double* c = (double*)mmap(0, nelem*sizeof(double), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    
    for(int i=0; i<nelem; ++i) {
        a[i] = i;
        b[i] = i;
        //c[i] = i;
    }   

    mic_alloc(a, nelem, 0, offload);
    mic_alloc(b, nelem, 0, offload);
    mic_alloc(c, nelem, 0, offload);
    printf("Calling voodoo..");
    for (int i=0; i<1; ++i) {
        voodoo(a, b, c, nelem, offload);
    }
    mic_pull_free(c, nelem, 0, offload);
    printf("c[nelem/2] = %f\n", c[nelem/2]);
    printf("Going down\n");
    munmap(a, nelem*sizeof(double));
    munmap(b, nelem*sizeof(double));
    munmap(c, nelem*sizeof(double));

    return 0;
}
