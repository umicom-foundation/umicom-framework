/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/release_inspector/inspection.c
 * Read-only package inspection. The Setup Centre catalogue and checked I/O are
 * reused, not reimplemented by an application or an interpreter. System DLL
 * classification records a deferred host requirement; it is never a loader test.
 *---------------------------------------------------------------------------*/
#include "umicom/release_inspector/inspection.h"
#include "../setup_centre/internal.h"
#include <inttypes.h>

typedef struct InspectionContext {
    const UmiSetupBundle *bundle;
    uint64_t selected;
    UmiReleaseInspection *result;
    UmiReleaseObserver observer;
    void *context;
    int stopped;
} InspectionContext;
static int Emit(InspectionContext *c, UmiReleaseObservationKind kind, UmiStatus status,
    const char *path,const char *dependency,const char *detail,int delayed)
{
    UmiReleaseInspection *r=c->result;
    if(c->stopped)return 0;
    if(r->observations==UMI_RELEASE_MAX_OBSERVATIONS) {
        r->status=UMI_STATUS_CAPACITY_EXCEEDED; c->stopped=1; return 0;
    }
    ++r->observations;
    if(kind==UMI_RELEASE_ISSUE) {
        ++r->issues;
        if(!r->firstIssue[0])
            (void)snprintf(r->firstIssue,sizeof r->firstIssue,"%.130s: %.170s",path,detail);
    }
    UmiReleaseObservation item={kind,status,path,dependency,detail,delayed};
    if(c->observer && c->observer(&item,c->context)) {
        r->status=UMI_STATUS_CANCELLED; c->stopped=1; return 0;
    }
    return 1;
}
static const ScFile *FindFile(const UmiSetupBundle *b,const char *path)
{
    size_t lo=0U,hi=b->fileCount;
    while(lo<hi) {
        size_t middle=lo+(hi-lo)/2U;
        int compared=ScEqualFold(b->files[middle].relative,path);
        if(compared<0)lo=middle+1U; else hi=middle;
    }
    return lo<b->fileCount && !ScEqualFold(b->files[lo].relative,path)?&b->files[lo]:NULL;
}
static int Extension(const char *path,const char *extension)
{
    size_t n=strlen(path),m=strlen(extension);
    return n>=m && ScEqualFold(path+n-m,extension)==0;
}
/* Match the DLL spelling family already accepted by Framework deployment:
 * optional lib prefix and optional numeric ABI suffix, never arbitrary names. */
static int LibraryFamily(const char *path,const char *stem)
{
    if(strncmp(path,"bin/",4U))return 0;
    char folded[UMI_SETUP_RELATIVE_CAPACITY];
    size_t n=strlen(path+4U);if(n>=sizeof folded)return 0;
    for(size_t i=0U;i<=n;++i) {
        unsigned char ch=(unsigned char)path[4U+i];
        folded[i]=(char)(ch>='A'&&ch<='Z'?ch+('a'-'A'):ch);
    }
    const char *name=folded;
    if(!strncmp(name,"lib",3U))name+=3U;
    size_t length=strlen(stem);
    if(strncmp(name,stem,length))return 0;
    name+=length;
    if(*name=='-') {
        ++name;
        if(*name<'0'||*name>'9')return 0;
        while(*name>='0'&&*name<='9')++name;
    }
    return !ScEqualFold(name,".dll");
}

