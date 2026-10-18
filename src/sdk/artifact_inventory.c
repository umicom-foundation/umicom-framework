/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/artifact_inventory.c
 * PURPOSE: Parse SDK artifact receipts and observe files without executing CMake.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/sdk/artifact_inventory.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/input_file.h"
#include <stdlib.h>
#include <string.h>

struct UmiSdkArtifactInventory {
    UmiSdkArtifactSummary summary;
    UmiSdkArtifactEntry *entries;
};

/* The generated receipt uses portable ASCII names, even when the installation
 * prefix contains Unicode. This deliberately excludes expressions and separators. */
static bool ArtifactName(const char *text, size_t capacity, bool empty_allowed)
{
    size_t size = strlen(text);
    if ((!empty_allowed && size == 0U) || size >= capacity) return false;
    for (size_t i = 0U; i < size; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.' || c == '+'))
            return false;
    }
    return true;
}

static bool ArtifactPath(const char *text)
{
    size_t length = strlen(text), segment = 0U;
    if (length == 0U || length >= UMI_SDK_ARTIFACT_PATH_CAPACITY) return false;
    for (size_t i = 0U; i <= length; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (c == '/' || c == 0U) {
            size_t count = i - segment;
            if (count == 0U || (count == 1U && text[segment] == '.') ||
                (count == 2U && text[segment] == '.' && text[segment + 1U] == '.') ||
                text[segment] == ' ' || text[i - 1U] == ' ' || text[i - 1U] == '.') return false;
            segment = i + 1U;
        } else if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.' || c == '+' || c == ' '))
            return false;
    }
    return true;
}


static bool ArtifactCount(const char *text, size_t expected)
{
    size_t value = 0U;
    if (*text == '\0') return false;
    for (; *text != '\0'; ++text) {
        if (*text < '0' || *text > '9') return false;
        size_t digit = (size_t)(*text - '0');
        if (value > (UMI_SDK_ARTIFACT_COUNT_LIMIT - digit) / 10U) return false;
        value = value * 10U + digit;
    }
    return value == expected && value != 0U;
}

static int ArtifactOrder(const void *left, const void *right)
{
    const UmiSdkArtifactEntry *a = *(const UmiSdkArtifactEntry *const *)left;
    const UmiSdkArtifactEntry *b = *(const UmiSdkArtifactEntry *const *)right;
    int order = strcmp(a->target, b->target);
    return order != 0 ? order : (int)a->kind - (int)b->kind;
}

/* Sort borrowed pointers, leaving the generated record order intact for callers.
 * A Windows shared target alone may own two records: its DLL and import archive. */
static UmiStatus ArtifactUnique(const UmiSdkArtifactInventory *inventory)
{
    size_t count = inventory->summary.entries;
    const UmiSdkArtifactEntry **ordered = malloc(count * sizeof(*ordered));
    if (ordered == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < count; ++i) ordered[i] = &inventory->entries[i];
    qsort(ordered, count, sizeof(*ordered), ArtifactOrder);
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 1U; i < count; ++i) {
        if (strcmp(ordered[i - 1U]->target, ordered[i]->target) != 0) continue;
        if (ordered[i - 1U]->kind != UMI_SDK_ARTIFACT_SHARED ||
            ordered[i]->kind != UMI_SDK_ARTIFACT_IMPORT) {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
    }
    free(ordered);
    return status;
}

