/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A saved configuration is not a running machine. This complete lesson stores
 * a profile in the real memory Data Server and detects an outdated edit.
 *---------------------------------------------------------------------------*/
#include "umicom/vm_manager/manager.h"
#include <stdio.h>
#include <string.h>
int main(void) {
    UmiDataServer *server=NULL;
    UmiVmProfile profile,loaded;
    uint64_t revision=0;
    if(umi_data_server_create_memory(&server)!=UMI_STATUS_OK)return 1;
    UmiVmProfileInit(&profile);
    strcpy(profile.id,"notes-lab");
    strcpy(profile.name,"Umicom Notes practice machine");
#ifdef _WIN32
    strcpy(profile.runtimeDirectory,"C:/Umicom-Practice/runtime");
    strcpy(profile.imageBundle,"C:/Umicom-Practice/image");
#else
    strcpy(profile.runtimeDirectory,"/home/practice/runtime");
    strcpy(profile.imageBundle,"/home/practice/image");
#endif
    int result=1;
    if(UmiVmProfileSave(server,&profile,0,&revision)!=UMI_STATUS_OK||revision!=1)goto done;
    if(UmiVmProfileLoad(server,profile.id,&loaded)!=UMI_STATUS_OK)goto done;
    printf("Saved: %s\nMemory: %u MiB; processors: %u\n",loaded.name,loaded.memoryMiB,loaded.processors);
    loaded.memoryMiB=1024;
    if(UmiVmProfileSave(server,&loaded,loaded.revision,&revision)!=UMI_STATUS_OK)goto done;
    if(UmiVmProfileSave(server,&profile,1,&revision)!=UMI_STATUS_INVALID_STATE)goto done;
    puts("The older edit was refused. Reload before changing the profile again.");
    puts("Practice complete. Memory only; no runtime, guest image or virtual disk was opened.");
    result=0;
    done:umi_data_server_destroy(server);
    return result;
}
