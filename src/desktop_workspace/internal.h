/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Internal checkpoint representation; never persist native struct padding. */
#ifndef UMICOM_DESKTOP_WORKSPACE_INTERNAL_H
#define UMICOM_DESKTOP_WORKSPACE_INTERNAL_H
#include "umicom/desktop_workspace/workspace.h"
#include "umicom/native_launcher/sha256.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DW_MAX_BYTES 70000U
#define DW_CHUNK 1024U
#define DW_VALUE 4096U
#define DW_HEAD "desktop.workspace.head"
typedef struct DwGuard DwGuard;
struct UmiDesktopWorkspace {
    UmiDataServer *server;
    DwGuard *guard;
    int ownsServer, closed, poisoned, unfinished;
    uint64_t oldest;
    char head[256], detail[256];
    UmiDesktopWorkspaceSnapshot snapshot;
};
int DwId(const char *text, size_t capacity);
int DwUtf8(const char *text, size_t capacity, int multiline);
UmiStatus DwEncode(const UmiDesktopWorkspaceSnapshot *snapshot,
    unsigned char **outBytes, size_t *outLength);
UmiStatus DwDecode(const void *bytes, size_t length,
    UmiDesktopWorkspaceSnapshot *outSnapshot);
UmiStatus DwGuardAcquire(const char *directory, DwGuard **outGuard,
    char databasePath[1024]);
void DwGuardRelease(DwGuard *guard);
#endif
