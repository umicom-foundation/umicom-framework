/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_types.c
 *
 * PURPOSE:
 *   Verify stable application productisation vocabulary and validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/application/productisation/types.h"

int main(void)
{
    assert(umi_productisation_asset_kind_valid(
        UMI_PRODUCTISATION_ASSET_LAYOUT));
    assert(!umi_productisation_asset_kind_valid(
        (UmiProductisationAssetKind)0));
    assert(umi_productisation_evidence_state_valid(
        UMI_PRODUCTISATION_EVIDENCE_ACCEPTED));
    assert(strcmp(umi_productisation_gap_kind_text(
        UMI_PRODUCTISATION_GAP_APPLICATION_ADOPTION),
        "application-adoption") == 0);
    assert(strcmp(umi_productisation_stage_text(
        UMI_PRODUCTISATION_STAGE_FRAMEWORK), "framework") == 0);
    return 0;
}
