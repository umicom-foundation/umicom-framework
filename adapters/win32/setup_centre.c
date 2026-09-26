/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/win32/setup_centre.c
 * Purpose: A native C23 Setup Centre for Windows application selection,
 * reviewed installation, QEMU launch and boot-media source preparation.
 * Architecture: This bootstrap uses Win32 rather than depending on the GTK
 * runtime that it is about to install. Every operation delegates to Framework
 * services. A worker owns blocking I/O; only this GUI thread touches controls.
 * Existing GTK applications, NSIS setup and alternative tools are unaffected.
 * Author: Sammy Hegab, Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#define COBJMACROS
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <windows.h>
#include <limits.h>
#include <wchar.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shlobj.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/setup_centre/win32.h"
#include "umicom/setup_centre/execution.h"
#define WM_SETUP_PROGRESS (WM_APP+41)
#define WM_SETUP_DONE (WM_APP+42)
enum {
    ID_TAB=100,ID_SOURCE,ID_SOURCE_BROWSE,ID_LOAD,ID_APPS,ID_ALL,ID_NONE,ID_DEST,ID_DEST_BROWSE,
    ID_DESKTOP,ID_MENU,ID_REVIEW,ID_INSTALL,ID_QEMU,ID_QEMU_BROWSE,ID_KERNEL,ID_KERNEL_BROWSE,
    ID_INITRD,ID_INITRD_BROWSE,ID_ARCH,ID_MEMORY,ID_CPUS,ID_RECOVERY,ID_INTERACTIVE,ID_VM_REVIEW,
    ID_VM_RUN,ID_MEDIA_KERNEL,ID_MEDIA_KERNEL_BROWSE,ID_MEDIA_INITRD,ID_MEDIA_INITRD_BROWSE,
    ID_MEDIA_DEST,ID_MEDIA_DEST_BROWSE,ID_MEDIA_PREPARE,ID_STATUS,ID_PROGRESS,ID_STOP,ID_VERIFY
};

typedef enum JobKind {
    JOB_LOAD,JOB_REVIEW,JOB_INSTALL,JOB_VM_REVIEW,JOB_VM_RUN,JOB_MEDIA,JOB_VERIFY
}
JobKind;

typedef struct Ui Ui;

typedef struct Job {

    Ui *ui;
    JobKind kind;
    UmiSetupReport report;
    UmiStatus status;

    UmiSetupBundle *loaded;
    UmiSetupVmPlan *plan;
    UmiCancellationToken *cancel;

    UmiProcessResult *process;
    HANDLE thread;
    ULONGLONG lastProgress;

    char source[UMI_SETUP_PATH_CAPACITY],destination[UMI_SETUP_PATH_CAPACITY];

    char qemu[UMI_SETUP_PATH_CAPACITY],kernel[UMI_SETUP_PATH_CAPACITY],initrd[UMI_SETUP_PATH_CAPACITY];

    char architecture[16],fingerprint[65];
    uint64_t selected;
    unsigned memory,processors;

    int recovery,interactive,desktop,menu;

}
Job;

typedef struct Control {
    HWND window;
    int id,page,x,y,width,height;
}
Control;

struct Ui {

    HWND window;
    HINSTANCE instance;
    HFONT font;
    HBITMAP logo;

    Control controls[80];
    size_t controlCount;
    Job *job;
    int page,closing,loaded;

    UmiSetupBundle *bundle;
    UmiSetupVmPlan *vmPlan;
    char fingerprint[65];

};

static HWND Find(Ui *u,int id) {
    for(size_t i=0; i<u->controlCount; ++i)if(u->controls[i].id==id)return u->controls[i].window;
    return NULL;
}

static wchar_t *Display(const char *text)
{
    int n=MultiByteToWideChar(CP_UTF8,0,text,-1,NULL,0);
    if(n<=0)return NULL;
    wchar_t *w=malloc((size_t)n*sizeof *w);
    if(!w)return NULL;
    if(!MultiByteToWideChar(CP_UTF8,0,text,-1,w,n)) {
        free(w);
        return NULL;
    }
    return w;
}

static void Status(Ui *u,const char *text) {
    wchar_t *w=Display(text);
    if(w) {
        SetWindowTextW(Find(u,ID_STATUS),w);
        free(w);
    }
}

static int ReadText(Ui *u,int id,char *out,size_t capacity)
{
    HWND control=Find(u,id);
    int n=GetWindowTextLengthW(control);
    if(n<0||n>4095)return 0;
    wchar_t wide[4096];
    if(GetWindowTextW(control,wide,4096)!=n)return 0;
    return capacity<=(size_t)INT_MAX&&WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide,-1,out,(int)capacity,NULL,NULL)>0;
}

