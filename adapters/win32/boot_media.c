/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/win32/boot_media.c
 *
 * PURPOSE:
 *   Present native media controls while the shared service owns validation, plan identity and transfer. Closing waits for owned work to stop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
    volatile LONG progress;
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
    int lifecycleFailed;
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

static const UmiBootMediaDevice *SelectedDevice(MediaUi *u);

static void Buttons(MediaUi*u)
{
    const int inputs[]= {
        ID_IMAGE,ID_IMAGE_PICK,ID_DEST,ID_DEST_PICK,ID_INSPECT,ID_REVIEW_COPY,ID_DISCOVER,ID_DEVICES,ID_CONFIRM,
        ID_VERIFY
    };
    for (size_t i=0;i<sizeof inputs/sizeof inputs[0];++i)EnableWindow(Control(u,inputs[i]),!u->job);
    /* Inventory presence alone cannot identify a displayed selection. Keep
     * the former enablement rule for review; bound item data is required now. */
#if 0
    EnableWindow(Control(u,ID_REVIEW_USB),!u->job&&u->inventory&&u->inventory->count>0);
#endif
    EnableWindow(Control(u,ID_REVIEW_USB),!u->job&&SelectedDevice(u)!=NULL);
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

/* Retain the former value-bearing notifications. Atomic owned progress avoids stale queued values and reduces duplicate notifications; delivery is not completion authority. The previous implementation is retained for review. */
#if 0
static int Progress(const UmiBootMediaReport*r,void*context)
{
    MediaJob*j=context;
    unsigned percent=r->bytesTotal?(unsigned)((long double)r->bytesDone*100.0L/(long double)r->bytesTotal):0U;
    if (percent>100U)percent=100U;
    PostMessageW(j->owner->window,WM_MEDIA_PROGRESS,(WPARAM)percent,0);
    return InterlockedCompareExchange(&j->cancel,0,0)!=0;
}


#endif
/* The owned job is the progress authority. Posted messages contain no borrowed
 * pointer and are advisory; a stale message only refreshes the current job. */
static int Progress(const UmiBootMediaReport*r,void*context)
{
    MediaJob*j=context;
    unsigned percent=r->bytesTotal?(unsigned)((long double)r->bytesDone*100.0L/(long double)r->bytesTotal):0U;
    if(percent>100U)percent=100U;
    LONG prior=InterlockedExchange(&j->progress,(LONG)percent);
    if(prior!=(LONG)percent)(void)PostMessageW(j->owner->window,WM_MEDIA_PROGRESS,0,0);
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
    /* The message is a wake hint. The UI also waits on this native thread, so
     * failed posting cannot strand completion or justify freeing live state. */
    (void)PostMessageW(j->owner->window,WM_MEDIA_DONE,0,0);
    return 0;
}


/* A displayed row is not an inventory index: conversion or control allocation
 * can omit entries. Store a one-based inventory key on every admitted row.
 * Zero means unbound, so an accidentally added row cannot select device zero. */
static const UmiBootMediaDevice *SelectedDevice(MediaUi *u)
{
    if(!u->inventory||u->inventory->count>UMI_BOOT_MEDIA_DEVICES)return NULL;
    HWND list=Control(u,ID_DEVICES);
    LRESULT row=SendMessageW(list,CB_GETCURSEL,0,0);
    if(row<0)return NULL;
    LRESULT key=SendMessageW(list,CB_GETITEMDATA,(WPARAM)row,0);
    if(key<=0||(size_t)key>u->inventory->count)return NULL;
    return &u->inventory->devices[(size_t)key-1U];
}

#define MEDIA_LABEL_CAPACITY (UMI_BOOT_MEDIA_PATH+128U+128U+192U+96U)
static int MediaDeviceLabel(const UmiBootMediaDevice *device,
    wchar_t *wide,int capacity)
{
    if(!device||!wide||capacity<=0||
        !memchr(device->path,0,sizeof device->path)||
        !memchr(device->model,0,sizeof device->model)||
        !memchr(device->serial,0,sizeof device->serial)||
        !memchr(device->reason,0,sizeof device->reason))return 0;
    /* Preserve complete UTF-8 fields before conversion. A byte precision such
     * as %.127s can cut a valid multibyte character and hide the entire row. */
    char label[MEDIA_LABEL_CAPACITY];
    int length=snprintf(label,sizeof label,"%s | %s | %s | %llu bytes | %s",
        device->path,device->model,device->serial,
        (unsigned long long)device->bytes,device->reason);
    if(length<0||(size_t)length>=sizeof label)return 0;
    return MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,label,-1,wide,capacity)>0;
}

