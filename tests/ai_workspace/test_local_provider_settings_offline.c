/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_provider_settings_offline.c
 *
 * PURPOSE:
 *   Verify the actual offline provider branch does not expose streaming settings or overwrite outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
#include "local_chat_internal.h"
/* Compile the real offline branch independently of the configured library. */
#ifdef UMICOM_AI_WORKSPACE_HAS_HTTP
#error This fixture must compile without HTTP provider support.
#endif
#include "../../src/ai_workspace/local_chat_provider.c"
static UmiStatus Generate(void *instance, const UmiAiRequest *request, UmiAiResponse *response)
{ (void)instance; (void)request; (void)response; return UMI_STATUS_OK; }
int main(void)
{
    UmiAiProvider provider = {0};
    provider.structure_size = (uint32_t)sizeof(provider); provider.abi_version = 1U;
    provider.provider_id = "umicom.local-chat"; provider.kind = UMI_AI_PROVIDER_LOCAL;
    provider.generate = Generate;
    AwLocalProviderSettings settings, before;
    memset(&settings, 0x5a, sizeof(settings)); memcpy(&before, &settings, sizeof(before));
    assert(!UmiAiWorkspaceLocalProviderAvailable());
    assert(AwLocalChatProviderSettingsRead(NULL, &settings) == UMI_STATUS_INVALID_ARGUMENT);
    assert(AwLocalChatProviderSettingsRead(&provider, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(AwLocalChatProviderSettingsRead(&provider, &settings) == UMI_STATUS_NOT_IMPLEMENTED);
    assert(memcmp(&before, &settings, sizeof(before)) == 0);
    return 0;
}
