/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_state_graph.c
 *
 * PURPOSE:
 *   Exercise the state graph reactive UI contract.
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
#include "umicom/ui/reactive/state_graph.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/state_graph.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveStateGraphTransferEqual(const UmiUiReactiveStateGraph *a, const UmiUiReactiveStateGraph *b)
{
    return a->node_count == b->node_count &&
        a->edge_count == b->edge_count &&
        a->computed_count == b->computed_count &&
        a->acyclic == b->acyclic &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveStateGraphTransferTails(UmiUiReactiveStateGraph *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveStateGraphTransferMalformed(const UmiUiReactiveStateGraph *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveStateGraphTransferCases, UmiUiReactiveStateGraph,
    umi_ui_reactive_state_graph_archive_encode, umi_ui_reactive_state_graph_archive_decode,
    UmiUiReactiveStateGraphTransferEqual, UmiUiReactiveStateGraphTransferTails, UmiUiReactiveStateGraphTransferMalformed)

int main(void) { UmiUiReactiveStateGraph item; umi_ui_reactive_state_graph_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveStateGraph populated = item;
    populated.node_count = (size_t)2;
    populated.edge_count = (size_t)3;
    populated.computed_count = (size_t)4;
    populated.acyclic = true;
    populated.revision = (uint64_t)6;
    if (UmiUiReactiveStateGraphTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_state_graph_valid(&item) ? 0 : 1; }
