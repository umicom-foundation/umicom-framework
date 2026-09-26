/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop_system/session.h
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESKTOP_SYSTEM_SESSION_H
#define UMICOM_DESKTOP_SYSTEM_SESSION_H
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiDesktopSessionBackend {
    UMI_DESKTOP_SESSION_NONE = 0,
    UMI_DESKTOP_SESSION_WAYLAND = 1,
    UMI_DESKTOP_SESSION_X11 = 2
} UmiDesktopSessionBackend;
typedef struct UmiDesktopSessionRequest {
    const char *deskPath;
    const char *home;
    const char *runtimeDirectory;
    const char *waylandDisplay;
    const char *display;
    const char *xauthority;
    const char *sessionBus;
    const char *language;
} UmiDesktopSessionRequest;
typedef struct UmiDesktopSessionCheck {
    UmiDesktopSessionBackend backend;
    UmiStatus status;
    char explanation[256];
} UmiDesktopSessionCheck;
/* Syntax only, portable and side-effect free. Wayland is preferred when set;
 * only a socket NAME within the runtime directory is accepted. X11 accepts a
 * local :display[.screen], never a remote server address. */
UmiStatus UmiDesktopSessionValidate(const UmiDesktopSessionRequest *request,
    UmiDesktopSessionCheck *outCheck);
/* Linux filesystem and identity preflight. Does not connect to the compositor,
 * change cwd, set process policy, mount anything or execute the selected file.
 * NOT_IMPLEMENTED on other systems. A successful check is NOT proof that GTK
 * dependencies or a display server will accept the eventual connection. */
UmiStatus UmiDesktopSessionInspect(const UmiDesktopSessionRequest *request,
    UmiDesktopSessionCheck *outCheck);
/* Explicit Linux process replacement. On success this function NEVER RETURNS:
 * Desk becomes this process, retaining all application lifecycle ownership.
 * It is not a new subprocess runner, display server, authentication service or
 * process supervisor. No session auto-restart is installed. Run as a normal
 * user from an already-established graphical session. This entry point is for
 * a dedicated, single-threaded launcher, not an existing application callback;
 * callers must prevent concurrent descriptor or environment changes. It refuses uid 0,
 * set-id execution, symlink executables, writable/shared executable files,
 * non-native/non-ELF files and unsafe runtime directories. The opened binary
 * descriptor is used for exec, rather than resolving the path a second time.
 * An allowlisted environment is supplied; no shell or LD_* values are used.
 * Home becomes cwd and no_new_privs is set just before exec. These process-local
 * settings remain changed if exec fails. Failure should end this launcher. */
UmiStatus UmiDesktopSessionEnter(const UmiDesktopSessionRequest *request,
    UmiDesktopSessionCheck *outCheck);
#ifdef __cplusplus
}
#endif
#endif
