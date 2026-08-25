/* Umicom Framework | kit catalogue/binding test | Sammy Hegab | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/kit_catalogue.h"

static void make_kit(UmiToolchainKitSnapshot *kit, const char *id)
{
    umi_toolchain_kit_init(kit, id, id);
    (void)strcpy(kit->profile_id, "profile");
    (void)strcpy(kit->target_triple, "x86_64-test");
    (void)strcpy(kit->c_compiler, "cc");
    kit->capabilities = UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD;
    kit->state = UMI_TOOLCHAIN_KIT_READY;
}

int main(void)
{
    UmiToolchainKitCatalogue *catalogue = NULL;
    UmiToolchainKitSnapshot kit;
    UmiToolchainKitSnapshot resolved;
    UmiToolchainKitCatalogueSnapshot snapshot;
    assert(umi_toolchain_kit_catalogue_create(&catalogue) == UMI_STATUS_OK);
    make_kit(&kit, "kit.one");
    assert(umi_toolchain_kit_catalogue_upsert(catalogue, &kit) == UMI_STATUS_OK);
    assert(umi_toolchain_kit_catalogue_bind(catalogue,
        UMI_TOOLCHAIN_KIT_BINDING_PROJECT, "project.one", "kit.one") == UMI_STATUS_OK);
    assert(umi_toolchain_kit_catalogue_resolve_binding(catalogue,
        UMI_TOOLCHAIN_KIT_BINDING_PROJECT, "project.one", &resolved) == UMI_STATUS_OK);
    assert(strcmp(resolved.id, "kit.one") == 0);
    assert(umi_toolchain_kit_catalogue_snapshot(catalogue, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.kit_count == 1U && snapshot.binding_count == 1U);
    umi_toolchain_kit_catalogue_destroy(catalogue);
    return 0;
}
