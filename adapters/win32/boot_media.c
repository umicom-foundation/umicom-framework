/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/win32/boot_media.c
 * PURPOSE:
 *   A native presentation adapter. All validation, plan identity and byte transfer live in
 *   the shared media service. Closing waits for owned work to stop.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: adapters/win32/boot_media.c
 * A native presentation adapter. All validation, plan identity and byte transfer
 * live in the shared media service. Closing waits for owned work to stop.
 *---------------------------------------------------------------------------*/

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "umicom/boot_media/media.h"
#include "umicom/boot_media/win32.h"
enum {
    ID_IMAGE=100,ID_IMAGE_PICK,ID_DEST,ID_DEST_PICK,ID_INSPECT,ID_REVIEW_COPY,ID_COPY,
    ID_DISCOVER,ID_DEVICES,ID_REVIEW_USB,ID_WRITE_USB,ID_CONFIRM,ID_OUTPUT,ID_PROGRESS,ID_CANCEL,ID_VERIFY
};

#define WM_MEDIA_DONE (WM_APP+421)
#define WM_MEDIA_PROGRESS (WM_APP+422)
typedef struct MediaUi MediaUi;
typedef struct MediaJob {
    MediaUi *owner;
    int kind;
    char image[UMI_BOOT_MEDIA_PATH],destination[UMI_BOOT_MEDIA_PATH],fingerprint[65],confirmation[96];
    UmiBootMediaDevice device;
    UmiBootMediaPlan *plan;
    UmiBootMediaInventory *inventory;
    UmiBootMediaImage observation;
    UmiBootMediaReport report;
    UmiStatus status;
    volatile LONG cancel;
    HANDLE thread;
}

MediaJob;
typedef struct MediaControl {
    HWND handle;
    int x,y,w,h;
}

MediaControl;
struct MediaUi {
    HINSTANCE instance;
    HWND window;
    HFONT font;
    HBITMAP logo;
    unsigned dpi;
    int closing,scrollX,scrollY;
    MediaControl controls[48];
    size_t controlCount;
    MediaJob *job;
    UmiBootMediaPlan *plan;
    UmiBootMediaInventory *inventory;
};
static HWND Control(MediaUi*u,int id) {
    return GetDlgItem(u->window,id);
}

static int Scale(MediaUi*u,int v) {
    return MulDiv(v,(int)u->dpi,96);
}

static HWND Add(MediaUi*u,int id,const wchar_t*type,const wchar_t*text,DWORD style,int x,int y,int w,int h)
{
    HWND c=CreateWindowExW(!wcscmp(type,L"EDIT")?WS_EX_CLIENTEDGE:0,type,text,WS_CHILD|WS_VISIBLE|style,
        Scale(u,x),Scale(u,y),Scale(u,w),Scale(u,h),u->window,(HMENU)(INT_PTR)id,u->instance,NULL);
    if (c&&u->font)SendMessageW(c,WM_SETFONT,(WPARAM)u->font,TRUE);
    if (c&&u->controlCount<48U)u->controls[u->controlCount++]=(MediaControl) {
        c,x,y,w,h
    };
    return c;
}

/* The form has a logical canvas. Scrollbars keep every control reachable
 * on smaller displays; DPI changes rescale from stored logical coordinates. */
static void MediaLayout(MediaUi *u)
{
    RECT client;
    if (!GetClientRect(u->window,&client))return;
    int width=client.right-client.left,height=client.bottom-client.top;
    int maxX=Scale(u,900)-width,maxY=Scale(u,720)-height;
    if (maxX<0)maxX=0;
    if (maxY<0)maxY=0;
    if (u->scrollX>maxX)u->scrollX=maxX;
    if (u->scrollY>maxY)u->scrollY=maxY;
    if (u->scrollX<0)u->scrollX=0;
    if (u->scrollY<0)u->scrollY=0;
    SCROLLINFO x= {
        sizeof x,SIF_RANGE|SIF_PAGE|SIF_POS,0,Scale(u,900)-1,(UINT)(width>0?width:1),u->scrollX,0
    };
    SCROLLINFO y= {
        sizeof y,SIF_RANGE|SIF_PAGE|SIF_POS,0,Scale(u,720)-1,(UINT)(height>0?height:1),u->scrollY,0
    };
    SetScrollInfo(u->window,SB_HORZ,&x,TRUE);
    SetScrollInfo(u->window,SB_VERT,&y,TRUE);
    for (size_t i=0;i<u->controlCount;++i) {
        MediaControl *c=&u->controls[i];
        MoveWindow(c->handle,Scale(u,c->x)-u->scrollX,Scale(u,c->y)-u->scrollY,Scale(u,c->w),Scale(u,c->h),TRUE);
    }
}