static HWND Add(Ui *u,int id,int page,const wchar_t *className,const wchar_t *text,DWORD style,int x,int y,int width,int height)
{

    if(u->controlCount==80U)return NULL;

    HWND h=CreateWindowExW(!wcscmp(className,L"EDIT")?WS_EX_CLIENTEDGE:0,className,text,WS_CHILD|WS_VISIBLE|style,0,0,0,0,u->window,(HMENU)(INT_PTR)id,u->instance,NULL);

    if(!h)return NULL;
    SendMessageW(h,WM_SETFONT,(WPARAM)u->font,TRUE);

    u->controls[u->controlCount++]=(Control) {
        h,id,page,x,y,width,height
    };
    return h;

}

static void Label(Ui *u,int page,const wchar_t *text,int x,int y,int width,int height)
{
    (void)Add(u,0,page,L"STATIC",text,SS_LEFT,x,y,width,height);
}

static void Edit(Ui *u,int id,int browse,int page,const wchar_t *label,int y)
{
    Label(u,page,label,30,y+4,150,24);
    (void)Add(u,id,page,L"EDIT",L"",ES_AUTOHSCROLL|WS_TABSTOP,185,y,-330,28);
    if(browse)(void)Add(u,browse,page,L"BUTTON",L"Browse...",BS_PUSHBUTTON|WS_TABSTOP,-130,y,100,28);
}

static void Layout(Ui *u)
{

    RECT rect;
    GetClientRect(u->window,&rect);
    UINT dpi=GetDpiForWindow(u->window);
    if(!dpi)dpi=96;

    int width=MulDiv(rect.right,96,(int)dpi),height=MulDiv(rect.bottom,96,(int)dpi);

    for(size_t i=0; i<u->controlCount; ++i) {
        Control *c=&u->controls[i];
        int x=c->x<0?width+c->x:c->x,y=c->y<0?height+c->y:c->y;

        int w=c->width<0?width+c->width:c->width,h=c->height<0?height+c->height:c->height;

        MoveWindow(c->window,MulDiv(x,(int)dpi,96),MulDiv(y,(int)dpi,96),MulDiv(w,(int)dpi,96),MulDiv(h,(int)dpi,96),TRUE);

        ShowWindow(c->window,c->page<0||c->page==u->page?SW_SHOW:SW_HIDE);
    }

    if(Find(u,ID_APPS))ListView_SetColumnWidth(Find(u,ID_APPS),0,MulDiv(width-80,(int)dpi,96));

}

static void Buttons(Ui *u)
{

    int busy=u->job!=NULL;

    for(size_t i=0; i<u->controlCount; ++i)if(u->controls[i].id&&u->controls[i].id!=ID_STATUS&&u->controls[i].id!=ID_PROGRESS)EnableWindow(u->controls[i].window,!busy);

    EnableWindow(Find(u,ID_REVIEW),!busy&&u->loaded);

    EnableWindow(Find(u,ID_INSTALL),!busy&&u->loaded&&u->fingerprint[0]);

    EnableWindow(Find(u,ID_VM_RUN),!busy&&u->vmPlan!=NULL);

    EnableWindow(Find(u,ID_STOP),busy);

}

static uint64_t Selected(Ui *u)
{
    uint64_t mask=0;
    size_t n=UmiSetupApplicationCount(u->bundle);
    for(size_t i=0; i<n; ++i)if(ListView_GetCheckState(Find(u,ID_APPS),(int)i))mask|=UINT64_C(1)<<i;
    return mask;
}

static void Populate(Ui *u)
{

    HWND list=Find(u,ID_APPS);
    ListView_DeleteAllItems(list);

    for(size_t i=0; i<UmiSetupApplicationCount(u->bundle); ++i) {
        const UmiSetupApplication *a=UmiSetupApplicationAt(u->bundle,i);
        wchar_t *name=Display(a->title);
        if(name) {
            LVITEMW item= {
                0
            };
            item.mask=LVIF_TEXT;
            item.iItem=(int)i;
            item.pszText=name;
            SendMessageW(list,LVM_INSERTITEMW,0,(LPARAM)&item);
            free(name);
            ListView_SetCheckState(list,(int)i,TRUE);
        }
    }

}

static int Progress(const UmiSetupReport *r,void *opaque)
{

    Job *j=opaque;
    if(umi_cancellation_token_is_requested(j->cancel))return 1;

    ULONGLONG now=GetTickCount64();
    if(now-j->lastProgress>=100U) {
        j->lastProgress=now;
        UmiSetupReport *copy=malloc(sizeof *copy);
        if(copy) {
            *copy=*r;
            if(!PostMessageW(j->ui->window,WM_SETUP_PROGRESS,0,(LPARAM)copy))free(copy);
        }
    }

    return 0;

}

