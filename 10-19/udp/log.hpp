#pragma once

#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <string>

#define DEBUG 0
#define NORMAL 1
#define WARNING 2
#define ERROR 3
#define FATAL 4

const char *gLevelMap[] = {
    "DEBUG",
    "NORMAL",
    "WARNING",
    "ERROR",
    "FATAL"
};

#define LOGFILE "./threadpool.log"

void logmessage(int level,const char* format,...)
{
#ifndef DEBUG_SHOW
    if(level== DEBUG) return;
#endif
    char timebuffer[1024];
    time_t timestamp=time(nullptr);
    snprintf(timebuffer,sizeof timebuffer,"[%s] [%ld]",gLevelMap[level],timestamp);

    char logbuffer[1024];
    va_list args;
    va_start(args,format);
    vsnprintf(logbuffer,sizeof logbuffer,format,args);

    // FILE* fp=fopen(LOGFILE,"a");
    printf("%s%s\n",timebuffer,logbuffer);
    // fprintf(fp,"%s%s\n",timebuffer,logbuffer);
    // fclose(fp);

}