static void MediaScroll(MediaUi *u,int bar,int command)
{
    SCROLLINFO i= {
        0
    };
    i.cbSize=sizeof i;
    i.fMask=SIF_ALL;
    if (!GetScrollInfo(u->window,bar,&i))return;
    int *position=bar==SB_HORZ?&u->scrollX:&u->scrollY;
    if (command==SB_LINEUP)*position-=Scale(u,24);
    else if (command==SB_LINEDOWN)*position+=Scale(u,24);
    else if (command==SB_PAGEUP)*position-=(int)i.nPage;
    else if (command==SB_PAGEDOWN)*position+=(int)i.nPage;
    else if (command==SB_THUMBTRACK||command==SB_THUMBPOSITION)*position=i.nTrackPos;
    else if (command==SB_TOP)*position=i.nMin;
    else if (command==SB_BOTTOM)*position=i.nMax;
    MediaLayout(u);
}

static void MediaFont(MediaUi *u)
{
    HFONT next=CreateFontW(-Scale(u,16),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,0,0,0,L"Segoe UI");
    if (!next)return;
    HFONT previous=u->font;
    u->font=next;
    for (size_t i=0;i<u->controlCount;++i)SendMessageW(u->controls[i].handle,WM_SETFONT,(WPARAM)next,TRUE);
    if (previous)DeleteObject(previous);
}

static void Output(MediaUi*u,const char*text)
{
    wchar_t wide[8192];
    if (!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text,-1,wide,8192))wcscpy(wide,L"Message could not be represented.");
    SetWindowTextW(Control(u,ID_OUTPUT),wide);
}

static int Read(MediaUi*u,int id,char*text,size_t cap)
{
    wchar_t wide[4096];
    int n=GetWindowTextLengthW(Control(u,id));
    if (n<=0||n>=4096)return 0;
    GetWindowTextW(Control(u,id),wide,4096);
    return WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide,-1,text,(int)cap,NULL,NULL)>0;
}

static void Buttons(MediaUi*u)
{
    const int inputs[]= {
        ID_IMAGE,ID_IMAGE_PICK,ID_DEST,ID_DEST_PICK,ID_INSPECT,ID_REVIEW_COPY,ID_DISCOVER,ID_DEVICES,ID_CONFIRM,
        ID_VERIFY
    };
    for (size_t i=0;i<sizeof inputs/sizeof inputs[0];++i)EnableWindow(Control(u,inputs[i]),!u->job);
    EnableWindow(Control(u,ID_REVIEW_USB),!u->job&&u->inventory&&u->inventory->count>0);
    int physical=u->plan&&UmiBootMediaPlanConfirmation(u->plan)[0]=='E';
    EnableWindow(Control(u,ID_COPY),!u->job&&u->plan&&!physical);
    EnableWindow(Control(u,ID_WRITE_USB),!u->job&&physical&&UmiBootMediaDeviceWritesEnabled());
    EnableWindow(Control(u,ID_CANCEL),u->job!=NULL);
}

static void Invalidate(MediaUi*u)
{
    if (u->job)return;
    UmiBootMediaPlanDestroy(u->plan);
    u->plan=NULL;
    Buttons(u);
}

static int Progress(const UmiBootMediaReport*r,void*context)
{
    MediaJob*j=context;
    unsigned percent=r->bytesTotal?(unsigned)((long double)r->bytesDone*100.0L/(long double)r->bytesTotal):0U;
    if (percent>100U)percent=100U;
    PostMessageW(j->owner->window,WM_MEDIA_PROGRESS,(WPARAM)percent,0);
    return InterlockedCompareExchange(&j->cancel,0,0)!=0;
}