static int SystemName(const char *name)
{
    /* This deliberately visible list is not a copy of Windows' KnownDLLs.
     * Unsupported system components are treated as unresolved until the list
     * is reviewed. API-set hosts cannot be determined from package bytes. */
    static const char *const names[]={
        "kernel32.dll","kernelbase.dll","ntdll.dll","advapi32.dll","bcrypt.dll",
        "bcryptprimitives.dll","cabinet.dll","cfgmgr32.dll","combase.dll",
        "comctl32.dll","comdlg32.dll","credui.dll","crypt32.dll","cryptbase.dll",
        "cryptnet.dll","cryptui.dll","d2d1.dll","d3d11.dll","d3d12.dll",
        "d3dcompiler_47.dll","dbghelp.dll","dcomp.dll","dnsapi.dll","dsound.dll",
        "dwmapi.dll","dwrite.dll","dxgi.dll","gdi32.dll","gdiplus.dll",
        "hid.dll","imagehlp.dll","imm32.dll","iphlpapi.dll","mpr.dll",
        "msimg32.dll","msvcrt.dll","netapi32.dll","normaliz.dll","ncrypt.dll",
        "ole32.dll","oleacc.dll","oleaut32.dll","opengl32.dll","powrprof.dll",
        "propsys.dll","psapi.dll","rpcrt4.dll","secur32.dll","setupapi.dll",
        "shell32.dll","shcore.dll","shlwapi.dll","user32.dll","userenv.dll",
        "ucrtbase.dll","usp10.dll","uxtheme.dll","version.dll","windowscodecs.dll",
        "winhttp.dll","wininet.dll","winmm.dll","winspool.drv","wintrust.dll",
        "wldap32.dll","ws2_32.dll","wtsapi32.dll"
    };
    if(strncmp(name,"api-ms-win-",11U)==0 || strncmp(name,"ext-ms-win-",11U)==0)return 1;
    for(size_t i=0U;i<sizeof names/sizeof names[0];++i)
        if(strcmp(name,names[i])==0)return 1;
    return 0;
}
static int Imported(InspectionContext *c,const char *path,const UmiReleasePeInfo *info)
{
    for(size_t i=0U;i<info->importCount && !c->stopped;++i) {
        char location[UMI_SETUP_RELATIVE_CAPACITY];
        const char *name=info->imports[i];
        (void)snprintf(location,sizeof location,"bin/%s",name);
        /* A bundled QEMU is a separate loader namespace. Never satisfy its
         * private import from Umicom's unrelated main bin inventory. This
         * profile requires runtime programs and DLLs in its private bin. */
        char providerPrefix[19]={0};
        if(strlen(path)>=18U)memcpy(providerPrefix,path,18U);
        if(!ScEqualFold(providerPrefix,"share/umicom/qemu/"))
            (void)snprintf(location,sizeof location,"share/umicom/qemu/bin/%s",name);

        const ScFile *file=FindFile(c->bundle,location);
        int selected=file && ScSelected(file,c->selected);
        if(info->delayed[i])++c->result->delayedImports;
        if(SystemName(name)) {
            if(selected) {
                Emit(c,UMI_RELEASE_ISSUE,UMI_STATUS_INVALID_STATE,path,name,
                    "A Windows-system-classified name is supplied privately; review this collision.",info->delayed[i]);
            } else {
                ++c->result->systemImports;
                Emit(c,UMI_RELEASE_SYSTEM_IMPORT,UMI_STATUS_UNAVAILABLE,path,name,
                    "Deferred Windows host/API-set requirement; not checked against this host.",info->delayed[i]);
            }
        } else if(!ScEqualFold(path,"bin/umicom-setup-centre.exe")) {
            Emit(c,UMI_RELEASE_ISSUE,UMI_STATUS_INVALID_STATE,path,name,
                "The standalone installer bootstrap cannot depend on a private DLL beside the payload.",info->delayed[i]);
        } else if(selected) {
            ++c->result->privateImports;
            Emit(c,UMI_RELEASE_PRIVATE_IMPORT,UMI_STATUS_OK,path,file->relative,
                "Declared in the selected private bin inventory; content and PE checks are separate observations.",info->delayed[i]);
        } else {
            Emit(c,UMI_RELEASE_ISSUE,UMI_STATUS_NOT_FOUND,path,name,
                file?"The required private DLL belongs to an unselected application.":
                "The required DLL is absent from private bin inventory and not classified as a Windows system dependency.",info->delayed[i]);
        }
    }
    return !c->stopped;
}
static int CatalogueAgrees(InspectionContext *c)
{
    char path[UMI_SETUP_PATH_CAPACITY],hash[65];
    UmiSetupReport report={0}; uint64_t size=0U;
    UmiStatus status=ScJoin(c->bundle->root,c->bundle->installed?UMI_SETUP_RECEIPT:UMI_SETUP_CATALOGUE,path);
    if(status==UMI_STATUS_OK)status=ScDigest(path,hash,&size,NULL,NULL,&report);
    if(status==UMI_STATUS_OK && strcmp(hash,c->bundle->catalogueHash))status=UMI_STATUS_INVALID_STATE;
    if(status!=UMI_STATUS_OK) {
        Emit(c,UMI_RELEASE_ISSUE,status,"catalogue","","Catalogue/receipt is changed, missing or unreadable.",0);
        if(!c->stopped)c->result->status=status;
        return 0;
    }
    return 1;
}
/* The release's clickable bootstrap is copied outside payload/bin. Verify
 * that copy too. It may use Windows components, but cannot rely on payload
 * DLLs which are not beside it until installation has happened. */
