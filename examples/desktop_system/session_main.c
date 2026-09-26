/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/desktop_system/session_main.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/desktop_system/session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc != 4 || (strcmp(argv[1], "--inspect") && strcmp(argv[1], "--run")) || strcmp(argv[2], "--desk")) {
        fputs("Usage: umicom-desk-session --inspect|--run --desk /absolute/path/to/umicom-desk\n", stderr); return 2;
    }
    UmiDesktopSessionRequest request = {argv[3], getenv("HOME"), getenv("XDG_RUNTIME_DIR"),
        getenv("WAYLAND_DISPLAY"), getenv("DISPLAY"), getenv("XAUTHORITY"), getenv("DBUS_SESSION_BUS_ADDRESS"), getenv("LANG")};
    UmiDesktopSessionCheck check;
    UmiStatus status = !strcmp(argv[1], "--inspect") ? UmiDesktopSessionInspect(&request, &check) : UmiDesktopSessionEnter(&request, &check);
    fprintf(status == UMI_STATUS_OK ? stdout : stderr, "%s\n", check.explanation);
    return status == UMI_STATUS_OK ? 0 : status == UMI_STATUS_NOT_IMPLEMENTED ? 77 : 1;
}