static DWORD WINAPI Run(LPVOID context)
{
    MediaJob*j=context;
    if (j->kind==1)j->status=UmiBootMediaInspect(j->image,&j->observation,Progress,j,&j->report);
    else if (j->kind==2)j->status=UmiBootMediaReviewFile(j->image,j->destination,&j->plan,Progress,j,&j->report);
    else if (j->kind==3)j->status=UmiBootMediaApply(j->plan,j->fingerprint,j->confirmation,Progress,j,&j->report);
    else if (j->kind==4)j->status=UmiBootMediaDiscover(j->inventory,&j->report);
    else if (j->kind==5)j->status=UmiBootMediaReviewDevice(j->image,&j->device,&j->plan,Progress,j,&j->report);
    else if (j->kind==6)j->status=UmiBootMediaVerifyFile(j->image,j->destination,Progress,j,&j->report);
    else j->status=UMI_STATUS_INVALID_ARGUMENT;
    PostMessageW(j->owner->window,WM_MEDIA_DONE,0,0);
    return 0;
}

static void Start(MediaUi*u,int kind)
{
    if (u->job)return;
    MediaJob*j=calloc(1,sizeof *j);
    if (!j) {
        Output(u,"Not enough memory.");
        return;
    }
    j->owner=u;
    j->kind=kind;
    int valid=kind==4||Read(u,ID_IMAGE,j->image,sizeof j->image);
    if (kind==2||kind==6)valid=valid&&Read(u,ID_DEST,j->destination,sizeof j->destination);
    if (kind==4) {
        j->inventory=calloc(1,sizeof *j->inventory);
        valid=j->inventory!=NULL;
    }
    if (kind==5) {
        LRESULT selected=SendMessageW(Control(u,ID_DEVICES),CB_GETCURSEL,0,0);
        valid=valid&&u->inventory&&selected>=0&&(size_t)selected<u->inventory->count;
        if (valid)j->device=u->inventory->devices[(size_t)selected];
    }
    if (kind==3) {
        valid=valid&&u->plan&&Read(u,ID_CONFIRM,j->confirmation,sizeof j->confirmation);
        if (valid) {
            strcpy(j->fingerprint,UmiBootMediaPlanFingerprint(u->plan));
            if (strcmp(j->confirmation,UmiBootMediaPlanConfirmation(u->plan))) {
                Output(u,"Type the exact confirmation shown by Review.");
                valid=0;
            }
            else if (j->confirmation[0]=='E'&&MessageBoxW(u->window,L"This overwrites the selected USB disk's partitions and files. A cancellation can leave it unbootable. Continue?",
                L"Erase selected device",MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON2)!=IDYES)valid=0;
        }
        if (valid) {
            j->plan=u->plan;
            u->plan=NULL;
        }
    }
    if (!valid) {
        free(j->inventory);
        free(j);
        Buttons(u);
        return;
    }
    if (kind!=3)Invalidate(u);
    u->job=j;
    Buttons(u);
    Output(u,"Working. No physical write happens without an enabled writer and an explicit reviewed confirmation.");
    j->thread=CreateThread(NULL,0,Run,j,0,NULL);
    if (!j->thread) {
        u->job=NULL;
        UmiBootMediaPlanDestroy(j->plan);
        free(j->inventory);
        free(j);
        Buttons(u);
        Output(u,"Could not start the owned worker.");
    }
}

