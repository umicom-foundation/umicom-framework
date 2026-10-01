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
#include "snapshot_contract_cases.h"
