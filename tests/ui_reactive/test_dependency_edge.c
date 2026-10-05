/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_dependency_edge.c
 *
 * PURPOSE:
 *   Exercise the dependency edge reactive UI contract.
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
#include "umicom/ui/reactive/dependency_edge.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/dependency_edge.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveDependencyEdgeTransferEqual(const UmiUiReactiveDependencyEdge *a, const UmiUiReactiveDependencyEdge *b)
{
    return strcmp(a->from_id, b->from_id) == 0 &&
        strcmp(a->to_id, b->to_id) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveDependencyEdgeTransferTails(UmiUiReactiveDependencyEdge *value)
{
    (void)value;
    {
        size_t used = strlen(value->from_id) + 1U;
        memset(value->from_id + used, 0xa5, sizeof(value->from_id) - used);
    }
    {
        size_t used = strlen(value->to_id) + 1U;
        memset(value->to_id + used, 0xa5, sizeof(value->to_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveDependencyEdgeTransferMalformed(const UmiUiReactiveDependencyEdge *sample)
{
    (void)sample;
    {
        UmiUiReactiveDependencyEdge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.from_id, 'x', sizeof(invalid.from_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_dependency_edge_valid(&invalid)) ||
            umi_ui_reactive_dependency_edge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated from_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveDependencyEdge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.to_id, 'x', sizeof(invalid.to_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_dependency_edge_valid(&invalid)) ||
            umi_ui_reactive_dependency_edge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated to_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveDependencyEdgeTransferCases, UmiUiReactiveDependencyEdge,
    umi_ui_reactive_dependency_edge_archive_encode, umi_ui_reactive_dependency_edge_archive_decode,
    UmiUiReactiveDependencyEdgeTransferEqual, UmiUiReactiveDependencyEdgeTransferTails, UmiUiReactiveDependencyEdgeTransferMalformed)

int main(void) { UmiUiReactiveDependencyEdge item; umi_ui_reactive_dependency_edge_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveDependencyEdge populated = item;
    (void)snprintf(populated.from_id, sizeof(populated.from_id), "field-0");
    (void)snprintf(populated.to_id, sizeof(populated.to_id), "field-1");
    if (UmiUiReactiveDependencyEdgeTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_dependency_edge_valid(&item) ? 0 : 1; }