static void Finish(MediaUi*u)
{
    MediaJob*j=u->job;
    if (!j)return;
    WaitForSingleObject(j->thread,INFINITE);
    CloseHandle(j->thread);
    char text[8192];
    (void)snprintf(text,sizeof text,"%s\r\nStatus: %d; payload write started: %s; read-back verified: %s",
        j->report.detail,(int)j->status,j->report.writeStarted?"yes":"no",j->report.verified?"yes":"no");
    if (j->status==UMI_STATUS_OK&&(j->kind==2||j->kind==5)) {
        u->plan=j->plan;
        j->plan=NULL;
        (void)snprintf(text,sizeof text,"%.383s\r\nDestination: %.4095s\r\nImage SHA-256: %.64s\r\nPlan fingerprint: %.64s\r\nType confirmation: %.95s\r\nReview does not write. Structural checks do not prove bootability.",
            j->report.detail,UmiBootMediaPlanDestination(u->plan),UmiBootMediaPlanImage(u->plan)->sha256,UmiBootMediaPlanFingerprint(u->plan),
            UmiBootMediaPlanConfirmation(u->plan));
        SetWindowTextW(Control(u,ID_CONFIRM),L"");
    }
    if (j->kind==4&&j->status==UMI_STATUS_OK) {
        free(u->inventory);
        u->inventory=j->inventory;
        j->inventory=NULL;
        SendMessageW(Control(u,ID_DEVICES),CB_RESETCONTENT,0,0);
        for (size_t i=0;i<u->inventory->count;++i) {
            UmiBootMediaDevice*d=&u->inventory->devices[i];
            char label[1024];
            wchar_t wide[1024];
            (void)snprintf(label,sizeof label,"%.127s | %.127s | %.127s | %llu bytes | %.191s",d->path,d->model,d->serial,
                (unsigned long long)d->bytes,d->reason);
            if (MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,label,-1,wide,1024))SendMessageW(Control(u,ID_DEVICES),
                CB_ADDSTRING,0,(LPARAM)wide);
        }
        SendMessageW(Control(u,ID_DEVICES),CB_SETCURSEL,(WPARAM)-1,0);
    }
    if (j->kind==1&&j->status==UMI_STATUS_OK)(void)snprintf(text,sizeof text,"%s\r\nBytes: %llu\r\nSHA-256: %s\r\nMBR: %d; GPT: %d; ISO9660: %d; El Torito: %d\r\nBIOS catalogue entry: %d; EFI catalogue entry: %d\r\nThese are file observations, not firmware or guest-boot test results.",
        j->report.detail,(unsigned long long)j->observation.bytes,j->observation.sha256,j->observation.mbrLayout,
        j->observation.gptLayout,j->observation.iso9660,j->observation.elTorito,j->observation.biosEntry,j->observation.efiEntry);
    Output(u,text);
    UmiBootMediaPlanDestroy(j->plan);
    free(j->inventory);
    free(j);
    u->job=NULL;
    Buttons(u);
    if (u->closing)DestroyWindow(u->window);
}

static void Pick(MediaUi*u,int destination)
{
    wchar_t path[4096]= {
        0
    };
    OPENFILENAMEW o= {
        0
    };
    o.lStructSize=sizeof o;
    o.hwndOwner=u->window;
    o.lpstrFile=path;
    o.nMaxFile=4096;
    o.lpstrFilter=L"Raw images and ISO files\0*.img;*.iso\0All files\0*.*\0\0";
    o.Flags=OFN_NOCHANGEDIR|OFN_PATHMUSTEXIST|(destination?0:OFN_FILEMUSTEXIST);
    BOOL picked=destination?GetSaveFileNameW(&o):GetOpenFileNameW(&o);
    if (picked)SetWindowTextW(Control(u,destination?ID_DEST:ID_IMAGE),path);
}

