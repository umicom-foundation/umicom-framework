/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/setup_centre/maintenance_plan.c
 * PURPOSE:
 *   A plan is a read-only comparison. A missing installed file is different from an
 *   unreadable file; unowned files are never mistaken for obsolete payload.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/setup_centre/maintenance_plan.c
 * A plan is a read-only comparison. A missing installed file is different from
 * an unreadable file; unowned files are never mistaken for obsolete payload.
 *---------------------------------------------------------------------------*/
#include "maintenance_internal.h"
#include <inttypes.h>

const char *UmiSetupMaintenanceActionText(UmiSetupMaintenanceAction action)
{
    switch (action) {
        case UMI_SETUP_MAINTENANCE_UPDATE: return "Update";
        case UMI_SETUP_MAINTENANCE_REPAIR: return "Repair";
        case UMI_SETUP_MAINTENANCE_REMOVE: return "Remove";
        default: return "Unknown";
    }
}
const char *UmiSetupMaintenanceChangeText(UmiSetupMaintenanceChange change)
{
    switch (change) {
        case UMI_SETUP_MAINTENANCE_KEEP: return "Keep";
        case UMI_SETUP_MAINTENANCE_WRITE: return "Install replacement";
        case UMI_SETUP_MAINTENANCE_RETIRE: return "Move to recovery archive";
        case UMI_SETUP_MAINTENANCE_PRESERVE_EDIT: return "Leave local edit in place";
        default: return "Unknown";
    }
}
int SmReserved(const char *relative)
{
    const char *slash = strchr(relative, '/');
    size_t count = slash != NULL ? (size_t)(slash - relative) : strlen(relative);
    char first[UMI_SETUP_RELATIVE_CAPACITY];
    if (count >= sizeof first) return 1;
    memcpy(first, relative, count); first[count] = '\0';
    return ScEqualFold(first, UMI_SETUP_MAINTENANCE_DIRECTORY) == 0 ||
        ScEqualFold(first, UMI_SETUP_MAINTENANCE_PENDING) == 0 ||
        ScEqualFold(first, UMI_SETUP_RECEIPT) == 0 || ScEqualFold(first, UMI_SETUP_CATALOGUE) == 0;
}
UmiStatus SmPath(const char *root, const char *child, char out[UMI_SETUP_PATH_CAPACITY])
{
    return ScJoin(root, child, out);
}
UmiStatus SmSlot(const char *root, const char *kind, size_t index, char out[UMI_SETUP_PATH_CAPACITY])
{
    char name[80];
    int count = snprintf(name, sizeof name, "%s/%zu", kind, index);
    if (count < 0 || (size_t)count >= sizeof name) return UMI_STATUS_CAPACITY_EXCEEDED;
    return SmPath(root, name, out);
}
UmiStatus SmSnapshot(const char *path, int *exists, uint64_t *bytes, char hash[65],
    UmiSetupProgress progress, void *context, UmiSetupReport *report)
{
    *exists = 0; *bytes = 0U; hash[0] = '\0';
    UmiStatus status = SmObserveRegular(path, report);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    status = ScDigest(path, hash, bytes, progress, context, report);
    if (status == UMI_STATUS_OK) *exists = 1;
    return status;
}
UmiStatus SmCheckSnapshot(const char *path, int exists, uint64_t bytes, const char *hash,
    UmiSetupProgress progress, void *context, UmiSetupReport *report)
{
    int actual = 0;
    uint64_t actualBytes = 0U;
    char actualHash[65];
    UmiStatus status = SmSnapshot(path, &actual, &actualBytes, actualHash, progress, context, report);
    if (status == UMI_STATUS_OK && (actual != exists || (exists &&
            (actualBytes != bytes || strcmp(hash, actualHash) != 0)))) {
        status = UMI_STATUS_INVALID_STATE;
        ScReport(report, status, "A reviewed file changed: %.220s", path);
    }
    return status;
}
static int FileOrder(const void *left, const void *right)
{
    const ScFile *a = *(const ScFile *const *)left, *b = *(const ScFile *const *)right;
    return ScEqualFold(a->relative, b->relative);
}
static const ScFile **Sorted(const UmiSetupBundle *bundle)
{
    size_t count = bundle != NULL ? bundle->fileCount : 0U;
    const ScFile **list = malloc((count != 0U ? count : 1U) * sizeof(*list));
    if (list == NULL) return NULL;
    for (size_t i = 0U; i < count; ++i) list[i] = &bundle->files[i];
    qsort(list, count, sizeof(*list), FileOrder);
    return list;
}
static const ScFile *Lookup(const ScFile *const *sorted, size_t count, const char *path)
{
    size_t left = 0U, right = count;
    while (left < right) {
        size_t middle = left + (right - left) / 2U;
        int order = ScEqualFold(sorted[middle]->relative, path);
        if (order == 0) return sorted[middle];
        if (order < 0) left = middle + 1U; else right = middle;
    }
    return NULL;
}
static const char *Owner(const UmiSetupBundle *bundle, const ScFile *file)
{
    return file->owner == SC_SHARED ? "shared" : bundle->apps[file->owner].id;
}
static UmiStatus Desired(UmiSetupMaintenancePlan *plan, UmiSetupBundle **out, UmiSetupReport *report)
{
    *out = NULL;
    uint64_t selected = 0U;
    const UmiSetupBundle *model = plan->before;
    if (plan->config.action == UMI_SETUP_MAINTENANCE_REMOVE) {
        if (!plan->config.removeMask || (plan->config.removeMask & ~UmiSetupAllApplications(plan->before)))
            return UMI_STATUS_INVALID_ARGUMENT;
        selected = UmiSetupAllApplications(plan->before) & ~plan->config.removeMask;
    } else {
        UmiStatus status = UmiSetupBundleOpen(plan->source, &plan->release, report);
        if (status != UMI_STATUS_OK) return status;
        strcpy(plan->sourceHash, plan->release->catalogueHash);
        for (size_t i = 0U; i < plan->before->appCount; ++i) {
            size_t j = 0U;
            for (; j < plan->release->appCount; ++j)
                if (strcmp(plan->before->apps[i].id, plan->release->apps[j].id) == 0) break;
            if (j == plan->release->appCount || strcmp(plan->before->apps[i].entry, plan->release->apps[j].entry) != 0) {
                ScReport(report, UMI_STATUS_INVALID_ARGUMENT, "The release must retain every installed application and its entry path.");
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            selected |= UINT64_C(1) << j;
        }
        if (plan->config.action == UMI_SETUP_MAINTENANCE_UPDATE) model = plan->release;
        else selected = UmiSetupAllApplications(plan->before);
    }
    if (selected == 0U) return UMI_STATUS_OK;
    ScText text;
    ScTextInit(&text);
    UmiStatus status;
    if (plan->config.action == UMI_SETUP_MAINTENANCE_REPAIR) {
        ScPrint(&text, "%s", plan->beforeText); status = text.status;
    } else status = ScEncode(model, selected, 1, &text);
    UmiSetupBundle *desired = NULL;
    if (status == UMI_STATUS_OK) {
        desired = calloc(1U, sizeof(*desired));
        if (desired == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK) status = ScDecode(text.data, text.size, desired, 1);
    if (status == UMI_STATUS_OK) {
        plan->afterText = text.data; plan->afterLength = text.size;
        text.data = NULL; *out = desired;
        status = UmiNativeSha256Buffer(plan->afterText, plan->afterLength, plan->summary.afterReceipt);
    } else UmiSetupBundleDestroy(desired);
    ScTextFree(&text);
    return status;
}
static UmiStatus AddRow(UmiSetupMaintenancePlan *plan, const ScFile *before,
    const ScFile *after, const UmiSetupBundle *desired, const ScFile *const *releaseFiles,
    UmiSetupProgress progress, void *context, UmiSetupReport *report)
{
    UmiSetupMaintenanceRow *row = &plan->rows[plan->rowCount];
    strcpy(row->relative, before != NULL ? before->relative : after->relative);
    if (SmReserved(row->relative) || (before != NULL && after != NULL && strcmp(before->relative, after->relative) != 0))
        return UMI_STATUS_INVALID_ARGUMENT;
    char path[UMI_SETUP_PATH_CAPACITY];
    UmiStatus status = SmPath(plan->root, row->relative, path);
    if (status == UMI_STATUS_OK) status = SmSnapshot(path, &row->beforeExists,
        &row->beforeBytes, row->beforeHash, progress, context, report);
    if (status != UMI_STATUS_OK) return status;
    if (before == NULL && row->beforeExists) {
        ScReport(report, UMI_STATUS_ALREADY_EXISTS, "An unowned file blocks this update: %.200s", row->relative);
        return UMI_STATUS_ALREADY_EXISTS;
    }
    row->modified = before != NULL && row->beforeExists &&
        (row->beforeBytes != before->bytes || strcmp(row->beforeHash, before->hash) != 0);
    if (plan->config.action == UMI_SETUP_MAINTENANCE_UPDATE && row->modified) {
        ScReport(report, UMI_STATUS_INVALID_STATE, "Update will not replace a local edit. Back up or repair it first: %.180s", row->relative);
        return UMI_STATUS_INVALID_STATE;
    }
    if (plan->config.action == UMI_SETUP_MAINTENANCE_REMOVE && after != NULL &&
        (!row->beforeExists || row->beforeBytes != after->bytes || strcmp(row->beforeHash, after->hash) != 0)) {
        ScReport(report, UMI_STATUS_INVALID_STATE, "Repair retained application files before removing another component: %.160s", row->relative);
        return UMI_STATUS_INVALID_STATE;
    }
    if (after != NULL) {
        row->afterExists = 1; row->afterBytes = after->bytes; strcpy(row->afterHash, after->hash);
        if (plan->release != NULL) {
            const ScFile *source = Lookup(releaseFiles, plan->release->fileCount, after->relative);
            if (source == NULL || strcmp(source->relative, after->relative) != 0 ||
                strcmp(Owner(plan->release, source), Owner(desired, after)) != 0 ||
                source->bytes != after->bytes || strcmp(source->hash, after->hash) != 0) {
                ScReport(report, UMI_STATUS_INVALID_STATE, "Repair requires the original recorded bytes and owners: %.180s", row->relative);
                return UMI_STATUS_INVALID_STATE;
            }
            char payload[UMI_SETUP_PATH_CAPACITY];
            status = SmPath(plan->source, "payload", payload);
            if (status == UMI_STATUS_OK) status = SmPath(payload, row->relative, path);
            if (status == UMI_STATUS_OK) status = SmCheckSnapshot(path, 1, row->afterBytes, row->afterHash, progress, context, report);
            if (status != UMI_STATUS_OK) return status;
        }
        row->change = row->beforeExists && row->beforeBytes == row->afterBytes &&
            strcmp(row->beforeHash, row->afterHash) == 0 ? UMI_SETUP_MAINTENANCE_KEEP : UMI_SETUP_MAINTENANCE_WRITE;
    } else if (row->modified) row->change = UMI_SETUP_MAINTENANCE_PRESERVE_EDIT;
    else row->change = row->beforeExists ? UMI_SETUP_MAINTENANCE_RETIRE : UMI_SETUP_MAINTENANCE_KEEP;
    if (row->change == UMI_SETUP_MAINTENANCE_WRITE) {
        if (row->afterBytes > UMI_SETUP_MAX_TOTAL_BYTES - plan->summary.stagingBytes) return UMI_STATUS_CAPACITY_EXCEEDED;
        plan->summary.stagingBytes += row->afterBytes; ++plan->summary.filesToWrite;
    } else if (row->change == UMI_SETUP_MAINTENANCE_RETIRE) ++plan->summary.filesToRetire;
    if (row->modified) ++plan->summary.modifiedFilesPreserved;
    ++plan->rowCount;
    return UMI_STATUS_OK;
}
UmiStatus UmiSetupMaintenancePlanCreate(const UmiSetupMaintenanceConfig *config,
    UmiSetupMaintenancePlan **out, UmiSetupProgress progress, void *context,
    UmiSetupMaintenanceReport *report)
{
    UmiSetupMaintenanceReport local = {0};
    if (report == NULL) report = &local;
    memset(report, 0, sizeof(*report));
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (config == NULL || UmiSetupValidateAbsolute(config->installationRoot) != UMI_STATUS_OK ||
        (config->expectedReceipt != NULL && !ScHashValid(config->expectedReceipt)) ||
        config->action < UMI_SETUP_MAINTENANCE_UPDATE || config->action > UMI_SETUP_MAINTENANCE_REMOVE ||
        (config->action != UMI_SETUP_MAINTENANCE_REMOVE &&
            (config->removeMask != 0U || UmiSetupValidateAbsolute(config->releaseRoot) != UMI_STATUS_OK)))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = SmCheckRoot(config->installationRoot, &report->io);
    if (status != UMI_STATUS_OK) return status;
    UmiSetupMaintenancePlan *plan = calloc(1U, sizeof(*plan));
    if (plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    plan->config = *config; strcpy(plan->root, config->installationRoot);
    if (config->expectedReceipt != NULL) {
        strcpy(plan->boundReceipt, config->expectedReceipt);
        plan->config.expectedReceipt = plan->boundReceipt;
    }
    /* Canonical separators keep a reviewed Windows root stable. */
    for (char *p = plan->root; *p != '\0'; ++p) if (*p == '\\') *p = '/';
    if (config->action != UMI_SETUP_MAINTENANCE_REMOVE) strcpy(plan->source, config->releaseRoot);
    for (char *p = plan->source; *p != '\0'; ++p) if (*p == '\\') *p = '/';
    plan->config.installationRoot = plan->root; plan->config.releaseRoot = plan->source;
    status = UmiSetupInstalledBundleOpen(plan->root, &plan->before, &report->io);
    char receipt[UMI_SETUP_PATH_CAPACITY];
    if (status == UMI_STATUS_OK) status = SmPath(plan->root, UMI_SETUP_RECEIPT, receipt);
    if (status == UMI_STATUS_OK) status = SmObserveRegular(receipt, &report->io);
    if (status == UMI_STATUS_OK) status = ScRead(receipt, UMI_SETUP_MANIFEST_LIMIT, &plan->beforeText, &plan->beforeLength, &report->io);
    if (status == UMI_STATUS_OK) status = UmiNativeSha256Buffer(plan->beforeText, plan->beforeLength, plan->summary.beforeReceipt);
    if (status == UMI_STATUS_OK && strcmp(plan->summary.beforeReceipt, plan->before->catalogueHash) != 0) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && plan->boundReceipt[0] != '\0' &&
        strcmp(plan->boundReceipt, plan->summary.beforeReceipt) != 0) {
        ScReport(&report->io, UMI_STATUS_INVALID_STATE, "The displayed installed application list is stale. Load it again before reviewing a selection.");
        status = UMI_STATUS_INVALID_STATE;
    }
    UmiSetupBundle *desired = NULL;
    if (status == UMI_STATUS_OK) status = Desired(plan, &desired, &report->io);
    const ScFile **beforeFiles = NULL, **afterFiles = NULL, **releaseFiles = NULL;
    if (status == UMI_STATUS_OK) {
        beforeFiles = Sorted(plan->before); afterFiles = Sorted(desired); releaseFiles = Sorted(plan->release);
        if (beforeFiles == NULL || afterFiles == NULL || releaseFiles == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    size_t afterCount = desired != NULL ? desired->fileCount : 0U;
    if (status == UMI_STATUS_OK) {
        plan->rowCapacity = plan->before->fileCount + afterCount;
        plan->rows = calloc(plan->rowCapacity != 0U ? plan->rowCapacity : 1U, sizeof(*plan->rows));
        if (plan->rows == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    size_t i = 0U, j = 0U;
    while (status == UMI_STATUS_OK && (i < plan->before->fileCount || j < afterCount)) {
        const ScFile *old = i < plan->before->fileCount ? beforeFiles[i] : NULL;
        const ScFile *next = j < afterCount ? afterFiles[j] : NULL;
        int order = old == NULL ? 1 : next == NULL ? -1 : ScEqualFold(old->relative, next->relative);
        status = AddRow(plan, order <= 0 ? old : NULL, order >= 0 ? next : NULL,
            desired, releaseFiles, progress, context, &report->io);
        if (order <= 0) ++i;
        if (order >= 0) ++j;
        if (status == UMI_STATUS_OK && progress != NULL) {
            ScReport(&report->io, UMI_STATUS_OK, "Reviewed %zu owned paths.", plan->rowCount);
            if (progress(&report->io, context)) status = UMI_STATUS_CANCELLED;
        }
    }
    if (status == UMI_STATUS_OK) {
        plan->summary.checkedFiles = plan->rowCount;
        plan->summary.applicationsBefore = plan->before->appCount;
        plan->summary.applicationsAfter = desired != NULL ? desired->appCount : 0U;
        plan->summary.noChange = plan->summary.filesToWrite == 0U && plan->summary.filesToRetire == 0U &&
            strcmp(plan->summary.beforeReceipt, plan->summary.afterReceipt) == 0;
        status = SmJournalWrite(plan);
    }
    free(beforeFiles); free(afterFiles); free(releaseFiles); UmiSetupBundleDestroy(desired);
    if (status != UMI_STATUS_OK) {
        if (report->io.detail[0] == '\0') ScReport(&report->io, status, "Maintenance review did not complete. No installed file was changed.");
        report->io.status = status; UmiSetupMaintenancePlanDestroy(plan); return status;
    }
    report->io.bytesPlanned = plan->summary.stagingBytes;
    ScReport(&report->io, UMI_STATUS_OK, "%s review: %zu replacements, %zu retired files, %zu local edits retained. Nothing changed yet.",
        UmiSetupMaintenanceActionText(config->action), plan->summary.filesToWrite,
        plan->summary.filesToRetire, plan->summary.modifiedFilesPreserved);
    *out = plan;
    return UMI_STATUS_OK;
}
void UmiSetupMaintenancePlanDestroy(UmiSetupMaintenancePlan *plan)
{
    if (plan == NULL) return;
    UmiSetupBundleDestroy(plan->before); UmiSetupBundleDestroy(plan->release);
    free(plan->rows); free(plan->beforeText); free(plan->afterText); free(plan->journal); free(plan);
}
const UmiSetupMaintenanceSummary *UmiSetupMaintenancePlanSummary(const UmiSetupMaintenancePlan *plan)
{
    return plan != NULL ? &plan->summary : NULL;
}
const UmiSetupMaintenanceRow *UmiSetupMaintenancePlanRow(const UmiSetupMaintenancePlan *plan, size_t index)
{
    return plan != NULL && index < plan->rowCount ? &plan->rows[index] : NULL;
}

const char *UmiSetupMaintenanceReceiptIdentity(const UmiSetupBundle *installed)
{
    return installed != NULL && installed->installed ? installed->catalogueHash : NULL;
}
