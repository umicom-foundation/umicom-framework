/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
 * Assertions stay active in Release; a failed check always returns a failure. */
#ifndef UMICOM_NATIVE_LAUNCHER_TEST_SUPPORT_H
#define UMICOM_NATIVE_LAUNCHER_TEST_SUPPORT_H
#include "umicom/native_launcher/stage.h"
#include "umicom/native_launcher/sha256.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition) do {if(!(condition)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#condition);return 1;}}while(0)
extern const char *TestBinary;
static inline UmiNativeStageManifest TestManifest(void)
{
    UmiNativeStageManifest m={0};
    strcpy(m.root,"/home/user/review");strcpy(m.desk.path,"/home/user/bin/desk");strcpy(m.launcher.path,"/home/user/bin/session");
    memset(m.desk.sha256,'a',64U);memset(m.launcher.sha256,'b',64U);memset(m.iconHash,'c',64U);memset(m.entryHash,'d',64U);
    m.desk.bytes=4096U;m.launcher.bytes=8192U;m.desk.machine=62U;m.launcher.machine=62U;return m;
}
static inline int TestReplace(char *text,size_t capacity,const char *from,const char *to)
{
    char *at=strstr(text,from);if(at==NULL)return 0;
    size_t oldLength=strlen(text),fromLength=strlen(from),toLength=strlen(to);
    if(toLength>=capacity||oldLength-fromLength>=capacity-toLength)return 0;
    memmove(at+toLength,at+fromLength,strlen(at+fromLength)+1U);memcpy(at,to,toLength);return 1;
}
#endif