static void Build(MediaUi*u)
{
    HDC dc=GetDC(u->window);
    u->dpi=dc?(unsigned)GetDeviceCaps(dc,LOGPIXELSX):96U;
    if (dc)ReleaseDC(u->window,dc);
    MediaFont(u);
    HWND logo=Add(u,-1,L"STATIC",L"",SS_BITMAP,20,12,185,43);
    u->logo=LoadBitmapW(u->instance,MAKEINTRESOURCEW(102));
    if (u->logo)SendMessageW(logo,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)u->logo);
    Add(u,-1,L"STATIC",L"Umicom Boot Media Centre",0,230,15,620,28);
    Add(u,-1,L"STATIC",L"Begin with a new-file practice copy. USB writing is destructive and build-gated.",
        0,20,65,850,28);
    Add(u,-1,L"STATIC",L"Complete raw image or ISO",0,20,102,820,20);
    Add(u,ID_IMAGE,L"EDIT",L"",WS_TABSTOP|ES_AUTOHSCROLL,20,126,715,28);
    Add(u,ID_IMAGE_PICK,L"BUTTON",L"Browse",WS_TABSTOP,745,126,115,28);
    Add(u,ID_INSPECT,L"BUTTON",L"Inspect image",WS_TABSTOP,20,166,180,30);
    Add(u,-1,L"STATIC",L"New practice file (must not already exist)",0,20,207,820,20);
    Add(u,ID_DEST,L"EDIT",L"",WS_TABSTOP|ES_AUTOHSCROLL,20,231,715,28);
    Add(u,ID_DEST_PICK,L"BUTTON",L"Choose",WS_TABSTOP,745,231,115,28);
    Add(u,ID_REVIEW_COPY,L"BUTTON",L"Review copy",WS_TABSTOP,20,271,180,30);
    Add(u,ID_COPY,L"BUTTON",L"Create verified copy",WS_TABSTOP,210,271,230,30);
    Add(u,ID_VERIFY,L"BUTTON",L"Verify file again",WS_TABSTOP,450,271,190,30);
    Add(u,ID_DISCOVER,L"BUTTON",L"Discover USB disks",WS_TABSTOP,20,312,210,30);
    Add(u,ID_DEVICES,L"COMBOBOX",L"",WS_TABSTOP|CBS_DROPDOWNLIST|WS_VSCROLL,20,352,840,200);
    Add(u,ID_REVIEW_USB,L"BUTTON",L"Review selected USB",WS_TABSTOP,20,393,230,30);
    Add(u,ID_WRITE_USB,L"BUTTON",L"Write reviewed USB",WS_TABSTOP,260,393,220,30);
    Add(u,-1,L"STATIC",L"Exact confirmation from the review",0,20,434,820,20);
    Add(u,ID_CONFIRM,L"EDIT",L"",WS_TABSTOP|ES_AUTOHSCROLL,20,458,840,28);
    Add(u,ID_OUTPUT,L"EDIT",L"Choose an image. Optical-disc burning and safe eject are not implemented here.",
        ES_MULTILINE|ES_READONLY|WS_VSCROLL|WS_TABSTOP,20,499,840,160);
    Add(u,ID_PROGRESS,PROGRESS_CLASSW,L"",0,20,672,655,20);
    Add(u,ID_CANCEL,L"BUTTON",L"Cancel",WS_TABSTOP,690,667,170,30);
    Buttons(u);
}

