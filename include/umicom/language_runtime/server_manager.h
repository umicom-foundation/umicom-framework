/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/server_manager.h
 *
 * PURPOSE:
 *   Own language servers per language/workspace and perform initialize/shutdown lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_SERVER_MANAGER_H
#define UMICOM_LANGUAGE_RUNTIME_SERVER_MANAGER_H
#include "umicom/language_runtime/builtin_profiles.h"
#include "umicom/platform/cancellation.h"
#include "umicom/language_runtime/profile_health.h"
#include "umicom/language_runtime/decoders/initialize.h"
#include "umicom/language_runtime/requests/initialize.h"
#include "umicom/language_runtime/requests/initialized.h"
#include "umicom/language_runtime/requests/shutdown.h"
#include "umicom/language_runtime/requests/exit.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime server manager data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeServerManager UmiLanguageRuntimeServerManager;
/**
 * Initialise language runtime server manager from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_runtime_server_manager_create(UmiLanguageService*l,UmiLanguageRuntimeServerManager**out);
/**
 * Release or reset state held by language runtime server manager so the same storage can
 * be reused safely.
 */
void umi_language_runtime_server_manager_destroy(UmiLanguageRuntimeServerManager*m);
/**
 * Provide the language runtime server manager start for language operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_server_manager_start_for_language(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root,const char*wd,uint32_t timeout,UmiLanguageRuntimeServer**out);
/**
 * Provide the language runtime server manager attach operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_server_manager_attach(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root,UmiLanguageRuntimeServer*s,const UmiLanguageRuntimeInitializeResult*caps);
/**
 * Find language runtime server manager while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiLanguageRuntimeServer *umi_language_runtime_server_manager_find(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root);
/**
 * Provide the language runtime server manager stop all operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_server_manager_stop_all(UmiLanguageRuntimeServerManager*m,uint32_t timeout);
/**
 * Return the number of records represented by language runtime server manager without
 * changing their state.
 */
size_t umi_language_runtime_server_manager_count(const UmiLanguageRuntimeServerManager*m);
/* Initialize one STARTING server and publish capabilities only after its
 * matching response and the initialized notification succeed. Timeout is one
 * elapsed read budget, not a fresh interval for each notification or fragment.
 * Cancellation is checked between reads (at most 50 ms per native read).
 * Launch and synchronous writes are not interruptible through this token.
 * Unrelated initialization messages are consumed as in the manager's original
 * handshake; this does not implement server-to-client request handling.
 * On failure after initialization begins, the server is marked FAILED and
 * remains caller-owned. Close it rather than retrying a partially sent request. */
UmiStatus UmiLanguageRuntimeServerInitialize(UmiLanguageRuntimeServer *server,
    const char *rootUri, uint32_t timeoutMs, const UmiCancellationToken *cancel,
    UmiLanguageRuntimeInitializeResult *outCapabilities);
/* Request protocol shutdown, then close the direct child even when the reply
 * fails or times out. The first protocol failure is returned after cleanup.
 * timeoutMs bounds reply waits, not synchronous writes or OS termination. */
UmiStatus UmiLanguageRuntimeServerShutdown(UmiLanguageRuntimeServer *server,
    uint32_t timeoutMs);
/* Explicitly start an enabled, caller-selected profile for a workspace.
 * The manager takes ownership only after a successful handshake. Outputs are
 * NULL on failure; the profile is borrowed until return. Existing entries are
 * never silently replaced, because document synchronization may borrow them.
 * ALREADY_EXISTS includes stopped entries; close their documents and recreate
 * the manager to establish a fresh lifetime. Autostart does not authorize an
 * implicit launch here: the application must obtain the user's selection.
 * Use one persistent worker for all manager/server operations. */
UmiStatus UmiLanguageRuntimeServerManagerStartProfile(UmiLanguageRuntimeServerManager *manager,
    const char *languageId, const UmiLanguageServerProfile *profile, const char *rootUri,
    const char *workingDirectory, uint32_t timeoutMs, const UmiCancellationToken *cancel,
    UmiLanguageRuntimeServer **outServer);
#ifdef __cplusplus
}
#endif
#endif
