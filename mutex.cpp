#include <array>
#include <atomic>
#include <cstring>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <utility>

class MutexGuard {
public:
  MutexGuard(int _id, int *_mem) : id(_id), mem(_mem) {}

  ~MutexGuard() { this->mem[this->id] = 0; }

  int *operator*() { return this->mem + 3; }

private:
  int id;
  int *mem;
};

class Mutex {
public:
  Mutex(int _id, std::string path, size_t size) {
    if (_id < 0 || _id > 1) {
      printf("invalid id\n");
      exit(-1);
    }

    this->id = _id;
    this->initialized = false;

    int shmid;
    key_t key;
    long i;

    if ((key = ftok(path.data(), 0)) < 0) {
      printf("Cant generate key\n");
      exit(-1);
    }

    if ((shmid = shmget(key, sizeof(int) * 3 + size,
                        0666 | IPC_CREAT | IPC_EXCL)) < 0) {
      if (errno != EEXIST) {
        printf("Cant create shared memory\n");
        exit(-1);
      }

      if ((shmid = shmget(key, size, 0)) < 0) {
        printf("Cant find shared memory\n");
        exit(-1);
      }

      this->initialized = true;
    }

    if ((this->mem = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)) {
      printf("Cant attach shared memory\n");
      exit(-1);
    }

    mem[0] = 0;
    mem[1] = 0;
    mem[2] = 0;
  }

  bool is_initialized() { return this->initialized; }

  MutexGuard lock() {
    int other = 1 - id;

    mem[this->id] = 1;
    mem[2] = this->id;

    while (this->mem[other] && this->mem[2] == this->id) {
    }

    return MutexGuard(this->id, this->mem);
  }

  ~Mutex() {
    if (shmdt(this->mem) < 0) {
      printf("Cant detach shared memory\n");
      exit(-1);
    }
  }

private:
  bool initialized;
  int *mem;
  int id;
};
