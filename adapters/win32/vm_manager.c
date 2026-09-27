/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/win32/vm_manager.c
 * Purpose: Saved profiles and reviewed QMP sessions in a native Windows window.
 * Architecture: The worker calls Framework services; the UI only projects their
 * results. Installer bootstrap, legacy QEMU launcher and GTK hosts stay intact.
 * Author: Sammy Hegab, Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <inttypes.h>
#include "umicom/vm_manager/manager.h"
#include "umicom/setup_centre/setup.h"
#include "umicom/vm_manager/win32.h"
#define VM_DONE (WM_APP+81)
enum {
    ID_PROFILE=201,ID_NAME,ID_DB,ID_RUNTIME,ID_IMAGE,ID_DISK,ID_OUTPUT,ID_ARCH,ID_MEMORY,ID_CPUS,ID_RECOVERY,ID_GIB,ID_LOAD,ID_SAVE,ID_REVIEW,ID_START,ID_PAUSE,ID_RESUME,ID_QUERY,ID_POWER,ID_CONSOLE_READ,ID_CONSOLE_INPUT,ID_CONSOLE_SEND,ID_RUNTIME_CHECK,ID_DISK_CREATE,ID_CHECKPOINT,ID_FORCE,ID_CONSOLE,ID_STATUS,ID_NEW_RUN
};
typedef enum Kind {
    LOAD,SAVE,REVIEW,START,CONTROL,RUNTIME_CHECK,DISK_CREATE,CHECKPOINT,FORCE
}
Kind;
typedef struct Control {
    HWND handle;
    int id,x,y,width,height;
}
Control;
typedef struct Ui Ui;
typedef struct Job {
    Ui*ui;
    Kind kind;
    UmiVmCommand command;
    UmiVmProfile profile;
    UmiVmSession*session;
    UmiVmReport report;
    UmiStatus status;
    HANDLE thread;
    char db[1024],output[1024],expected[65],console[2048];
    size_t consoleLength;
    unsigned gib;
    uint64_t revision;
}
Job;
struct Ui {
    HWND window;
    HINSTANCE instance;
    HFONT font;
    HBITMAP logo;
    Control controls[80];
    size_t count;
    Job*job;
    UmiVmSession*session;
    uint64_t revision;
    int closing,setting,scrollX,scrollY;
    char fingerprint[65];
};
static HWND Find(Ui*u,int id){
    for(size_t i=0;i<u->count;++i)if(u->controls[i].id==id)return u->controls[i].handle;
    return NULL;
}
static wchar_t*Wide(const char*s){
    int n=MultiByteToWideChar(CP_UTF8,0,s,-1,NULL,0);
    if(n<=0)return NULL;
    wchar_t*w=malloc((size_t)n*sizeof *w);
    if(w&&!MultiByteToWideChar(CP_UTF8,0,s,-1,w,n)){
        free(w);
        return NULL;
    }
    return w;
}
static void Text(Ui*u,int id,const char*s){
    wchar_t*w=Wide(s);
    if(w){
        SetWindowTextW(Find(u,id),w);
        free(w);
    }
}
static int Read(Ui*u,int id,char*out,size_t cap){
    wchar_t w[2048];
    int count=GetWindowTextLengthW(Find(u,id));
    if(count<0||count>=2048)return 0;
    if(GetWindowTextW(Find(u,id),w,2048)!=count)return 0;
    return WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,w,-1,out,(int)cap,NULL,NULL)>0;
}
static int Number(Ui*u,int id,unsigned*out){
    char b[32];
    if(!Read(u,id,b,sizeof b)||!*b)return 0;
    uint64_t n=0;
    for(size_t i=0;b[i];++i){
        if(b[i]<'0'||b[i]>'9'||n>1000000U)return 0;
        n=n*10U+(unsigned)(b[i]-'0');
    }
    if(n>32768)return 0;
    *out=(unsigned)n;
    return 1;
}
static HWND Add(Ui*u,int id,const wchar_t*className,const wchar_t*text,DWORD style,int x,int y,int w,int h){
    if(u->count>=80)return NULL;
    HWND c=CreateWindowExW(!wcscmp(className,L"EDIT")?WS_EX_CLIENTEDGE:0,className,text,WS_CHILD|WS_VISIBLE|style,0,0,0,0,u->window,(HMENU)(INT_PTR)id,u->instance,NULL);
    if(c){
        u->controls[u->count++]=(Control){
            c,id,x,y,w,h
        };
        SendMessageW(c,WM_SETFONT,(WPARAM)u->font,TRUE);
    }
    return c;
}
static void Label(Ui*u,const wchar_t*t,int x,int y,int w){
    (void)Add(u,0,L"STATIC",t,SS_LEFT,x,y,w,24);
}
static void Entry(Ui*u,int id,const wchar_t*label,int y){
    Label(u,label,20,y+4,165);
    (void)Add(u,id,L"EDIT",L"",ES_AUTOHSCROLL|WS_TABSTOP,190,y,-212,27);
    SendMessageW(Find(u,id),EM_SETLIMITTEXT,1023,0);
}
static void Font(Ui*u){
    UINT dpi=GetDpiForWindow(u->window);
    if(!dpi)dpi=96;
    HFONT next=CreateFontW(-MulDiv(10,(int)dpi,72),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    if(!next)return;
    HFONT old=u->font;
    u->font=next;
    for(size_t i=0;i<u->count;++i)SendMessageW(u->controls[i].handle,WM_SETFONT,(WPARAM)next,TRUE);
    if(old)DeleteObject(old);
}
/* Keep a minimum logical form surface without forcing a window larger than
 * the monitor. Native scrollbars preserve access at high DPI and small sizes. */
static void Layout(Ui*u){
    RECT r;
    GetClientRect(u->window,&r);
    UINT dpi=GetDpiForWindow(u->window);
    if(!dpi)dpi=96;
    int viewWidth=MulDiv(r.right,96,(int)dpi);
    int viewHeight=MulDiv(r.bottom,96,(int)dpi);
    int width=viewWidth<950?950:viewWidth;
    int height=viewHeight<800?800:viewHeight;
    int maxX=width-viewWidth,maxY=height-viewHeight;
    if(u->scrollX<0)u->scrollX=0;
    if(u->scrollY<0)u->scrollY=0;
    if(u->scrollX>maxX)u->scrollX=maxX;
    if(u->scrollY>maxY)u->scrollY=maxY;
    SCROLLINFO sx={sizeof sx,SIF_RANGE|SIF_PAGE|SIF_POS|SIF_DISABLENOSCROLL,0,width-1,(UINT)viewWidth,u->scrollX,0};
    SCROLLINFO sy={sizeof sy,SIF_RANGE|SIF_PAGE|SIF_POS|SIF_DISABLENOSCROLL,0,height-1,(UINT)viewHeight,u->scrollY,0};
    SetScrollInfo(u->window,SB_HORZ,&sx,TRUE);
    SetScrollInfo(u->window,SB_VERT,&sy,TRUE);
    for(size_t i=0;i<u->count;++i){
        Control*c=&u->controls[i];
        int x=(c->x<0?width+c->x:c->x)-u->scrollX;
        int y=(c->y<0?height+c->y:c->y)-u->scrollY;
        int w=c->width<0?width+c->width:c->width;
        int h=c->height<0?height+c->height:c->height;
        MoveWindow(c->handle,MulDiv(x,(int)dpi,96),MulDiv(y,(int)dpi,96),MulDiv(w,(int)dpi,96),MulDiv(h,(int)dpi,96),TRUE);
    }
}
static void Scroll(Ui*u,int bar,int command){
    SCROLLINFO info={0};
    info.cbSize=sizeof info;
    info.fMask=SIF_ALL;
    if(!GetScrollInfo(u->window,bar,&info))return;
    int position=info.nPos;
    switch(command){
        case SB_LINEUP:position-=24;break;
        case SB_LINEDOWN:position+=24;break;
        case SB_PAGEUP:position-=(int)info.nPage;break;
        case SB_PAGEDOWN:position+=(int)info.nPage;break;
        case SB_TOP:position=0;break;
        case SB_BOTTOM:position=info.nMax;break;
        case SB_THUMBTRACK:case SB_THUMBPOSITION:position=info.nTrackPos;break;
        default:return;
    }
    if(bar==SB_HORZ)u->scrollX=position;else u->scrollY=position;
    Layout(u);
}
static void RevealFocus(Ui*u){
    HWND focus=GetFocus();
    if(!focus||!IsChild(u->window,focus))return;
    RECT item,client;
    GetWindowRect(focus,&item);
    MapWindowPoints(NULL,u->window,(POINT*)&item,2);
    GetClientRect(u->window,&client);
    UINT dpi=GetDpiForWindow(u->window);
    if(!dpi)dpi=96;
    int dx=item.left<0?item.left:item.right>client.right?item.right-client.right:0;
    int dy=item.top<0?item.top:item.bottom>client.bottom?item.bottom-client.bottom:0;
    if(dx||dy){
        u->scrollX+=MulDiv(dx,96,(int)dpi);
        u->scrollY+=MulDiv(dy,96,(int)dpi);
        Layout(u);
    }
}
static void Buttons(Ui*u){
    int busy=u->job!=NULL,alive=0,control=0;
    UmiVmSnapshot snapshot;
    if(!busy&&u->session&&UmiVmObserve(u->session,&snapshot)==UMI_STATUS_OK){
        alive=snapshot.processRunning;
        control=snapshot.controlAvailable;
    }
    for(size_t i=0;i<u->count;++i){
        int id=u->controls[i].id;
        if(id&&id!=ID_CONSOLE&&id!=ID_STATUS)EnableWindow(u->controls[i].handle,!busy);
    }
    EnableWindow(Find(u,ID_START),!busy&&!alive&&u->fingerprint[0]);
    EnableWindow(Find(u,ID_FORCE),!busy&&alive);
    int commands[]={
        ID_PAUSE,ID_RESUME,ID_QUERY,ID_POWER,ID_CONSOLE_READ,ID_CONSOLE_SEND
    };
    for(size_t i=0;i<sizeof commands/sizeof commands[0];++i)EnableWindow(Find(u,commands[i]),!busy&&alive&&control);
    EnableWindow(Find(u,ID_REVIEW),!busy&&!alive);
    EnableWindow(Find(u,ID_DISK_CREATE),!busy&&!alive);
    EnableWindow(Find(u,ID_CHECKPOINT),!busy&&!alive);
}
static void Append(Ui*u,const char*bytes,size_t n){
    char safe[8193];
    size_t used=0;
    for(size_t i=0;i<n&&used+5U<sizeof safe;++i){
        unsigned char c=(unsigned char)bytes[i];
        if(c=='\n'){
            safe[used++]='\r';
            safe[used++]='\n';
        }
        else if(c=='\r'||c=='\t'||c>=32){
            safe[used++]=(char)c;
        }
        else{
            int wrote=snprintf(safe+used,sizeof safe-used,"\\x%02x",c);
            if(wrote>0)used+=(size_t)wrote;
        }
    }
    safe[used]=0;
    wchar_t*w=Wide(safe);
    if(!w)return;
    HWND edit=Find(u,ID_CONSOLE);
    int length=GetWindowTextLengthW(edit);
    if(length>100000){
        SendMessageW(edit,EM_SETSEL,0,30000);
        SendMessageW(edit,EM_REPLACESEL,FALSE,(LPARAM)L"");
    }
    SendMessageW(edit,EM_SETSEL,(WPARAM)-1,(LPARAM)-1);
    SendMessageW(edit,EM_REPLACESEL,FALSE,(LPARAM)w);
    free(w);
}
static int ReadProfile(Ui*u,UmiVmProfile*p){
    UmiVmProfileInit(p);
    p->revision=u->revision;
    return Read(u,ID_PROFILE,p->id,sizeof p->id)&&Read(u,ID_NAME,p->name,sizeof p->name)&&Read(u,ID_RUNTIME,p->runtimeDirectory,sizeof p->runtimeDirectory)&&Read(u,ID_IMAGE,p->imageBundle,sizeof p->imageBundle)&&Read(u,ID_DISK,p->diskDirectory,sizeof p->diskDirectory)&&Number(u,ID_MEMORY,&p->memoryMiB)&&Number(u,ID_CPUS,&p->processors)&&((p->architecture=SendMessageW(Find(u,ID_ARCH),CB_GETCURSEL,0,0)==1?UMI_VM_RISCV64:UMI_VM_X86_64),(p->recovery=SendMessageW(Find(u,ID_RECOVERY),BM_GETCHECK,0,0)==BST_CHECKED),1);
}
static void Fill(Ui*u,const UmiVmProfile*p){
    u->setting=1;
    Text(u,ID_PROFILE,p->id);
    Text(u,ID_NAME,p->name);
    Text(u,ID_RUNTIME,p->runtimeDirectory);
    Text(u,ID_IMAGE,p->imageBundle);
    Text(u,ID_DISK,p->diskDirectory);
    char number[32];
    snprintf(number,sizeof number,"%u",p->memoryMiB);
    Text(u,ID_MEMORY,number);
    snprintf(number,sizeof number,"%u",p->processors);
    Text(u,ID_CPUS,number);
    SendMessageW(Find(u,ID_ARCH),CB_SETCURSEL,p->architecture==UMI_VM_RISCV64?1:0,0);
    SendMessageW(Find(u,ID_RECOVERY),BM_SETCHECK,p->recovery?BST_CHECKED:BST_UNCHECKED,0);
    u->revision=p->revision;
    u->setting=0;
    u->fingerprint[0]=0;
}
static DWORD WINAPI Worker(void*context){
    Job*j=context;
    UmiDataServer*db=NULL;
    j->status=UMI_STATUS_OK;
    switch(j->kind){
        case LOAD:case SAVE:j->status=umi_data_server_create_sqlite(j->db,&db);
        if(j->status==UMI_STATUS_OK){
            if(j->kind==LOAD)j->status=UmiVmProfileLoad(db,j->profile.id,&j->profile);
            else j->status=UmiVmProfileSave(db,&j->profile,j->revision,&j->revision);
        }
        umi_data_server_destroy(db);
        break;
        case REVIEW:j->status=UmiVmReview(&j->profile,j->output,&j->report);
        break;
        case START:j->status=UmiVmStart(&j->profile,j->output,j->expected,&j->session,&j->report);
        break;
        case CONTROL:j->status=UmiVmControl(j->session,j->command,j->console,j->consoleLength,j->console,sizeof j->console,&j->consoleLength,&j->report);
        break;
        case RUNTIME_CHECK:j->status=UmiVmRuntimeVerify(j->profile.runtimeDirectory,&j->report);
        break;
        case DISK_CREATE:j->status=UmiVmDiskCreate(j->profile.runtimeDirectory,j->profile.diskDirectory,(uint64_t)j->gib*UINT64_C(1073741824),&j->report);
        break;
        case CHECKPOINT:j->status=UmiVmDiskCheckpoint(j->profile.runtimeDirectory,j->profile.diskDirectory,j->output,&j->report);
        break;
        case FORCE:j->status=UmiVmForceStop(j->session,&j->report);
        break;
    }
    PostMessageW(j->ui->window,VM_DONE,0,0);
    return 0;
}
static void Begin(Ui*u,Kind kind,UmiVmCommand command){
    if(u->job)return;
    Job*j=calloc(1,sizeof *j);
    if(!j){
        Text(u,ID_STATUS,"Not enough memory.");
        return;
    }
    j->ui=u;
    j->kind=kind;
    j->command=command;
    j->revision=u->revision;
    j->session=u->session;
    strcpy(j->expected,u->fingerprint);
    if(kind!=FORCE&&kind!=CONTROL&&(!ReadProfile(u,&j->profile)||!Read(u,ID_DB,j->db,sizeof j->db)||!Read(u,ID_OUTPUT,j->output,sizeof j->output)||!Number(u,ID_GIB,&j->gib)||j->gib<1||j->gib>128)){
        free(j);
        Text(u,ID_STATUS,"Check the field lengths and numeric values.");
        return;
    }
    if(kind==CONTROL&&command==UMI_VM_CONSOLE_WRITE){
        if(!Read(u,ID_CONSOLE_INPUT,j->console,sizeof j->console-2)){
            free(j);
            return;
        }
        j->consoleLength=strlen(j->console);
        j->console[j->consoleLength++]='\n';
    }
    if(kind==START){
        if(u->session){
            UmiVmSnapshot state;
            if(UmiVmObserve(u->session,&state)!=UMI_STATUS_OK||state.processRunning){
                free(j);
                return;
            }
            UmiVmSessionDestroy(u->session);
            u->session=NULL;
        }
        j->session=NULL;
    }
    u->job=j;
    Text(u,ID_STATUS,"Working... No action is repeated automatically after an error.");
    Buttons(u);
    j->thread=CreateThread(NULL,0,Worker,j,0,NULL);
    if(!j->thread){
        u->job=NULL;
        free(j);
        Text(u,ID_STATUS,"Could not start a worker.");
        Buttons(u);
    }
}
static void Complete(Ui*u){
    Job*j=u->job;
    if(!j)return;
    WaitForSingleObject(j->thread,INFINITE);
    CloseHandle(j->thread);
    u->job=NULL;
    if(j->status==UMI_STATUS_OK){
        if(j->kind==LOAD)Fill(u,&j->profile);
        if(j->kind==SAVE){
            u->revision=j->revision;
            u->fingerprint[0]=0;
        }
        if(j->kind==REVIEW)strcpy(u->fingerprint,j->report.fingerprint);
        if(j->kind==START){
            u->session=j->session;
            u->fingerprint[0]=0;
        }
        if(j->kind==CONTROL&&j->command==UMI_VM_CONSOLE_READ)Append(u,j->console,j->consoleLength);
    }
    else u->fingerprint[0]=0;
    if(j->report.detail[0])Text(u,ID_STATUS,j->report.detail);
    else Text(u,ID_STATUS,j->status==UMI_STATUS_OK?j->kind==SAVE?"Profile saved. No virtual machine was started.":"Profile loaded. Review its paths before starting.":UmiSetupStatusText(j->status));
    int failedStop=j->kind==FORCE&&j->status!=UMI_STATUS_OK;
    free(j);
    Buttons(u);
    if(u->closing&&failedStop){
        u->closing=0;
        return;
    }
    if(u->closing){
        if(u->session){
            UmiVmSnapshot state;
            if(UmiVmObserve(u->session,&state)==UMI_STATUS_OK&&state.processRunning){
                Begin(u,FORCE,UMI_VM_QUERY);
                return;
            }
        }
        DestroyWindow(u->window);
    }
}
static void Defaults(Ui*u,int check){
    u->setting=1;
    Text(u,ID_PROFILE,"notes-lab");
    Text(u,ID_NAME,"Umicom OS practice machine");
    Text(u,ID_MEMORY,"1024");
    Text(u,ID_CPUS,"2");
    Text(u,ID_GIB,"8");
    SendMessageW(Find(u,ID_ARCH),CB_ADDSTRING,0,(LPARAM)L"x86-64");
    SendMessageW(Find(u,ID_ARCH),CB_ADDSTRING,0,(LPARAM)L"RISC-V 64");
    SendMessageW(Find(u,ID_ARCH),CB_SETCURSEL,0,0);
    if(!check){
        wchar_t local[MAX_PATH];
        if(SHGetFolderPathW(NULL,CSIDL_LOCAL_APPDATA,NULL,SHGFP_TYPE_CURRENT,local)==S_OK){
            wchar_t path[1024];
            int n=swprintf(path,1024,L"%ls\\Umicom",local);
            if(n>0){
                CreateDirectoryW(path,NULL);
                n=swprintf(path,1024,L"%ls\\Umicom\\vm-profiles.sqlite",local);
                if(n>0)SetWindowTextW(Find(u,ID_DB),path);
            }
        }
        wchar_t exe[1024];
        DWORD n=GetModuleFileNameW(NULL,exe,1024);
        if(n&&n<1024){
            wchar_t*end=wcsrchr(exe,L'\\');
            if(end){
                *end=0;
                wchar_t runtime[2048];
                int m=swprintf(runtime,2048,L"%ls\\..\\share\\umicom\\qemu",exe);
                if(m>0){
                    wchar_t full[2048];
                    DWORD z=GetFullPathNameW(runtime,2048,full,NULL);
                    if(z&&z<2048)SetWindowTextW(Find(u,ID_RUNTIME),full);
                }
            }
        }
    }
    u->setting=0;
    Text(u,ID_STATUS,"Choose a sealed runtime and a verified native image bundle. New machines start paused.");
}
static int Build(Ui*u,int check){
    Font(u);
    HWND brand=Add(u,0,L"STATIC",L"",SS_BITMAP,20,14,165,45);
    u->logo=LoadBitmapW(u->instance,MAKEINTRESOURCEW(102));
    if(u->logo)SendMessageW(brand,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)u->logo);
    Label(u,L"Umicom Virtual Machine Manager",210,18,520);
    Label(u,L"TCG emulation · private QMP channel · no network or shared folders",210,45,700);
    Label(u,L"Profile ID",20,86,150);
    (void)Add(u,ID_PROFILE,L"EDIT",L"",ES_AUTOHSCROLL|WS_TABSTOP,190,82,190,27);
    Label(u,L"Name",400,86,65);
    (void)Add(u,ID_NAME,L"EDIT",L"",ES_AUTOHSCROLL|WS_TABSTOP,470,82,-492,27);
    Entry(u,ID_DB,L"Profile database",116);
    Entry(u,ID_RUNTIME,L"Sealed QEMU runtime",150);
    Entry(u,ID_IMAGE,L"Native image bundle",184);
    Entry(u,ID_DISK,L"Managed disk (optional)",218);
    Entry(u,ID_OUTPUT,L"New run / checkpoint",252);
    Label(u,L"Architecture",20,290,130);
    (void)Add(u,ID_ARCH,L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_TABSTOP,190,286,150,180);
    Label(u,L"Memory MiB",365,290,100);
    (void)Add(u,ID_MEMORY,L"EDIT",L"",ES_NUMBER|WS_TABSTOP,470,286,85,27);
    Label(u,L"CPUs",575,290,50);
    (void)Add(u,ID_CPUS,L"EDIT",L"",ES_NUMBER|WS_TABSTOP,630,286,50,27);
    (void)Add(u,ID_RECOVERY,L"BUTTON",L"Recovery boot",BS_AUTOCHECKBOX|WS_TABSTOP,705,286,160,28);
    Label(u,L"Blank disk size (GiB)",20,324,165);
    (void)Add(u,ID_GIB,L"EDIT",L"",ES_NUMBER|WS_TABSTOP,190,320,70,27);
    Label(u,L"Disk creation uses the Managed disk field; checkpoint uses New run / checkpoint.",280,324,680);
    const int ids[]={
        ID_LOAD,ID_SAVE,ID_REVIEW,ID_START,ID_PAUSE,ID_RESUME,ID_QUERY,ID_POWER,ID_RUNTIME_CHECK,ID_DISK_CREATE,ID_CHECKPOINT,ID_FORCE
    };
    const wchar_t*names[]={
        L"Load profile",L"Save profile",L"Review launch",L"Start paused",L"Pause",L"Resume",L"Refresh state",L"Request powerdown",L"Check runtime",L"Create blank disk",L"Cold checkpoint",L"Force stop"
    };
    for(int i=0;i<12;++i)(void)Add(u,ids[i],L"BUTTON",names[i],BS_PUSHBUTTON|WS_TABSTOP,20+(i%4)*222,358+(i/4)*38,210,30);
    (void)Add(u,ID_CONSOLE_READ,L"BUTTON",L"Read console",BS_PUSHBUTTON|WS_TABSTOP,20,478,155,28);
    (void)Add(u,ID_CONSOLE_INPUT,L"EDIT",L"",ES_AUTOHSCROLL|WS_TABSTOP,190,478,-352,28);
    (void)Add(u,ID_CONSOLE_SEND,L"BUTTON",L"Send to guest",BS_PUSHBUTTON|WS_TABSTOP,-145,478,125,28);
    (void)Add(u,ID_CONSOLE,L"EDIT",L"",ES_MULTILINE|ES_READONLY|ES_AUTOVSCROLL|WS_VSCROLL|WS_TABSTOP,20,518,-40,-582);
    (void)Add(u,ID_STATUS,L"STATIC",L"",SS_LEFT,20,-53,-40,45);
    Defaults(u,check);
    Layout(u);
    Buttons(u);
    return u->count>=42&&Find(u,ID_START)&&Find(u,ID_CONSOLE)&&Find(u,ID_SAVE);
}
static LRESULT CALLBACK Window(HWND window,UINT message,WPARAM w,LPARAM l){
    Ui*u=(Ui*)GetWindowLongPtrW(window,GWLP_USERDATA);
    if(message==WM_NCCREATE){
        CREATESTRUCTW*c=(CREATESTRUCTW*)l;
        u=c->lpCreateParams;
        u->window=window;
        SetWindowLongPtrW(window,GWLP_USERDATA,(LONG_PTR)u);
        return TRUE;
    }
    if(!u)return DefWindowProcW(window,message,w,l);
    switch(message){
        case WM_HSCROLL:Scroll(u,SB_HORZ,LOWORD(w));return 0;
        case WM_VSCROLL:Scroll(u,SB_VERT,LOWORD(w));return 0;
        case WM_MOUSEWHEEL:u->scrollY-=((short)HIWORD(w)/WHEEL_DELTA)*48;Layout(u);return 0;
        case WM_SIZE:Layout(u);
        return 0;
        case WM_DPICHANGED:{
            RECT*r=(RECT*)l;
            SetWindowPos(window,NULL,r->left,r->top,r->right-r->left,r->bottom-r->top,SWP_NOZORDER|SWP_NOACTIVATE);
            Font(u);
            Layout(u);
            return 0;
        }
        case WM_GETMINMAXINFO:{
            MINMAXINFO*m=(MINMAXINFO*)l;
            UINT dpi=GetDpiForWindow(window);
            if(!dpi)dpi=96;
            m->ptMinTrackSize.x=MulDiv(420,(int)dpi,96);
            m->ptMinTrackSize.y=MulDiv(360,(int)dpi,96);
            return 0;
        }
        case VM_DONE:Complete(u);
        return 0;
        case WM_TIMER:if(!u->job&&u->session){
            UmiVmSnapshot s;
            if(UmiVmObserve(u->session,&s)==UMI_STATUS_OK){
                char text[256];
                snprintf(text,sizeof text,"QMP: %.60s | process %s | control %s | version %.40s",s.state,s.processRunning?"running":"exited",s.controlAvailable?"available":"lost",s.qemuVersion);
                wchar_t*title=Wide(text);
                if(title){
                    SetWindowTextW(window,title);
                    free(title);
                }
                Buttons(u);
            }
        }
        return 0;
        case WM_COMMAND:{
            int id=LOWORD(w),notice=HIWORD(w);
            if(!u->setting&&(notice==EN_CHANGE||notice==CBN_SELCHANGE||(id==ID_RECOVERY&&notice==BN_CLICKED))){
                u->fingerprint[0]=0;
                if(id==ID_PROFILE||id==ID_DB)u->revision=0;
                Buttons(u);
            }
            if(notice!=BN_CLICKED||u->job)return 0;
            switch(id){
                case ID_LOAD:Begin(u,LOAD,UMI_VM_QUERY);
                break;
                case ID_SAVE:Begin(u,SAVE,UMI_VM_QUERY);
                break;
                case ID_REVIEW:Begin(u,REVIEW,UMI_VM_QUERY);
                break;
                case ID_START:Begin(u,START,UMI_VM_QUERY);
                break;
                case ID_PAUSE:Begin(u,CONTROL,UMI_VM_PAUSE);
                break;
                case ID_RESUME:Begin(u,CONTROL,UMI_VM_RESUME);
                break;
                case ID_QUERY:Begin(u,CONTROL,UMI_VM_QUERY);
                break;
                case ID_POWER:Begin(u,CONTROL,UMI_VM_POWERDOWN);
                break;
                case ID_CONSOLE_READ:Begin(u,CONTROL,UMI_VM_CONSOLE_READ);
                break;
                case ID_CONSOLE_SEND:Begin(u,CONTROL,UMI_VM_CONSOLE_WRITE);
                break;
                case ID_RUNTIME_CHECK:Begin(u,RUNTIME_CHECK,UMI_VM_QUERY);
                break;
                case ID_DISK_CREATE:Begin(u,DISK_CREATE,UMI_VM_QUERY);
                break;
                case ID_CHECKPOINT:Begin(u,CHECKPOINT,UMI_VM_QUERY);
                break;
                case ID_FORCE:if(MessageBoxW(window,L"Force-stop the owned VM? Unsaved guest work may be lost and its filesystem may be inconsistent.",L"Stop virtual machine",MB_YESNO|MB_ICONWARNING)==IDYES)Begin(u,FORCE,UMI_VM_QUERY);
                break;
            }
            return 0;
        }
        case WM_CLOSE:{
            UmiVmSnapshot s;
            int alive=!u->job&&u->session&&UmiVmObserve(u->session,&s)==UMI_STATUS_OK&&s.processRunning;
            if(u->job||alive){
                if(MessageBoxW(window,L"Closing will wait for current work, then force-stop an active VM. Shut down from inside the guest first to protect its data. Continue?",L"Close VM Manager",MB_YESNO|MB_ICONWARNING)!=IDYES)return 0;
                u->closing=1;
                if(u->job)return 0;
                Begin(u,FORCE,UMI_VM_QUERY);
                return 0;
            }
            DestroyWindow(window);
            return 0;
        }
        case WM_DESTROY:KillTimer(window,1);
        UmiVmSessionDestroy(u->session);
        u->session=NULL;
        if(u->font)DeleteObject(u->font);
        if(u->logo)DeleteObject(u->logo);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window,message,w,l);
}
int UmiVmWin32Run(void*module,int show,int check){
    HINSTANCE instance=module;
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    INITCOMMONCONTROLSEX common={
        sizeof common,ICC_STANDARD_CLASSES
    };
    InitCommonControlsEx(&common);
    HRESULT com=CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);
    Ui*u=calloc(1,sizeof *u);
    if(!u){
        if(SUCCEEDED(com))CoUninitialize();
        return 1;
    }
    u->instance=instance;
    WNDCLASSW wc={
        0
    };
    wc.lpfnWndProc=Window;
    wc.hInstance=instance;
    wc.lpszClassName=L"UmicomVmManager";
    wc.hCursor=LoadCursorW(NULL,IDC_ARROW);
    wc.hIcon=LoadIconW(instance,MAKEINTRESOURCEW(101));
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    if(!RegisterClassW(&wc)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS){
        free(u);
        if(SUCCEEDED(com))CoUninitialize();
        return 1;
    }
    HWND window=CreateWindowExW(0,wc.lpszClassName,L"Umicom Virtual Machine Manager",WS_OVERLAPPEDWINDOW|WS_HSCROLL|WS_VSCROLL,CW_USEDEFAULT,CW_USEDEFAULT,1100,850,NULL,NULL,instance,u);
    int result=1;
    if(window&&Build(u,check)){
        if(check){
            DestroyWindow(window);
            result=0;
        }
        else{
            SetTimer(window,1,750,NULL);
            ShowWindow(window,show);
            MSG msg;
            int code;
            while((code=(int)GetMessageW(&msg,NULL,0,0))>0){
                if(IsDialogMessageW(window,&msg)){
                    if(msg.message==WM_KEYDOWN||msg.message==WM_CHAR)RevealFocus(u);
                }else{
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
            result=code<0?1:0;
        }
    }
    else if(window)DestroyWindow(window);
    free(u);
    if(SUCCEEDED(com))CoUninitialize();
    return result;
}
