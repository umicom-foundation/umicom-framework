/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_disassembly_viewport_model.c
 *
 * PURPOSE:
 *   Verify track disassembly viewport range, active instruction and follow-PC state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/debug/workbench/disassembly_viewport_model.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/debug/workbench/disassembly_viewport_model.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDebugWorkbenchDisassemblyViewportModelTransferEqual(const UmiDebugWorkbenchDisassemblyViewportModel *a, const UmiDebugWorkbenchDisassemblyViewportModel *b)
{
    return strcmp(a->value.id, b->value.id) == 0 &&
        strcmp(a->value.label, b->value.label) == 0 &&
        strcmp(a->value.detail, b->value.detail) == 0 &&
        strcmp(a->value.location.path, b->value.location.path) == 0 &&
        a->value.location.range.start.line == b->value.location.range.start.line &&
        a->value.location.range.start.column == b->value.location.range.start.column &&
        a->value.location.range.end.line == b->value.location.range.end.line &&
        a->value.location.range.end.column == b->value.location.range.end.column &&
        a->value.state == b->value.state &&
        a->value.flags == b->value.flags &&
        a->value.value == b->value.value &&
        a->value.revision == b->value.revision &&
        a->start_address == b->start_address &&
        a->extent == b->extent &&
        a->cursor_address == b->cursor_address &&
        a->follow_execution == b->follow_execution &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDebugWorkbenchDisassemblyViewportModelTransferTails(UmiDebugWorkbenchDisassemblyViewportModel *value)
{
    (void)value;
    {
        size_t used = strlen(value->value.id) + 1U;
        memset(value->value.id + used, 0xa5, sizeof(value->value.id) - used);
    }
    {
        size_t used = strlen(value->value.label) + 1U;
        memset(value->value.label + used, 0xa5, sizeof(value->value.label) - used);
    }
    {
        size_t used = strlen(value->value.detail) + 1U;
        memset(value->value.detail + used, 0xa5, sizeof(value->value.detail) - used);
    }
    {
        size_t used = strlen(value->value.location.path) + 1U;
        memset(value->value.location.path + used, 0xa5, sizeof(value->value.location.path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDebugWorkbenchDisassemblyViewportModelTransferMalformed(const UmiDebugWorkbenchDisassemblyViewportModel *sample)
{
    (void)sample;
    {
        UmiDebugWorkbenchDisassemblyViewportModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value.id, 'x', sizeof(invalid.value.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_disassembly_viewport_model_valid(&invalid)) ||
            umi_debug_workbench_disassembly_viewport_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value.id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDebugWorkbenchDisassemblyViewportModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value.label, 'x', sizeof(invalid.value.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_disassembly_viewport_model_valid(&invalid)) ||
            umi_debug_workbench_disassembly_viewport_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value.label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDebugWorkbenchDisassemblyViewportModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value.detail, 'x', sizeof(invalid.value.detail));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_disassembly_viewport_model_valid(&invalid)) ||
            umi_debug_workbench_disassembly_viewport_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value.detail was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDebugWorkbenchDisassemblyViewportModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value.location.path, 'x', sizeof(invalid.value.location.path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_disassembly_viewport_model_valid(&invalid)) ||
            umi_debug_workbench_disassembly_viewport_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value.location.path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDebugWorkbenchDisassemblyViewportModelTransferCases, UmiDebugWorkbenchDisassemblyViewportModel,
    umi_debug_workbench_disassembly_viewport_model_archive_encode, umi_debug_workbench_disassembly_viewport_model_archive_decode,
    UmiDebugWorkbenchDisassemblyViewportModelTransferEqual, UmiDebugWorkbenchDisassemblyViewportModelTransferTails, UmiDebugWorkbenchDisassemblyViewportModelTransferMalformed)

int main(void)
{
    UmiDebugWorkbenchDisassemblyViewportModel model;
    UMI_TEST_CHECK(umi_debug_workbench_disassembly_viewport_model_init(&model, "range-1", UINT64_C(0x1000), UINT64_C(256)) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_disassembly_viewport_model_contains(&model, UINT64_C(0x1080)));
    UMI_TEST_CHECK(!umi_debug_workbench_disassembly_viewport_model_contains(&model, UINT64_C(0x1200)));
    UMI_TEST_CHECK(umi_debug_workbench_disassembly_viewport_model_set_cursor(&model, UINT64_C(0x10f0)) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_disassembly_viewport_model_set_follow_execution(&model, true) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_disassembly_viewport_model_valid(&model));
    if (UmiDebugWorkbenchDisassemblyViewportModelTransferCases(&model) != 0) return 1;

    return 0;
}