static DWORD WINAPI Worker(void *opaque)
{

    Job *j=opaque;
    Ui *u=j->ui;
    UmiSetupProcessContext process= {
        j->cancel,j->interactive?0U:300000U,j->process
    };

    switch(j->kind) {

        case JOB_LOAD:j->status=UmiSetupBundleOpen(j->source,&j->loaded,&j->report);
        break;

        case JOB_REVIEW:j->status=UmiSetupReview(u->bundle,j->selected,j->destination,j->fingerprint,Progress,j,&j->report);
        break;

        case JOB_INSTALL:
        j->status=UmiSetupInstall(u->bundle,j->selected,j->destination,j->fingerprint,Progress,j,&j->report);

        if(j->status==UMI_STATUS_OK&&(j->desktop||j->menu)) {

            const char *last=strrchr(j->destination,'\\'),*slash=strrchr(j->destination,'/');
            if(slash&&(!last||slash>last))last=slash;

            char group[300];
            (void)snprintf(group,sizeof group,"Umicom - %.240s",last?last+1:j->destination);
            UmiSetupReport links= {
                0
            };

            UmiStatus linkStatus=UmiSetupCreateShortcuts(j->destination,group,j->desktop,j->menu,&links);

            if(linkStatus!=UMI_STATUS_OK)(void)snprintf(j->report.detail,sizeof j->report.detail,"Applications installed. Shortcuts were not completed: %.220s",links.detail);
        }

        break;

        case JOB_VM_REVIEW: {

            j->plan=malloc(sizeof *j->plan);
            if(!j->plan) {
                j->status=UMI_STATUS_OUT_OF_MEMORY;
                break;
            }

            UmiSetupVmConfig c= {
                j->qemu,j->kernel,j->initrd,j->architecture,j->memory,j->processors,j->recovery,j->interactive
            };
            j->status=UmiSetupVmReview(&c,j->plan,&j->report);
            break;
        }

        case JOB_VM_RUN:j->status=UmiSetupVmRun(u->vmPlan,u->vmPlan->fingerprint,UmiSetupExecuteProcess,&process,&j->report);
        break;

        case JOB_VERIFY:j->status=UmiSetupVerifyInstallation(j->destination,&j->report);
        break;

        case JOB_MEDIA:j->status=UmiSetupMediaPrepare(j->kernel,j->initrd,j->destination,&j->report);
        break;

    }

    PostMessageW(u->window,WM_SETUP_DONE,0,0);
    return 0;

}

static void Start(Ui *u,JobKind kind)
{

    if(u->job)return;
    Job *j=calloc(1,sizeof *j);
    if(!j) {
        Status(u,"Not enough memory.");
        return;
    }
    j->ui=u;
    j->kind=kind;
    if(kind==JOB_LOAD){u->loaded=0;u->fingerprint[0]=0;}
    j->selected=Selected(u);

    int valid=ReadText(u,ID_SOURCE,j->source,sizeof j->source)&&ReadText(u,ID_DEST,j->destination,sizeof j->destination);

    strcpy(j->fingerprint,u->fingerprint);
    j->desktop=IsDlgButtonChecked(u->window,ID_DESKTOP)==BST_CHECKED;
    j->menu=IsDlgButtonChecked(u->window,ID_MENU)==BST_CHECKED;

    if(kind==JOB_VM_REVIEW||kind==JOB_VM_RUN) {

        valid=ReadText(u,ID_QEMU,j->qemu,sizeof j->qemu)&&ReadText(u,ID_KERNEL,j->kernel,sizeof j->kernel)&&ReadText(u,ID_INITRD,j->initrd,sizeof j->initrd);

        strcpy(j->architecture,SendMessageW(Find(u,ID_ARCH),CB_GETCURSEL,0,0)==1?"riscv64":"x86_64");

        BOOL ok=FALSE;
        j->memory=GetDlgItemInt(u->window,ID_MEMORY,&ok,FALSE);
        valid=valid&&ok;
        j->processors=GetDlgItemInt(u->window,ID_CPUS,&ok,FALSE);
        valid=valid&&ok;

        j->recovery=IsDlgButtonChecked(u->window,ID_RECOVERY)==BST_CHECKED;
        j->interactive=IsDlgButtonChecked(u->window,ID_INTERACTIVE)==BST_CHECKED;

        if(kind==JOB_VM_RUN&&u->vmPlan)j->interactive=u->vmPlan->config.interactive;

    }

    if(kind==JOB_MEDIA)valid=ReadText(u,ID_MEDIA_KERNEL,j->kernel,sizeof j->kernel)&&ReadText(u,ID_MEDIA_INITRD,j->initrd,sizeof j->initrd)&&ReadText(u,ID_MEDIA_DEST,j->destination,sizeof j->destination);

    if(!valid||umi_cancellation_token_create(&j->cancel)!=UMI_STATUS_OK) {
        free(j);
        Status(u,"A form field is too long or invalid.");
        return;
    }

    j->process=calloc(1,sizeof *j->process);
    if(!j->process) {
        umi_cancellation_token_destroy(j->cancel);
        free(j);
        return;
    }

    u->job=j;
    Buttons(u);
    Status(u,kind==JOB_VM_RUN?"Running QEMU. Use the guest console's poweroff command to leave normally. Stop forces termination.":"Working. No automatic application launch or drive erasure is performed.");

    j->thread=CreateThread(NULL,0,Worker,j,0,NULL);

    if(!j->thread) {
        u->job=NULL;
        umi_cancellation_token_destroy(j->cancel);
        free(j->process);
        free(j);
        Status(u,"Could not start the worker.");
        Buttons(u);
    }

}

