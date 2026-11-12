/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/boot_media/test_worker_windows.c
 *
 * PURPOSE:
 *   Exercise native media worker ownership with synthetic work, lost notifications and abnormal message-loop exits.
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
#include "../support/assertions.h"
#include "umicom/boot_media/media.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
static const char *mode;
static HANDLE began,releaseWork,observedThread,retainedThread;
static void *observedJob;
static unsigned jobFrees,threadStarts;
static volatile LONG workerCalls;
static int failWait,failDrain,failIdle,failStart,closeAdmission;
static UmiStatus SyntheticInspect(const char *path,UmiBootMediaImage *image,
    UmiBootMediaProgress callback,void *context,UmiBootMediaReport *report)
{
    (void)path;
    InterlockedIncrement(&workerCalls);
    memset(report,0,sizeof *report);memset(image,0,sizeof *image);
    report->phase=UMI_BOOT_MEDIA_INSPECT;report->bytesDone=37;report->bytesTotal=100;
    int cancelled=callback&&callback(report,context);
    SetEvent(began);
    while(!cancelled&&WaitForSingleObject(releaseWork,10)==WAIT_TIMEOUT) {
        /* Freeze progress for the stale-value case so the test controls one
         * atomic observation without racing the synthetic producer. */
        if(strcmp(mode,"stale-progress"))cancelled=callback&&callback(report,context);
    }
    report->status=cancelled?UMI_STATUS_CANCELLED:UMI_STATUS_OK;
    strcpy(report->detail,cancelled?"Synthetic worker cancelled.":"Synthetic worker completed.");
    image->bytes=512;image->mbrLayout=1;image->partitions=1;
    memset(image->sha256,'c',64);image->sha256[64]=0;
    return report->status;
}
static BOOL ObservePost(HWND window,UINT message,WPARAM w,LPARAM l)
{
    if(!strcmp(mode,"lost-notification")&&(message==WM_APP+421||message==WM_APP+422)){
        SetLastError(ERROR_NOT_ENOUGH_QUOTA);return FALSE;
    }
    return PostMessageW(window,message,w,l);
}
static DWORD ObserveMessageWait(DWORD count,const HANDLE *handles,DWORD timeout,DWORD mask,DWORD flags)
{
    (void)timeout;
    if(failWait){failWait=0;SetLastError(ERROR_INVALID_HANDLE);return WAIT_FAILED;}
    return MsgWaitForMultipleObjectsEx(count,handles,2000,mask,flags);
}
static DWORD ObserveSingleWait(HANDLE handle,DWORD timeout)
{
    if(failDrain&&handle==observedThread&&timeout==INFINITE){
        failDrain=0;SetLastError(ERROR_INVALID_HANDLE);return WAIT_FAILED;
    }
    return WaitForSingleObject(handle,timeout);
}
static BOOL ObserveClose(HANDLE handle)
{
    BOOL closed=CloseHandle(handle);
    if(!strcmp(mode,"release-failure")&&handle==observedThread){
        assert(closed);SetLastError(ERROR_INVALID_HANDLE);return FALSE;
    }
    return closed;
}
static BOOL ObserveGetMessage(LPMSG message,HWND window,UINT low,UINT high)
{
    if(failIdle){failIdle=0;SetLastError(ERROR_INVALID_WINDOW_HANDLE);return -1;}
    return GetMessageW(message,window,low,high);
}
static HANDLE ObserveCreate(LPSECURITY_ATTRIBUTES attributes,SIZE_T stack,
    LPTHREAD_START_ROUTINE function,LPVOID context,DWORD flags,LPDWORD identity)
{
    ++threadStarts;
    if(failStart){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return NULL;}
    return CreateThread(attributes,stack,function,context,flags,identity);
}
static BOOL ObserveText(HWND window,LPCWSTR text)
{
    BOOL result=SetWindowTextW(window,text);
    if(closeAdmission&&text&&wcsncmp(text,L"Working.",8)==0){
        closeAdmission=0;SendMessageW(GetParent(window),WM_CLOSE,0,0);
    }
    return result;
}
static void ObserveFree(void *pointer)
{
    if(pointer&&pointer==observedJob){
        assert(retainedThread&&WaitForSingleObject(retainedThread,0)==WAIT_OBJECT_0);
        ++jobFrees;
    }
    free(pointer);
}
/* Native window controls and real threads are retained. Only the image service
 * and selected notification/error outcomes are seams; no file or device opens. */
#define UmiBootMediaInspect SyntheticInspect
#define PostMessageW ObservePost
#define MsgWaitForMultipleObjectsEx ObserveMessageWait
#define WaitForSingleObject ObserveSingleWait
#define CloseHandle ObserveClose
#define GetMessageW ObserveGetMessage
#define CreateThread ObserveCreate
#define SetWindowTextW ObserveText
#define free ObserveFree
#include "../../adapters/win32/boot_media.c"
#undef free
#undef SetWindowTextW
#undef CreateThread
#undef GetMessageW
#undef CloseHandle
#undef WaitForSingleObject
#undef MsgWaitForMultipleObjectsEx
#undef PostMessageW
#undef UmiBootMediaInspect

