/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/runtime_catalogue_fixture.h
 *
 * PURPOSE:
 *   Share a valid catalogue fixture across admission, revision and callback regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CATALOGUE_FIXTURE_H
#define UMICOM_TEST_RUNTIME_CATALOGUE_FIXTURE_H
#include "umicom/ai/runtime_catalogue.h"
#include <stdio.h>
#include <string.h>

static UmiAiRuntimeDescriptor catalogue_fixture(const char *id)
{
    UmiAiRuntimeDescriptor value = {0};
    (void)snprintf(value.runtime_id, sizeof(value.runtime_id), "%s", id);
    (void)strcpy(value.provider_id, "provider.local");
    (void)strcpy(value.display_name, "Local metadata fixture");
    (void)strcpy(value.model_id, "model.initial");
    value.provider_kind = UMI_AI_PROVIDER_LOCAL;
    value.transport = UMI_AI_RUNTIME_HTTP;
    value.capabilities = UMI_AI_RUNTIME_CHAT | UMI_AI_RUNTIME_STREAMING;
    value.context_tokens = 8192U;
    value.max_output_tokens = 1024U;
    value.configured = 1;
    value.local = 1;
    return value;
}
#endif