static void Done(Ui *u)
{

    Job *j=u->job;
    if(!j)return;
    WaitForSingleObject(j->thread,INFINITE);
    CloseHandle(j->thread);

    if(j->status==UMI_STATUS_OK&&j->kind==JOB_LOAD) {
        UmiSetupBundleDestroy(u->bundle);
        u->bundle=j->loaded;
        j->loaded=NULL;
        u->loaded=1;
        Populate(u);
    }

    if(j->kind==JOB_REVIEW) {
        u->fingerprint[0]=0;
        if(j->status==UMI_STATUS_OK)strcpy(u->fingerprint,j->fingerprint);
    }

    if(j->kind==JOB_VM_REVIEW) {
        free(u->vmPlan);
        u->vmPlan=NULL;
        if(j->status==UMI_STATUS_OK) {
            u->vmPlan=j->plan;
            j->plan=NULL;
        }
    }

    if(j->kind==JOB_INSTALL)u->fingerprint[0]=0;

    /* Show the actual selection and command arguments before enabling a
      * consequential action. Hashes are change detectors, not a signature. */
    size_t capacity = UMI_PROCESS_OUTPUT_CAPACITY + 65536U;
    char *text = calloc(1U, capacity);
    if (text != NULL) {
        size_t used = (size_t)snprintf(text, capacity, "%s\r\n%s",
            UmiSetupStatusText(j->status), j->report.detail);
        if (j->status == UMI_STATUS_OK && j->kind == JOB_REVIEW) {
            used += (size_t)snprintf(text+used, capacity-used,
                "\r\nDestination: %s\r\nFiles: %zu; bytes: %" PRIu64
                "\r\nPlan: %s\r\nSelected applications:",
                j->destination, j->report.filesCompleted, j->report.bytesPlanned,
                j->fingerprint);
            for (size_t i=0; i<UmiSetupApplicationCount(u->bundle); ++i)
                if ((j->selected >> i) & UINT64_C(1))
                    used += (size_t)snprintf(text+used, capacity-used,"\r\n  %s",
                        UmiSetupApplicationAt(u->bundle,i)->title);
        }
        if (j->status == UMI_STATUS_OK && j->kind == JOB_VM_REVIEW && u->vmPlan) {
            const UmiSetupVmPlan *plan=u->vmPlan;
            used += (size_t)snprintf(text+used,capacity-used,
                "\r\nQEMU: %s\r\nQEMU hash: %s\r\nKernel hash: %s"
                "\r\nRoot filesystem hash: %s\r\nPlan: %s\r\nArguments (one per line):",
                plan->program,plan->programHash,plan->kernelHash,
                plan->initrdHash,plan->fingerprint);
            for (size_t i=0;i<plan->argumentCount;++i)
                used += (size_t)snprintf(text+used,capacity-used,"\r\n  %s",plan->arguments[i]);
        }
        if (j->process->output[0] && used < capacity)
            (void)snprintf(text+used,capacity-used,"\r\nNative tool diagnostics:\r\n%s",j->process->output);
        Status(u,text);
        free(text);
    }

    UmiSetupBundleDestroy(j->loaded);
    free(j->plan);
    umi_cancellation_token_destroy(j->cancel);
    free(j->process);
    free(j);
    u->job=NULL;
    Buttons(u);

    if(u->closing)DestroyWindow(u->window);

}

