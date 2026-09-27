/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_VM_MANAGER_INTERNAL_H
#define UMICOM_VM_MANAGER_INTERNAL_H
#include "umicom/vm_manager/manager.h"
#include "umicom/setup_centre/files.h"
#include "umicom/os_image/image.h"
#include "umicom/native_launcher/sha256.h"
#include "umicom/platform/process.h"
#include "json_internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#define VM_META_LIMIT (1024U*1024U)
#define VM_PROFILE_BYTES 16384U
#define VM_RUNTIME_HEADER "UMICOM_QEMU_RUNTIME\t1\n"
typedef struct VmRuntimeFile {
    char relative[768],hash[65];
    uint64_t size;
}
VmRuntimeFile;
typedef struct VmRuntime {
    char host[32],emulator[768],imageTool[768],firmware[768],licence[768],sourceNotice[768],version[96];
    UmiVmArchitecture architecture;
    VmRuntimeFile *files;
    size_t count;
    char identity[65];
}
VmRuntime;
typedef struct VmText {
    char *data;
    size_t used,capacity;
    UmiStatus status;
}
VmText;
void VmTextInit(VmText *text);
void VmTextPrint(VmText *text,const char *format,...);
void VmTextFree(VmText *text);
UmiStatus VmReport(UmiVmReport *report,UmiStatus status,const char *message);
int VmId(const char *id);
int VmHash(const char *hash);
int VmNumber(const char *text,uint64_t *number);
int VmPath(const char *path,int optional);
UmiStatus VmRuntimeRead(const char *root,VmRuntime *runtime,int verify);
void VmRuntimeFree(VmRuntime *runtime);
UmiStatus VmRuntimeInventory(const char *root,const VmRuntime *runtime);
const char *VmHost(void);
UmiStatus VmReadJoined(const char *root,const char *relative,size_t limit,char **data,size_t *size);
UmiStatus VmWriteJoined(const char *root,const char *relative,const void *data,size_t size);
UmiStatus VmDiskLease(const char *directory,void **outLease);
void VmDiskUnlease(void *lease);
UmiStatus VmDiskValidate(const char *directory,uint64_t *virtualBytes);
UmiStatus VmDiskRun(const char *runtime,const char *directory,const char *const *arguments,size_t count,UmiVmReport *report);
UmiStatus VmProfileEncode(const UmiVmProfile *profile,VmText *text);
UmiStatus VmProfileDecode(const char *text,UmiVmProfile *profile);
uint64_t VmClock(void);
#endif
