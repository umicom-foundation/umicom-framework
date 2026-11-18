/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_window_activation.inc
 *
 * PURPOSE:
 *   Request foreground activation only for a window owned by the supervised live process.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifdef _WIN32
typedef struct UmiWindowSearch {
    DWORD processId;
    HWND window;
} UmiWindowSearch;

static BOOL CALLBACK UmiFindProcessWindow(HWND window, LPARAM parameter)
{
    UmiWindowSearch *search = (UmiWindowSearch *)parameter;
    DWORD processId = 0U;
    (void)GetWindowThreadProcessId(window, &processId);
    if (processId == search->processId && IsWindowVisible(window) &&
        GetWindow(window, GW_OWNER) == NULL) {
        search->window = window;
        return FALSE;
    }
    return TRUE;
}
#endif

/* The caller holds the supervisor mutex so the worker cannot revoke/close
 * this handle during the request. Holding the original process object avoids
 * targeting a later process that happens to reuse its numeric PID. This is a
 * foreground request, not a focus-lock bypass or proof of application health. */
UmiStatus UmiProcessActivateNativeWindow(void *nativeHandle)
{
#ifdef _WIN32
    UmiWindowSearch search = {0};
    DWORD waited;
    HWND target;
    DWORD actualId = 0U;
    if (nativeHandle == NULL) return UMI_STATUS_BUSY;
    waited = WaitForSingleObject((HANDLE)nativeHandle, 0U);
    if (waited == WAIT_OBJECT_0) return UMI_STATUS_INVALID_STATE;
    if (waited != WAIT_TIMEOUT) return UMI_STATUS_IO_ERROR;
    search.processId = GetProcessId((HANDLE)nativeHandle);
    if (search.processId == 0U) return UMI_STATUS_IO_ERROR;
    (void)EnumWindows(UmiFindProcessWindow, (LPARAM)&search);
    if (search.window == NULL) return UMI_STATUS_UNAVAILABLE;
    /* Prefer the application's last visible modal dialog when present. A
     * bootstrap error dialog must be reachable rather than hiding behind its
     * owner. Recheck identity after enumeration; windows can disappear. */
    target = GetLastActivePopup(search.window);
    if (target == NULL || !IsWindowVisible(target)) target = search.window;
    (void)GetWindowThreadProcessId(target, &actualId);
    if (actualId != search.processId ||
        WaitForSingleObject((HANDLE)nativeHandle, 0U) != WAIT_TIMEOUT)
        return UMI_STATUS_INVALID_STATE;
    if (IsIconic(target) && !ShowWindowAsync(target, SW_RESTORE))
        return UMI_STATUS_UNAVAILABLE;
    /* Respect Windows foreground policy. Do not attach input threads or
     * report success after a rejected request. No second process is started. */
    return SetForegroundWindow(target) ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED;
#else
    (void)nativeHandle;
    return UMI_STATUS_NOT_IMPLEMENTED;
#endif
}
