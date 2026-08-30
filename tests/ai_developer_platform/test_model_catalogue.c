/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_model_catalogue.c
 *
 * PURPOSE:
 *   Implement the test model catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/model_catalogue.h"
int main(void) { UmiAiDevModelCatalogue c; umi_ai_dev_model_catalogue_init(&c); if(umi_ai_dev_model_catalogue_upsert(&c,"x",5U,1U)!=UMI_STATUS_OK) return 1; if(umi_ai_dev_model_catalogue_find(&c,"x")==0) return 2; if(umi_ai_dev_model_catalogue_enabled_count(&c)!=1U) return 3; return 0; }
