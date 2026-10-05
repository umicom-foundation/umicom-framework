/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_dependency_node.c
 *
 * PURPOSE:
 *   Exercise the dependency node reactive UI contract.
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
#include "umicom/ui/reactive/dependency_node.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/dependency_node.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveDependencyNodeTransferEqual(const UmiUiReactiveDependencyNode *a, const UmiUiReactiveDependencyNode *b)
{
    return strcmp(a->node_id, b->node_id) == 0 &&
        a->computed == b->computed &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveDependencyNodeTransferTails(UmiUiReactiveDependencyNode *value)
{
    (void)value;
    {
        size_t used = strlen(value->node_id) + 1U;
        memset(value->node_id + used, 0xa5, sizeof(value->node_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveDependencyNodeTransferMalformed(const UmiUiReactiveDependencyNode *sample)
{
    (void)sample;
    {
        UmiUiReactiveDependencyNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.node_id, 'x', sizeof(invalid.node_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_dependency_node_valid(&invalid)) ||
            umi_ui_reactive_dependency_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated node_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveDependencyNodeTransferCases, UmiUiReactiveDependencyNode,
    umi_ui_reactive_dependency_node_archive_encode, umi_ui_reactive_dependency_node_archive_decode,
    UmiUiReactiveDependencyNodeTransferEqual, UmiUiReactiveDependencyNodeTransferTails, UmiUiReactiveDependencyNodeTransferMalformed)

int main(void) { UmiUiReactiveDependencyNode item; umi_ui_reactive_dependency_node_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveDependencyNode populated = item;
    (void)snprintf(populated.node_id, sizeof(populated.node_id), "field-0");
    populated.computed = true;
    populated.revision = (uint64_t)4;
    if (UmiUiReactiveDependencyNodeTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_dependency_node_valid(&item) ? 0 : 1; }