static int Folder(Ui *u,wchar_t out[4096])
{
    BROWSEINFOW b= {
        0
    };
    b.hwndOwner=u->window;
    b.lpszTitle=L"Choose an existing folder";
    b.ulFlags=BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
    PIDLIST_ABSOLUTE pidl=SHBrowseForFolderW(&b);
    if(!pidl)return 0;
    BOOL ok=SHGetPathFromIDListW(pidl,out);
    CoTaskMemFree(pidl);
    return ok!=0;
}

static void Browse(Ui *u,int destination,int folder,int newChild)
{

    wchar_t path[4096]= {
        0
    };
    if(folder) {
        if(!Folder(u,path))return;
        if(newChild) {
            size_t n=wcslen(path);
            const wchar_t *suffix=destination==ID_MEDIA_DEST?L"\\Umicom Media Preparation":L"\\Umicom Applications";
            if(n+wcslen(suffix)>=4096U)return;
            wcscat(path,suffix);
        }
    }

    else {
        OPENFILENAMEW o= {
            0
        };
        o.lStructSize=sizeof o;
        o.hwndOwner=u->window;
        o.lpstrFilter=L"All files\0*.*\0\0";
        o.lpstrFile=path;
        o.nMaxFile=4096;
        o.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR;
        if(!GetOpenFileNameW(&o))return;
    }

    SetWindowTextW(Find(u,destination),path);

}

static void Cancel(Ui *u)
{
    if(!u->job)return;
    if(u->job->kind==JOB_VM_RUN&&MessageBoxW(u->window,L"Stop QEMU now? Unsaved guest work will be lost.",L"Stop virtual machine",MB_YESNO|MB_ICONWARNING)!=IDYES)return;
    umi_cancellation_token_request(u->job->cancel);
    Status(u,"Stop requested. Waiting for the current native operation to return.");
}

