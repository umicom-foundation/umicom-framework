/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_terminal_split_model.c
 *
 * PURPOSE:
 *   Implement the test terminal split model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework terminal split-tree tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/terminal/split_model.h"

int main(void)
{
    UmiTerminalSplitModel *model = NULL;
    UmiTerminalSplitSnapshot root;
    UmiTerminalSplitSnapshot leaf;
    uint64_t new_leaf = 0U;
    assert(umi_terminal_split_model_create("primary", &model) == UMI_STATUS_OK);
    assert(umi_terminal_split_model_split(model,
        umi_terminal_split_model_root_id(model), "secondary",
        UMI_TERMINAL_ORIENTATION_HORIZONTAL, 0.6, &new_leaf) == UMI_STATUS_OK);
    assert(umi_terminal_split_model_count(model) == 3U);
    assert(umi_terminal_split_model_find(model,
        umi_terminal_split_model_root_id(model), &root) == UMI_STATUS_OK);
    assert(!root.leaf && root.orientation == UMI_TERMINAL_ORIENTATION_HORIZONTAL);
    assert(umi_terminal_split_model_find(model, new_leaf, &leaf) == UMI_STATUS_OK);
    assert(leaf.leaf && strcmp(leaf.session_id, "secondary") == 0);
    assert(umi_terminal_split_model_set_ratio(model, root.node_id, 0.4) == UMI_STATUS_OK);
    assert(umi_terminal_split_model_close(model, new_leaf) == UMI_STATUS_OK);
    assert(umi_terminal_split_model_count(model) == 1U);
    assert(umi_terminal_split_model_find(model, root.node_id, &root) == UMI_STATUS_OK);
    assert(root.leaf && strcmp(root.session_id, "primary") == 0);
    umi_terminal_split_model_destroy(model);
    return 0;
}
