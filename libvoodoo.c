#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <omp.h>
#include <math.h>

__attribute__((target(mic)))
void voodoo(double* a, double* b, double* c, int nelem, int offload)
{
    /*
    #pragma offload_transfer                        \
    target(mic:0)                                   \
    nocopy(a:length(nelem) alloc_if(1) free_if(0))      \
    nocopy(b:length(nelem) alloc_if(1) free_if(0))      \
    nocopy(c:length(nelem) alloc_if(1) free_if(0))      \
    if(offload)
    */

    int mthreads;
    #pragma offload target(mic:0) if(offload) 
    {
        mthreads = omp_get_max_threads();
    }
    const int64_t nworkers = nelem > mthreads ? mthreads : 1;
    const int64_t work_split= nelem / nworkers;
    const int64_t work_spill= nelem % nworkers;

    printf("nworkers: %d, split=%d, spill=%d\n", nworkers, work_split, work_spill);

    /*
    #pragma offload                                     \
        target(mic:0)                                   \
        in(a:length(nelem) alloc_if(0) free_if(0)),     \
        in(b:length(nelem) alloc_if(0) free_if(0)),     \
        in(c:length(nelem) alloc_if(0) free_if(0))      \
        if(offload)
    */
    #pragma offload                                     \
        target(mic:0)                                   \
        in(a:length(nelem) alloc_if(0) free_if(0)),     \
        in(b:length(nelem) alloc_if(0) free_if(0)),     \
        in(c:length(nelem) alloc_if(0) free_if(0))      \
        in(work_split)                                  \
        in(work_spill)                                  \
        if(offload)
    {
        #pragma omp parallel num_threads(nworkers)
        {
            for(int i=0; i<100; ++i) {
                const int tid = omp_get_thread_num();

                int64_t work=0, work_offset=0, work_end=0;
                if (tid < work_spill) {
                    work = work_split + 1;
                    work_offset = tid * work;
                } else {
                    work = work_split;
                    work_offset = tid * work + work_spill;
                }
                work_end = work_offset + work;

                if (work) {
                    for (int eidx=work_offset; eidx<work_end; ++eidx) {
                        c[eidx] = ((a[eidx] + b[eidx]) / 100.0);
                    }
                }
            }
        }
    }
    /*
    #pragma offload_transfer                        \
        target(mic:0)                               \
        out(c:length(nelem) alloc_if(0) free_if(1)) \
        if(offload)
    */
}

