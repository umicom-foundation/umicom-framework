/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_model_descriptor.c
 *
 * PURPOSE:
 *   Implement the test model descriptor behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/model_descriptor.h"
int main(void) {
    UmiAiDevModelDescriptor v;
    umi_ai_dev_model_descriptor_init(&v);
    if (umi_ai_dev_model_descriptor_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_model_descriptor_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_model_descriptor_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
