#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h> /* For O_* constants */
#include <vector>
#include <algorithm>  //for std::generate_n
#include <string>
#include <cstdio>

class SharedMemory
{
    const char *SHM_NAME;
    void *memory;
    size_t size;
    pid_t creator;
    std::vector<pid_t> processesWithAccess;
public:
    // !!! ctor should be only called in Parent thread !!!
    // shared memory is being accessable as long as parent does not destruct the object
    SharedMemory(size_t s);
    ~SharedMemory();

    // can be called multiple times
    void *getPtr();

private:
    void *getSHM();
    static const char *getRandomMemoryName(size_t len);
};

#endif // SHAREDMEMORY_H
