/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Maintenance is part of the installer authority, not another file catalogue. */
#ifndef UMICOM_SETUP_MAINTENANCE_INTERNAL_H
#define UMICOM_SETUP_MAINTENANCE_INTERNAL_H
#include "internal.h"
#include "umicom/setup_centre/maintenance.h"
#define SM_JOURNAL_LIMIT (96U * 1024U * 1024U)
#define SM_OWNER "Umicom maintenance archives; retained, not user documents.\n"
struct UmiSetupMaintenancePlan {
    UmiSetupMaintenanceConfig config;
    char root[UMI_SETUP_PATH_CAPACITY];
    char source[UMI_SETUP_PATH_CAPACITY];
    char sourceHash[65];
    char boundReceipt[65];
    UmiSetupBundle *before;
    UmiSetupBundle *release;
    UmiSetupMaintenanceSummary summary;
    UmiSetupMaintenanceRow *rows;
    size_t rowCount;
    size_t rowCapacity;
    char *beforeText;
    size_t beforeLength;
    char *afterText;
    size_t afterLength;
    char *journal;
    size_t journalLength;
    int attempted;
};
int SmReserved(const char *relative);
UmiStatus SmSnapshot(const char *path, int *exists, uint64_t *bytes, char hash[65],
    UmiSetupProgress progress, void *context, UmiSetupReport *report);
UmiStatus SmJournalWrite(UmiSetupMaintenancePlan *plan);
UmiStatus SmJournalRead(const char *text, size_t length, UmiSetupMaintenancePlan **out);
UmiStatus SmCheckSnapshot(const char *path, int exists, uint64_t bytes, const char *hash,
    UmiSetupProgress progress, void *context, UmiSetupReport *report);
UmiStatus SmPath(const char *root, const char *child, char out[UMI_SETUP_PATH_CAPACITY]);
UmiStatus SmSlot(const char *transactionRoot, const char *kind, size_t index,
    char out[UMI_SETUP_PATH_CAPACITY]);
UmiStatus SmObserveRegular(const char *path, UmiSetupReport *report);
UmiStatus SmMoveNew(const char *source, const char *destination, UmiSetupReport *report);
UmiStatus SmSyncParent(const char *path, UmiSetupReport *report);
UmiStatus SmLock(const char *root, void **out, UmiSetupReport *report);
void SmUnlock(void *lock);
UmiStatus SmCheckRoot(const char *path, UmiSetupReport *report);
#endif
