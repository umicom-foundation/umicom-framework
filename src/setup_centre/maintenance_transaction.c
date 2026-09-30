/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/setup_centre/maintenance_transaction.c
 * PURPOSE:
 *   A receipt describes one complete owned state. During a change a pending marker makes
 *   ordinary readers refuse that state. Originals are moved into an immutable journal before
 *   replacement; user extras are never enumerated.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/setup_centre/maintenance_transaction.c
 * A receipt describes one complete owned state. During a change a pending
 * marker makes ordinary readers refuse that state. Originals are moved into
 * an immutable journal before replacement; user extras are never enumerated.
 *---------------------------------------------------------------------------*/
#include "maintenance_internal.h"
#include <inttypes.h>

static UmiStatus Write(const char *root, const char *name, const void *bytes, size_t length, UmiSetupReport *report)
{
    char path[UMI_SETUP_PATH_CAPACITY];
    UmiStatus status = SmPath(root, name, path);
    if (status == UMI_STATUS_OK) status = ScWriteNew(path, bytes, length, report);
    if (status == UMI_STATUS_OK) status = SmSyncParent(path, report);
    return status;
}
static UmiStatus Read(const char *root, const char *name, size_t limit, char **text, size_t *length, UmiSetupReport *report)
{
    char path[UMI_SETUP_PATH_CAPACITY];
    UmiStatus status = SmPath(root, name, path);
    if (status == UMI_STATUS_OK) status = SmObserveRegular(path, report);
    if (status == UMI_STATUS_OK) status = ScRead(path, limit, text, length, report);
    return status;
}
static UmiStatus Pulse(UmiSetupMaintenanceReport *report, UmiSetupProgress progress,
    void *context, const char *phase)
{
    ScReport(&report->io, UMI_STATUS_OK, "%s", phase);
    return progress != NULL && progress(&report->io, context) ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}
