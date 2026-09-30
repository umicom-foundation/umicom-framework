/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/os_image/archive_lesson.c
 * PURPOSE:
 *   Lesson: an archive is a description of files, not a request to install them. This
 *   complete example stays in memory and uses only public Framework APIs.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Lesson: an archive is a description of files, not a request to install them.
 * This complete example stays in memory and uses only public Framework APIs.
 *---------------------------------------------------------------------------*/
#include "umicom/os_image/image.h"
#include <stdio.h>
#include <stdlib.h>
int main(void) {
    static const unsigned char message[]="Umicom Notes practice workspace\n";
    UmiOsImageEntry files[]= {
        {
            "etc",0040755U,0,0,NULL,0
        }
        , {
            "etc/umicom",0040755U,0,0,NULL,0
        }
        , {
            "etc/umicom/welcome.txt",0100444U,0,0,message,sizeof message-1U
        }
    }
    ;
    unsigned char *bytes=NULL;
    size_t size=0;
    UmiOsImageArchive *archive=NULL;
    UmiStatus status=UmiOsImageArchiveBuild(files,3,&bytes,&size);
    if(status==UMI_STATUS_OK)status=UmiOsImageArchiveOpen(bytes,size,&archive);
    if(status==UMI_STATUS_OK) {
        for(size_t i=0;i<UmiOsImageArchiveCount(archive);++i) {
            const UmiOsImageEntry *file=UmiOsImageArchiveAt(archive,i);
            printf("%s: %zu bytes\n",file->name,file->size);
        }
        puts("Practice complete. The archive stayed in memory; no files were extracted and no guest was started.");
    }
    UmiOsImageArchiveDestroy(archive);
    free(bytes);
    return status==UMI_STATUS_OK?0:1;
}
