/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Private transport contract. Caller-visible plans never contain raw handles. */
#ifndef UMICOM_BOOT_MEDIA_INTERNAL_H
#define UMICOM_BOOT_MEDIA_INTERNAL_H
#include "umicom/boot_media/media.h"
#include "umicom/native_launcher/sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#define BM_CHUNK (1024U*1024U)
#define BM_TAIL (1024U*1024U)
#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
typedef struct BmFile { HANDLE handle; HANDLE volumes[64]; size_t volumeCount; uint64_t bytes; int writable; } BmFile;
#else
typedef struct BmFile { int fd; uint64_t bytes; int writable; } BmFile;
#endif
struct UmiBootMediaPlan {
    char source[UMI_BOOT_MEDIA_PATH],destination[UMI_BOOT_MEDIA_PATH];
    char fingerprint[65],confirmation[96];
    UmiBootMediaImage image;
    UmiBootMediaDevice device;
    int physical,consumed;
};
UmiStatus BmMessage(UmiBootMediaReport *r,UmiStatus s,const char *fmt,...);
UmiStatus BmTick(UmiBootMediaReport *r,UmiBootMediaPhase phase,uint64_t done,uint64_t total,
    UmiBootMediaProgress cb,void *context);
UmiStatus BmSourceOpen(const char *path,BmFile **out,uint64_t *bytes);
UmiStatus BmCreate(const char *path,BmFile **out);
UmiStatus BmRead(BmFile *f,uint64_t offset,void *data,size_t bytes);
UmiStatus BmWrite(BmFile *f,uint64_t offset,const void *data,size_t bytes);
UmiStatus BmFlush(BmFile *f);
void BmClose(BmFile *f);
UmiStatus BmHash(BmFile *f,uint64_t bytes,char hex[65],UmiBootMediaProgress cb,void *ctx,UmiBootMediaReport *r);
UmiStatus BmInspect(BmFile *f,uint64_t bytes,UmiBootMediaImage *out);
UmiStatus BmDeviceOpen(const UmiBootMediaDevice *expected,int write,BmFile **out);
UmiStatus BmDeviceCheck(const char *path,UmiBootMediaDevice *out);
UmiStatus BmPath(const char *path);
int BmSameDevice(const UmiBootMediaDevice *a,const UmiBootMediaDevice *b);
int BmHashValid(const char *hash);
uint32_t BmCrc32(const void *data,size_t bytes);
#endif