static void BootstrapCopy(InspectionContext *c)
{
    if(c->bundle->installed || c->stopped)return;
    const ScFile *file=FindFile(c->bundle,"bin/umicom-setup-centre.exe");
    if(!file || !ScSelected(file,c->selected))return;
    char path[UMI_SETUP_PATH_CAPACITY],hash[65];uint64_t bytes=0U;
    UmiSetupReport report={0};
    UmiStatus status=ScJoin(c->bundle->root,"Umicom-Setup.exe",path);
    if(status==UMI_STATUS_OK)status=ScDigest(path,hash,&bytes,NULL,NULL,&report);
    if(status==UMI_STATUS_OK && (strcmp(hash,file->hash) || bytes!=file->bytes))status=UMI_STATUS_INVALID_STATE;
    if(status!=UMI_STATUS_OK)Emit(c,UMI_RELEASE_ISSUE,status,"Umicom-Setup.exe","",
        "Standalone bootstrap is missing, changed or differs from its recorded payload copy.",0);
    else {
        ++c->result->filesChecked;c->result->bytesChecked+=bytes;
        Emit(c,UMI_RELEASE_FILE_CHECKED,UMI_STATUS_OK,"Umicom-Setup.exe","",
            "Standalone bootstrap agrees with the inventoried installer; dependencies were checked separately.",0);
    }
}

