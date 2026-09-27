/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Compare every sealed runtime entry, not only listed hashes. An unexpected DLL
 * must not enter the private loader directory unnoticed. No path is executed.
 *---------------------------------------------------------------------------*/
#define _POSIX_C_SOURCE 200809L
#include "internal.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif
static int Expected(const VmRuntime *r,const char *name,int directory){
    if(!directory&&!strcmp(name,"runtime.umi"))return 1;
    size_t n=strlen(name);
    for(size_t i=0;i<r->count;++i){
        const char*p=r->files[i].relative;
        if(!directory&&!strcmp(name,p))return 1;
        if(directory&&strlen(p)>n&&!strncmp(name,p,n)&&p[n]=='/')return 1;
    }
    return 0;
}
static UmiStatus Walk(const char *root,const VmRuntime*r,const char *relative,unsigned depth,size_t *count){
    if(depth>32U||*count>4096U)return UMI_STATUS_CAPACITY_EXCEEDED;
    char path[UMI_SETUP_PATH_CAPACITY];
    UmiStatus status=relative[0]?UmiSetupPathJoin(root,relative,path):UMI_STATUS_OK;
    if(!relative[0]){
        if(strlen(root)>=sizeof path)return UMI_STATUS_CAPACITY_EXCEEDED;
        strcpy(path,root);
    }
    if(status!=UMI_STATUS_OK)return status;
    status=UmiSetupFileCheck(path,1,NULL);
    if(status!=UMI_STATUS_OK)return status;
#ifdef _WIN32
    wchar_t wide[UMI_SETUP_PATH_CAPACITY+4U];
    if(!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,wide,UMI_SETUP_PATH_CAPACITY))return UMI_STATUS_INVALID_ARGUMENT;
    wcscat(wide,L"\\*");
    WIN32_FIND_DATAW entry;
    HANDLE iterator=FindFirstFileW(wide,&entry);
    if(iterator==INVALID_HANDLE_VALUE)return UMI_STATUS_IO_ERROR;
    do{
        if(!wcscmp(entry.cFileName,L".")||!wcscmp(entry.cFileName,L".."))continue;
        char name[768];
        if(!WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,entry.cFileName,-1,name,sizeof name,NULL,NULL)){
            status=UMI_STATUS_PARSE_ERROR;
            break;
        }
        int directory=(entry.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0;
        if(entry.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT){
            status=UMI_STATUS_PERMISSION_DENIED;
            break;
        }
#else
        DIR *iterator=opendir(path);
        if(!iterator)return UMI_STATUS_IO_ERROR;
        struct dirent *entry;
        while((entry=readdir(iterator))!=NULL){
            const char *name=entry->d_name;
            if(!strcmp(name,".")||!strcmp(name,".."))continue;
            char childPath[UMI_SETUP_PATH_CAPACITY];
            struct stat info;
            if(UmiSetupPathJoin(path,name,childPath)!=UMI_STATUS_OK||lstat(childPath,&info)){
                status=UMI_STATUS_IO_ERROR;
                break;
            }
            if(!S_ISDIR(info.st_mode)&&!S_ISREG(info.st_mode)){
                status=UMI_STATUS_PERMISSION_DENIED;
                break;
            }
            if(S_ISREG(info.st_mode)&&info.st_nlink!=1){
                status=UMI_STATUS_PERMISSION_DENIED;
                break;
            }
            int directory=S_ISDIR(info.st_mode);
#endif
            char child[768];
            size_t a=strlen(relative),b=strlen(name);
            if(a+b+2U>sizeof child){
                status=UMI_STATUS_CAPACITY_EXCEEDED;
                break;
            }
            memcpy(child,relative,a);
            if(a)child[a++]='/';
            memcpy(child+a,name,b+1U);
            ++*count;
            if(!Expected(r,child,directory)){
                status=UMI_STATUS_INVALID_STATE;
                break;
            }
            if(directory){
                status=Walk(root,r,child,depth+1U,count);
                if(status!=UMI_STATUS_OK)break;
            }
#ifdef _WIN32
        }
        while(FindNextFileW(iterator,&entry));
        if(status==UMI_STATUS_OK&&GetLastError()!=ERROR_NO_MORE_FILES)status=UMI_STATUS_IO_ERROR;
        FindClose(iterator);
#else
    }
    closedir(iterator);
#endif
    return status;
}
UmiStatus VmRuntimeInventory(const char*root,const VmRuntime*r){
    size_t count=0;
    return Walk(root,r,"",0,&count);
}
