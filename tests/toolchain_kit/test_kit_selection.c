/* Umicom Framework | deterministic kit selection test | Sammy Hegab | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/kit_selection.h"

static UmiToolchainKitSnapshot make_kit(const char *id, uint32_t priority)
{
    UmiToolchainKitSnapshot kit;
    umi_toolchain_kit_init(&kit, id, id);
    (void)strcpy(kit.profile_id, "profile");
    (void)strcpy(kit.host_triple, "x86_64-test");
    (void)strcpy(kit.target_triple, "x86_64-test");
    (void)strcpy(kit.c_compiler, "cc");
    kit.capabilities = UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD;
    kit.priority = priority;
    kit.state = UMI_TOOLCHAIN_KIT_READY;
    return kit;
}

int main(void)
{
    UmiToolchainKitCatalogue *catalogue = NULL;
    UmiToolchainKitSnapshot low = make_kit("kit.low", 10U);
    UmiToolchainKitSnapshot high = make_kit("kit.high", 100U);
    UmiToolchainKitSelectionRequest request;
    UmiToolchainKitSelectionSnapshot selection;
    assert(umi_toolchain_kit_catalogue_create(&catalogue) == UMI_STATUS_OK);
    assert(umi_toolchain_kit_catalogue_upsert(catalogue, &low) == UMI_STATUS_OK);
    assert(umi_toolchain_kit_catalogue_upsert(catalogue, &high) == UMI_STATUS_OK);
    umi_toolchain_kit_selection_request_init(&request);
    request.target_triple = "x86_64-test";
    request.required_capabilities = UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD;
    assert(umi_toolchain_kit_select(catalogue, &request, &selection) == UMI_STATUS_OK);
    assert(strcmp(selection.kit.id, "kit.high") == 0);
    request.preferred_kit_id = "kit.low";
    assert(umi_toolchain_kit_select(catalogue, &request, &selection) == UMI_STATUS_OK);
    assert(strcmp(selection.kit.id, "kit.low") == 0);
    assert(selection.source == UMI_TOOLCHAIN_KIT_SELECTION_PREFERRED);
    umi_toolchain_kit_catalogue_destroy(catalogue);
    return 0;
}
