/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Build a NON-BOOTABLE 2 MiB practice image to teach inspection and file copying.
 * The partition table is synthetic; there is no filesystem or operating system.
 * The few x86 bytes halt rather than loading a guest. Never use this on a USB.
 * No argument: memory only. --output NEW_FILE: exclusive ordinary-file output.
 *---------------------------------------------------------------------------*/

#include "umicom/setup_centre/files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
static void Little32(unsigned char *out, unsigned value)
{
    for (unsigned i=0;i<4U;++i)out[i]=(unsigned char)(value>>(i*8U));
}

static int PracticeImageMain(int argc,char **argv)
{
    if (argc!=1&&(argc!=3||strcmp(argv[1],"--output"))) {
        fputs("Usage: umicom-media-image-example [--output NEW_ABSOLUTE_FILE]\n",stderr);
        return 2;
    }
    const size_t size=2U*1024U*1024U;
    unsigned char *data=calloc(1,size);
    if (!data)return 1;

    /* cli; hlt; jmp to hlt: no disk access or filesystem bootloader. */
    data[0]=0xfa;
    data[1]=0xf4;
    data[2]=0xeb;
    data[3]=0xfd;
    const char notice[]="UMICOM NON-BOOTABLE PRACTICE IMAGE - REGULAR FILE EXERCISE ONLY";
    memcpy(data+32,notice,sizeof notice);
    data[446]=0x80;
    data[450]=0x0c;
    Little32(data+454,1);
    Little32(data+458,(unsigned)(size/512U)-1U);
    data[510]=0x55;
    data[511]=0xaa;
    UmiStatus status=UMI_STATUS_OK;
    if (argc==3) {
#ifdef _WIN32
        if(strlen(argv[2])<3U||argv[2][1]!=':'||(argv[2][2]!='/'&&argv[2][2]!='\\')){
            free(data);fputs("Use an absolute drive-qualified Windows path.\n",stderr);return 2;
        }
#endif
        UmiSetupReport report= {
            0
        };
        status=UmiSetupFileWriteNew(argv[2],data,size,&report);
        if (status!=UMI_STATUS_OK)fprintf(stderr,"New-file creation failed (%d). Existing files are not replaced.\n",
            (int)status);
    }
    free(data);
    if (status!=UMI_STATUS_OK)return 1;
    puts(argc==1?"Practice image created in memory only.":"Created a new 2 MiB regular-file practice image.");
    puts("Not bootable: no filesystem or operating system. Do not write it to a physical device.");
    return 0;
}

#ifdef _WIN32
int wmain(int argc,wchar_t **wide)
{
    if (argc<1||argc>3)return 2;
    char *argv[4]= {
        0
    };
    int result=1;
    for (int i=0;i<argc;++i) {
        int count=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide[i],-1,NULL,0,NULL,NULL);
        if (count<=0)goto cleanup;
        argv[i]=malloc((size_t)count);
        if (!argv[i])goto cleanup;
        if (!WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide[i],-1,argv[i],count,NULL,NULL))goto cleanup;
    }
    result=PracticeImageMain(argc,argv);
    cleanup:
    for (int i=0;i<argc;++i)free(argv[i]);
    return result;
}

#else
int main(int argc,char **argv) {
    return PracticeImageMain(argc,argv);
}

#endif