static void Build(Ui *u)
{

    u->font=(HFONT)GetStockObject(DEFAULT_GUI_FONT);

    HWND picture=Add(u,0,-1,L"STATIC",L"",SS_BITMAP,20,15,185,43);
    u->logo=LoadBitmapW(u->instance,MAKEINTRESOURCEW(102));
    if(u->logo)SendMessageW(picture,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)u->logo);

    Label(u,-1,L"Umicom Setup Centre",235,18,550,28);
    Label(u,-1,L"Install selected applications, or try an existing Umicom OS image.",235,46,650,24);

    HWND tabs=Add(u,ID_TAB,-1,WC_TABCONTROLW,L"",WS_TABSTOP,20,80,-40,-265);
    TCITEMW item= {
        0
    };
    item.mask=TCIF_TEXT;
    item.pszText=L"Applications";
    TabCtrl_InsertItem(tabs,0,&item);
    item.pszText=L"Umicom OS in QEMU";
    TabCtrl_InsertItem(tabs,1,&item);
    item.pszText=L"Boot media";
    TabCtrl_InsertItem(tabs,2,&item);

    Edit(u,ID_SOURCE,ID_SOURCE_BROWSE,0,L"Offline release folder",125);

    Add(u,ID_LOAD,0,L"BUTTON",L"Load applications",BS_PUSHBUTTON|WS_TABSTOP,30,166,150,28);

    Add(u,ID_ALL,0,L"BUTTON",L"Select all",BS_PUSHBUTTON|WS_TABSTOP,195,166,115,28);
    Add(u,ID_NONE,0,L"BUTTON",L"Select none",BS_PUSHBUTTON|WS_TABSTOP,325,166,115,28);

    HWND list=Add(u,ID_APPS,0,WC_LISTVIEWW,L"",LVS_REPORT|LVS_NOCOLUMNHEADER|WS_BORDER|WS_TABSTOP,30,207,-60,185);
    ListView_SetExtendedListViewStyle(list,LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER);
    LVCOLUMNW column= {
        0
    };
    column.mask=LVCF_TEXT|LVCF_WIDTH;
    column.pszText=L"Applications";
    column.cx=800;
    SendMessageW(list,LVM_INSERTCOLUMNW,0,(LPARAM)&column);

    Edit(u,ID_DEST,ID_DEST_BROWSE,0,L"New install folder",407);

    Add(u,ID_DESKTOP,0,L"BUTTON",L"Desktop shortcut group",BS_AUTOCHECKBOX|WS_TABSTOP,30,448,240,24);
    Add(u,ID_MENU,0,L"BUTTON",L"Start menu shortcut group",BS_AUTOCHECKBOX|WS_TABSTOP,285,448,260,24);

    Add(u,ID_REVIEW,0,L"BUTTON",L"Review selection",BS_PUSHBUTTON|WS_TABSTOP,30,488,160,30);
    Add(u,ID_INSTALL,0,L"BUTTON",L"Install reviewed selection",BS_PUSHBUTTON|WS_TABSTOP,205,488,205,30);

    Add(u,ID_VERIFY,0,L"BUTTON",L"Verify installation",BS_PUSHBUTTON|WS_TABSTOP,435,488,175,30);

    Edit(u,ID_QEMU,ID_QEMU_BROWSE,1,L"QEMU program",125);
    Edit(u,ID_KERNEL,ID_KERNEL_BROWSE,1,L"Umicom Linux kernel",166);
    Edit(u,ID_INITRD,ID_INITRD_BROWSE,1,L"Root filesystem (.gz)",207);

    Label(u,1,L"Guest architecture",30,252,145,24);
    HWND arch=Add(u,ID_ARCH,1,L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_TABSTOP,185,248,150,100);
    SendMessageW(arch,CB_ADDSTRING,0,(LPARAM)L"x86_64");
    SendMessageW(arch,CB_ADDSTRING,0,(LPARAM)L"riscv64");
    SendMessageW(arch,CB_SETCURSEL,0,0);

    Label(u,1,L"Memory (MiB)",370,252,120,24);
    Add(u,ID_MEMORY,1,L"EDIT",L"512",ES_NUMBER|WS_TABSTOP,490,248,70,28);
    Label(u,1,L"Processors",600,252,100,24);
    Add(u,ID_CPUS,1,L"EDIT",L"1",ES_NUMBER|WS_TABSTOP,700,248,60,28);

    Add(u,ID_RECOVERY,1,L"BUTTON",L"Start in recovery",BS_AUTOCHECKBOX|WS_TABSTOP,30,294,250,24);
    Add(u,ID_INTERACTIVE,1,L"BUTTON",L"Open QEMU serial-console window",BS_AUTOCHECKBOX|WS_TABSTOP,300,294,330,24);
    CheckDlgButton(u->window,ID_INTERACTIVE,BST_CHECKED);

    Label(u,1,L"This runs an existing diskless image. No host disk, shared folder or network is attached.\nThe current foundation provides a text recovery console, not a graphical Umicom OS desktop.\nChoose trusted QEMU binaries and guest images. Header checks are not signatures.",30,341,850,95);

    Add(u,ID_VM_REVIEW,1,L"BUTTON",L"Review VM inputs",BS_PUSHBUTTON|WS_TABSTOP,30,450,165,30);
    Add(u,ID_VM_RUN,1,L"BUTTON",L"Start reviewed VM",BS_PUSHBUTTON|WS_TABSTOP,210,450,170,30);

    Edit(u,ID_MEDIA_KERNEL,ID_MEDIA_KERNEL_BROWSE,2,L"x86-64 Linux kernel",125);
    Edit(u,ID_MEDIA_INITRD,ID_MEDIA_INITRD_BROWSE,2,L"Root filesystem (.gz)",166);
    Edit(u,ID_MEDIA_DEST,ID_MEDIA_DEST_BROWSE,2,L"New preparation folder",207);

    Label(u,2,L"Prepare a checked kernel, root filesystem and GRUB menu in a new folder.\nThis folder is not yet a bootable USB or CD. The native Linux media-iso command can\nask GRUB to make an ISO, which still needs BIOS/UEFI boot testing.\n\nUSB-device writing and optical burning are not enabled in this release.\nNo physical disk is opened, formatted or erased by this tool.",30,264,850,160);

    Add(u,ID_MEDIA_PREPARE,2,L"BUTTON",L"Prepare boot-media source",BS_PUSHBUTTON|WS_TABSTOP,30,450,230,30);

    Add(u,ID_STATUS,-1,L"EDIT",L"Load a trusted offline release to begin. No installation starts automatically.",ES_MULTILINE|ES_READONLY|ES_AUTOVSCROLL|WS_VSCROLL,20,-168,-40,112);

    Add(u,ID_PROGRESS,-1,PROGRESS_CLASSW,L"",0,20,-39,-190,16);
    SendMessageW(Find(u,ID_PROGRESS),PBM_SETRANGE32,0,1000);

    Add(u,ID_STOP,-1,L"BUTTON",L"Stop",BS_PUSHBUTTON|WS_TABSTOP,-150,-45,130,28);

    wchar_t path[4096];
    DWORD n=GetModuleFileNameW(NULL,path,4096);
    if(n>0&&n<4096) {
        wchar_t *last=wcsrchr(path,L'\\');
        if(last)*last=0;
        SetWindowTextW(Find(u,ID_SOURCE),path);
    }

    PWSTR local=NULL;
    if(SUCCEEDED(SHGetKnownFolderPath(&FOLDERID_LocalAppData,0,NULL,&local))) {
        if(wcslen(local)<3900U) {
            swprintf(path,4096,L"%ls\\Umicom Applications",local);
            SetWindowTextW(Find(u,ID_DEST),path);
            swprintf(path,4096,L"%ls\\Umicom Media Preparation",local);
            SetWindowTextW(Find(u,ID_MEDIA_DEST),path);
        }
        CoTaskMemFree(local);
    }

    Layout(u);
    Buttons(u);

}

