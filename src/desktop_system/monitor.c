/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/monitor.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <string.h>
#include <stdlib.h>

void UmiDesktopSystemInitialise(UmiDesktopSystemSnapshot *snapshot)
{
    memset(snapshot, 0, sizeof *snapshot);
    snapshot->memoryStatus = snapshot->cpuStatus = snapshot->processStatus =
        snapshot->networkStatus = snapshot->storageStatus = snapshot->bootStatus = UMI_STATUS_UNAVAILABLE;
}
static int SafePath(const char *path)
{
    if (!path) return 1;
    size_t length = strlen(path);
    if (length == 0 || length >= UMI_DESKTOP_SYSTEM_PATH_CAPACITY) return 0;
    for (size_t i = 0; i < length; ++i) if ((unsigned char)path[i] < 32U || path[i] == 127) return 0;
#ifdef _WIN32
    /* A local volume only; do not make UI refresh contact a UNC server. */
    return length >= 3 && ((path[0] >= 'A' && path[0] <= 'Z') ||
        (path[0] >= 'a' && path[0] <= 'z')) && path[1] == ':' && (path[2] == '\\' || path[2] == '/');
#else
    return path[0] == '/';
#endif
}
void UmiDesktopSystemReadBoot(const char *path, UmiDesktopSystemSnapshot *snapshot)
{
    if (!path) { snapshot->bootStatus = UMI_STATUS_NOT_IMPLEMENTED; return; }
    char buffer[UMI_BOOT_REPORT_MAX_BYTES + 1]; size_t length = 0;
    snapshot->bootStatus = UmiDesktopSystemReadText(path, buffer, sizeof buffer, &length);
    if (snapshot->bootStatus == UMI_STATUS_OK)
        snapshot->bootStatus = UmiBootReportParse(buffer, length, &snapshot->boot);
}
UmiStatus UmiDesktopSystemCapture(const UmiDesktopSystemOptions *options,
    UmiDesktopSystemSnapshot *outSnapshot)
{
    UmiDesktopSystemOptions defaults = {0};
    if (!options) options = &defaults;
    if (!outSnapshot || !SafePath(options->storagePath) || !SafePath(options->bootReportPath))
        return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    if (options->linuxProcRoot) return UMI_STATUS_NOT_IMPLEMENTED;
#else
    if (!SafePath(options->linuxProcRoot)) return UMI_STATUS_INVALID_ARGUMENT;
#endif
    UmiDesktopSystemSnapshot *value = malloc(sizeof *value);
    if (!value) return UMI_STATUS_OUT_OF_MEMORY;
    UmiDesktopSystemInitialise(value); value->capturedMilliseconds = UmiDesktopSystemClock();
    UmiDesktopSystemCaptureNative(options, value);
    uint64_t end = UmiDesktopSystemClock();
    if (end >= value->capturedMilliseconds) value->elapsedMilliseconds = end - value->capturedMilliseconds;
    *outSnapshot = *value; free(value); return UMI_STATUS_OK;
}
