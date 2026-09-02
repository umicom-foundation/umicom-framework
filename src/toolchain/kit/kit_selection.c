/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/kit_selection.c
 * PURPOSE: Implement deterministic developer-kit selection policy.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/kit_selection.h"

#include <stdio.h>
#include <string.h>

/* Provide the eligible operation used by this module and its client applications. */
static int eligible(const UmiToolchainKitSnapshot *kit,
                    const UmiToolchainKitSelectionRequest *request)
{
    /* Apply this operation only while the related capability or state is available. */
    if (kit->enabled == 0 || kit->state == UMI_TOOLCHAIN_KIT_UNAVAILABLE)
        return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->state == UMI_TOOLCHAIN_KIT_DEGRADED && request->allow_degraded == 0)
        return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_kit_is_cross_compile(kit) != 0 &&
        request->allow_cross_compile == 0)
        return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_toolchain_kit_supports(kit, request->required_capabilities))
        return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request->target_triple != NULL && request->target_triple[0] != '\0' &&
        strcmp(kit->target_triple, request->target_triple) != 0)
        return 0;
    return 1;
}

/* Provide the score kit operation used by this module and its client applications. */
static uint64_t score_kit(const UmiToolchainKitSnapshot *kit,
                          const UmiToolchainKitSelectionRequest *request)
{
    uint64_t score = (uint64_t)kit->priority * UINT64_C(1000);
    /* Apply this operation only while the related capability or state is available. */
    if (kit->state == UMI_TOOLCHAIN_KIT_READY) score += UINT64_C(1000000000);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request->target_triple != NULL && request->target_triple[0] != '\0' &&
        strcmp(kit->target_triple, request->target_triple) == 0)
        score += UINT64_C(100000000);
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_kit_is_cross_compile(kit) == 0)
        score += UINT64_C(1000000);
    /* Apply this branch only when its contract condition is satisfied. */
    if ((kit->capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_SDK) != 0U)
        score += UINT64_C(10000);
    return score;
}

/* Provide the finish operation used by this module and its client applications. */
static void finish(UmiToolchainKitSelectionSnapshot *selection,
                   const UmiToolchainKitSnapshot *kit,
                   UmiToolchainKitSelectionSource source,
                   uint64_t score,
                   const char *reason)
{
    selection->kit = *kit;
    selection->source = source;
    selection->score = score;
    (void)snprintf(selection->reason, sizeof(selection->reason), "%s", reason);
}

/*
 * Initialise toolchain kit selection request from caller-provided values so later
 * operations receive a known state.
 */
void umi_toolchain_kit_selection_request_init(
    UmiToolchainKitSelectionRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->struct_size = (uint32_t)sizeof(*request);
    request->api_version = UMI_TOOLCHAIN_KIT_SELECTION_API_VERSION;
    request->allow_cross_compile = 1;
}

/*
 * Provide the toolchain kit select operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_kit_select(
    const UmiToolchainKitCatalogue *catalogue,
    const UmiToolchainKitSelectionRequest *request,
    UmiToolchainKitSelectionSnapshot *out_selection)
{
    UmiToolchainKitSnapshot candidate;
    UmiToolchainKitSnapshot best;
    size_t index;
    uint64_t best_score = 0U;
    int found = 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || request == NULL || out_selection == NULL ||
        request->struct_size < sizeof(*request) ||
        request->api_version != UMI_TOOLCHAIN_KIT_SELECTION_API_VERSION)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_selection, 0, sizeof(*out_selection));
    out_selection->struct_size = (uint32_t)sizeof(*out_selection);
    out_selection->api_version = UMI_TOOLCHAIN_KIT_SELECTION_API_VERSION;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request->preferred_kit_id != NULL && request->preferred_kit_id[0] != '\0' &&
        umi_toolchain_kit_catalogue_find(catalogue, request->preferred_kit_id,
                                         &candidate) == UMI_STATUS_OK &&
        eligible(&candidate, request)) {
        finish(out_selection, &candidate, UMI_TOOLCHAIN_KIT_SELECTION_PREFERRED,
               UINT64_MAX, "explicit preferred kit");
        return UMI_STATUS_OK;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->binding_kind != UMI_TOOLCHAIN_KIT_BINDING_UNKNOWN &&
        request->consumer_id != NULL && request->consumer_id[0] != '\0' &&
        umi_toolchain_kit_catalogue_resolve_binding(catalogue,
                                                    request->binding_kind,
                                                    request->consumer_id,
                                                    &candidate) == UMI_STATUS_OK &&
        eligible(&candidate, request)) {
        finish(out_selection, &candidate, UMI_TOOLCHAIN_KIT_SELECTION_BINDING,
               UINT64_MAX - UINT64_C(1), "consumer binding");
        return UMI_STATUS_OK;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_toolchain_kit_catalogue_count(catalogue); ++index) {
        uint64_t score;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_toolchain_kit_catalogue_at(catalogue, index, &candidate) !=
            UMI_STATUS_OK) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!eligible(&candidate, request)) continue;
        score = score_kit(&candidate, request);
        /* Apply this branch only when its contract condition is satisfied. */
        if (!found || score > best_score ||
            (score == best_score && strcmp(candidate.id, best.id) < 0)) {
            best = candidate;
            best_score = score;
            found = 1;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!found) return UMI_STATUS_NOT_FOUND;
    finish(out_selection, &best, UMI_TOOLCHAIN_KIT_SELECTION_RANKED,
           best_score, "highest-ranked compatible kit");
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain kit selection source text operation used by this module and its
 * client applications.
 */
const char *umi_toolchain_kit_selection_source_text(
    UmiToolchainKitSelectionSource source)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (source) {
        case UMI_TOOLCHAIN_KIT_SELECTION_PREFERRED: return "PREFERRED";
        case UMI_TOOLCHAIN_KIT_SELECTION_BINDING: return "BINDING";
        case UMI_TOOLCHAIN_KIT_SELECTION_RANKED: return "RANKED";
        default: return "NONE";
    }
}
