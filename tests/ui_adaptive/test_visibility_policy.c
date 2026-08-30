/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_visibility_policy.c
 *
 * PURPOSE:
 *   Validate evaluate semantic visibility policy from size class and content priority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/visibility_policy.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveVisibilityPolicy policy = { UMI_DESIGN_SIZE_EXPANDED, 50U, 1 };
    UmiAdaptiveContentPriority content;
    CHECK(umi_adaptive_content_priority_init(&content,"critical",1U,1)==UMI_STATUS_OK);
    CHECK(umi_adaptive_visibility_policy_evaluate(&policy, UMI_DESIGN_SIZE_COMPACT, &content));
    return 0;
}
