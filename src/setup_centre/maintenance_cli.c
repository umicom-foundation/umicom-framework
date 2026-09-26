/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * The CLI is a projection of the public maintenance API, not a second engine. */
#include "maintenance_internal.h"
#include <inttypes.h>

static void Usage(void)
{
    puts("Umicom application-file maintenance\n"
        "  umicom-maintain review --root ABS --action update|repair --release ABS\n"
        "  umicom-maintain review --root ABS --action remove --remove id[,id...]\n"
        "  umicom-maintain review --root ABS --action remove --all\n"
        "  umicom-maintain apply  [same options] --expect-plan HASH\n"
        "  umicom-maintain pending --root ABS\n"
        "  umicom-maintain recover --root ABS --transaction HASH\n"
        "  umicom-maintain undo --root ABS --transaction HASH\n"
        "  umicom-maintain --self-test\n\n"
        "Review reads only. Apply stages replacements and retains originals.\n"
        "Recover restores an unfinished change. Undo reverses a completed one\n"
        "only while its resulting owned state still agrees. No registry, data\n"
        "migration, shortcut removal, publisher verification or recursive erase.\n"
        "Close applications and trust the release before maintenance.");
}
static UmiStatus Mask(const char *root, const char *names, int all, uint64_t *mask, char receipt[65], UmiSetupReport *report)
{
    *mask = 0U;
    UmiSetupBundle *installed = NULL;
    UmiStatus status = UmiSetupInstalledBundleOpen(root, &installed, report);
    if (status != UMI_STATUS_OK) return status;
    strcpy(receipt, installed->catalogueHash);
    if (all) *mask = UmiSetupAllApplications(installed);
    else if (names == NULL || *names == '\0') status = UMI_STATUS_INVALID_ARGUMENT;
    else {
        const char *start = names;
        do {
            const char *end = strchr(start, ',');
            size_t length = end != NULL ? (size_t)(end - start) : strlen(start);
            size_t index = 0U;
            for (; index < installed->appCount; ++index)
                if (strlen(installed->apps[index].id) == length && memcmp(start, installed->apps[index].id, length) == 0) break;
            if (index == installed->appCount || ((*mask >> index) & UINT64_C(1))) { status = UMI_STATUS_INVALID_ARGUMENT; break; }
            *mask |= UINT64_C(1) << index;
            start = end != NULL ? end + 1 : NULL;
        } while (start != NULL);
    }
    UmiSetupBundleDestroy(installed);
    return status;
}
int UmiSetupMaintenanceMain(int argc, char **argv)
{
    if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "help"))) { Usage(); return 0; }
    if (argc == 2 && !strcmp(argv[1], "--self-test")) {
        char hash[65];
        if (UmiNativeSha256Buffer("abc", 3U, hash) != UMI_STATUS_OK ||
            strcmp(hash,"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") != 0 ||
            !SmReserved(".umicom-maintenance/journal") || SmReserved("share/notes.txt")) return 1;
        puts("Native maintenance self-check passed. No file, process or network operation was requested."); return 0;
    }
    if (argc < 4) { Usage(); return 2; }
    const char *command = argv[1], *root = NULL, *release = NULL, *action = NULL, *remove = NULL, *expected = NULL, *transaction = NULL;
    int all = 0;
    for (int i = 2; i < argc; ++i) {
        if (!strcmp(argv[i], "--all")) { if (all) return 2; all = 1; continue; }
        if (i + 1 >= argc) return 2;
        const char **slot = !strcmp(argv[i], "--root") ? &root : !strcmp(argv[i], "--release") ? &release :
            !strcmp(argv[i], "--action") ? &action : !strcmp(argv[i], "--remove") ? &remove :
            !strcmp(argv[i], "--expect-plan") ? &expected : !strcmp(argv[i], "--transaction") ? &transaction : NULL;
        if (slot == NULL || *slot != NULL) return 2;
        *slot = argv[++i];
    }
    if (root == NULL || (all && remove != NULL)) return 2;
    UmiSetupMaintenanceReport report = {0};
    UmiStatus status;
    if (!strcmp(command, "pending") || !strcmp(command, "recover") || !strcmp(command, "undo")) {
        if (release || action || remove || expected || all) return 2;
        if (!strcmp(command,"pending")) {
            if (transaction != NULL) return 2;
            status = UmiSetupMaintenancePending(root, &report);
        } else status = UmiSetupMaintenanceRestore(root, transaction, !strcmp(command,"recover"), NULL, NULL, &report);
    } else if (!strcmp(command,"review") || !strcmp(command,"apply")) {
        if (transaction != NULL || action == NULL || (!strcmp(command,"apply") && expected == NULL) ||
            (!strcmp(command,"review") && expected != NULL)) return 2;
        char boundReceipt[65] = {0};
        UmiSetupMaintenanceConfig config = {root, release, 0, 0U, NULL};
        if (!strcmp(action,"update")) config.action = UMI_SETUP_MAINTENANCE_UPDATE;
        else if (!strcmp(action,"repair")) config.action = UMI_SETUP_MAINTENANCE_REPAIR;
        else if (!strcmp(action,"remove")) config.action = UMI_SETUP_MAINTENANCE_REMOVE;
        else return 2;
        status = UMI_STATUS_OK;
        if (config.action == UMI_SETUP_MAINTENANCE_REMOVE) {
            if (release != NULL) return 2;
            status = Mask(root, remove, all, &config.removeMask, boundReceipt, &report.io);
            if (status == UMI_STATUS_OK) config.expectedReceipt = boundReceipt;
        } else if (remove != NULL || all) return 2;
        UmiSetupMaintenancePlan *plan = NULL;
        if (status == UMI_STATUS_OK) status = UmiSetupMaintenancePlanCreate(&config, &plan, NULL, NULL, &report);
        if (status == UMI_STATUS_OK) {
            const UmiSetupMaintenanceSummary *summary = UmiSetupMaintenancePlanSummary(plan);
            printf("Action: %s\nInstallation: %s\nPlan: %s\nBefore receipt: %s\nAfter receipt: %s\n"
                "Applications: %zu -> %zu\nFiles to replace: %zu\nFiles to archive: %zu\n"
                "Local edits retained: %zu\nStaged payload bytes: %" PRIu64 "\n",
                UmiSetupMaintenanceActionText(config.action), root, summary->fingerprint,
                summary->beforeReceipt, summary->afterReceipt[0] ? summary->afterReceipt : "none",
                summary->applicationsBefore, summary->applicationsAfter, summary->filesToWrite,
                summary->filesToRetire, summary->modifiedFilesPreserved, summary->stagingBytes);
            for (size_t i = 0U; i < summary->checkedFiles; ++i) {
                const UmiSetupMaintenanceRow *row = UmiSetupMaintenancePlanRow(plan, i);
                if (row->change != UMI_SETUP_MAINTENANCE_KEEP)
                    printf("  %s: %s\n", UmiSetupMaintenanceChangeText(row->change), row->relative);
            }
            if (!strcmp(command,"apply")) status = UmiSetupMaintenanceApply(plan, expected, NULL, NULL, &report);
        }
        UmiSetupMaintenancePlanDestroy(plan);
    } else { Usage(); return 2; }
    printf("%s\n%s\n", UmiSetupStatusText(status), report.io.detail);
    if (report.transaction[0]) printf("Transaction: %s\n", report.transaction);
    if (report.archive[0]) printf("Recovery archive: %s\n", report.archive);
    const int noPending = !strcmp(command, "pending") && status == UMI_STATUS_NOT_FOUND;
    printf("Recovery required: %s\n", report.recoveryRequired ? "yes" :
        status == UMI_STATUS_OK || noPending ? "no pending change reported" : "not established; inspect the pending state");
    if (!strcmp(command, "pending")) return noPending ? 0 : status == UMI_STATUS_OK ? 3 : 1;
    return status == UMI_STATUS_OK ? 0 : 1;
}
