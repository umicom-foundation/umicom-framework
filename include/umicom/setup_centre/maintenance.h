/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/setup_centre/maintenance.h
 * Purpose: Review, apply and recover offline application-file maintenance.
 * Author: Sammy Hegab, Umicom Foundation | Licence: MIT
 *
 * Framework owns file selection, evidence and recovery. Neither a GUI nor an
 * application may invent its own deletion rules. Existing install APIs remain
 * available. No operation here changes a database, registry, shortcut or user
 * document that is absent from the installed receipt.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SETUP_CENTRE_MAINTENANCE_H
#define UMICOM_SETUP_CENTRE_MAINTENANCE_H
#include "umicom/setup_centre/setup.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SETUP_MAINTENANCE_DIRECTORY ".umicom-maintenance"
#define UMI_SETUP_MAINTENANCE_PENDING "umicom-maintenance.pending"
#define UMI_SETUP_MAINTENANCE_MAX_ROWS (2U * UMI_SETUP_MAX_FILES)
/** Update keeps every currently installed application. A replacement release
 * must supply them all, at the same entry paths. Repair uses EXACT receipt
 * hashes, not the latest version. Remove interprets removeMask against the
 * installed catalogue. Shared files remain while any application remains. */
typedef enum UmiSetupMaintenanceAction {
    UMI_SETUP_MAINTENANCE_UPDATE = 1,
    UMI_SETUP_MAINTENANCE_REPAIR = 2,
    UMI_SETUP_MAINTENANCE_REMOVE = 3
} UmiSetupMaintenanceAction;
typedef enum UmiSetupMaintenanceChange {
    UMI_SETUP_MAINTENANCE_KEEP = 0,
    UMI_SETUP_MAINTENANCE_WRITE = 1,
    UMI_SETUP_MAINTENANCE_RETIRE = 2,
    UMI_SETUP_MAINTENANCE_PRESERVE_EDIT = 3
} UmiSetupMaintenanceChange;
typedef struct UmiSetupMaintenanceConfig {
    const char *installationRoot;
    const char *releaseRoot; /* Required for Update/Repair; ignored for Remove. */
    UmiSetupMaintenanceAction action;
    uint64_t removeMask;
    const char *expectedReceipt; /* Optional identity of the displayed installed list. */
} UmiSetupMaintenanceConfig;
typedef struct UmiSetupMaintenanceRow {
    char relative[UMI_SETUP_RELATIVE_CAPACITY];
    UmiSetupMaintenanceChange change;
    int beforeExists;
    int afterExists; /* The desired owned state, not user-created extras. */
    int modified;
    uint64_t beforeBytes;
    uint64_t afterBytes;
    char beforeHash[65];
    char afterHash[65];
} UmiSetupMaintenanceRow;
typedef struct UmiSetupMaintenanceSummary {
    size_t checkedFiles;
    size_t filesToWrite;
    size_t filesToRetire;
    size_t modifiedFilesPreserved;
    size_t applicationsBefore;
    size_t applicationsAfter;
    uint64_t stagingBytes;
    int noChange;
    char beforeReceipt[65];
    char afterReceipt[65]; /* Empty after removal of the last application. */
    char fingerprint[65];
} UmiSetupMaintenanceSummary;
typedef struct UmiSetupMaintenanceReport {
    UmiSetupReport io;
    int recoveryRequired;
    int restored;
    int allApplicationsRemoved;
    char transaction[65];
    char archive[UMI_SETUP_PATH_CAPACITY];
} UmiSetupMaintenanceReport;
typedef struct UmiSetupMaintenancePlan UmiSetupMaintenancePlan;
/** Read only. The plan owns its catalogues and strings. The fingerprint binds
 * the complete before snapshot, desired receipt, operation and roots. It is
 * change detection, NOT a publisher signature or user authentication token.
 * Application writers must be stopped. Use a trusted local installation and
 * release, not a concurrently modified or network filesystem. */
UmiStatus UmiSetupMaintenancePlanCreate(const UmiSetupMaintenanceConfig *config,
    UmiSetupMaintenancePlan **outPlan, UmiSetupProgress progress, void *context,
    UmiSetupMaintenanceReport *report);
void UmiSetupMaintenancePlanDestroy(UmiSetupMaintenancePlan *plan);
/** Borrowed identity binds an application list to a later review. NULL for a
 * release catalogue rather than an installed receipt. */
const char *UmiSetupMaintenanceReceiptIdentity(const UmiSetupBundle *installed);
const UmiSetupMaintenanceSummary *UmiSetupMaintenancePlanSummary(const UmiSetupMaintenancePlan *plan);
const UmiSetupMaintenanceRow *UmiSetupMaintenancePlanRow(const UmiSetupMaintenancePlan *plan, size_t index);
/** Single-use attempt. Rechecks the plan under a native per-installation lock;
 * stages all replacement bytes before marking the installation as pending;
 * moves old files into a retained journal; publishes the new receipt last.
 * Never recursively deletes anything. Missing/corrupt owned files can be
 * repaired. Update refuses locally edited owned files; Remove leaves edits
 * in their original locations and removes their ownership from the receipt.
 *
 * Cancellation after publication of the pending marker requires recovery.
 * A process crash is recoverable; sudden-power-loss/storage-device guarantees
 * require target-filesystem qualification. A successful return is NOT an
 * application startup test, database compatibility check or signed update. */
UmiStatus UmiSetupMaintenanceApply(UmiSetupMaintenancePlan *plan,
    const char *expectedFingerprint, UmiSetupProgress progress, void *context,
    UmiSetupMaintenanceReport *report);
/** Read the pending journal identity. NOT_FOUND means no pending marker. A
 * malformed marker is an error, never evidence that recovery is unnecessary. */
UmiStatus UmiSetupMaintenancePending(const char *installationRoot,
    UmiSetupMaintenanceReport *report);
/** Restore the exact pre-operation file snapshot. With pendingOnly nonzero,
 * only an interrupted transaction may be restored. With zero, a completed
 * transaction can be undone only if its resulting owned state still agrees.
 * Newer edits are not overwritten. Archives and staged/undone files remain.
 * Recovery is repeatable after another interruption. Restoring a damaged
 * pre-repair snapshot does not make it healthy: run verification separately. */
UmiStatus UmiSetupMaintenanceRestore(const char *installationRoot,
    const char *transaction, int pendingOnly, UmiSetupProgress progress,
    void *context, UmiSetupMaintenanceReport *report);
const char *UmiSetupMaintenanceActionText(UmiSetupMaintenanceAction action);
const char *UmiSetupMaintenanceChangeText(UmiSetupMaintenanceChange change);
int UmiSetupMaintenanceMain(int argc, char **argv);
#ifdef __cplusplus
}
#endif
#endif