static UmiStatus Move(const char *source, const char *destination, int exists,
    uint64_t bytes, const char *hash, UmiSetupReport *report)
{
    if (!exists) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = SmCheckSnapshot(source, 1, bytes, hash, NULL, NULL, report);
    if (status == UMI_STATUS_OK) status = SmCheckSnapshot(destination, 0, 0U, "", NULL, NULL, report);
    if (status == UMI_STATUS_OK) status = SmMoveNew(source, destination, report);
    if (status == UMI_STATUS_OK) status = SmCheckSnapshot(destination, 1, bytes, hash, NULL, NULL, report);
    return status;
}
static UmiStatus BaseLock(const char *root, char base[UMI_SETUP_PATH_CAPACITY], void **lock, UmiSetupReport *report)
{
    *lock = NULL;
    UmiStatus status = SmCheckRoot(root, report);
    if (status == UMI_STATUS_OK) status = SmPath(root, UMI_SETUP_MAINTENANCE_DIRECTORY, base);
    if (status != UMI_STATUS_OK) return status;
    status = ScFileRegular(base, 1, report);
    if (status == UMI_STATUS_NOT_FOUND) {
        status = ScMakeDirectory(base, report);
        if (status == UMI_STATUS_OK) status = SmSyncParent(base, report);
        if (status == UMI_STATUS_OK) status = Write(base, "owner.txt", SM_OWNER, strlen(SM_OWNER), report);
    }
    if (status != UMI_STATUS_OK) return status;
    status = SmCheckRoot(base, report);
    char *text = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK) status = Read(base, "owner.txt", 256U, &text, &length, report);
    if (status == UMI_STATUS_OK && (length != strlen(SM_OWNER) || memcmp(text, SM_OWNER, length) != 0)) status = UMI_STATUS_INVALID_STATE;
    free(text);
    char path[UMI_SETUP_PATH_CAPACITY];
    if (status == UMI_STATUS_OK) status = SmPath(base, "lock", path);
    if (status == UMI_STATUS_OK) status = SmLock(path, lock, report);
    return status;
}
static UmiStatus Marker(char out[192], const char *transaction, const char *hash, size_t *length)
{
    if (!ScHashValid(transaction) || !ScHashValid(hash)) return UMI_STATUS_INVALID_ARGUMENT;
    int count = snprintf(out, 192U, "UMICOM_MAINTENANCE_PENDING\t1\n%s\n%s\n", transaction, hash);
    if (count < 0 || count >= 192) return UMI_STATUS_INTERNAL_ERROR;
    *length = (size_t)count;
    return UMI_STATUS_OK;
}
static UmiStatus MarkerRead(const char *text, size_t length, char transaction[65], char hash[65])
{
    const char header[] = "UMICOM_MAINTENANCE_PENDING\t1\n";
    const size_t offset = sizeof header - 1U;
    if (length != offset + 130U || memcmp(text, header, offset) != 0 ||
        text[offset + 64U] != '\n' || text[offset + 129U] != '\n') return UMI_STATUS_PARSE_ERROR;
    memcpy(transaction, text + offset, 64U); transaction[64] = '\0';
    memcpy(hash, text + offset + 65U, 64U); hash[64] = '\0';
    return ScHashValid(transaction) && ScHashValid(hash) ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
static UmiStatus Pending(const char *root, char transaction[65], char hash[65], UmiSetupReport *report)
{
    char *text = NULL;
    size_t length = 0U;
    UmiStatus status = Read(root, UMI_SETUP_MAINTENANCE_PENDING, 192U, &text, &length, report);
    if (status == UMI_STATUS_OK) status = MarkerRead(text, length, transaction, hash);
    free(text); return status;
}
UmiStatus UmiSetupMaintenancePending(const char *root, UmiSetupMaintenanceReport *report)
{
    if (report == NULL || UmiSetupValidateAbsolute(root) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    memset(report, 0, sizeof(*report));
    char hash[65], base[UMI_SETUP_PATH_CAPACITY];
    UmiStatus status = Pending(root, report->transaction, hash, &report->io);
    if (status == UMI_STATUS_OK) status = SmPath(root, UMI_SETUP_MAINTENANCE_DIRECTORY, base);
    if (status == UMI_STATUS_OK) status = SmPath(base, report->transaction, report->archive);
    report->recoveryRequired = status == UMI_STATUS_OK;
    ScReport(&report->io, status, status == UMI_STATUS_OK ?
        "An unfinished maintenance transaction needs review and recovery." :
        status == UMI_STATUS_NOT_FOUND ? "No pending maintenance marker was found." : "The pending marker cannot be trusted or read.");
    return status;
}
static UmiStatus MakeArchive(const char *base, const char *fingerprint,
    UmiSetupMaintenanceReport *report)
{
    /* A repeated, unchanged plan still receives a different archive after an
     * earlier attempt. Exclusive mkdir, not time or a random guess, owns it. */
    for (unsigned attempt = 1U; attempt <= 65535U; ++attempt) {
        char seed[128];
        int count = snprintf(seed, sizeof seed, "Umicom maintenance attempt\n%s\n%u\n", fingerprint, attempt);
        if (count < 0 || (size_t)count >= sizeof seed) return UMI_STATUS_INTERNAL_ERROR;
        UmiStatus status = UmiNativeSha256Buffer(seed, (size_t)count, report->transaction);
        if (status == UMI_STATUS_OK) status = SmPath(base, report->transaction, report->archive);
        if (status == UMI_STATUS_OK) status = ScMakeDirectory(report->archive, &report->io);
        if (status == UMI_STATUS_ALREADY_EXISTS) continue;
        if (status != UMI_STATUS_OK) return status;
        report->io.outputCreated = 1;
        status = SmSyncParent(report->archive, &report->io);
        const char *directories[] = {"new", "old", "undone"};
        for (size_t i = 0U; i < 3U && status == UMI_STATUS_OK; ++i) {
            char path[UMI_SETUP_PATH_CAPACITY];
            status = SmPath(report->archive, directories[i], path);
            if (status == UMI_STATUS_OK) status = ScMakeDirectory(path, &report->io);
            if (status == UMI_STATUS_OK) status = SmSyncParent(path, &report->io);
        }
        return status;
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}
static UmiStatus CheckState(const UmiSetupMaintenancePlan *plan, int after,
    UmiSetupProgress progress, void *context, UmiSetupReport *report)
{
    for (size_t i = 0U; i < plan->rowCount; ++i) {
        const UmiSetupMaintenanceRow *row = &plan->rows[i];
        int previous = !after || row->change == UMI_SETUP_MAINTENANCE_PRESERVE_EDIT;
        char path[UMI_SETUP_PATH_CAPACITY];
        UmiStatus status = SmPath(plan->root, row->relative, path);
        if (status == UMI_STATUS_OK) status = SmCheckSnapshot(path,
            previous ? row->beforeExists : row->afterExists,
            previous ? row->beforeBytes : row->afterBytes,
            previous ? row->beforeHash : row->afterHash, progress, context, report);
        if (status != UMI_STATUS_OK) return status;
    }
    char receipt[UMI_SETUP_PATH_CAPACITY];
    UmiStatus status = SmPath(plan->root, UMI_SETUP_RECEIPT, receipt);
    if (status == UMI_STATUS_OK) status = SmCheckSnapshot(receipt,
        !after || plan->afterLength != 0U, after ? (uint64_t)plan->afterLength : (uint64_t)plan->beforeLength,
        after ? plan->summary.afterReceipt : plan->summary.beforeReceipt, progress, context, report);
    return status;
}
UmiStatus UmiSetupMaintenanceApply(UmiSetupMaintenancePlan *plan, const char *expected,
    UmiSetupProgress progress, void *context, UmiSetupMaintenanceReport *report)
{
    UmiSetupMaintenanceReport local = {0};
    if (report == NULL) report = &local;
    memset(report, 0, sizeof(*report));
    if (plan == NULL || plan->attempted || !ScHashValid(expected)) return UMI_STATUS_INVALID_ARGUMENT;
    plan->attempted = 1;
    if (strcmp(expected, plan->summary.fingerprint) != 0) return UMI_STATUS_INVALID_STATE;
    char base[UMI_SETUP_PATH_CAPACITY];
    void *lock = NULL;
    UmiStatus status = BaseLock(plan->root, base, &lock, &report->io);
    UmiSetupMaintenancePlan *fresh = NULL;
    UmiSetupMaintenanceReport reviewed = {0};
    if (status == UMI_STATUS_OK) status = UmiSetupMaintenancePlanCreate(&plan->config, &fresh, progress, context, &reviewed);
    if (status == UMI_STATUS_OK && strcmp(fresh->summary.fingerprint, expected) != 0) status = UMI_STATUS_INVALID_STATE;
    UmiSetupMaintenancePlanDestroy(fresh);
    int pending = 0;
    if (status != UMI_STATUS_OK) goto finish;
    if (plan->summary.noChange) { report->io.completed = 1; goto finish; }
    uint64_t required = plan->summary.stagingBytes + (uint64_t)plan->journalLength +
        (uint64_t)plan->beforeLength + 2U * (uint64_t)plan->afterLength + UINT64_C(1048576);
    status = ScFreeSpace(plan->root, required, &report->io);
    if (status == UMI_STATUS_OK) status = MakeArchive(base, expected, report);
    if (status != UMI_STATUS_OK) goto finish;
    report->io.bytesPlanned = plan->summary.stagingBytes;
    for (size_t i = 0U; i < plan->rowCount && status == UMI_STATUS_OK; ++i) {
        const UmiSetupMaintenanceRow *row = &plan->rows[i];
        if (row->change != UMI_SETUP_MAINTENANCE_WRITE) continue;
        char payload[UMI_SETUP_PATH_CAPACITY], source[UMI_SETUP_PATH_CAPACITY], target[UMI_SETUP_PATH_CAPACITY];
        status = SmPath(plan->source, "payload", payload);
        if (status == UMI_STATUS_OK) status = SmPath(payload, row->relative, source);
        if (status == UMI_STATUS_OK) status = SmSlot(report->archive, "new", i, target);
        if (status == UMI_STATUS_OK) status = ScCopy(source, target, row->afterHash, row->afterBytes, progress, context, &report->io);
        if (status == UMI_STATUS_OK) status = SmSyncParent(target, &report->io);
    }
    if (status == UMI_STATUS_OK) status = Write(report->archive, "before.umi", plan->beforeText, plan->beforeLength, &report->io);
    if (status == UMI_STATUS_OK && plan->afterLength != 0U) status = Write(report->archive, "after.umi", plan->afterText, plan->afterLength, &report->io);
    if (status == UMI_STATUS_OK && plan->afterLength != 0U) status = Write(report->archive, "publish.umi", plan->afterText, plan->afterLength, &report->io);
    if (status == UMI_STATUS_OK) status = Write(report->archive, "plan.umi", plan->journal, plan->journalLength, &report->io);
    char marker[192], ready[UMI_SETUP_PATH_CAPACITY], active[UMI_SETUP_PATH_CAPACITY];
    size_t markerLength = 0U;
    if (status == UMI_STATUS_OK) status = Marker(marker, report->transaction, expected, &markerLength);
    if (status == UMI_STATUS_OK) status = Write(report->archive, "identity.umi", marker, markerLength, &report->io);
    if (status == UMI_STATUS_OK) status = Write(report->archive, "activate.pending", marker, markerLength, &report->io);
    if (status == UMI_STATUS_OK) status = CheckState(plan, 0, progress, context, &report->io);
    if (status == UMI_STATUS_OK) status = Pulse(report, progress, context, "All replacements staged; installed files are still unchanged.");
    if (status == UMI_STATUS_OK) status = SmPath(report->archive, "activate.pending", ready);
    if (status == UMI_STATUS_OK) status = SmPath(plan->root, UMI_SETUP_MAINTENANCE_PENDING, active);
    if (status == UMI_STATUS_OK) {
        status = SmMoveNew(ready, active, &report->io);
        pending = ScFileRegular(active, 0, NULL) == UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK) status = Pulse(report, progress, context, "Maintenance pending; ordinary receipt readers are now blocked.");
    char receipt[UMI_SETUP_PATH_CAPACITY], backup[UMI_SETUP_PATH_CAPACITY];
    if (status == UMI_STATUS_OK) status = SmPath(plan->root, UMI_SETUP_RECEIPT, receipt);
    if (status == UMI_STATUS_OK) status = SmPath(report->archive, "old-receipt.umi", backup);
    if (status == UMI_STATUS_OK) status = Move(receipt, backup, 1, (uint64_t)plan->beforeLength, plan->summary.beforeReceipt, &report->io);
    if (status == UMI_STATUS_OK) status = Pulse(report, progress, context, "Previous receipt archived.");
    for (size_t i = 0U; i < plan->rowCount && status == UMI_STATUS_OK; ++i) {
        const UmiSetupMaintenanceRow *row = &plan->rows[i];
        if (row->change != UMI_SETUP_MAINTENANCE_WRITE && row->change != UMI_SETUP_MAINTENANCE_RETIRE) continue;
        char live[UMI_SETUP_PATH_CAPACITY], old[UMI_SETUP_PATH_CAPACITY], replacement[UMI_SETUP_PATH_CAPACITY];
        status = SmPath(plan->root, row->relative, live);
        if (status == UMI_STATUS_OK) status = SmCheckSnapshot(live, row->beforeExists, row->beforeBytes, row->beforeHash, NULL, NULL, &report->io);
        if (status == UMI_STATUS_OK && row->beforeExists) {
            status = SmSlot(report->archive, "old", i, old);
            if (status == UMI_STATUS_OK) status = Move(live, old, 1, row->beforeBytes, row->beforeHash, &report->io);
            if (status == UMI_STATUS_OK) status = Pulse(report, progress, context, "Original file archived.");
        }
        if (status == UMI_STATUS_OK && row->change == UMI_SETUP_MAINTENANCE_WRITE) {
            status = ScParents(plan->root, row->relative, &report->io);
            if (status == UMI_STATUS_OK) status = SmSlot(report->archive, "new", i, replacement);
            if (status == UMI_STATUS_OK) status = Move(replacement, live, 1, row->afterBytes, row->afterHash, &report->io);
            if (status == UMI_STATUS_OK) status = Pulse(report, progress, context, "Replacement file installed.");
        }
        if (status == UMI_STATUS_OK) ++report->io.filesCompleted;
    }
    /* The before/after payload is checked before the receipt can become visible. */
    if (status == UMI_STATUS_OK) {
        for (size_t i = 0U; i < plan->rowCount && status == UMI_STATUS_OK; ++i) {
            const UmiSetupMaintenanceRow *row = &plan->rows[i];
            int preserve = row->change == UMI_SETUP_MAINTENANCE_PRESERVE_EDIT;
            char live[UMI_SETUP_PATH_CAPACITY];
            status = SmPath(plan->root, row->relative, live);
            if (status == UMI_STATUS_OK) status = SmCheckSnapshot(live,
                preserve ? row->beforeExists : row->afterExists, preserve ? row->beforeBytes : row->afterBytes,
                preserve ? row->beforeHash : row->afterHash, progress, context, &report->io);
        }
    }
    if (status == UMI_STATUS_OK && plan->afterLength != 0U) {
        status = SmPath(report->archive, "publish.umi", ready);
        if (status == UMI_STATUS_OK) status = Move(ready, receipt, 1, (uint64_t)plan->afterLength, plan->summary.afterReceipt, &report->io);
    }
    if (status == UMI_STATUS_OK) status = Pulse(report, progress, context, "New receipt published; completion marker is next.");
    if (status == UMI_STATUS_OK) status = CheckState(plan, 1, progress, context, &report->io);
    if (status == UMI_STATUS_OK) status = SmPath(report->archive, "committed.pending", ready);
    if (status == UMI_STATUS_OK) {
        status = SmMoveNew(active, ready, &report->io);
        pending = ScFileRegular(active, 0, NULL) == UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK) {
        report->io.completed = 1;
        report->allApplicationsRemoved = plan->afterLength == 0U;
    }
finish:
    report->recoveryRequired = pending;
    SmUnlock(lock);
    if (status == UMI_STATUS_OK) ScReport(&report->io, status,
        plan->summary.noChange ? "The installed files already agree. No payload changed." :
        report->allApplicationsRemoved ? "All application ownership removed. Personal extras and recovery archives remain; no recursive deletion occurred." :
        "Maintenance completed. Original files remain in the recovery archive. No application or database was started.");
    else if (pending) ScReport(&report->io, status, "Maintenance stopped (%s). Recover transaction %.64s before using this installation.", UmiSetupStatusText(status), report->transaction);
    else if (report->io.outputCreated) ScReport(&report->io, status, "Maintenance did not complete. Its archive is retained; inspect the pending state before retrying.");
    else ScReport(&report->io, status, "Maintenance did not start (%s). Review the current files again.", UmiSetupStatusText(status));
    return status;
}

/* Recovery decodes the same immutable plan. It never accepts commands or a
 * source pathname from the journal; only checked, receipt-owned destinations. */
static UmiStatus LoadPlan(const char *root, const char *transaction, const char *archive,
    UmiSetupMaintenancePlan **out, char marker[192], size_t *markerLength, UmiSetupReport *report)
{
    char *identity = NULL, *text = NULL;
    size_t identityLength = 0U, textLength = 0U;
    char id[65], expected[65], hash[65];
    UmiStatus status = Read(archive, "identity.umi", 192U, &identity, &identityLength, report);
    if (status == UMI_STATUS_OK) status = MarkerRead(identity, identityLength, id, expected);
    if (status == UMI_STATUS_OK && strcmp(id, transaction) != 0) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = Read(archive, "plan.umi", UMI_SETUP_MANIFEST_LIMIT, &text, &textLength, report);
    if (status == UMI_STATUS_OK) status = UmiNativeSha256Buffer(text, textLength, hash);
    if (status == UMI_STATUS_OK && strcmp(hash, expected) != 0) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = SmJournalRead(text, textLength, out);
    if (status == UMI_STATUS_OK && strcmp((*out)->root, root) != 0) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) { memcpy(marker, identity, identityLength); *markerLength = identityLength; }
    free(identity); free(text);
    return status;
}
static const UmiSetupMaintenanceRow *FindRow(const UmiSetupMaintenancePlan *plan, const char *relative)
{
    size_t left = 0U, right = plan->rowCount;
    while (left < right) {
        size_t middle = left + (right - left) / 2U;
        int order = ScEqualFold(plan->rows[middle].relative, relative);
        if (order == 0) return strcmp(plan->rows[middle].relative, relative) == 0 ? &plan->rows[middle] : NULL;
        if (order < 0) left = middle + 1U; else right = middle;
    }
    return NULL;
}
static UmiStatus Receipts(UmiSetupMaintenancePlan *plan, const char *archive, UmiSetupReport *report)
{
    size_t before = 0U, after = 0U;
    UmiStatus status = Read(archive, "before.umi", UMI_SETUP_MANIFEST_LIMIT, &plan->beforeText, &before, report);
    char hash[65];
    if (status == UMI_STATUS_OK) status = UmiNativeSha256Buffer(plan->beforeText, before, hash);
    if (status == UMI_STATUS_OK && (before != plan->beforeLength || strcmp(hash, plan->summary.beforeReceipt) != 0)) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && plan->afterLength != 0U) {
        status = Read(archive, "after.umi", UMI_SETUP_MANIFEST_LIMIT, &plan->afterText, &after, report);
        if (status == UMI_STATUS_OK) status = UmiNativeSha256Buffer(plan->afterText, after, hash);
        if (status == UMI_STATUS_OK && (after != plan->afterLength || strcmp(hash, plan->summary.afterReceipt) != 0)) status = UMI_STATUS_INVALID_STATE;
    }
    UmiSetupBundle *desired = NULL;
    if (status == UMI_STATUS_OK) {
        plan->before = calloc(1U, sizeof(*plan->before));
        desired = calloc(1U, sizeof(*desired));
        if (plan->before == NULL || desired == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK) status = ScDecode(plan->beforeText, before, plan->before, 1);
    if (status == UMI_STATUS_OK && after != 0U) status = ScDecode(plan->afterText, after, desired, 1);
    unsigned char *seen = NULL;
    if (status == UMI_STATUS_OK) {
        seen = calloc(plan->rowCount, 1U);
        if (seen == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < plan->before->fileCount; ++i) {
        const ScFile *file = &plan->before->files[i];
        const UmiSetupMaintenanceRow *row = FindRow(plan, file->relative);
        if (row == NULL) { status = UMI_STATUS_PARSE_ERROR; break; }
        seen[(size_t)(row - plan->rows)] |= 1U;
        int modified = row->beforeExists && (row->beforeBytes != file->bytes || strcmp(row->beforeHash, file->hash) != 0);
        if (modified != row->modified) status = UMI_STATUS_PARSE_ERROR;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < desired->fileCount; ++i) {
        const ScFile *file = &desired->files[i];
        const UmiSetupMaintenanceRow *row = FindRow(plan, file->relative);
        if (row == NULL || !row->afterExists || row->afterBytes != file->bytes || strcmp(row->afterHash, file->hash) != 0) { status = UMI_STATUS_PARSE_ERROR; break; }
        seen[(size_t)(row - plan->rows)] |= 2U;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < plan->rowCount; ++i) {
        const UmiSetupMaintenanceRow *row = &plan->rows[i];
        if (seen[i] == 0U || (((seen[i] & 2U) != 0U) != !!row->afterExists) ||
            (!(seen[i] & 1U) && (row->beforeExists || row->modified))) status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK && plan->config.action == UMI_SETUP_MAINTENANCE_REPAIR &&
        (before != after || memcmp(plan->beforeText, plan->afterText, before) != 0)) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && plan->config.action == UMI_SETUP_MAINTENANCE_REMOVE) {
        if ((plan->config.removeMask & ~UmiSetupAllApplications(plan->before)) != 0U) status = UMI_STATUS_PARSE_ERROR;
        uint64_t keep = UmiSetupAllApplications(plan->before) & ~plan->config.removeMask;
        if (status == UMI_STATUS_OK && keep != 0U) {
            ScText encoded; ScTextInit(&encoded);
            status = ScEncode(plan->before, keep, 1, &encoded);
            if (status == UMI_STATUS_OK && (encoded.size != after || memcmp(encoded.data, plan->afterText, after) != 0)) status = UMI_STATUS_PARSE_ERROR;
            ScTextFree(&encoded);
        } else if (status == UMI_STATUS_OK && after != 0U) status = UMI_STATUS_PARSE_ERROR;
    }
    free(seen); UmiSetupBundleDestroy(desired);
    return status;
}
static int Equal(int present, uint64_t bytes, const char *hash, int expected, uint64_t expectedBytes, const char *expectedHash)
{
    return present == expected && (!present || (bytes == expectedBytes && strcmp(hash, expectedHash) == 0));
}
/* A row can be before, between its two moves, after, or already restored.
 * Anything else is a conflict. Each branch leaves every unexpected byte alone. */
static UmiStatus RestoreRow(const UmiSetupMaintenanceRow *row, const char *live,
    const char *backup, const char *undone, int apply, UmiSetupReport *report)
{
    int liveExists, oldExists, undoExists;
    uint64_t liveBytes, oldBytes, undoBytes;
    char liveHash[65], oldHash[65], undoHash[65];
    UmiStatus status = SmSnapshot(live, &liveExists, &liveBytes, liveHash, NULL, NULL, report);
    if (status == UMI_STATUS_OK) status = SmSnapshot(backup, &oldExists, &oldBytes, oldHash, NULL, NULL, report);
    if (status == UMI_STATUS_OK) status = SmSnapshot(undone, &undoExists, &undoBytes, undoHash, NULL, NULL, report);
    if (status != UMI_STATUS_OK) return status;
    if (oldExists && !Equal(oldExists, oldBytes, oldHash, row->beforeExists, row->beforeBytes, row->beforeHash)) return UMI_STATUS_INVALID_STATE;
    if (undoExists && !Equal(undoExists, undoBytes, undoHash, row->afterExists, row->afterBytes, row->afterHash)) return UMI_STATUS_INVALID_STATE;
    if (Equal(liveExists, liveBytes, liveHash, row->beforeExists, row->beforeBytes, row->beforeHash) && !oldExists)
        return UMI_STATUS_OK; /* Before, or a previously restored row. */
    if (liveExists) {
        if (!row->afterExists || undoExists ||
            !Equal(liveExists, liveBytes, liveHash, 1, row->afterBytes, row->afterHash) ||
            (row->beforeExists && !oldExists)) return UMI_STATUS_INVALID_STATE;
        if (apply) {
            status = Move(live, undone, 1, row->afterBytes, row->afterHash, report);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    if (row->beforeExists) {
        if (!oldExists) return UMI_STATUS_INVALID_STATE;
        if (apply) return Move(backup, live, 1, row->beforeBytes, row->beforeHash, report);
    } else if (oldExists) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
static UmiStatus RestorePass(const UmiSetupMaintenancePlan *plan, const char *archive, int apply,
    UmiSetupProgress progress, void *context, UmiSetupMaintenanceReport *report)
{
    for (size_t reverse = plan->rowCount; reverse != 0U; --reverse) {
        size_t i = reverse - 1U;
        const UmiSetupMaintenanceRow *row = &plan->rows[i];
        char live[UMI_SETUP_PATH_CAPACITY], old[UMI_SETUP_PATH_CAPACITY], undone[UMI_SETUP_PATH_CAPACITY];
        UmiStatus status = SmPath(plan->root, row->relative, live);
        if (status != UMI_STATUS_OK) return status;
        if (row->change == UMI_SETUP_MAINTENANCE_KEEP || row->change == UMI_SETUP_MAINTENANCE_PRESERVE_EDIT)
            status = SmCheckSnapshot(live, row->beforeExists, row->beforeBytes, row->beforeHash, NULL, NULL, &report->io);
        else {
            status = SmSlot(archive, "old", i, old);
            if (status == UMI_STATUS_OK) status = SmSlot(archive, "undone", i, undone);
            if (status == UMI_STATUS_OK) status = RestoreRow(row, live, old, undone, apply, &report->io);
        }
        if (status != UMI_STATUS_OK) return status;
        if (apply) {
            ++report->io.filesCompleted;
            status = Pulse(report, progress, context, "Previous file state restored.");
            if (status != UMI_STATUS_OK) return status;
        }
    }
    UmiSetupMaintenanceRow receipt = {0};
    receipt.beforeExists = 1; receipt.beforeBytes = (uint64_t)plan->beforeLength;
    strcpy(receipt.beforeHash, plan->summary.beforeReceipt);
    receipt.afterExists = plan->afterLength != 0U; receipt.afterBytes = (uint64_t)plan->afterLength;
    strcpy(receipt.afterHash, plan->summary.afterReceipt);
    char live[UMI_SETUP_PATH_CAPACITY], old[UMI_SETUP_PATH_CAPACITY], undone[UMI_SETUP_PATH_CAPACITY];
    UmiStatus status = SmPath(plan->root, UMI_SETUP_RECEIPT, live);
    if (status == UMI_STATUS_OK) status = SmPath(archive, "old-receipt.umi", old);
    if (status == UMI_STATUS_OK) status = SmPath(archive, "undone-receipt.umi", undone);
    if (status == UMI_STATUS_OK) status = RestoreRow(&receipt, live, old, undone, apply, &report->io);
    return status;
}
UmiStatus UmiSetupMaintenanceRestore(const char *inputRoot, const char *transaction,
    int pendingOnly, UmiSetupProgress progress, void *context, UmiSetupMaintenanceReport *report)
{
    UmiSetupMaintenanceReport local = {0};
    if (report == NULL) report = &local;
    memset(report, 0, sizeof(*report));
    if (UmiSetupValidateAbsolute(inputRoot) != UMI_STATUS_OK || !ScHashValid(transaction)) return UMI_STATUS_INVALID_ARGUMENT;
    char root[UMI_SETUP_PATH_CAPACITY], base[UMI_SETUP_PATH_CAPACITY];
    strcpy(root, inputRoot);
    for (char *p = root; *p != '\0'; ++p) if (*p == '\\') *p = '/';
    strcpy(report->transaction, transaction);
    void *lock = NULL;
    UmiStatus status = BaseLock(root, base, &lock, &report->io);
    if (status == UMI_STATUS_OK) status = SmPath(base, transaction, report->archive);
    UmiSetupMaintenancePlan *plan = NULL;
    char marker[192]; size_t markerLength = 0U;
    if (status == UMI_STATUS_OK) status = LoadPlan(root, transaction, report->archive, &plan, marker, &markerLength, &report->io);
    if (status == UMI_STATUS_OK) status = Receipts(plan, report->archive, &report->io);
    char id[65], hash[65], active[UMI_SETUP_PATH_CAPACITY], finished[UMI_SETUP_PATH_CAPACITY];
    int pending = 0;
    if (status == UMI_STATUS_OK) {
        UmiStatus observed = Pending(root, id, hash, &report->io);
        if (observed == UMI_STATUS_OK) {
            pending = 1;
            if (strcmp(id, transaction) != 0 || strcmp(hash, plan->summary.fingerprint) != 0) status = UMI_STATUS_BUSY;
        } else if (observed == UMI_STATUS_NOT_FOUND && !pendingOnly) {
            char *completed = NULL; size_t length = 0U;
            status = Read(report->archive, "committed.pending", 192U, &completed, &length, &report->io);
            if (status == UMI_STATUS_OK && (length != markerLength || memcmp(completed, marker, length) != 0)) status = UMI_STATUS_INVALID_STATE;
            free(completed);
            if (status == UMI_STATUS_OK) status = CheckState(plan, 1, progress, context, &report->io);
            /* No automatic reversal of a newer or locally modified state. */
            if (status == UMI_STATUS_OK) status = RestorePass(plan, report->archive, 0, NULL, NULL, report);
            char ready[UMI_SETUP_PATH_CAPACITY];
            if (status == UMI_STATUS_OK) status = SmPath(report->archive, "restore.pending", ready);
            if (status == UMI_STATUS_OK) status = Write(report->archive, "restore.pending", marker, markerLength, &report->io);
            if (status == UMI_STATUS_OK) status = SmPath(root, UMI_SETUP_MAINTENANCE_PENDING, active);
            if (status == UMI_STATUS_OK) {
                status = SmMoveNew(ready, active, &report->io);
                pending = ScFileRegular(active, 0, NULL) == UMI_STATUS_OK;
            }
        } else status = observed;
    }
    if (status == UMI_STATUS_OK) status = RestorePass(plan, report->archive, 0, NULL, NULL, report);
    if (status == UMI_STATUS_OK) status = Pulse(report, progress, context, "Recovery checked; unexpected local edits will not be overwritten.");
    if (status == UMI_STATUS_OK) status = RestorePass(plan, report->archive, 1, progress, context, report);
    if (status == UMI_STATUS_OK) status = CheckState(plan, 0, progress, context, &report->io);
    if (status == UMI_STATUS_OK) status = SmPath(root, UMI_SETUP_MAINTENANCE_PENDING, active);
    if (status == UMI_STATUS_OK) status = SmPath(report->archive, "restored.pending", finished);
    if (status == UMI_STATUS_OK) {
        status = SmMoveNew(active, finished, &report->io);
        pending = ScFileRegular(active, 0, NULL) == UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK) { report->restored = 1; report->io.completed = 1; }
    report->recoveryRequired = pending;
    UmiSetupMaintenancePlanDestroy(plan); SmUnlock(lock);
    ScReport(&report->io, status, status == UMI_STATUS_OK ?
        "Previous file snapshot restored. Archives remain. Verify integrity separately; the previous state may already have been damaged." :
        pending ? "Recovery stopped. The pending marker and all available files remain; resolve the conflict before retrying recovery." :
        "Recovery refused or not needed. No unrecognised file was overwritten.");
    return status;
}
