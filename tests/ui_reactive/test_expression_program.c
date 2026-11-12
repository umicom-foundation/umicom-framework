/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_program.c
 *
 * PURPOSE:
 *   Exercise the expression program reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_program.h"
#include <stdio.h>
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Exercise null, boundary and malformed counts while preserving the original initialized fixture for review. The previous implementation is retained for review. */
#if 0
int main(void) { UmiUiReactiveExpressionProgram item; umi_ui_reactive_expression_program_init(&item); return umi_ui_reactive_expression_program_valid(&item) ? 0 : 1; }
#endif
int main(void)
{
    UmiUiReactiveExpressionProgram item, retained;
    const size_t counts[] = {0U, 1U, UMI_UI_REACTIVE_MAX_TOKENS - 1U,
        UMI_UI_REACTIVE_MAX_TOKENS, UMI_UI_REACTIVE_MAX_TOKENS + 1U, SIZE_MAX};
    if (umi_ui_reactive_expression_program_valid(NULL)) return 1;
    for (size_t i = 0U; i < sizeof(counts) / sizeof(counts[0]); ++i) {
        umi_ui_reactive_expression_program_init(&item);
        item.count = counts[i];
        /* Arbitrary opcode bytes emphasize that this is an extent check,
         * not a new interpreter or a claim that the record can execute. */
        item.opcodes[0] = -99;
        item.numbers[0] = 12.5;
        memcpy(&retained, &item, sizeof(item));
        const int expected = counts[i] <= UMI_UI_REACTIVE_MAX_TOKENS;
        if (umi_ui_reactive_expression_program_valid(&item) != expected ||
            memcmp(&item, &retained, sizeof(item)) != 0) {
            fprintf(stderr, "Expression extent %zu was not admitted as expected.\n", counts[i]);
            return 1;
        }
    }
    return 0;
}
