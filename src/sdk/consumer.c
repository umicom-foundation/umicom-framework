/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/consumer.c
 *
 * PURPOSE:
 *   Describe one external application that consumes the installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/consumer.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
/* Check that sdk consumer satisfies its contract before another service relies on it. */
/* The earlier validator indexed a counted array without confirming storage. The replacement checks array bounds and shares requirement validation; retain the former implementation for ownership review. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_sdk_consumer_validate(const UmiSdkConsumer *c){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->application_id==NULL||c->application_id[0]=='\0'||c->requirement.required_abi==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->target_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->required_targets[i]==NULL||c->required_targets[i][0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
#endif
UmiStatus umi_sdk_consumer_validate(const UmiSdkConsumer *consumer)
{
    /* A count does not supply storage. Check the borrowed array before reading
     * its entries; the caller remains responsible for its actual allocation. */
    if (consumer == NULL || consumer->application_id == NULL ||
        consumer->application_id[0] == '\0' ||
        umi_sdk_requirement_validate(&consumer->requirement) != UMI_STATUS_OK ||
        (consumer->target_count != 0U && consumer->required_targets == NULL) ||
        consumer->target_count > SIZE_MAX / sizeof *consumer->required_targets)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < consumer->target_count; ++index) {
        if (consumer->required_targets[index] == NULL ||
            consumer->required_targets[index][0] == '\0')
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_sdk_consumer_check(const UmiSdkConsumer *consumer,
    const UmiSdkManifest *manifest, UmiSdkConsumerCheck *out)
{
    if (out == NULL || umi_sdk_consumer_validate(consumer) != UMI_STATUS_OK ||
        umi_sdk_manifest_validate(manifest) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Keep reusable admission policy in Framework. These are borrowed metadata
     * declarations, not inspected files or loaded libraries. Publish one owned
     * summary only after both complete input inventories have been validated. */
    UmiSdkConsumerCheck candidate = {0};
    candidate.first_missing_index = SIZE_MAX;
    candidate.version_abi_compatible = umi_sdk_compatible(&consumer->requirement, manifest);
    candidate.targets_checked = consumer->target_count;
    for (size_t required = 0U; required < consumer->target_count; ++required) {
        int found = 0;
        for (size_t available = 0U; available < manifest->component_count; ++available) {
            if (strcmp(consumer->required_targets[required],
                       manifest->components[available].target_name) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            if (candidate.targets_missing == 0U)
                candidate.first_missing_index = required;
            ++candidate.targets_missing;
        }
    }
    candidate.declarations_satisfied = candidate.version_abi_compatible &&
        candidate.targets_missing == 0U;
    *out = candidate;
    return UMI_STATUS_OK;
}
