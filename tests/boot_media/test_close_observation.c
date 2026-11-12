/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/boot_media/test_close_observation.c
 *
 * PURPOSE:
 *   Verify one-attempt native media release, first-error retention and complete ownership disposal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#define _FILE_OFFSET_BITS 64
#endif
#include "../support/assertions.h"
#include "../../src/boot_media/internal.h"
#include "umicom/setup_centre/files.h"
#include <assert.h>
#ifndef _WIN32
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#endif
static const char *mode;
static size_t closes,frees;
static BmFile *owned;
#ifdef _WIN32
static int Seen[5];
#endif
static void ReleaseFree(void *pointer)
{
    if(pointer==owned)++frees;
    free(pointer);
}
#ifdef _WIN32
static BOOL ReleaseHandle(HANDLE handle)
{
    size_t index=(size_t)(uintptr_t)handle;
    assert(index>=1U&&index<=4U);
    ++closes;
    assert(Seen[index]++==0); /* Releasing the same handle twice is a failure. */
    int denied=(!strcmp(mode,"first-volume")&&index==1U)||
        (!strcmp(mode,"middle-volume")&&index==2U)||
        (!strcmp(mode,"last-volume")&&index==3U)||
        (!strcmp(mode,"multiple-failures")&&index==1U);
    int failed=denied||(!strcmp(mode,"multiple-failures")&&index>=2U)||
        ((!strcmp(mode,"main-failure")||!strcmp(mode,"legacy-failure"))&&index==4U);
    if(failed) {SetLastError(denied?ERROR_ACCESS_DENIED:ERROR_WRITE_FAULT);return FALSE;}
    return TRUE;
}
#define CloseHandle ReleaseHandle
#else
static int ReleaseDescriptor(int descriptor)
{
    assert(descriptor==12);
    ++closes;assert(closes==1U);
    if(!strcmp(mode,"main-failure")||!strcmp(mode,"legacy-failure")){errno=EIO;return -1;}
    if(!strcmp(mode,"interrupted")){errno=EINTR;return -1;}
    return 0;
}
#define close ReleaseDescriptor
#endif
/* Synthetic identifiers never reach an operating-system close. The real release
 * implementation must still consume its wrapper and attempt every owned handle. */
#define free ReleaseFree
#include "../../src/boot_media/io.c"
#undef free
#ifdef _WIN32
#undef CloseHandle
#else
#undef close
#endif
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    mode=argv[1];
    const char *cases[]={"null","success","main-failure","legacy","legacy-failure",
#ifdef _WIN32
        "first-volume","middle-volume","last-volume","multiple-failures"
#else
        "interrupted"
#endif
    };
    int known=0;
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i)if(!strcmp(mode,cases[i]))known=1;
    if(!known)return 2;
    if(!strcmp(mode,"null")) {
        assert(BmCloseResult(NULL)==UMI_STATUS_OK);
        BmClose(NULL);assert(closes==0U&&frees==0U);return 0;
    }
    owned=calloc(1,sizeof *owned);assert(owned!=NULL);
#ifdef _WIN32
    owned->handle=(HANDLE)(uintptr_t)4U;
    owned->volumeCount=3U;
    for(size_t i=0;i<3U;++i)owned->volumes[i]=(HANDLE)(uintptr_t)(i+1U);
    size_t expectedCloses=4U;
#else
    owned->fd=12;
    size_t expectedCloses=1U;
#endif
    int legacy=!strcmp(mode,"legacy")||!strcmp(mode,"legacy-failure");
    if(legacy)BmClose(owned);
    else {
        UmiStatus expected=UMI_STATUS_OK;
        if(!strcmp(mode,"main-failure")||!strcmp(mode,"interrupted"))expected=UMI_STATUS_IO_ERROR;
#ifdef _WIN32
        if(strstr(mode,"volume")||!strcmp(mode,"multiple-failures"))expected=UMI_STATUS_PERMISSION_DENIED;
#endif
        assert(BmCloseResult(owned)==expected);
    }
    /* The pointer is consumed on every result; never retry or dereference it. */
    owned=NULL;
    assert(closes==expectedCloses&&frees==1U);
    return 0;
}