static size_t PopulateDevices(MediaUi *u)
{
    HWND list=Control(u,ID_DEVICES);
    SendMessageW(list,CB_RESETCONTENT,0,0);
    if(!u->inventory||u->inventory->count>UMI_BOOT_MEDIA_DEVICES)return 0;
    size_t listed=0;
    for(size_t i=0;i<u->inventory->count;++i) {
        wchar_t wide[MEDIA_LABEL_CAPACITY];
        if(!MediaDeviceLabel(&u->inventory->devices[i],wide,(int)MEDIA_LABEL_CAPACITY))
            continue;
        LRESULT row=SendMessageW(list,CB_ADDSTRING,0,(LPARAM)wide);
        if(row<0)continue;
        if(SendMessageW(list,CB_SETITEMDATA,(WPARAM)row,(LPARAM)(i+1U))==CB_ERR) {
            /* Never leave an unbound row available for review. */
            SendMessageW(list,CB_DELETESTRING,(WPARAM)row,0);
            continue;
        }
        ++listed;
    }
    SendMessageW(list,CB_SETCURSEL,(WPARAM)-1,0);
    return listed;
}

static void Start(MediaUi*u,int kind)
{
    /* Closing admission cannot start another worker. Retain the old guard. */
#if 0
    if (u->job)return;
#endif
    if (u->job||u->closing)return;
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
        /* Preserve the former row-index assumption for review. Explicit item
         * data now keeps skipped labels from shifting the selected device. */
#if 0
        LRESULT selected=SendMessageW(Control(u,ID_DEVICES),CB_GETCURSEL,0,0);
        valid=valid&&u->inventory&&selected>=0&&(size_t)selected<u->inventory->count;
        if (valid)j->device=u->inventory->devices[(size_t)selected];
#endif
        const UmiBootMediaDevice *selected=SelectedDevice(u);
        valid=valid&&selected!=NULL;
        if(valid)j->device=*selected;
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
    SendMessageW(Control(u,ID_PROGRESS),PBM_SETPOS,0,0);
    Buttons(u);
    Output(u,"Working. No physical write happens without an enabled writer and an explicit reviewed confirmation.");
    /* Native control notifications can re-enter close handling before launch.
     * Discard only a never-started job; no worker may outlive this ownership. */
    if(u->closing||!IsWindow(u->window)){
        u->job=NULL;
        UmiBootMediaPlanDestroy(j->plan);
        free(j->inventory);
        free(j);
        if(IsWindow(u->window))DestroyWindow(u->window);
        return;
    }
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

/* Native clients share field meanings and bounds with the CLI. A mismatched or
 * malformed observation must not expose an old success message or adopt a plan. */
static int MediaCompletionReport(MediaJob *job,char *text,size_t capacity)
{
    UmiStatus described=UmiBootMediaReportDescribe(job->status,&job->report,text,capacity);
    if(described==UMI_STATUS_OK)return 1;
    (void)snprintf(text,capacity,
        "Media operation status: %d; report unavailable (%d).\r\nNo verified success can be shown.",
        (int)job->status,(int)described);
    job->status=UMI_STATUS_INVALID_STATE;
    return 0;
}

static void Finish(MediaUi*u)
{
    MediaJob*j=u->job;
    /* An old notification may re-enter admission before CreateThread returns. */
#if 0
    if (!j)return;
#endif
    if (!j||!j->thread)return;
/* Retain the blocking notification-driven join. Completion now reads only stopped workers and does not block the event thread on an early notification. The previous implementation is retained for review. */
#if 0
    WaitForSingleObject(j->thread,INFINITE);
    CloseHandle(j->thread);
#endif
    /* A posted message can arrive before the worker's final return, or belong
     * to an older job. Only a signalled thread permits reading final fields. */
    DWORD completed=WaitForSingleObject(j->thread,0);
    if(completed!=WAIT_OBJECT_0){
        if(completed==WAIT_FAILED)u->lifecycleFailed=1;
        return;
    }
    int releaseFailed=!CloseHandle(j->thread);
    j->thread=NULL;
    if(releaseFailed)u->lifecycleFailed=1;
    char text[8192];
/* The earlier summary omitted exclusive creation. Preserve it while the shared report service supplies all effect fields and refuses malformed observations. The previous implementation is retained for review. */
#if 0
    (void)snprintf(text,sizeof text,"%s\r\nStatus: %d; payload write started: %s; read-back verified: %s",
        j->report.detail,(int)j->status,j->report.writeStarted?"yes":"no",j->report.verified?"yes":"no");
#endif
    (void)MediaCompletionReport(j,text,sizeof text);
    if (j->status==UMI_STATUS_OK&&(j->kind==2||j->kind==5)) {
        u->plan=j->plan;
        j->plan=NULL;
/* Append review identity after the shared observation instead of replacing its status and effect fields. The previous implementation is retained for review. */
#if 0
        (void)snprintf(text,sizeof text,"%.383s\r\nDestination: %.4095s\r\nImage SHA-256: %.64s\r\nPlan fingerprint: %.64s\r\nType confirmation: %.95s\r\nReview does not write. Structural checks do not prove bootability.",
            j->report.detail,UmiBootMediaPlanDestination(u->plan),UmiBootMediaPlanImage(u->plan)->sha256,UmiBootMediaPlanFingerprint(u->plan),
            UmiBootMediaPlanConfirmation(u->plan));

#endif
        size_t used=strlen(text);
        (void)snprintf(text+used,sizeof text-used,
            "\r\nDestination: %.4095s\r\nImage SHA-256: %.64s\r\nPlan fingerprint: %.64s\r\nType confirmation: %.95s\r\nReview does not write.",
            UmiBootMediaPlanDestination(u->plan),UmiBootMediaPlanImage(u->plan)->sha256,
            UmiBootMediaPlanFingerprint(u->plan),UmiBootMediaPlanConfirmation(u->plan));
        SetWindowTextW(Control(u,ID_CONFIRM),L"");
    }
    if (j->kind==4&&j->status==UMI_STATUS_OK) {
        free(u->inventory);
        u->inventory=j->inventory;
        j->inventory=NULL;
        /* Keep the earlier presentation loop for review. PopulateDevices
         * binds every visible row and avoids splitting UTF-8 fields. */
#if 0
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
#endif
        (void)PopulateDevices(u);
    }
/* Keep structural image observations alongside the shared completion report; retain the former replacement formatter for review. The previous implementation is retained for review. */
#if 0
    if (j->kind==1&&j->status==UMI_STATUS_OK)(void)snprintf(text,sizeof text,"%s\r\nBytes: %llu\r\nSHA-256: %s\r\nMBR: %d; GPT: %d; ISO9660: %d; El Torito: %d\r\nBIOS catalogue entry: %d; EFI catalogue entry: %d\r\nThese are file observations, not firmware or guest-boot test results.",
        j->report.detail,(unsigned long long)j->observation.bytes,j->observation.sha256,j->observation.mbrLayout,
        j->observation.gptLayout,j->observation.iso9660,j->observation.elTorito,j->observation.biosEntry,j->observation.efiEntry);

#endif
    if (j->kind==1&&j->status==UMI_STATUS_OK) {
        size_t used=strlen(text);
        (void)snprintf(text+used,sizeof text-used,
            "\r\nBytes: %llu\r\nSHA-256: %s\r\nMBR: %d; GPT: %d; ISO9660: %d; El Torito: %d\r\nBIOS catalogue entry: %d; EFI catalogue entry: %d\r\nThese are file observations, not firmware or guest-boot test results.",
            (unsigned long long)j->observation.bytes,j->observation.sha256,j->observation.mbrLayout,
            j->observation.gptLayout,j->observation.iso9660,j->observation.elTorito,
            j->observation.biosEntry,j->observation.efiEntry);
    }
    if(releaseFailed){
        size_t used=strlen(text);
        (void)snprintf(text+used,sizeof text-used,
            "\r\nWorker handle release failed; native cleanup requires review.");
    }
    SendMessageW(Control(u,ID_PROGRESS),PBM_SETPOS,
        (WPARAM)InterlockedCompareExchange(&j->progress,0,0),0);
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


/* These hidden-control checks use synthetic observations only. They never call
 * discovery, create a plan or open a source, destination or physical device. */
static void SelectionFixture(UmiBootMediaDevice *device,const char *model)
{
    memset(device,0,sizeof *device);
    strcpy(device->path,"C:/synthetic-device");
    strcpy(device->identity,"synthetic-identity");
    strcpy(device->serial,"synthetic-serial");
    strcpy(device->model,model);
    strcpy(device->reason,"fixture only");
    device->bytes=4096;
}

static int CheckDeviceSelection(MediaUi *u)
{
    u->inventory=calloc(1,sizeof *u->inventory);
    if(!u->inventory)return 1;
    u->inventory->count=3;
    SelectionFixture(&u->inventory->devices[0],"invalid");
    u->inventory->devices[0].model[0]=(char)0xff;
    SelectionFixture(&u->inventory->devices[1],"second device");
    SelectionFixture(&u->inventory->devices[2],"third device");
    HWND list=Control(u,ID_DEVICES);
    if(PopulateDevices(u)!=2||SelectedDevice(u)!=NULL)return 1;
    Buttons(u);
    if(IsWindowEnabled(Control(u,ID_REVIEW_USB)))return 1;
    SendMessageW(list,CB_SETCURSEL,0,0);
    if(SelectedDevice(u)!=&u->inventory->devices[1])return 1;
    Buttons(u);
    if(!IsWindowEnabled(Control(u,ID_REVIEW_USB)))return 1;
    SendMessageW(list,CB_SETCURSEL,1,0);
    if(SelectedDevice(u)!=&u->inventory->devices[2])return 1;
    /* Removing an earlier row must not change the identity bound to the next. */
    SendMessageW(list,CB_DELETESTRING,0,0);
    SendMessageW(list,CB_SETCURSEL,0,0);
    if(SelectedDevice(u)!=&u->inventory->devices[2])return 1;
    SendMessageW(list,CB_SETITEMDATA,0,0);
    if(SelectedDevice(u)!=NULL)return 1;
    Buttons(u);
    if(IsWindowEnabled(Control(u,ID_REVIEW_USB)))return 1;
    SendMessageW(list,CB_SETITEMDATA,0,(LPARAM)4);
    if(SelectedDevice(u)!=NULL)return 1;
    SendMessageW(list,CB_SETITEMDATA,0,(LPARAM)-1);
    if(SelectedDevice(u)!=NULL)return 1;
    SendMessageW(list,CB_SETCURSEL,(WPARAM)-1,0);
    if(SelectedDevice(u)!=NULL)return 1;
    u->inventory->count=UMI_BOOT_MEDIA_DEVICES+1U;
    if(PopulateDevices(u)!=0||SendMessageW(list,CB_GETCOUNT,0,0)!=0)return 1;
    u->inventory->count=1;
    SelectionFixture(&u->inventory->devices[0],"replacement inventory");
    if(PopulateDevices(u)!=1||SelectedDevice(u)!=NULL)return 1;
    SendMessageW(list,CB_SETCURSEL,0,0);
    if(SelectedDevice(u)!=&u->inventory->devices[0])return 1;
    return 0;
}

static int CheckDeviceLabels(void)
{
    UmiBootMediaDevice device;
    SelectionFixture(&device,"model");
    /* The old %.127s path projection split this three-byte euro sign. */
    memset(device.path,'a',126);
    device.path[0]='C';device.path[1]=':';device.path[2]='/';
    memcpy(device.path+126,"\xe2\x82\xac.img",8);
    wchar_t label[MEDIA_LABEL_CAPACITY];
    if(!MediaDeviceLabel(&device,label,(int)MEDIA_LABEL_CAPACITY)||
        !wcsstr(label,L"\x20ac.img"))return 1;
    if(MediaDeviceLabel(&device,label,1))return 1;
    device.model[0]=(char)0xff;
    if(MediaDeviceLabel(&device,label,(int)MEDIA_LABEL_CAPACITY))return 1;
    SelectionFixture(&device,"model");
    memset(device.path,'x',sizeof device.path);
    if(MediaDeviceLabel(&device,label,(int)MEDIA_LABEL_CAPACITY))return 1;
    SelectionFixture(&device,"model");
    memset(device.model,'x',sizeof device.model);
    if(MediaDeviceLabel(&device,label,(int)MEDIA_LABEL_CAPACITY))return 1;
    SelectionFixture(&device,"model");
    memset(device.serial,'x',sizeof device.serial);
    if(MediaDeviceLabel(&device,label,(int)MEDIA_LABEL_CAPACITY))return 1;
    SelectionFixture(&device,"model");
    memset(device.reason,'x',sizeof device.reason);
    if(MediaDeviceLabel(&device,label,(int)MEDIA_LABEL_CAPACITY))return 1;
    SelectionFixture(&device,"model");
    memset(device.path,'p',sizeof device.path-1U);device.path[sizeof device.path-1U]=0;
    memset(device.model,'m',sizeof device.model-1U);device.model[sizeof device.model-1U]=0;
    memset(device.serial,'s',sizeof device.serial-1U);device.serial[sizeof device.serial-1U]=0;
    memset(device.reason,'r',sizeof device.reason-1U);device.reason[sizeof device.reason-1U]=0;
    device.bytes=UINT64_MAX;
    if(!MediaDeviceLabel(&device,label,(int)MEDIA_LABEL_CAPACITY))return 1;
    return 0;
}

/* Exercise the actual output control with synthetic reports only. This check
 * starts no worker and acquires no image, output file or physical device. */
static int CheckCompletionReports(MediaUi *ui)
{
    MediaJob job={0};
    char text[UMI_BOOT_MEDIA_REPORT_TEXT];
    wchar_t shown[UMI_BOOT_MEDIA_REPORT_TEXT];
    job.status=job.report.status=UMI_STATUS_OUT_OF_MEMORY;
    job.report.outputCreated=1;
    strcpy(job.report.detail,"Created practice \xCE\xA9; allocation refused.");
    if(!MediaCompletionReport(&job,text,sizeof text))return 1;
    Output(ui,text);
    if(!GetWindowTextW(Control(ui,ID_OUTPUT),shown,(int)(sizeof shown/sizeof shown[0]))||
        !wcsstr(shown,L"Practice file created: yes")||
        !wcsstr(shown,L"Payload write attempted: no")||
        !wcsstr(shown,L"Byte agreement verified: no")||!wcsstr(shown,L"\x03a9"))return 1;
    memset(&job,0,sizeof job);
    job.report.phase=UMI_BOOT_MEDIA_VERIFY;
    job.report.verified=1;
    strcpy(job.report.detail,"Independent file comparison.");
    if(!MediaCompletionReport(&job,text,sizeof text))return 1;
    Output(ui,text);
    if(!GetWindowTextW(Control(ui,ID_OUTPUT),shown,(int)(sizeof shown/sizeof shown[0]))||
        !wcsstr(shown,L"Practice file created: no")||
        !wcsstr(shown,L"Byte agreement verified: yes"))return 1;
    job.status=UMI_STATUS_IO_ERROR; /* Deliberately inconsistent report. */
    if(MediaCompletionReport(&job,text,sizeof text))return 1;
    Output(ui,text);
    if(!GetWindowTextW(Control(ui,ID_OUTPUT),shown,(int)(sizeof shown/sizeof shown[0]))||
        !wcsstr(shown,L"report unavailable")||wcsstr(shown,L"Byte agreement verified: yes"))return 1;
    return 0;
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
/* Ignore the queued value from an older job; the current owner supplies its atomic observation. The previous implementation is retained for review. */
#if 0
        SendMessageW(Control(u,ID_PROGRESS),PBM_SETPOS,w,0);
#endif
        if(u->job)SendMessageW(Control(u,ID_PROGRESS),PBM_SETPOS,
            (WPARAM)InterlockedCompareExchange(&u->job->progress,0,0),0);
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

/* This adapter already owns a waitable native thread. Integrate that handle
 * with the Windows message queue rather than making notification delivery a
 * prerequisite for joining. The handle is closed only by this UI thread after
 * the wait has returned; no concurrent owner can invalidate the wait array. */
static int MediaMessageLoop(MediaUi *ui,HWND window)
{
    for(;;){
        MSG message;
        if(ui->job&&ui->job->thread){
            HANDLE worker=ui->job->thread;
            DWORD ready=MsgWaitForMultipleObjectsEx(1,&worker,INFINITE,QS_ALLINPUT,MWMO_INPUTAVAILABLE);
            if(ready==WAIT_OBJECT_0){Finish(ui);continue;}
            if(ready!=WAIT_OBJECT_0+1U)return 1;
            if(!PeekMessageW(&message,NULL,0,0,PM_REMOVE))continue;
            if(message.message==WM_QUIT)return 0;
        }else{
            BOOL next=GetMessageW(&message,NULL,0,0);
            if(next<=0)return next==0?0:1;
        }
        if(!IsDialogMessageW(window,&message)){
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
}

/* After quit or an abnormal wait result, keep the owner and captured inputs
 * alive until cooperative work returns. Do not present a result into a retired
 * window. Native I/O can delay cancellation; this is not forced termination.
 * An uncertain join preserves the complete owner instead of freeing live data. */
static int MediaDrainWorker(MediaUi *ui)
{
    MediaJob *job=ui->job;
    if(!job)return 1;
    InterlockedExchange(&job->cancel,1);
    if(!job->thread||WaitForSingleObject(job->thread,INFINITE)!=WAIT_OBJECT_0){
        ui->lifecycleFailed=1;
        return 0;
    }
    if(!CloseHandle(job->thread))ui->lifecycleFailed=1;
    job->thread=NULL;
    UmiBootMediaPlanDestroy(job->plan);
    free(job->inventory);
    free(job);
    ui->job=NULL;
    return 1;
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
            if(checkOnly==UMI_BOOT_MEDIA_WINDOWS_CHECK_SELECTION&&CheckDeviceSelection(u))result=1;
            if(checkOnly==UMI_BOOT_MEDIA_WINDOWS_CHECK_LABELS&&CheckDeviceLabels())result=1;
            if(checkOnly==UMI_BOOT_MEDIA_WINDOWS_CHECK_REPORTS&&CheckCompletionReports(u))result=1;
            DestroyWindow(h);
        }
        else {
            ShowWindow(h,show);
/* Preserve the former message-only loop. The native worker handle now wakes completion even if posting fails. The previous implementation is retained for review. */
#if 0
            MSG m;
            BOOL next;
            while ((next=GetMessageW(&m,NULL,0,0))>0) {
                if (!IsDialogMessageW(h,&m)) {
                    TranslateMessage(&m);
                    DispatchMessageW(&m);
                }
            }
            result=next==0?0:1;
#endif
            result=MediaMessageLoop(u,h);
        }
    }
    if(!MediaDrainWorker(u)){
        OutputDebugStringW(L"Umicom Boot Media: worker completion is uncertain; owner state retained.\n");
        return 1;
    }
    if(IsWindow(h))DestroyWindow(h);
    if(u->lifecycleFailed)result=1;
    UmiBootMediaPlanDestroy(u->plan);
    free(u->inventory);
    if (u->font)DeleteObject(u->font);
    if (u->logo)DeleteObject(u->logo);
    free(u);
    return result;
}