static LRESULT CALLBACK WindowProc(HWND window,UINT message,WPARAM w,LPARAM l)
{

    Ui *u=(Ui *)GetWindowLongPtrW(window,GWLP_USERDATA);

    if(message==WM_NCCREATE) {
        CREATESTRUCTW *c=(CREATESTRUCTW *)l;
        u=c->lpCreateParams;
        u->window=window;
        SetWindowLongPtrW(window,GWLP_USERDATA,(LONG_PTR)u);
        return TRUE;
    }

    if(!u)return DefWindowProcW(window,message,w,l);

    switch(message) {

        case WM_CREATE:Build(u);
        return 0;

        case WM_SIZE:Layout(u);
        return 0;

        case WM_GETMINMAXINFO: {
            MINMAXINFO *m=(MINMAXINFO *)l;
            UINT dpi=GetDpiForWindow(window);if(!dpi)dpi=96;
            m->ptMinTrackSize.x=MulDiv(950,(int)dpi,96);
            m->ptMinTrackSize.y=MulDiv(740,(int)dpi,96);
            return 0;
        }

        case WM_DPICHANGED: {
            RECT *r=(RECT *)l;
            SetWindowPos(window,NULL,r->left,r->top,r->right-r->left,r->bottom-r->top,SWP_NOZORDER|SWP_NOACTIVATE);
            Layout(u);
            return 0;
        }

        case WM_NOTIFY: {
            NMHDR *n=(NMHDR *)l;
            if(n->idFrom==ID_TAB&&n->code==TCN_SELCHANGE) {
                u->page=TabCtrl_GetCurSel(Find(u,ID_TAB));
                Layout(u);
            }
            if(n->idFrom==ID_APPS&&n->code==LVN_ITEMCHANGED&&!u->job) {
                u->fingerprint[0]=0;
                Buttons(u);
            }
            return 0;
        }

        case WM_COMMAND: {
            int id=LOWORD(w),notification=HIWORD(w);
            if(u->job&&id!=ID_STOP)return 0;

            if(notification==EN_CHANGE) {
                if(id==ID_SOURCE) {
                    u->loaded=0;
                    u->fingerprint[0]=0;
                }
                if(id==ID_DEST)u->fingerprint[0]=0;

                if(id==ID_QEMU||id==ID_KERNEL||id==ID_INITRD||id==ID_MEMORY||id==ID_CPUS) {
                    free(u->vmPlan);
                    u->vmPlan=NULL;
                }
                Buttons(u);
                return 0;
            }

            if(id==ID_ARCH&&notification==CBN_SELCHANGE) {
                free(u->vmPlan);
                u->vmPlan=NULL;
                Buttons(u);
                return 0;
            }

            switch(id) {
                case ID_SOURCE_BROWSE:Browse(u,ID_SOURCE,1,0);
                break;
                case ID_DEST_BROWSE:Browse(u,ID_DEST,1,1);
                break;

                case ID_LOAD:Start(u,JOB_LOAD);
                break;
                case ID_REVIEW:Start(u,JOB_REVIEW);
                break;
                case ID_INSTALL:Start(u,JOB_INSTALL);
                break;
                case ID_VERIFY:Start(u,JOB_VERIFY);
                break;

                case ID_ALL:case ID_NONE:for(size_t i=0; i<UmiSetupApplicationCount(u->bundle); ++i)ListView_SetCheckState(Find(u,ID_APPS),(int)i,id==ID_ALL);
                u->fingerprint[0]=0;
                break;

                case ID_QEMU_BROWSE:Browse(u,ID_QEMU,0,0);
                break;
                case ID_KERNEL_BROWSE:Browse(u,ID_KERNEL,0,0);
                break;
                case ID_INITRD_BROWSE:Browse(u,ID_INITRD,0,0);
                break;

                case ID_RECOVERY:case ID_INTERACTIVE:free(u->vmPlan);
                u->vmPlan=NULL;
                break;

                case ID_VM_REVIEW:Start(u,JOB_VM_REVIEW);
                break;
                case ID_VM_RUN:Start(u,JOB_VM_RUN);
                break;

                case ID_MEDIA_KERNEL_BROWSE:Browse(u,ID_MEDIA_KERNEL,0,0);
                break;
                case ID_MEDIA_INITRD_BROWSE:Browse(u,ID_MEDIA_INITRD,0,0);
                break;
                case ID_MEDIA_DEST_BROWSE:Browse(u,ID_MEDIA_DEST,1,1);
                break;

                case ID_MEDIA_PREPARE:Start(u,JOB_MEDIA);
                break;
                case ID_STOP:Cancel(u);
                break;
                default:break;
            }
            Buttons(u);
            return 0;
        }

        case WM_SETUP_PROGRESS: {
            UmiSetupReport *r=(UmiSetupReport *)l;
            if(r) {
                Status(u,r->detail);
                uint64_t value=r->bytesPlanned?r->bytesCompleted*1000U/r->bytesPlanned:0;
                SendMessageW(Find(u,ID_PROGRESS),PBM_SETPOS,(WPARAM)(value>1000U?1000U:value),0);
                free(r);
            }
            return 0;
        }

        case WM_SETUP_DONE:Done(u);
        return 0;

        case WM_CLOSE:if(u->job) {
            if(MessageBoxW(window,L"Stop the current operation and close when it returns? Incomplete new files are retained; unsaved guest work can be lost.",L"Close Setup Centre",MB_YESNO|MB_ICONWARNING)==IDYES) {
                u->closing=1;
                umi_cancellation_token_request(u->job->cancel);
            }
            return 0;
        }
        DestroyWindow(window);
        return 0;

        case WM_DESTROY:UmiSetupBundleDestroy(u->bundle);
        free(u->vmPlan);
        if(u->logo)DeleteObject(u->logo);
        PostQuitMessage(0);
        return 0;

        default:return DefWindowProcW(window,message,w,l);
    }

}

