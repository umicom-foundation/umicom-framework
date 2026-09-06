/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/workspace_storage.h
 *
 * PURPOSE:
 *   Explicitly open the existing Data Server SQLite backend for user-local
 *   workspace checkpoints. This is lifecycle setup, not another storage engine.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_WORKSTATION_WORKSPACE_STORAGE_H
#define UMICOM_UI_GTK4_WORKSTATION_WORKSPACE_STORAGE_H

#include "umicom/data/data_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Explicitly enable user-local durable workspace storage for a canonical app.
 * Opens <user-config>/umicom/<application-id>/workspace-layouts.sqlite3 and
 * creates its parent directory if necessary. Never substitutes an in-memory
 * backend. Existing constructors do not call this helper implicitly.
 * On success the caller owns *out_server and destroys it after all borrowers
 * with umi_data_server_destroy. Its path is available through the Data Server.
 * On failure *out_server is NULL and the returned status must remain visible.
 */
UmiStatus umi_gtk4_workspace_storage_open(
    const char *canonical_application_id, UmiDataServer **out_server);

#ifdef __cplusplus
}
#endif
#endif