UmiStatus UmiSdkArtifactInventoryParse(const void *bytes, size_t length,
    UmiSdkArtifactInventory **out_inventory)
{
    if (out_inventory == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_inventory = NULL;
    if (bytes == NULL || length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_SDK_ARTIFACT_BYTE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    const unsigned char *input = bytes;
    size_t lines = 0U;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char c = input[i];
        if (c == '\n') ++lines;
        else if (c == '\r') {
            if (i + 1U == length || input[i + 1U] != '\n') return UMI_STATUS_PARSE_ERROR;
        } else if (c < 32U || c > 126U) return UMI_STATUS_PARSE_ERROR;
    }
    if (input[length - 1U] != '\n' || lines < 4U) return UMI_STATUS_PARSE_ERROR;
    if (lines - 3U > UMI_SDK_ARTIFACT_COUNT_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiSdkArtifactInventory *candidate = calloc(1U, sizeof(*candidate));
    char *text = malloc(length + 1U);
    if (candidate == NULL || text == NULL) {
        free(candidate); free(text); return UMI_STATUS_OUT_OF_MEMORY;
    }
    candidate->entries = calloc(lines - 3U, sizeof(*candidate->entries));
    if (candidate->entries == NULL) {
        free(candidate); free(text); return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(text, bytes, length); text[length] = '\0';
    static const char *const kinds[] = {"archive", "shared", "import", "tool", "interface"};
    UmiStatus status = UMI_STATUS_OK;
    char *cursor = text;
    for (size_t row = 0U; row < lines; ++row) {
        char *line = cursor, *end = strchr(cursor, '\n');
        if (end == NULL) { status = UMI_STATUS_PARSE_ERROR; break; }
        *end = '\0'; cursor = end + 1;
        if (end > line && end[-1] == '\r') end[-1] = '\0';
        if (row == 0U) {
            if (strcmp(line, "schema=umicom.sdk.artifacts/1") != 0) status = UMI_STATUS_PARSE_ERROR;
        } else if (row == 1U) {
            const char key[] = "configuration=";
            if (strncmp(line, key, sizeof(key) - 1U) != 0 ||
                !ArtifactName(line + sizeof(key) - 1U, sizeof(candidate->summary.configuration), true))
                status = UMI_STATUS_PARSE_ERROR;
            else strcpy(candidate->summary.configuration, line + sizeof(key) - 1U);
        } else if (row == 2U) {
            const char key[] = "entries=";
            if (strncmp(line, key, sizeof(key) - 1U) != 0 ||
                !ArtifactCount(line + sizeof(key) - 1U, lines - 3U)) status = UMI_STATUS_PARSE_ERROR;
        } else {
            char *kind = strchr(line, '|');
            char *relative = kind == NULL ? NULL : strchr(kind + 1, '|');
            if (kind == NULL || relative == NULL || strchr(relative + 1, '|') != NULL) {
                status = UMI_STATUS_PARSE_ERROR; break;
            }
            *kind++ = '\0'; *relative++ = '\0';
            size_t k = 0U;
            while (k < 5U && strcmp(kind, kinds[k]) != 0) ++k;
            if (!ArtifactName(line, UMI_SDK_ARTIFACT_NAME_CAPACITY, false) || k == 5U ||
                (k == UMI_SDK_ARTIFACT_INTERFACE ? strcmp(relative, "-") != 0 : !ArtifactPath(relative))) {
                status = UMI_STATUS_PARSE_ERROR; break;
            }
            UmiSdkArtifactEntry *entry = &candidate->entries[candidate->summary.entries++];
            strcpy(entry->target, line); strcpy(entry->relative_path, relative);
            entry->kind = (UmiSdkArtifactKind)k;
            if (entry->kind != UMI_SDK_ARTIFACT_INTERFACE) ++candidate->summary.files;
        }
        if (status != UMI_STATUS_OK) break;
    }
    if (status == UMI_STATUS_OK) status = ArtifactUnique(candidate);
    free(text);
    if (status != UMI_STATUS_OK) UmiSdkArtifactInventoryDestroy(candidate);
    else *out_inventory = candidate;
    return status;
}

UmiStatus UmiSdkArtifactInventoryInspect(const char *prefix,
    UmiSdkArtifactInventory **out_inventory)
{
    if (out_inventory == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_inventory = NULL;
    if (prefix == NULL || !umi_path_is_absolute(prefix)) return UMI_STATUS_INVALID_ARGUMENT;
    char selected[UMI_PATH_CAPACITY], receipt[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_normalise(prefix, selected, sizeof selected);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(selected, UMI_SDK_ARTIFACT_INVENTORY_PATH, receipt, sizeof receipt);
    unsigned char *bytes = NULL;
    size_t length = 0U;
    UmiSdkArtifactInventory *candidate = NULL;
    if (status == UMI_STATUS_OK) status = UmiInputFileRead(receipt, UMI_SDK_ARTIFACT_BYTE_LIMIT, &bytes, &length);
    if (status == UMI_STATUS_OK) status = UmiSdkArtifactInventoryParse(bytes, length, &candidate);
    UmiInputFileFree(bytes);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < candidate->summary.entries; ++i) {
        UmiSdkArtifactEntry *entry = &candidate->entries[i];
        if (entry->kind == UMI_SDK_ARTIFACT_INTERFACE) continue;
        char filename[UMI_PATH_CAPACITY];
        status = umi_path_join(selected, entry->relative_path, filename, sizeof filename);
        if (status != UMI_STATUS_OK) break;
        entry->present = umi_fs_is_file(filename) != 0;
        if (entry->present) ++candidate->summary.present;
        else ++candidate->summary.missing;
    }
    if (status == UMI_STATUS_OK) {
        candidate->summary.inspected = true;
        *out_inventory = candidate;
    } else UmiSdkArtifactInventoryDestroy(candidate);
    return status;
}

UmiStatus UmiSdkArtifactInventoryReadSummary(const UmiSdkArtifactInventory *inventory,
    UmiSdkArtifactSummary *out_summary)
{
    if (out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_summary, 0, sizeof(*out_summary));
    if (inventory == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = inventory->summary;
    return UMI_STATUS_OK;
}

const UmiSdkArtifactEntry *UmiSdkArtifactInventoryEntryAt(const UmiSdkArtifactInventory *inventory,
    size_t index)
{
    return inventory != NULL && index < inventory->summary.entries ? &inventory->entries[index] : NULL;
}

void UmiSdkArtifactInventoryDestroy(UmiSdkArtifactInventory *inventory)
{
    if (inventory != NULL) { free(inventory->entries); free(inventory); }
}
