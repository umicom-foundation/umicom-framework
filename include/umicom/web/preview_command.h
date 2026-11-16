/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/preview_command.h
 *
 * PURPOSE:
 *   Expose a reusable native console entry for explicit loopback static preview.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_PREVIEW_COMMAND_H
#define UMICOM_WEB_PREVIEW_COMMAND_H
#ifdef __cplusplus
extern "C" {
#endif
/* Arguments exclude the executable and any subcommand. Strings are borrowed
 * UTF-8 and must remain accessible for the call. Return 0 on success, 2 on
 * argument refusal, 1 on operational/output failure. Help and dry-run perform
 * no root I/O or listener creation. Dry-run does not verify files or the port.
 *
 * Call only from a dedicated console's main thread, once at a time across the
 * process. The command temporarily owns SIGINT/SIGTERM and restores handlers.
 * It is not reentrant or suitable for a GUI event loop. No TLS, credentials,
 * uploads, public hosting, browser launch or service installation is added. */
int UmiWebPreviewCommand(int argc, char **argv);
#ifdef __cplusplus
}
#endif
#endif
