#include "sharedmemory.h"

SharedMemory::SharedMemory(size_t s)
    : SHM_NAME(getRandomMemoryName(100)), creator(getpid()), size(s) {
    // via fork() address space is being duplicated -
    // so no second construction occurs and only owner (parent)
    // has right to unlink shared memory
    // children has "creator" member set to pid of parent

    // random memory name allows to create multiple memories in the same parent
    // (and even children can create own shared memory, which can be shared with
    // children's children)
}

SharedMemory::~SharedMemory() {
    shmdt(memory);
    if (getpid() == creator) {
        shm_unlink(SHM_NAME);
    }
}

void *SharedMemory::getPtr() {
    pid_t thisProc = getpid();
    for (auto&& p : processesWithAccess) {
        if (p == thisProc) {
            return memory;
        }
    }
    memory = getSHM();
    processesWithAccess.push_back(thisProc);
    return memory;
}

void *SharedMemory::getSHM() {
    int shm;
    void *memory;
    shm = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRWXU | S_IRWXO);
    ftruncate(shm, size);
    memory = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, shm, 0);
    return memory;
}

const char *SharedMemory::getRandomMemoryName(size_t len)
{
    srand(time(NULL));
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(len,0);
    std::generate_n( str.begin(), len, randchar );
    str = "/" + str;
    return str.data();
}
