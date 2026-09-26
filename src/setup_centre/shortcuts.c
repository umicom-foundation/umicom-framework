/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "internal.h"
#ifndef _WIN32
UmiStatus UmiSetupCreateShortcuts(const char *root,const char *group,int desktop,int menu,UmiSetupReport *r)
{
    (void)root;
    (void)group;
    (void)desktop;
    (void)menu;
    ScReport(r,UMI_STATUS_UNAVAILABLE,"Current-user Windows shortcuts are not available on this host.");
    return UMI_STATUS_UNAVAILABLE;
}

#endif
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define COBJMACROS
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <windows.h>
#include <limits.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <objbase.h>
static int ToWide(const char *s,wchar_t *out,size_t capacity)
{
    if(capacity>(size_t)INT_MAX)return 0;
    int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,s,-1,out,(int)capacity);
    if(n<=0)return 0;
    for(int i=0; i<n; ++i)if(out[i]==L'/')out[i]=L'\\';
    return 1;
}

static UmiStatus Shortcut(const char *target,const char *working,const char *title,const char *link,UmiSetupReport *r)
{

    wchar_t wt[UMI_SETUP_PATH_CAPACITY],ww[UMI_SETUP_PATH_CAPACITY],wd[256];

    if(!ToWide(target,wt,UMI_SETUP_PATH_CAPACITY)||!ToWide(working,ww,UMI_SETUP_PATH_CAPACITY)||!ToWide(title,wd,256))return UMI_STATUS_INVALID_ARGUMENT;

    /* Serialise the Shell Link into memory, then use Framework's checked,
      * exclusive file writer. IPersistFile::Save would overwrite its path and
      * leave a race between checking a filename and writing the shortcut. */
    IShellLinkW *shell = NULL;
    IPersistStream *persist = NULL;
    IStream *stream = NULL;
    HGLOBAL memory = NULL;
    HRESULT hr = CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
        &IID_IShellLinkW, (void **)&shell);
    if (SUCCEEDED(hr)) hr = IShellLinkW_SetPath(shell, wt);
    if (SUCCEEDED(hr)) hr = IShellLinkW_SetWorkingDirectory(shell, ww);
    if (SUCCEEDED(hr)) hr = IShellLinkW_SetDescription(shell, wd);
    if (SUCCEEDED(hr)) hr = IShellLinkW_SetIconLocation(shell, wt, 0);
    if (SUCCEEDED(hr)) hr = IShellLinkW_QueryInterface(shell,
        &IID_IPersistStream, (void **)&persist);
    if (SUCCEEDED(hr)) hr = CreateStreamOnHGlobal(NULL, TRUE, &stream);
    if (SUCCEEDED(hr)) hr = IPersistStream_Save(persist, stream, TRUE);
    STATSTG statistics = {0};
    if (SUCCEEDED(hr)) hr = IStream_Stat(stream, &statistics, STATFLAG_NONAME);
    if (SUCCEEDED(hr)) hr = GetHGlobalFromStream(stream, &memory);
    UmiStatus status = UMI_STATUS_IO_ERROR;
    if (SUCCEEDED(hr) && statistics.cbSize.HighPart == 0U &&
        statistics.cbSize.LowPart > 0U && statistics.cbSize.LowPart <= 1048576U) {
        const void *bytes = GlobalLock(memory);
        if (bytes != NULL) {
            status = ScWriteNew(link, bytes, (size_t)statistics.cbSize.LowPart, r);
            (void)GlobalUnlock(memory);
        }
    }
    if (stream != NULL) IStream_Release(stream);
    if (persist != NULL) IPersistStream_Release(persist);
    if (shell != NULL) IShellLinkW_Release(shell);
    if (FAILED(hr) && r != NULL) r->systemError = (int)hr;
    return status;

}

static UmiStatus Group(const KNOWNFOLDERID *folder,const char *root,const char *name,const UmiSetupBundle *b,UmiSetupReport *r)
{

    PWSTR wide=NULL;
    HRESULT hr=SHGetKnownFolderPath(folder,0,NULL,&wide);
    if(FAILED(hr))return UMI_STATUS_UNAVAILABLE;

    char base[UMI_SETUP_PATH_CAPACITY],group[UMI_SETUP_PATH_CAPACITY];
    int converted=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide,-1,base,sizeof base,NULL,NULL);
    CoTaskMemFree(wide);
    if(!converted)return UMI_STATUS_INVALID_ARGUMENT;

    UmiStatus s=ScJoin(base,name,group);
    if(s==UMI_STATUS_OK)s=ScMakeDirectory(group,r);
    if(s!=UMI_STATUS_OK)return s;

    if(r)r->outputCreated=1;

    for(size_t i=0; i<b->appCount&&s==UMI_STATUS_OK; ++i) {
        char target[UMI_SETUP_PATH_CAPACITY],link[UMI_SETUP_PATH_CAPACITY],relative[100],working[UMI_SETUP_PATH_CAPACITY];

        s=ScJoin(root,b->apps[i].entry,target);
        if(s==UMI_STATUS_OK)s=ScJoin(root,"bin",working);
        (void)snprintf(relative,sizeof relative,"%s.lnk",b->apps[i].id);
        if(s==UMI_STATUS_OK)s=ScJoin(group,relative,link);

        if(s==UMI_STATUS_OK)s=Shortcut(target,working,b->apps[i].title,link,r);
        if(s==UMI_STATUS_OK&&r)++r->filesCompleted;
    }

    return s;

}

UmiStatus UmiSetupCreateShortcuts(const char *root,const char *name,int desktop,int menu,UmiSetupReport *r)
{

    UmiSetupReport check= {
        0
    };
    if(r)memset(r,0,sizeof *r);

    if(!name||UmiSetupValidateRelative(name)!=UMI_STATUS_OK||strchr(name,'/')||(desktop!=0&&desktop!=1)||(menu!=0&&menu!=1)||(!desktop&&!menu))return UMI_STATUS_INVALID_ARGUMENT;

    UmiStatus s=UmiSetupVerifyInstallation(root,&check);
    if(s!=UMI_STATUS_OK) {
        if(r)*r=check;
        return s;
    }

    char path[UMI_SETUP_PATH_CAPACITY],*data=NULL;
    size_t length=0;
    UmiSetupBundle *b=calloc(1,sizeof *b);
    if(!b)return UMI_STATUS_OUT_OF_MEMORY;

    s=ScJoin(root,UMI_SETUP_RECEIPT,path);
    if(s==UMI_STATUS_OK)s=ScRead(path,UMI_SETUP_MANIFEST_LIMIT,&data,&length,r);
    if(s==UMI_STATUS_OK)s=ScDecode(data,length,b,1);
    free(data);

    HRESULT hr=E_FAIL;
    if(s==UMI_STATUS_OK) {
        hr=CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);
        if(FAILED(hr))s=UMI_STATUS_UNAVAILABLE;
    }

    if(s==UMI_STATUS_OK&&desktop)s=Group(&FOLDERID_Desktop,root,name,b,r);

    if(s==UMI_STATUS_OK&&menu)s=Group(&FOLDERID_Programs,root,name,b,r);

    if(SUCCEEDED(hr))CoUninitialize();
    UmiSetupBundleDestroy(b);

    if(r)r->completed=s==UMI_STATUS_OK;
    ScReport(r,s,s==UMI_STATUS_OK?"Current-user shortcut groups created. Existing groups were not replaced.":"Shortcut creation was not completed. Installed application files remain intact; inspect any newly created shortcut group.");
    return s;

}

#endif
