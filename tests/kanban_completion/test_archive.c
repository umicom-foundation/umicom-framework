/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_completion/test_archive.c
 *
 * PURPOSE:
 *   Reject corrupted private review archives and preserve independent decoded ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include "../../src/base/value_archive_internal.h"
#define SCHEMA UINT64_C(0x554d494b434d5001)
static size_t Next(const unsigned char *bytes, size_t at)
{
    return at + 8U + (size_t)UmiArchiveLoadInteger(bytes + at, 8U);
}
int main(int argc, char **argv)
{
    const char *cases[] = {"roundtrip","measure","short","null","truncated","checksum","schema",
        "trailing","embedded-nul","invalid-sha","invalid-decision","owned","unused-tail",
        "stale-reopen","oversized"};
    REQUIRE(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *name = argv[1];
    UmiKanbanBoard *board = Ready();
    UmiKanbanCompletionRecord record; Record(&record, "alpha");
    UmiKanbanCompletionPlan *plan = NULL, *decoded = NULL;
    REQUIRE(Capture(board, &record, 1, &plan) == UMI_STATUS_OK);
    size_t size = 0;
    REQUIRE(UmiKanbanCompletionPlanEncode(plan, NULL, 0, &size) == UMI_STATUS_OK);
    REQUIRE(size > UMI_VALUE_ARCHIVE_HEADER_SIZE && size < UMI_KANBAN_COMPLETION_MAXIMUM_BYTES);
    unsigned char *bytes = malloc(size + 1U); REQUIRE(bytes);
    REQUIRE(UmiKanbanCompletionPlanEncode(plan, bytes, size, &size) == UMI_STATUS_OK);
    if (!strcmp(name, "short")) {
        unsigned char *small = malloc(size); REQUIRE(small); memset(small, 0xa5, size);
        size_t wanted = 0;
        REQUIRE(UmiKanbanCompletionPlanEncode(plan, small, size - 1U, &wanted) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(wanted == size);
        for (size_t i = 0; i < size; ++i) REQUIRE(small[i] == 0xa5);
        free(small); goto done;
    }
    if (!strcmp(name, "null")) {
        REQUIRE(UmiKanbanCompletionPlanDecode(NULL, size, &decoded) == UMI_STATUS_INVALID_ARGUMENT && !decoded);
        REQUIRE(UmiKanbanCompletionPlanDecode(bytes, size, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiKanbanCompletionPlanEncode(NULL, bytes, size, &size) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiKanbanCompletionPlanEncode(plan, NULL, 1, &size) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiKanbanCompletionPlanEncode(plan, bytes, size, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    if (!strcmp(name, "unused-tail")) {
        record.rationale[strlen(record.rationale) + 1U] = (char)0xff;
        UmiKanbanCompletionPlan *other = NULL; REQUIRE(Capture(board, &record, 1, &other) == UMI_STATUS_OK);
        size_t other_size = 0; unsigned char *other_bytes = malloc(size); REQUIRE(other_bytes);
        REQUIRE(UmiKanbanCompletionPlanEncode(other, other_bytes, size, &other_size) == UMI_STATUS_OK);
        REQUIRE(other_size == size && !memcmp(bytes, other_bytes, size));
        free(other_bytes); UmiKanbanCompletionPlanDestroy(other); goto done;
    }
    if (!strcmp(name, "oversized")) {
        REQUIRE(UmiKanbanCompletionPlanDecode(bytes, UMI_KANBAN_COMPLETION_MAXIMUM_BYTES + 1U,
            &decoded) == UMI_STATUS_PARSE_ERROR && !decoded);
        goto done;
    }
    if (!strcmp(name, "checksum")) bytes[size - 1U] ^= 1U;
    if (!strcmp(name, "schema")) UmiArchiveFinish(bytes, size, SCHEMA + 1U);
    if (!strcmp(name, "trailing")) { bytes[size++] = 0U; UmiArchiveFinish(bytes, size, SCHEMA); }
    if (!strcmp(name, "invalid-decision") || !strcmp(name, "invalid-sha") || !strcmp(name, "embedded-nul")) {
        size_t at = UMI_VALUE_ARCHIVE_HEADER_SIZE;
        for (size_t field = 0; field < 6U; ++field) at = Next(bytes, at);
        at += 16U;
        at = Next(bytes, at);
        if (!strcmp(name, "invalid-decision")) UmiArchiveStoreInteger(bytes + at, 77U, 8U);
        else {
            at += 8U;
            if (!strcmp(name, "invalid-sha")) bytes[at + 8U] = 'G';
            else { at = Next(bytes, at); bytes[at + 8U] = 0U; }
        }
        UmiArchiveFinish(bytes, size, SCHEMA);
    }
    UmiStatus status = UmiKanbanCompletionPlanDecode(bytes,
        !strcmp(name, "truncated") ? size - 1U : size, &decoded);
    if (!strcmp(name, "truncated") || !strcmp(name, "checksum") || !strcmp(name, "schema") ||
        !strcmp(name, "trailing") || !strcmp(name, "invalid-decision") ||
        !strcmp(name, "invalid-sha") || !strcmp(name, "embedded-nul")) {
        REQUIRE(status != UMI_STATUS_OK && !decoded);
    } else {
        REQUIRE(status == UMI_STATUS_OK);
        if (!strcmp(name, "owned")) { memset(bytes, 0, size); UmiKanbanCompletionPlanDestroy(plan); plan = NULL; }
        UmiKanbanCompletionPlanInfo info;
        REQUIRE(UmiKanbanCompletionPlanRead(decoded, &info) == UMI_STATUS_OK);
        REQUIRE(info.card_count == 1 && !strcmp(info.reviewer, "reviewer"));
        REQUIRE(!strcmp(UmiKanbanCompletionPlanAt(decoded, 0)->rationale, record.rationale));
        REQUIRE(UmiKanbanCompletionPlanCheck(decoded, board, umi_kanban_board_revision(board)) == UMI_STATUS_OK);
        if (!strcmp(name, "stale-reopen")) {
            size_t board_size = 0; void *board_bytes = Snapshot(board, &board_size);
            UmiKanbanBoard *reopened = NULL; uint64_t historical = 0;
            REQUIRE(umi_kanban_board_decode(board_bytes, board_size, &reopened, &historical) == UMI_STATUS_OK);
            REQUIRE(historical == info.reviewed_revision && umi_kanban_board_revision(reopened) == 1U);
            REQUIRE(UmiKanbanCompletionPlanCheck(decoded, reopened, 1U) == UMI_STATUS_INVALID_STATE);
            free(board_bytes); umi_kanban_board_destroy(reopened);
        }
    }
done:
    free(bytes); UmiKanbanCompletionPlanDestroy(plan); UmiKanbanCompletionPlanDestroy(decoded);
    umi_kanban_board_destroy(board); return 0;
}
