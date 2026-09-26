/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: examples/release_notes/main_windows.c
 * A Windows release laboratory: real DLL, executable-relative starter text,
 * native controls, UTF-8 save-as-new and an explicit no-window startup check.
 * It does not replace any existing Notes/editor implementation. Production
 * document editing belongs to the existing Framework document services.
 *---------------------------------------------------------------------------*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "model.h"
#define NOTE_CAPACITY 32769U
#define ID_EDITOR 201
#define ID_COUNT 202
#define ID_SAVE 203
#define ID_STATUS 204
static HWND editor, statusControl;
static HBITMAP logo;

/* Resolve a resource beside the executable, not the caller's working folder.
 * GetModuleFileNameW reports truncation, which is treated as failure. */
static int ReadStarter(char out[NOTE_CAPACITY])
{
    wchar_t path[4096]; DWORD n = GetModuleFileNameW(NULL,path,4096);
    if (!n || n >= 4096) return 0;
    wchar_t *last = wcsrchr(path,L'\\');
    if (!last || (size_t)(last-path) + 30U >= 4096U) return 0;
    wcscpy(last+1,L"example-note.txt");
    HANDLE file = CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OPEN_REPARSE_POINT,NULL);
    if (file == INVALID_HANDLE_VALUE) return 0;
    BY_HANDLE_FILE_INFORMATION info; LARGE_INTEGER size; DWORD readBytes=0;
    int ok = GetFileType(file)==FILE_TYPE_DISK && GetFileInformationByHandle(file,&info) &&
        !(info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT)) &&
        GetFileSizeEx(file,&size) && size.QuadPart>0 && size.QuadPart<(LONGLONG)NOTE_CAPACITY;
    if(ok)ok=ReadFile(file,out,(DWORD)size.QuadPart,&readBytes,NULL) && readBytes==(DWORD)size.QuadPart;
    if(ok) { out[readBytes]=0;ok=memchr(out,0,readBytes)==NULL; }
    if(ok)ok=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,out,-1,NULL,0)>0;
    if(!CloseHandle(file))ok=0;
    return ok;
}
static char *Text(void)
{
    int chars=GetWindowTextLengthW(editor);
    if(chars<0 || chars>32768)return NULL;
    wchar_t *wide=calloc((size_t)chars+1U,sizeof *wide);
    if(!wide)return NULL;
    if(GetWindowTextW(editor,wide,chars+1)!=chars){free(wide);return NULL;}
    int count=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide,-1,NULL,0,NULL,NULL);
    char *text=count>0 && count<=(int)NOTE_CAPACITY?malloc((size_t)count):NULL;
    if(text && WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide,-1,text,count,NULL,NULL)!=count) {
        free(text);text=NULL;
    }
    free(wide);return text;
}
static void CountWords(void)
{
    char *text=Text();uint32_t words=0;
    if(!text || UmiReleaseNotesWordCount(text,strlen(text),&words)) {
        SetWindowTextW(statusControl,L"Text is invalid or exceeds the 32 KiB practice limit.");
    } else {
        wchar_t message[128];
        (void)swprintf(message,128,L"Words separated by ASCII whitespace: %lu",(unsigned long)words);
        SetWindowTextW(statusControl,message);
    }
    free(text);
}
static void SaveNew(HWND parent)
{
    char *text=Text();if(!text){SetWindowTextW(statusControl,L"Could not read valid bounded text.");return;}
    wchar_t name[4096]=L"Umicom practice note.txt";
    OPENFILENAMEW dialog={0};dialog.lStructSize=sizeof dialog;dialog.hwndOwner=parent;
    dialog.lpstrFilter=L"Text files\0*.txt\0All files\0*.*\0";dialog.lpstrFile=name;
    dialog.nMaxFile=4096;dialog.lpstrDefExt=L"txt";
    dialog.Flags=OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR;
    if(!GetSaveFileNameW(&dialog)){free(text);return;}
    HANDLE file=CreateFileW(name,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE) {
        MessageBoxW(parent,L"Choose a new filename. Existing files are never overwritten.",L"Save a new copy",MB_OK|MB_ICONINFORMATION);
        free(text);return;
    }
    DWORD written=0;size_t bytes=strlen(text);
    int ok=WriteFile(file,text,(DWORD)bytes,&written,NULL) && written==(DWORD)bytes;
    if(ok)ok=FlushFileBuffers(file)!=0;
    if(!CloseHandle(file))ok=0;
    free(text);
    SetWindowTextW(statusControl,ok?L"New UTF-8 copy saved.":L"Write failed. The new partial file was retained; use another filename.");
}
static LRESULT CALLBACK Window(HWND window,UINT message,WPARAM w,LPARAM l)
{
    if(message==WM_CREATE) {
        HINSTANCE instance=(HINSTANCE)GetWindowLongPtrW(window,GWLP_HINSTANCE);
        logo=LoadBitmapW(instance,MAKEINTRESOURCEW(102));
        HWND brand=CreateWindowW(L"STATIC",L"",WS_CHILD|WS_VISIBLE|SS_BITMAP,16,12,240,52,window,NULL,instance,NULL);
        SendMessageW(brand,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)logo);
        editor=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_MULTILINE|ES_AUTOVSCROLL|WS_VSCROLL,16,80,730,340,window,(HMENU)(INT_PTR)ID_EDITOR,instance,NULL);
        SendMessageW(editor,EM_SETLIMITTEXT,32768,0);
        CreateWindowW(L"BUTTON",L"Count words",WS_CHILD|WS_VISIBLE|WS_TABSTOP,16,432,150,32,window,(HMENU)(INT_PTR)ID_COUNT,instance,NULL);
        CreateWindowW(L"BUTTON",L"Save a new copy...",WS_CHILD|WS_VISIBLE|WS_TABSTOP,180,432,190,32,window,(HMENU)(INT_PTR)ID_SAVE,instance,NULL);
        statusControl=CreateWindowW(L"STATIC",L"Practice release: no existing files are overwritten.",WS_CHILD|WS_VISIBLE,16,480,730,38,window,(HMENU)(INT_PTR)ID_STATUS,instance,NULL);
        if(!editor || !statusControl)return -1;
        char text[NOTE_CAPACITY];wchar_t wide[NOTE_CAPACITY];
        if(ReadStarter(text) && MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text,-1,wide,(int)NOTE_CAPACITY))SetWindowTextW(editor,wide);
        else SetWindowTextW(statusControl,L"The packaged starter is missing or invalid. Check example-note.txt beside this executable.");
        return 0;
    }
    if(message==WM_COMMAND) {
        if(LOWORD(w)==ID_COUNT)CountWords();
        if(LOWORD(w)==ID_SAVE)SaveNew(window);
        return 0;
    }
    if(message==WM_CLOSE) {
        if(MessageBoxW(window,L"Close this practice note? Unsaved text will not be retained.",L"Umicom Notes release laboratory",MB_YESNO|MB_ICONQUESTION)==IDYES)DestroyWindow(window);
        return 0;
    }
    if(message==WM_DESTROY){if(logo)DeleteObject(logo);PostQuitMessage(0);return 0;}
    return DefWindowProcW(window,message,w,l);
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE previous,LPWSTR command,int show)
{
    (void)previous;
    if(!wcscmp(command,L"--smoke-test")) {
        char text[NOTE_CAPACITY];uint32_t words=0;
        /* This checks the real loaded DLL plus a real neighbouring resource.
         * It does not construct controls or pretend to validate a whole GUI. */
        return ReadStarter(text) && !UmiReleaseNotesWordCount(text,strlen(text),&words) && words>0?0:13;
    }
    WNDCLASSW cls={0};cls.lpfnWndProc=Window;cls.hInstance=instance;
    cls.hCursor=LoadCursorW(NULL,IDC_ARROW);cls.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    cls.hIcon=LoadIconW(instance,MAKEINTRESOURCEW(101));cls.lpszClassName=L"UmicomReleaseNotes";
    if(!RegisterClassW(&cls))return 1;
    HWND window=CreateWindowW(cls.lpszClassName,L"Umicom Notes — release laboratory",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,790,570,NULL,NULL,instance,NULL);
    if(!window)return 1;
    ShowWindow(window,show);MSG message;int result;
    while((result=(int)GetMessageW(&message,NULL,0,0))>0) {
        if(!IsDialogMessageW(window,&message)){TranslateMessage(&message);DispatchMessageW(&message);}
    }
    return result<0?1:(int)message.wParam;
}
