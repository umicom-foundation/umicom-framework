/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_inline_history.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration inline history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ide_integration/inline_history.h"
int main(void)
{
    UmiIdeInlineHistory *history = NULL;
    UmiIdeInlineSuggestion item = {0};
    UmiIdeInlineSuggestion copy;
    assert(umi_ide_inline_history_create(&history) == UMI_STATUS_OK);
    (void)strcpy(item.suggestion_id, "inline.1");
    item.state = UMI_IDE_INLINE_ACCEPTED;
    assert(umi_ide_inline_history_record(history, &item) == UMI_STATUS_OK);
    assert(umi_ide_inline_history_at(history, 0U, &copy) == UMI_STATUS_OK);
    assert(copy.state == UMI_IDE_INLINE_ACCEPTED);
    umi_ide_inline_history_destroy(history);
    return 0;
}

