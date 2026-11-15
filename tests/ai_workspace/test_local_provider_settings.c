/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_provider_settings.c
 *
 * PURPOSE:
 *   Verify canonical local-provider settings without treating a matching name as a private instance type.
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

static UmiStatus OtherGenerate(void *instance, const UmiAiRequest *request, UmiAiResponse *response)
{ (void)instance; (void)request; (void)response; return UMI_STATUS_NOT_IMPLEMENTED; }
int main(void)
{
    if (!UmiAiWorkspaceLocalProviderAvailable()) return 77;
    UmiAiWorkspaceCancellation *token = NULL;
    assert(UmiAiWorkspaceCancellationCreate(&token) == UMI_STATUS_OK);
    UmiAiProvider provider = {0};
    UmiStatus status = UmiAiWorkspaceLocalProviderCreate(8080U, 2500U, token, &provider);
    if (status == UMI_STATUS_UNAVAILABLE) { UmiAiWorkspaceCancellationDestroy(token); return 77; }
    assert(status == UMI_STATUS_OK);
    AwLocalProviderSettings snapshot, before;
    memset(&snapshot, 0x5a, sizeof(snapshot));
    memcpy(&before, &snapshot, sizeof(before));
    assert(AwLocalChatProviderSettingsRead(NULL, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
    assert(AwLocalChatProviderSettingsRead(&provider, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UmiAiProvider other = provider;
    int unrelated_storage = 0;
    other.instance = &unrelated_storage; other.generate = OtherGenerate;
    assert(AwLocalChatProviderSettingsRead(&other, &snapshot) == UMI_STATUS_NOT_IMPLEMENTED);
    assert(memcmp(&before, &snapshot, sizeof(before)) == 0);
    other = provider; other.kind = UMI_AI_PROVIDER_REMOTE;
    assert(AwLocalChatProviderSettingsRead(&other, &snapshot) == UMI_STATUS_NOT_IMPLEMENTED);
    other = provider; other.provider_id = "renamed";
    assert(AwLocalChatProviderSettingsRead(&other, &snapshot) == UMI_STATUS_NOT_IMPLEMENTED);
    other = provider; other.destroy = NULL;
    assert(AwLocalChatProviderSettingsRead(&other, &snapshot) == UMI_STATUS_NOT_IMPLEMENTED);
    other = provider; other.instance = NULL;
    assert(AwLocalChatProviderSettingsRead(&other, &snapshot) == UMI_STATUS_NOT_IMPLEMENTED);
    assert(memcmp(&before, &snapshot, sizeof(before)) == 0);
    assert(AwLocalChatProviderSettingsRead(&provider, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.port == 8080U && snapshot.timeout_millis == 2500U && snapshot.cancellation == token);
    provider.destroy(provider.instance);
    /* The copied numeric settings do not depend on private adapter storage;
     * the token is still borrowed from its separate owner. */
    assert(snapshot.port == 8080U && snapshot.timeout_millis == 2500U);
    UmiAiWorkspaceCancellationDestroy(token);
    return 0;
}
