/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_helix_bridge/test_support.h
 *
 * PURPOSE:
 *   Provide a deterministic existing-runtime test provider for AI/Helix bridge tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef TESTS_AI_HELIX_BRIDGE_TEST_SUPPORT_H
#define TESTS_AI_HELIX_BRIDGE_TEST_SUPPORT_H

#include "umicom/ai/helix/ai_helix.h"
#include "umicom/ai/runtime.h"
#include "umicom/ai/provider.h"

typedef struct TestAiProviderState {
    char response_text[UMI_AI_HELIX_RAW_RESPONSE_CAPACITY];
    UmiStatus generate_status;
    UmiAiFinishReason finish_reason;
    UmiAiRequest last_request;
    int call_count;
} TestAiProviderState;

UmiStatus test_ai_runtime_prepare(UmiAiRuntime *runtime,
                                  TestAiProviderState *state,
                                  const char *response_text);
UmiStatus test_ai_provider_set_response(TestAiProviderState *state,
                                        const char *response_text);
void test_bridge_request(UmiAiHelixAgentRequest *request,
                         UmiHelixAgentRole role);

#define TEST_CHECK(condition) do { if (!(condition)) return 1; } while (0)

#endif