static void Resource(InspectionContext *c,const char *name)
{
    const ScFile *f=FindFile(c->bundle,name);
    if(!f || !ScSelected(f,c->selected) || !f->bytes) {
        Emit(c,UMI_RELEASE_ISSUE,UMI_STATUS_NOT_FOUND,name,"",
            "Required resource is absent, empty or outside the selection.",0);
    } else {
        ++c->result->resourcesChecked;
        Emit(c,UMI_RELEASE_RESOURCE_CHECKED,UMI_STATUS_OK,f->relative,"",
            "Required resource is in the inventory; its bytes are checked with all selected files.",0);
    }
}
static int CancelDigest(const UmiSetupReport *r,void *opaque)
{
    (void)r;
    InspectionContext *c=opaque;
    /* Quiet heartbeat lets a GUI request Stop during a large file digest.
     * The caller's observer is not re-entered; a heartbeat has empty path. */
    UmiReleaseObservation item={UMI_RELEASE_FILE_CHECKED,UMI_STATUS_OK,"","","",0};
    if(c->observer && c->observer(&item,c->context)) {
        c->stopped=1; c->result->status=UMI_STATUS_CANCELLED; return 1;
    }
    return 0;
}
static UmiStatus Inspect(InspectionContext *c)
{
    UmiReleaseInspection *r=c->result;
    if(!CatalogueAgrees(c))return r->status;
    UmiReleasePeInfo *info=malloc(sizeof *info);
    if(!info)return UMI_STATUS_OUT_OF_MEMORY;
    int gtk=0,sourceView=0;
    for(size_t i=0U;i<c->bundle->appCount && !c->stopped;++i)if((c->selected>>i)&UINT64_C(1)) {
        const char *entry=c->bundle->apps[i].entry;
        if(strncmp(entry,"bin/",4U) || strchr(entry+4U,'/') || !Extension(entry,".exe"))
            Emit(c,UMI_RELEASE_ISSUE,UMI_STATUS_INVALID_STATE,entry,"",
                "This Windows release profile requires bin/<application>.exe entries.",0);
    }
    for(size_t i=0U;i<c->bundle->fileCount && !c->stopped;++i) {
        const ScFile *f=&c->bundle->files[i];
        if(!ScSelected(f,c->selected))continue;
        if(CancelDigest(NULL,c))break;
        char path[UMI_SETUP_PATH_CAPACITY],root[UMI_SETUP_PATH_CAPACITY],hash[65];
        UmiSetupReport io={0}; uint64_t size=0U;
        UmiStatus status=UMI_STATUS_OK;
        if(c->bundle->installed)strcpy(root,c->bundle->root);
        else status=ScJoin(c->bundle->root,"payload",root);
        if(status==UMI_STATUS_OK)status=ScJoin(root,f->relative,path);
        int image=Extension(f->relative,".exe") || Extension(f->relative,".dll") || Extension(f->relative,".drv");
        char *data=NULL; size_t length=0U;
        if(status==UMI_STATUS_OK && image) {
            status=ScRead(path,UMI_RELEASE_MAX_PE_BYTES,&data,&length,&io);
            size=length;
            if(status==UMI_STATUS_OK)status=UmiNativeSha256Buffer(data,length,hash);
        } else if(status==UMI_STATUS_OK) {
            status=ScDigest(path,hash,&size,CancelDigest,c,&io);
        }
        if(status==UMI_STATUS_OK && (size!=f->bytes || strcmp(hash,f->hash)))status=UMI_STATUS_INVALID_STATE;
        if(status!=UMI_STATUS_OK) {
            free(data);
            if(!c->stopped)Emit(c,UMI_RELEASE_ISSUE,status,f->relative,"",
                "File is missing, changed, unreadable or beyond the bounded PE inspection size.",0);
            continue;
        }
        ++r->filesChecked; r->bytesChecked+=size;
        if(!Emit(c,UMI_RELEASE_FILE_CHECKED,UMI_STATUS_OK,f->relative,"","Bytes agree with the recorded SHA-256 and size.",0)) {
            free(data); break;
        }
        if(image) {
            status=UmiReleasePeInspect(data,length,info);
            if(status==UMI_STATUS_OK && (info->machine!=UMI_RELEASE_MACHINE_AMD64 || info->optionalMagic!=0x20bU ||
                (info->subsystem!=2U && info->subsystem!=3U)))status=UMI_STATUS_UNAVAILABLE;
            if(status==UMI_STATUS_OK && (!!(info->characteristics&0x2000U) != !Extension(f->relative,".exe")))
                status=UMI_STATUS_INVALID_STATE;
            if(status==UMI_STATUS_OK) {
                ++r->imagesChecked;
                Imported(c,f->relative,info);
            } else Emit(c,UMI_RELEASE_ISSUE,status,f->relative,"",
                "PE structures, AMD64/PE32+ profile, subsystem or EXE/DLL identity did not pass inspection.",0);
            if(LibraryFamily(f->relative,"gtk-4"))gtk=1;
            if(LibraryFamily(f->relative,"gtksourceview-5"))sourceView=1;
        }
        free(data);
    }
    BootstrapCopy(c);
    if(gtk && !c->stopped) {
        Resource(c,"share/umicom/runtime/deployment.marker");
        Resource(c,"share/glib-2.0/schemas/gschemas.compiled");
        Resource(c,"bin/branding/umicom-icon.svg");
        Resource(c,"bin/branding/umicom-logo-on-dark.svg");
    }
    if(sourceView && !c->stopped)Resource(c,"share/gtksourceview-5/language-specs/language2.rng");
    free(info);
    if(c->stopped)return r->status;
    if(!CatalogueAgrees(c))return r->status;
    r->complete=1;
    return r->issues?UMI_STATUS_INVALID_STATE:UMI_STATUS_OK;
}
UmiStatus UmiReleaseInspectBundle(const UmiSetupBundle *b,uint64_t selected,
    UmiReleaseObserver observer,void *context,UmiReleaseInspection *out)
{
    if(!out)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof *out); out->status=UMI_STATUS_INVALID_ARGUMENT;
    if(!b || !selected || (selected & ~UmiSetupAllApplications(b)))return out->status;
    InspectionContext c={b,selected,out,observer,context,0};
    out->status=Inspect(&c);
    return out->status;
}
UmiStatus UmiReleaseInspectInstallation(const char *root,UmiReleaseObserver observer,
    void *context,UmiReleaseInspection *out)
{
    if(!out)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);
    UmiSetupBundle *b=NULL; UmiSetupReport report={0};
    UmiStatus status=UmiSetupInstalledBundleOpen(root,&b,&report);
    if(status==UMI_STATUS_OK)status=UmiReleaseInspectBundle(b,UmiSetupAllApplications(b),observer,context,out);
    else { out->status=status; (void)snprintf(out->firstIssue,sizeof out->firstIssue,"%.319s",report.detail); }
    UmiSetupBundleDestroy(b);
    return status;
}
UmiStatus UmiReleaseInspectionSummary(const UmiReleaseInspection *r,char *out,size_t capacity)
{
    if(!r || !out || !capacity)return UMI_STATUS_INVALID_ARGUMENT;
    int n=snprintf(out,capacity,"Static inspection %s: %zu files, %zu PE images, %zu private and %zu deferred system imports, %zu issues. No application was launched.",
        r->complete?(r->issues?"found issues":"completed"):"did not complete",
        r->filesChecked,r->imagesChecked,r->privateImports,r->systemImports,r->issues);
    if(n<0 || (size_t)n>=capacity) { out[0]=0; return UMI_STATUS_CAPACITY_EXCEEDED; }
    return UMI_STATUS_OK;
}
