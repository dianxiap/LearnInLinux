#pragma once

#include <iostream>
#include <cstdio>
#include <cassert>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "Log.hpp"

using namespace std;

#define PATH_NAME "/home/wph1.0"
#define PROJ_ID 0x66
#define SHM_SIZE 4096   //共享内存的大小，最好是页表大小（4096）的整数倍