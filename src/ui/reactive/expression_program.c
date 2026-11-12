/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/expression_program.c
 *
 * PURPOSE:
 *   Implement a bounded reverse-polish expression program.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_program.h"
#include <string.h>

/* Initialise the expression program contract to deterministic zero/default state. */
void umi_ui_reactive_expression_program_init(UmiUiReactiveExpressionProgram *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
/* Validate the declared extent against both fixed arrays before another service consumes the record. The previous implementation is retained for review. */
#if 0
int umi_ui_reactive_expression_program_valid(const UmiUiReactiveExpressionProgram *item) {
    return item != NULL;
}
#endif
int umi_ui_reactive_expression_program_valid(const UmiUiReactiveExpressionProgram *item)
{
    /* Establish both fixed-array extents before a consumer indexes count
     * entries. This record defines no opcode semantics or execution service;
     * a readable extent must not be advertised as an executable program. */
    return item != NULL &&
        item->count <= sizeof(item->opcodes) / sizeof(item->opcodes[0]) &&
        item->count <= sizeof(item->numbers) / sizeof(item->numbers[0]);
}
