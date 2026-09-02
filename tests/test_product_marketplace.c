/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_product_marketplace.c
 *
 * PURPOSE:
 *   Exercise reusable catalogue, update-policy and installation-state contracts
 *   for future software-centre and product-management applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <string.h>

#include "umicom/product/installation_state.h"
#include "umicom/product/marketplace.h"
#include "umicom/product/update_policy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiProductMarketplaceItemRegistry *marketplace = NULL;
    UmiProductMarketplaceItemSnapshot product = {0};
    UmiProductUpdatePolicyRegistry *policies = NULL;
    UmiProductUpdatePolicySnapshot policy = {0};
    UmiProductInstallationRegistry *installations = NULL;
    UmiProductInstallationSnapshot installation = {0};

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_product_marketplace_registry_create(&marketplace) != UMI_STATUS_OK) return 1;
    (void)strcpy(product.id, "studio");
    product.compatible = 1;
    product.trusted = 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_product_marketplace_registry_upsert(marketplace, &product) != UMI_STATUS_OK) return 2;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_product_update_policy_registry_create(&policies) != UMI_STATUS_OK) return 3;
    (void)strcpy(policy.id, "studio-policy");
    (void)strcpy(policy.product_id, "studio");
    policy.require_signature = 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_product_update_policy_registry_upsert(policies, &policy) != UMI_STATUS_OK) return 4;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_product_installation_state_registry_create(&installations) != UMI_STATUS_OK) return 5;
    (void)strcpy(installation.id, "studio-installed");
    (void)strcpy(installation.product_id, "studio");
    (void)strcpy(installation.version, "0.15.0");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_product_installation_state_registry_upsert(installations, &installation) != UMI_STATUS_OK) return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_product_installation_state_registry_set_state(
            installations, "studio-installed", 2, 1, 1) != UMI_STATUS_OK) return 7;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_product_installation_state_registry_find(
            installations, "studio-installed", &installation) != UMI_STATUS_OK ||
        installation.state != 2 || !installation.verified || !installation.rollback_available) return 8;

    umi_product_installation_state_registry_destroy(installations);
    umi_product_update_policy_registry_destroy(policies);
    umi_product_marketplace_registry_destroy(marketplace);
    return 0;
}
