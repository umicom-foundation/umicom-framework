/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_locale_layout_policy.c
 *
 * PURPOSE:
 *   Validate model right-to-left mirroring and text-expansion allowances in responsive designer previews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/locale_layout_policy.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerLocaleLayoutPolicy p;
    CHECK(umi_designer_locale_layout_policy_init(&p,1,1,30U)==UMI_STATUS_OK);
    CHECK(umi_designer_locale_layout_policy_expand_width(&p,100)==130);
    return 0;
}