static LRESULT CALLBACK Window(HWND h,UINT message,WPARAM w,LPARAM l)
{
    MediaUi*u=(MediaUi*)GetWindowLongPtrW(h,GWLP_USERDATA);
    if (message==WM_NCCREATE) {
        u=((CREATESTRUCTW*)l)->lpCreateParams;
        u->window=h;
        SetWindowLongPtrW(h,GWLP_USERDATA,(LONG_PTR)u);
    }
    if (!u)return DefWindowProcW(h,message,w,l);
    if (message==WM_CREATE) {
        Build(u);
        for(int id=ID_IMAGE;id<=ID_VERIFY;++id)if(!Control(u,id))return -1;
        MediaLayout(u);
        return 0;
    }
    if (message==WM_SIZE) {
        MediaLayout(u);
        return 0;
    }
    if (message==WM_HSCROLL) {
        MediaScroll(u,SB_HORZ,LOWORD(w));
        return 0;
    }
    if (message==WM_VSCROLL) {
        MediaScroll(u,SB_VERT,LOWORD(w));
        return 0;
    }
    if (message==WM_DPICHANGED) {
        unsigned dpi=HIWORD(w);
        if (dpi<48U||dpi>768U)return 0;
        u->dpi=dpi;
        MediaFont(u);
        const RECT *r=(const RECT *)l;
        SetWindowPos(h,NULL,r->left,r->top,r->right-r->left,r->bottom-r->top,SWP_NOZORDER|SWP_NOACTIVATE);
        MediaLayout(u);
        return 0;
    }
    if (message==WM_GETMINMAXINFO) {
        MINMAXINFO*m=(MINMAXINFO*)l;
        m->ptMinTrackSize.x=480;
        m->ptMinTrackSize.y=360;
        return 0;
    }
    if (message==WM_MEDIA_PROGRESS) {
        SendMessageW(Control(u,ID_PROGRESS),PBM_SETPOS,w,0);
        return 0;
    }
    if (message==WM_MEDIA_DONE) {
        Finish(u);
        return 0;
    }
    if (message==WM_COMMAND) {
        int id=LOWORD(w),notify=HIWORD(w);
        if ((id==ID_IMAGE||id==ID_DEST)&&notify==EN_CHANGE) {
            Invalidate(u);
            return 0;
        }
        if (id==ID_DEVICES&&notify==CBN_SELCHANGE) {
            Invalidate(u);
            return 0;
        }
        if (notify!=BN_CLICKED)return 0;
        if (id==ID_IMAGE_PICK)Pick(u,0);
        else if (id==ID_DEST_PICK)Pick(u,1);
        else if (id==ID_INSPECT)Start(u,1);
        else if (id==ID_REVIEW_COPY)Start(u,2);
        else if (id==ID_COPY||id==ID_WRITE_USB)Start(u,3);
        else if (id==ID_DISCOVER)Start(u,4);
        else if (id==ID_REVIEW_USB)Start(u,5);
        else if (id==ID_VERIFY)Start(u,6);
        else if (id==ID_CANCEL&&u->job)InterlockedExchange(&u->job->cancel,1);
        return 0;
    }
    if (message==WM_CLOSE) {
        if (u->job) {
            u->closing=1;
            InterlockedExchange(&u->job->cancel,1);
            return 0;
        }
        DestroyWindow(h);
        return 0;
    }
    if (message==WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(h,message,w,l);
}

int UmiBootMediaWindowsMain(void*instance,int show,int checkOnly)
{

    /* The embedded manifest requests per-monitor awareness; an older host may
     * fall back to system scaling without changing transfer semantics. */
    MediaUi*u=calloc(1,sizeof *u);
    if (!u)return 1;
    u->instance=(HINSTANCE)instance;
    u->dpi=96;
    INITCOMMONCONTROLSEX controls= {
        sizeof controls,ICC_STANDARD_CLASSES|ICC_PROGRESS_CLASS
    };
    if (!InitCommonControlsEx(&controls)) {
        free(u);
        return 1;
    }
    WNDCLASSW c= {
        0
    };
    c.lpfnWndProc=Window;
    c.hInstance=u->instance;
    c.lpszClassName=L"UmicomBootMediaCentre";
    c.hCursor=LoadCursorW(NULL,IDC_ARROW);
    c.hIcon=LoadIconW(u->instance,MAKEINTRESOURCEW(101));
    c.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    if (!RegisterClassW(&c)) {
        free(u);
        return 1;
    }
    HWND h=CreateWindowW(c.lpszClassName,L"Umicom Boot Media Centre",WS_OVERLAPPEDWINDOW|WS_HSCROLL|WS_VSCROLL,
        CW_USEDEFAULT,CW_USEDEFAULT,930,810,NULL,NULL,u->instance,u);
    int result=1;
    if (h) {
        if (checkOnly) {
            result=Control(u,ID_IMAGE)&&Control(u,ID_WRITE_USB)&&Control(u,ID_COPY)&&!IsWindowEnabled(Control(u,ID_WRITE_USB))?0:1;
            SetWindowPos(h,NULL,0,0,520,420,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
            MediaScroll(u,SB_VERT,SB_BOTTOM);
            if (u->scrollY<=0)result=1;
            DestroyWindow(h);
        }
        else {
            ShowWindow(h,show);
            MSG m;
            BOOL next;
            while ((next=GetMessageW(&m,NULL,0,0))>0) {
                if (!IsDialogMessageW(h,&m)) {
                    TranslateMessage(&m);
                    DispatchMessageW(&m);
                }
            }
            result=next==0?0:1;
        }
    }
    UmiBootMediaPlanDestroy(u->plan);
    free(u->inventory);
    if (u->font)DeleteObject(u->font);
    if (u->logo)DeleteObject(u->logo);
    free(u);
    return result;
}
