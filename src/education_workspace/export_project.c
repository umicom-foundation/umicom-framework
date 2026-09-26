/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Framework-owned learning output; applications do not duplicate this service. */
#include "umicom/education_workspace/projects.h"
#include "private.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <direct.h>
/* The public path contract is UTF-8 on Windows, not the current ANSI page. */
static int Wide(const char *source,wchar_t *target,size_t capacity)
{
    if (capacity>(size_t)INT_MAX) return 0;
    return MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,source,-1,target,(int)capacity);
}
#else
#include <sys/stat.h>
#endif
UmiStatus UmiEducationExportProject(const char *id,const char *destination,size_t *outWritten)
{
    if (outWritten==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outWritten=0U;
    size_t count=UmiEducationProjectFileCount(id);
    if (count==0U) return UMI_STATUS_NOT_FOUND;
    if (!EwTextValid(destination,1024U,false)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t n=strlen(destination);
    if (destination[n-1U]=='/' || destination[n-1U]=='\\') return UMI_STATUS_INVALID_ARGUMENT;
    /* No record or command path is accepted as a destination. The native view
     * makes this a separate, explicit action in a caller-controlled folder. */
#ifdef _WIN32
    if (n<3U || !((destination[0]>='A'&&destination[0]<='Z') ||
        (destination[0]>='a'&&destination[0]<='z')) || destination[1]!=':' ||
        (destination[2]!='/'&&destination[2]!='\\')) return UMI_STATUS_INVALID_ARGUMENT;
    wchar_t wide[1200];
    if (!Wide(destination,wide,sizeof wide/sizeof wide[0])) return UMI_STATUS_INVALID_ARGUMENT;
    if (_wmkdir(wide)!=0) return errno==EEXIST?UMI_STATUS_ALREADY_EXISTS:UMI_STATUS_IO_ERROR;
#else
    if (destination[0]!='/') return UMI_STATUS_INVALID_ARGUMENT;
    if (mkdir(destination,0700)!=0) return errno==EEXIST?UMI_STATUS_ALREADY_EXISTS:UMI_STATUS_IO_ERROR;
#endif
    for (size_t i=0U;i<count;++i) {
        UmiEducationProjectFile file;char path[1200];
        UmiStatus status=UmiEducationProjectFileAt(id,i,&file);
        if (status!=UMI_STATUS_OK) return status;
        /* Resource names are built-in flat basenames, never caller paths. */
        if (strchr(file.name,'/')!=NULL || strchr(file.name,'\\')!=NULL ||
            strcmp(file.name,".")==0 || strcmp(file.name,"..")==0) return UMI_STATUS_INVALID_STATE;
        int printed=snprintf(path,sizeof path,"%s/%s",destination,file.name);
        if (printed<0 || (size_t)printed>=sizeof path) return UMI_STATUS_CAPACITY_EXCEEDED;
#ifdef _WIN32
        if (!Wide(path,wide,sizeof wide/sizeof wide[0])) return UMI_STATUS_INVALID_ARGUMENT;
        FILE *output=_wfopen(wide,L"wbx");
#else
        FILE *output=fopen(path,"wbx");
#endif
        if (output==NULL) return errno==EEXIST?UMI_STATUS_ALREADY_EXISTS:UMI_STATUS_IO_ERROR;
        bool ok=fwrite(file.bytes,1U,file.size,output)==file.size;
        if (fclose(output)!=0) ok=false;
        if (!ok) return UMI_STATUS_IO_ERROR;
        ++*outWritten;
    }
    return UMI_STATUS_OK;
}
