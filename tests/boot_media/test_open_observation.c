/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/boot_media/test_open_observation.c
 *
 * PURPOSE:
 *   Observe exclusive regular-file creation even when later native admission fails.
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
#include "../../src/boot_media/internal.h"
#include "umicom/setup_centre/files.h"
#include "../build_log/fixture.h"
#ifndef _WIN32
#include <fcntl.h>
#include <sys/file.h>
#endif
static int refuseAllocation, refuseMetadata;
static void *CreationCalloc(size_t count,size_t size)
{
    if (refuseAllocation) return NULL;
    return calloc(count,size);
}
#ifdef _WIN32
static BOOL CreationInformation(HANDLE handle,LPBY_HANDLE_FILE_INFORMATION information)
{
    if (refuseMetadata) { SetLastError(ERROR_READ_FAULT); return FALSE; }
    return GetFileInformationByHandle(handle,information);
}
#define GetFileInformationByHandle CreationInformation
#else
static int CreationStat(int descriptor,struct stat *information)
{
    if (refuseMetadata) { errno=EIO; return -1; }
    return fstat(descriptor,information);
}
#define fstat CreationStat
#endif
/* Native open/create and close remain real. Inject only post-open metadata or
 * allocation failure, never replace a path with a device fixture. */
#define calloc CreationCalloc
#include "../../src/boot_media/io.c"
#undef calloc
#ifdef _WIN32
#undef GetFileInformationByHandle
#else
#undef fstat
#endif

int main(int argc,char **argv)
{
    if (argc!=2) return 2;
    const char *cases[]={ "success", "existing", "no-memory", "metadata-failure", "invalid-path", "missing-parent", "null-output", "null-observation", "unicode", "legacy" };
    int known=0;
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i)
        if(!strcmp(argv[1],cases[i]))known=1;
    if(!known)return 2;
    char root[UMI_PATH_CAPACITY],target[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(target,root,!strcmp(argv[1],"unicode")?"source-\xCE\xA9.img":"source.img");
    printf("Retained acquisition fixture: %s\n",root);
    BmFile *file=NULL;
    int created=7;
    const unsigned char sentinel[]={1U,4U,9U,16U};
    if(!strcmp(argv[1],"existing")){
        CHECK(BmCreate(target,&file)==UMI_STATUS_OK);
        CHECK(BmWrite(file,0,sentinel,sizeof sentinel)==UMI_STATUS_OK);
        CHECK(BmFlush(file)==UMI_STATUS_OK);
        BmClose(file);file=NULL;
    }
    const char *requested=target;
    if(!strcmp(argv[1],"invalid-path"))requested="relative.img";
    if(!strcmp(argv[1],"missing-parent"))FixturePath(target,root,"missing/source.img");
    refuseAllocation=!strcmp(argv[1],"no-memory");
    refuseMetadata=!strcmp(argv[1],"metadata-failure");
    UmiStatus status;
    if(!strcmp(argv[1],"legacy"))status=BmCreate(target,&file);
    else status=BmCreateObserved(requested,!strcmp(argv[1],"null-output")?NULL:&file,
        !strcmp(argv[1],"null-observation")?NULL:&created);
    refuseAllocation=refuseMetadata=0;
    UmiStatus expected=UMI_STATUS_OK;
    if(!strcmp(argv[1],"existing"))expected=UMI_STATUS_ALREADY_EXISTS;
    if(!strcmp(argv[1],"no-memory"))expected=UMI_STATUS_OUT_OF_MEMORY;
    if(!strcmp(argv[1],"metadata-failure")||!strcmp(argv[1],"invalid-path")||
        !strcmp(argv[1],"null-output"))expected=UMI_STATUS_INVALID_ARGUMENT;
    if(!strcmp(argv[1],"missing-parent"))expected=UMI_STATUS_NOT_FOUND;
    CHECK(status==expected);
    int nativeCreated=!strcmp(argv[1],"success")||!strcmp(argv[1],"unicode")||
        !strcmp(argv[1],"no-memory")||!strcmp(argv[1],"metadata-failure")||
        !strcmp(argv[1],"legacy")||!strcmp(argv[1],"null-observation");
    if(strcmp(argv[1],"legacy")&&strcmp(argv[1],"null-observation"))
        CHECK(created==nativeCreated);
    if(status==UMI_STATUS_OK){CHECK(file!=NULL);CHECK(BmCheckRegularSize(file,0)==UMI_STATUS_OK);}
    else CHECK(file==NULL);
    BmClose(file);
    if(nativeCreated||!strcmp(argv[1],"existing")){
        size_t length=0;
        unsigned char *bytes=FixtureRead(target,&length);
        if(!strcmp(argv[1],"existing"))CHECK(length==sizeof sentinel&&!memcmp(bytes,sentinel,length));
        else CHECK(length==0U);
        free(bytes);
    }else CHECK(UmiSetupFileCheck(target,0,NULL)==UMI_STATUS_NOT_FOUND);
    return 0;
}
