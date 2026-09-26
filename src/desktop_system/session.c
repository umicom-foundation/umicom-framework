/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/session.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "session_internal.h"
#include <string.h>
#include <stdio.h>
UmiStatus UmiDesktopSessionExplain(UmiDesktopSessionCheck *check, UmiStatus status, const char *message)
{
    if (check) { check->status = status; (void)snprintf(check->explanation, sizeof check->explanation, "%s", message); }
    return status;
}
static int Present(const char *s) { return s && *s; }
static int Text(const char *s, size_t maximum)
{
    if (!Present(s) || strlen(s) > maximum) return 0;
    for (; *s; ++s) if ((unsigned char)*s < 32U || (unsigned char)*s == 127U) return 0;
    return 1;
}
static int Absolute(const char *s) { return Text(s, 2047) && s[0] == '/'; }
static int SocketName(const char *s)
{
    if (!Text(s, 100) || strcmp(s, ".") == 0 || strcmp(s, "..") == 0) return 0;
    for (; *s; ++s) if (!((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z') ||
        (*s >= '0' && *s <= '9') || *s == '-' || *s == '_' || *s == '.')) return 0;
    return 1;
}
static int XDisplay(const char *s)
{
    if (!Text(s, 32) || *s++ != ':') return 0;
    if (*s < '0' || *s > '9') return 0;
    unsigned count = 0; while (*s >= '0' && *s <= '9') { ++s; if (++count > 5) return 0; }
    if (*s == '.') { ++s; count = 0; if (*s < '0' || *s > '9') return 0;
        while (*s >= '0' && *s <= '9') { ++s; if (++count > 5) return 0; } }
    return *s == 0;
}
UmiStatus UmiDesktopSessionValidate(const UmiDesktopSessionRequest *request, UmiDesktopSessionCheck *outCheck)
{
    if (!outCheck) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outCheck, 0, sizeof *outCheck);
    if (!request || !Absolute(request->deskPath) || !Absolute(request->home) || !Absolute(request->runtimeDirectory))
        return UmiDesktopSessionExplain(outCheck, UMI_STATUS_INVALID_ARGUMENT, "Desk, home and runtime directory must be absolute Linux paths.");
    if (Present(request->waylandDisplay)) {
        if (!SocketName(request->waylandDisplay)) return UmiDesktopSessionExplain(outCheck, UMI_STATUS_INVALID_ARGUMENT,
            "WAYLAND_DISPLAY must name one socket inside XDG_RUNTIME_DIR.");
        outCheck->backend = UMI_DESKTOP_SESSION_WAYLAND;
    } else if (XDisplay(request->display)) outCheck->backend = UMI_DESKTOP_SESSION_X11;
    else return UmiDesktopSessionExplain(outCheck, UMI_STATUS_UNAVAILABLE, "Start inside a local Wayland or X11 graphical session first.");
    if (outCheck->backend == UMI_DESKTOP_SESSION_X11 && Present(request->xauthority) && !Absolute(request->xauthority))
        return UmiDesktopSessionExplain(outCheck, UMI_STATUS_INVALID_ARGUMENT, "XAUTHORITY must be an absolute local file path.");
    if (Present(request->sessionBus) && (!Text(request->sessionBus, 2047) || strchr(request->sessionBus, ';') ||
        !((strncmp(request->sessionBus, "unix:path=", 10) == 0 && request->sessionBus[10] == '/') ||
          (strncmp(request->sessionBus, "unix:abstract=", 14) == 0 && request->sessionBus[14]))))
        return UmiDesktopSessionExplain(outCheck, UMI_STATUS_INVALID_ARGUMENT, "The optional session bus must be one local UNIX address.");
    if (Present(request->language) && !Text(request->language, 128)) return UmiDesktopSessionExplain(outCheck, UMI_STATUS_INVALID_ARGUMENT, "The language setting is too long or contains control characters.");
    return UmiDesktopSessionExplain(outCheck, UMI_STATUS_OK, "Request syntax accepted; native checks have not run.");
}
