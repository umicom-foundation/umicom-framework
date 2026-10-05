/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_problem.c
 * PURPOSE: Exercise ui problem snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/problem.h"
#include "umicom/ui/problem.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiProblemSnapshot
#define CONTRACT_REGISTRY UmiUiProblemRegistry
#define CONTRACT_CAPACITY UMI_UI_PROBLEM_CAPACITY
#define CONTRACT_VALIDATE umi_ui_problem_snapshot_validate
#define CONTRACT_BATCH umi_ui_problem_registry_upsert_many
#define CONTRACT_CREATE umi_ui_problem_registry_create
#define CONTRACT_DESTROY umi_ui_problem_registry_destroy
#define CONTRACT_UPSERT umi_ui_problem_registry_upsert
#define CONTRACT_REMOVE umi_ui_problem_registry_remove
#define CONTRACT_FIND umi_ui_problem_registry_find
#define CONTRACT_AT umi_ui_problem_registry_at
#define CONTRACT_COUNT umi_ui_problem_registry_count
#define CONTRACT_REVISION umi_ui_problem_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiProblemSnapshot, id), sizeof(((UmiUiProblemSnapshot *)0)->id), 1 },
    {"source", offsetof(UmiUiProblemSnapshot, source), sizeof(((UmiUiProblemSnapshot *)0)->source), 0 },
    {"code", offsetof(UmiUiProblemSnapshot, code), sizeof(((UmiUiProblemSnapshot *)0)->code), 0 },
    {"message", offsetof(UmiUiProblemSnapshot, message), sizeof(((UmiUiProblemSnapshot *)0)->message), 0 },
    {"uri", offsetof(UmiUiProblemSnapshot, uri), sizeof(((UmiUiProblemSnapshot *)0)->uri), 0 }
};
static int ContractSnapshotEqual(const UmiUiProblemSnapshot *left,
    const UmiUiProblemSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->source, right->source, sizeof(left->source)) == 0 &&
        memcmp(left->code, right->code, sizeof(left->code)) == 0 &&
        memcmp(left->message, right->message, sizeof(left->message)) == 0 &&
        memcmp(left->uri, right->uri, sizeof(left->uri)) == 0 &&
        left->line == right->line &&
        left->column == right->column &&
        left->severity == right->severity &&
        left->resolved == right->resolved &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiProblemSnapshot *item)
{
    item->source[0] = 'v';
    item->code[0] = 'v';
    item->message[0] = 'v';
    item->uri[0] = 'v';
    item->line = (uint32_t)8U;
    item->column = (uint32_t)9U;
    item->severity = (int)10U;
    item->resolved = (int)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_problem_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_problem_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiProblemEdit
#define CONTRACT_EDIT_CURRENT umi_ui_problem_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_ui_problem_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiUiProblemSnapshot ArchiveSample(void)
{
    UmiUiProblemSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiUiProblemSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->source) + 1U;
        memset(value->source + used, 0xa5, sizeof(value->source) - used);
    }
    {
        size_t used = strlen(value->code) + 1U;
        memset(value->code + used, 0xa5, sizeof(value->code) - used);
    }
    {
        size_t used = strlen(value->message) + 1U;
        memset(value->message + used, 0xa5, sizeof(value->message) - used);
    }
    {
        size_t used = strlen(value->uri) + 1U;
        memset(value->uri + used, 0xa5, sizeof(value->uri) - used);
    }
}
#define ARCHIVE_TYPE UmiUiProblemSnapshot
#define ARCHIVE_ENCODE umi_ui_problem_snapshot_archive_encode
#define ARCHIVE_DECODE umi_ui_problem_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_ui_problem_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_ui_problem_registry_archive_restore
#include "snapshot_contract_cases.h"
