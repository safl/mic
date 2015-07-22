#include <inttypes.h>
#include <omp.h>

void mic_alloc(double* data, size_t nelem, int dev, int offload)
{
    #pragma offload_transfer                                    \
            target(mic:dev)                                     \
            nocopy(data:length(nelem) alloc_if(1) free_if(0))   \
            if (offload)
}

void mic_push(double* data, size_t nelem, int dev, int offload)
{
    #pragma offload_transfer                                    \
            target(mic:dev)                                     \
            in(data:length(nelem) alloc_if(0) free_if(0))       \
            if (offload)
}

void mic_push_alloc(double* data, size_t nelem, int dev, int offload)
{
    #pragma offload_transfer                                    \
            target(mic:dev)                                     \
            in(data:length(nelem) alloc_if(1) free_if(0))       \
            if (offload)
}

void mic_pull(double* data, size_t nelem, int dev, int offload)
{
    #pragma offload_transfer                                    \
            target(mic:dev)                                     \
            out(data:length(nelem) alloc_if(0) free_if(0))      \
            if (offload)
}

void mic_pull_free(double* data, size_t nelem, int dev, int offload)
{
    #pragma offload_transfer                                    \
            target(mic:dev)                                     \
            out(data:length(nelem) alloc_if(0) free_if(1))      \
            if (offload)
}

int mic_get_max_threads(int dev, int offload)
{
    int mthreads;
    #pragma offload target(mic:dev) \
        out(mthreads)               \
        if (offload)
    {
        mthreads = omp_get_max_threads();
    }
    return mthreads;
}