int main(int argc,char **argv)
{
    /* Argument admission must complete before reading argv[1]. Keep the
     * compressed former spelling for review; explicit control flow also lets
     * strict compilers diagnose future misleading fixture indentation. */
#if 0
    if(argc!=2)return 2;mode=argv[1];
#endif
    if(argc!=2) {
        return 2;
    }
    mode=argv[1];
    const char *cases[]={"completion", "lost-notification", "premature-notification", "stale-progress", "close-running", "quit-running", "window-destroyed", "wait-failure", "idle-loop-error", "drain-failure", "start-failure", "admission-close", "release-failure"};
    int known=0;
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i)if(!strcmp(mode,cases[i]))known=1;
    if(!known)return 2;
    INITCOMMONCONTROLSEX controls={sizeof controls,ICC_STANDARD_CLASSES|ICC_PROGRESS_CLASS};
    assert(InitCommonControlsEx(&controls));
    MediaUi ui={0};ui.instance=GetModuleHandleW(NULL);ui.dpi=96;
    WNDCLASSW type={0};type.lpfnWndProc=Window;type.hInstance=ui.instance;
    type.lpszClassName=L"UmicomMediaWorkerQualification";
    assert(RegisterClassW(&type));
    HWND window=CreateWindowW(type.lpszClassName,L"Umicom Media Worker Qualification",
        WS_OVERLAPPEDWINDOW,0,0,930,810,NULL,NULL,ui.instance,&ui);
    assert(window!=NULL); /* Hidden: ShowWindow is never called. */
    began=CreateEventW(NULL,TRUE,FALSE,NULL);
    releaseWork=CreateEventW(NULL,TRUE,FALSE,NULL);
    assert(began&&releaseWork);
    if(!strcmp(mode,"idle-loop-error")){
        failIdle=1;assert(MediaMessageLoop(&ui,window)==1);assert(MediaDrainWorker(&ui));
    }else{
        SetWindowTextW(Control(&ui,ID_IMAGE),L"C:\\synthetic-image.img");
        failStart=!strcmp(mode,"start-failure");
        closeAdmission=!strcmp(mode,"admission-close");
        SendMessageW(Control(&ui,ID_PROGRESS),PBM_SETPOS,85,0);
        Start(&ui,1);
        if(IsWindow(window))assert(SendMessageW(Control(&ui,ID_PROGRESS),PBM_GETPOS,0,0)==0);
        if(failStart||!strcmp(mode,"admission-close")){
            assert(ui.job==NULL&&InterlockedCompareExchange(&workerCalls,0,0)==0);
            assert(threadStarts==(failStart?1U:0U));
        }else{
            assert(ui.job&&ui.job->thread);
            observedJob=ui.job;observedThread=ui.job->thread;
            assert(DuplicateHandle(GetCurrentProcess(),observedThread,GetCurrentProcess(),
                &retainedThread,SYNCHRONIZE,FALSE,0));
            assert(WaitForSingleObject(began,2000)==WAIT_OBJECT_0);
            if(!strcmp(mode,"premature-notification")){
                SendMessageW(window,WM_MEDIA_DONE,0,0);
                assert(ui.job==observedJob&&jobFrees==0U);
            }
            if(!strcmp(mode,"stale-progress")){
                InterlockedExchange(&ui.job->progress,42);
                SendMessageW(window,WM_MEDIA_PROGRESS,99,0);
                assert(SendMessageW(Control(&ui,ID_PROGRESS),PBM_GETPOS,0,0)==42);
            }
            int abnormal=!strcmp(mode,"quit-running")||!strcmp(mode,"window-destroyed")||
                !strcmp(mode,"wait-failure")||!strcmp(mode,"drain-failure");
            if(!strcmp(mode,"close-running")){
                SendMessageW(window,WM_CLOSE,0,0);
                assert(IsWindow(window)&&ui.job==observedJob);
            }else if(!strcmp(mode,"window-destroyed"))DestroyWindow(window);
            else if(!strcmp(mode,"wait-failure"))failWait=1;
            else if(abnormal)PostQuitMessage(0);
            else{ui.closing=1;SetEvent(releaseWork);}
            int result=MediaMessageLoop(&ui,window);
            assert(result==(!strcmp(mode,"wait-failure")?1:0));
            if(abnormal){
                assert(ui.job==observedJob&&jobFrees==0U);
                if(!strcmp(mode,"drain-failure")){
                    failDrain=1;assert(!MediaDrainWorker(&ui));
                    assert(ui.job==observedJob&&jobFrees==0U&&ui.lifecycleFailed);
                }
                assert(MediaDrainWorker(&ui));
            }
            assert(ui.job==NULL&&jobFrees==1U);
            assert(WaitForSingleObject(retainedThread,0)==WAIT_OBJECT_0);
            assert(InterlockedCompareExchange(&workerCalls,0,0)==1);
            if(!strcmp(mode,"release-failure"))assert(ui.lifecycleFailed);
        }
    }
    if(IsWindow(window))DestroyWindow(window);
    MSG message;while(PeekMessageW(&message,NULL,WM_QUIT,WM_QUIT,PM_REMOVE)){}
    if(ui.font)DeleteObject(ui.font);
    if(ui.logo)DeleteObject(ui.logo);
    UmiBootMediaPlanDestroy(ui.plan);free(ui.inventory);
    if(retainedThread)CloseHandle(retainedThread);
    CloseHandle(began);CloseHandle(releaseWork);
    assert(UnregisterClassW(type.lpszClassName,ui.instance));
    return 0;
}
