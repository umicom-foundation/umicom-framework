/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/setup_centre/maintenance_codec.c
 * A closed journal format: no commands, interpolation or arbitrary source paths.
 * Its hash detects damage; it does not authenticate a publisher or the user.
 *---------------------------------------------------------------------------*/
#include "maintenance_internal.h"
#include <inttypes.h>

UmiStatus SmJournalWrite(UmiSetupMaintenancePlan *plan)
{
    ScText text;
    ScTextInit(&text);
    ScPrint(&text, "UMICOM_MAINTENANCE\t1\nroot\t%s\nsource\t%s\nsource-hash\t%s\naction\t%u\nremove\t%" PRIu64
        "\nbefore\t%zu\t%s\nafter\t%zu\t%s\nrows\t%zu\n", plan->root,
        plan->source[0] != '\0' ? plan->source : "-", plan->sourceHash[0] != '\0' ? plan->sourceHash : "-",
        (unsigned)plan->config.action, plan->config.removeMask,
        plan->beforeLength, plan->summary.beforeReceipt, plan->afterLength,
        plan->summary.afterReceipt[0] != '\0' ? plan->summary.afterReceipt : "-", plan->rowCount);
    for (size_t i = 0U; i < plan->rowCount; ++i) {
        const UmiSetupMaintenanceRow *row = &plan->rows[i];
        ScPrint(&text, "file\t%u\t%d\t%" PRIu64 "\t%s\t%d\t%" PRIu64 "\t%s\t%d\t%s\n",
            (unsigned)row->change, row->beforeExists, row->beforeBytes,
            row->beforeExists ? row->beforeHash : "-", row->afterExists, row->afterBytes,
            row->afterExists ? row->afterHash : "-", row->modified, row->relative);
    }
    UmiStatus status = text.status;
    if (status == UMI_STATUS_OK) status = UmiNativeSha256Buffer(text.data, text.size, plan->summary.fingerprint);
    if (status == UMI_STATUS_OK) {
        free(plan->journal); plan->journal = text.data; plan->journalLength = text.size; text.data = NULL;
    }
    ScTextFree(&text);
    return status;
}
static int Fields(char **cursor, const char *name, char **fields, size_t count)
{
    if (*cursor == NULL || **cursor == '\0') return 0;
    char *line = *cursor, *end = strchr(line, '\n');
    if (end == NULL) return 0;
    *end = '\0'; *cursor = end + 1;
    for (size_t i = 0U; i < count; ++i) {
        char *tab = strchr(line, '\t');
        fields[i] = line;
        if (i + 1U < count) {
            if (tab == NULL) return 0;
            *tab = '\0'; line = tab + 1;
        } else if (tab != NULL) return 0;
    }
    return strcmp(fields[0], name) == 0;
}
static int Number(const char *text, uint64_t maximum, uint64_t *out)
{
    return ScNumber(text, out) && *out <= maximum;
}
static int RowValid(const UmiSetupMaintenanceRow *row, UmiSetupMaintenanceAction action)
{
    if (UmiSetupValidateRelative(row->relative) != UMI_STATUS_OK || SmReserved(row->relative)) return 0;
    if ((!row->beforeExists && (row->beforeBytes != 0U || row->beforeHash[0] != '\0' || row->modified)) ||
        (!row->afterExists && (row->afterBytes != 0U || row->afterHash[0] != '\0'))) return 0;
    if (action == UMI_SETUP_MAINTENANCE_UPDATE && row->modified) return 0;
    int same = row->beforeExists == row->afterExists && row->beforeBytes == row->afterBytes &&
        strcmp(row->beforeHash, row->afterHash) == 0;
    switch (row->change) {
        case UMI_SETUP_MAINTENANCE_KEEP: return same;
        case UMI_SETUP_MAINTENANCE_WRITE: return row->afterExists && !same && action != UMI_SETUP_MAINTENANCE_REMOVE;
        case UMI_SETUP_MAINTENANCE_RETIRE: return row->beforeExists && !row->afterExists && !row->modified && action != UMI_SETUP_MAINTENANCE_REPAIR;
        case UMI_SETUP_MAINTENANCE_PRESERVE_EDIT:
            return row->beforeExists && !row->afterExists && row->modified && action == UMI_SETUP_MAINTENANCE_REMOVE;
        default: return 0;
    }
}
UmiStatus SmJournalRead(const char *text, size_t length, UmiSetupMaintenancePlan **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    const char header[] = "UMICOM_MAINTENANCE\t1\n";
    if (text == NULL || length < sizeof header || length > UMI_SETUP_MANIFEST_LIMIT ||
        memcmp(text, header, sizeof header - 1U) != 0 || text[length - 1U] != '\n' || memchr(text, 0, length) != NULL)
        return UMI_STATUS_PARSE_ERROR;
    char *copy = malloc(length + 1U);
    UmiSetupMaintenancePlan *plan = calloc(1U, sizeof(*plan));
    if (copy == NULL || plan == NULL) { free(copy); free(plan); return UMI_STATUS_OUT_OF_MEMORY; }
    memcpy(copy, text, length); copy[length] = '\0';
    char *cursor = copy + sizeof header - 1U, *field[10];
    uint64_t value = 0U;
    UmiStatus status = UMI_STATUS_PARSE_ERROR;
    if (!Fields(&cursor, "root", field, 2U) || UmiSetupValidateAbsolute(field[1]) != UMI_STATUS_OK) goto done;
    strcpy(plan->root, field[1]);
    if (!Fields(&cursor, "source", field, 2U)) goto done;
    if (strcmp(field[1], "-") != 0) {
        if (UmiSetupValidateAbsolute(field[1]) != UMI_STATUS_OK) goto done;
        strcpy(plan->source, field[1]);
    }
    if (!Fields(&cursor, "source-hash", field, 2U)) goto done;
    if (strcmp(field[1], "-") != 0) {
        if (!ScHashValid(field[1])) goto done;
        strcpy(plan->sourceHash, field[1]);
    }
    if (!Fields(&cursor, "action", field, 2U) || !Number(field[1], 3U, &value) || value == 0U) goto done;
    plan->config.action = (UmiSetupMaintenanceAction)value;
    if ((plan->config.action == UMI_SETUP_MAINTENANCE_REMOVE) != (plan->source[0] == '\0') ||
        (plan->source[0] == '\0') != (plan->sourceHash[0] == '\0')) goto done;
    if (!Fields(&cursor, "remove", field, 2U) || !Number(field[1], UINT64_MAX, &plan->config.removeMask) ||
        (plan->config.action == UMI_SETUP_MAINTENANCE_REMOVE) != (plan->config.removeMask != 0U)) goto done;
    if (!Fields(&cursor, "before", field, 3U) || !Number(field[1], UMI_SETUP_MANIFEST_LIMIT, &value) ||
        value == 0U || !ScHashValid(field[2])) goto done;
    plan->beforeLength = (size_t)value; strcpy(plan->summary.beforeReceipt, field[2]);
    if (!Fields(&cursor, "after", field, 3U) || !Number(field[1], UMI_SETUP_MANIFEST_LIMIT, &value)) goto done;
    plan->afterLength = (size_t)value;
    if (value == 0U) { if (strcmp(field[2], "-") != 0 || plan->config.action != UMI_SETUP_MAINTENANCE_REMOVE) goto done; }
    else { if (!ScHashValid(field[2])) goto done; strcpy(plan->summary.afterReceipt, field[2]); }
    if (!Fields(&cursor, "rows", field, 2U) || !Number(field[1], UMI_SETUP_MAINTENANCE_MAX_ROWS, &value) || value == 0U) goto done;
    plan->rowCount = plan->rowCapacity = (size_t)value;
    plan->rows = calloc(plan->rowCount, sizeof(*plan->rows));
    if (plan->rows == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    for (size_t i = 0U; i < plan->rowCount; ++i) {
        UmiSetupMaintenanceRow *row = &plan->rows[i];
        if (!Fields(&cursor, "file", field, 10U) || !Number(field[1], 3U, &value)) goto done;
        row->change = (UmiSetupMaintenanceChange)value;
        if (!Number(field[2], 1U, &value)) goto done;
        row->beforeExists = (int)value;
        if (!Number(field[3], UMI_SETUP_MAX_FILE_BYTES, &row->beforeBytes)) goto done;
        if (row->beforeExists) { if (!ScHashValid(field[4])) goto done; strcpy(row->beforeHash, field[4]); }
        else if (strcmp(field[4], "-") != 0) goto done;
        if (!Number(field[5], 1U, &value)) goto done;
        row->afterExists = (int)value;
        if (!Number(field[6], UMI_SETUP_MAX_FILE_BYTES, &row->afterBytes)) goto done;
        if (row->afterExists) { if (!ScHashValid(field[7])) goto done; strcpy(row->afterHash, field[7]); }
        else if (strcmp(field[7], "-") != 0) goto done;
        if (!Number(field[8], 1U, &value) || strlen(field[9]) >= sizeof row->relative) goto done;
        row->modified = (int)value; strcpy(row->relative, field[9]);
        if (!RowValid(row, plan->config.action) || (i != 0U && ScEqualFold(plan->rows[i - 1U].relative, row->relative) >= 0)) goto done;
        if (row->change == UMI_SETUP_MAINTENANCE_WRITE) {
            if (row->afterBytes > UMI_SETUP_MAX_TOTAL_BYTES - plan->summary.stagingBytes) goto done;
            plan->summary.stagingBytes += row->afterBytes; ++plan->summary.filesToWrite;
        }
        if (row->change == UMI_SETUP_MAINTENANCE_RETIRE) ++plan->summary.filesToRetire;
        if (row->modified) ++plan->summary.modifiedFilesPreserved;
    }
    if (*cursor != '\0') goto done;
    plan->config.installationRoot = plan->root; plan->config.releaseRoot = plan->source;
    plan->summary.checkedFiles = plan->rowCount;
    /* Retain original canonical bytes. Regeneration must agree exactly: this
     * rejects alternate encodings and makes the fingerprint reproducible. */
    status = SmJournalWrite(plan);
    if (status == UMI_STATUS_OK && (plan->journalLength != length || memcmp(plan->journal, text, length) != 0)) status = UMI_STATUS_PARSE_ERROR;
done:
    free(copy);
    if (status != UMI_STATUS_OK) UmiSetupMaintenancePlanDestroy(plan); else *out = plan;
    return status;
}
