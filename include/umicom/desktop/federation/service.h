/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/federation/service.h
 *
 * PURPOSE:
 *   Manage saved application sets and explicit, single-use launch reviews.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_FEDERATION_SERVICE_H
#define UMICOM_DESKTOP_FEDERATION_SERVICE_H
#include "umicom/data/data_server.h"
#include "umicom/desktop/federation/ports.h"
#include "umicom/desktop/federation/links.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDeskFederation UmiDeskFederation;
/* Single owner thread. Data Server and port context are borrowed and must
 * outlive this object. Use a dedicated server/connection; Federation rejects
 * an already active transaction and never commits a caller's transaction.
 * Create performs no I/O. Call Reload explicitly before workspace operations.
 * No constructor, reload, poll or destructor launches/stops an application. */
UmiStatus UmiDeskFederationCreate(UmiDataServer *server,
    const UmiDeskFederationPorts *ports, const char *hostApplicationId,
    UmiDeskFederation **outService);
void UmiDeskFederationDestroy(UmiDeskFederation *service);
/* Reload is transactional. Malformed, partial or future-schema storage is
 * rejected without replacing the last valid in-memory workspace snapshot.
 * Any attempted reload cancels the old review, including a failed reload. */
UmiStatus UmiDeskFederationReload(UmiDeskFederation *service);
/* Save replaces a matching kind+ID or adds a record. It changes only the saved
 * application set, not live processes. Titles are valid UTF-8 without control
 * characters. IDs are bounded ASCII. Empty sets and duplicate members fail.
 * Each committed write compares the stored revision and complete contents
 * inside a transaction. Same-revision contents that changed are rejected.
 * Local storage is not authenticated; protect it with OS account permissions. */
UmiStatus UmiDeskFederationSaveWorkspace(UmiDeskFederation *service,
    const UmiDeskWorkspace *workspace);
UmiStatus UmiDeskFederationRemoveWorkspace(UmiDeskFederation *service,
    UmiDeskWorkspaceKind kind, const char *id);
UmiStatus UmiDeskFederationWorkspaceAt(const UmiDeskFederation *service,
    size_t index, UmiDeskWorkspace *outWorkspace);
UmiStatus UmiDeskFederationWorkspaceFind(const UmiDeskFederation *service,
    UmiDeskWorkspaceKind kind, const char *id, UmiDeskWorkspace *outWorkspace);
/* Capture only currently observed running application IDs, excluding Desk.
 * Document buffers, passwords, process IDs, business records and window
 * geometry are NOT a part of this application-set session. */
UmiStatus UmiDeskFederationCaptureSession(UmiDeskFederation *service,
    const char *id, const char *title);
UmiStatus UmiDeskFederationPrepareWorkspace(UmiDeskFederation *service,
    UmiDeskWorkspaceKind kind, const char *id, UmiDeskFederationPlan *outPlan);
UmiStatus UmiDeskFederationPrepareApplication(UmiDeskFederation *service,
    const char *applicationId, UmiDeskFederationPlan *outPlan);
UmiStatus UmiDeskFederationPrepareLink(UmiDeskFederation *service,
    const char *link, UmiDeskFederationPlan *outPlan);
/* token is single use and refers to a service-owned immutable plan. The whole
 * plan is rechecked before any request, then each host request rechecks its
 * own item. Changed workspaces/catalogue return BUSY without early requests.
 * Once dispatch begins, all items are reported; later items still run after
 * a failure. An accepted start cannot be rolled back. Return value is the
 * first non-OK item status (including UNAVAILABLE for a skipped item).
 * outReport is valid on failure too. Accepted is not equivalent to ready. */
UmiStatus UmiDeskFederationExecute(UmiDeskFederation *service, uint64_t token,
    UmiDeskFederationReport *outReport);
UmiStatus UmiDeskFederationCancelPreview(UmiDeskFederation *service);
/* Poll adds activity only for changed observations after its initial baseline.
 * Activity is local, bounded, non-durable and is not an OS notification feed.
 * No live state is inferred from a saved session. */
UmiStatus UmiDeskFederationPoll(UmiDeskFederation *service);
UmiStatus UmiDeskFederationActivityAt(const UmiDeskFederation *service,
    size_t index, UmiDeskFederationActivity *outActivity);
UmiStatus UmiDeskFederationAcknowledgeAll(UmiDeskFederation *service);
UmiStatus UmiDeskFederationSnapshotGet(const UmiDeskFederation *service,
    UmiDeskFederationSnapshot *outSnapshot);
#ifdef __cplusplus
}
#endif
#endif
