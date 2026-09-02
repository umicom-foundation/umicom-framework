/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_config.c
 *
 * PURPOSE:
 *   Verify the reusable AI coding runtime config contract.
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
#include "umicom/ai_coding_runtime/config.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{

    UmiAiCodingRuntimeConfig config;
    umi_ai_coding_runtime_config_init(&config);
    assert(config.maximum_iterations == 3U);
    assert(config.rollback_on_validation_failure == 1);
    (void)strcpy(config.provider_id, "provider");
    (void)strcpy(config.model_id, "model");
    assert(umi_ai_coding_runtime_config_validate(&config) == UMI_STATUS_OK);

    return 0;
}