static int RunWindow(void *instance,int show,int checkOnly)
{

    INITCOMMONCONTROLSEX controls= {
        sizeof controls,ICC_WIN95_CLASSES|ICC_PROGRESS_CLASS
    };
    if(!InitCommonControlsEx(&controls))return 1;

    HRESULT com=CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);
    if(FAILED(com))return 1;

    Ui *u=calloc(1,sizeof *u);
    if(!u) {
        CoUninitialize();
        return 1;
    }
    u->instance=(HINSTANCE)instance;

    WNDCLASSEXW cls= {
        0
    };
    cls.cbSize=sizeof cls;
    cls.lpfnWndProc=WindowProc;
    cls.hInstance=u->instance;
    cls.hCursor=LoadCursorW(NULL,IDC_ARROW);
    cls.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    cls.lpszClassName=L"UmicomSetupCentre";
    cls.hIcon=LoadIconW(u->instance,MAKEINTRESOURCEW(101));
    cls.hIconSm=cls.hIcon;

    if(!RegisterClassExW(&cls)) {
        free(u);
        CoUninitialize();
        return 1;
    }

    HWND window=CreateWindowExW(0,cls.lpszClassName,L"Umicom Setup Centre",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,1020,780,NULL,NULL,u->instance,u);

    if(!window) {
        free(u);
        CoUninitialize();
        return 1;
    }
    if (checkOnly) {
        /* Construct the real controls without presenting a window, starting a
          * worker, reading a release or making any installation changes. */
        int passed = u->logo != NULL && u->job == NULL && u->bundle == NULL &&
            TabCtrl_GetItemCount(Find(u,ID_TAB)) == 3 &&
            !IsWindowEnabled(Find(u,ID_INSTALL)) &&
            !IsWindowEnabled(Find(u,ID_VM_RUN)) &&
            !IsWindowEnabled(Find(u,ID_STOP)) &&
            Find(u,ID_APPS) != NULL && Find(u,ID_MEDIA_PREPARE) != NULL;
        u->page = 1;
        Layout(u);
        passed = passed && ((GetWindowLongW(Find(u,ID_QEMU),GWL_STYLE) & WS_VISIBLE) != 0) &&
            ((GetWindowLongW(Find(u,ID_APPS),GWL_STYLE) & WS_VISIBLE) == 0);
        DestroyWindow(window);
        MSG quit;
        (void)PeekMessageW(&quit,NULL,WM_QUIT,WM_QUIT,PM_REMOVE);
        free(u);
        CoUninitialize();
        return passed ? 0 : 1;
    }
    ShowWindow(window,show);
    UpdateWindow(window);

    MSG message;
    int got;
    while((got=GetMessageW(&message,NULL,0,0))>0) {
        if(!IsDialogMessageW(window,&message)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    free(u);
    CoUninitialize();
    return got<0?1:(int)message.wParam;

}

int UmiSetupWin32Run(void *instance, int showCommand)
{
    return RunWindow(instance, showCommand, 0);
}

int UmiSetupWin32Check(void *instance)
{
    return RunWindow(instance, SW_HIDE, 1);
}
