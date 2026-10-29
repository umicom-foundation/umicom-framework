/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/observed_impact.h
 *
 * PURPOSE:
 *   Derive conservative build selections from owned source observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_OBSERVED_IMPACT_H
#define UMICOM_DISTRIBUTION_RUNTIME_OBSERVED_IMPACT_H
#include "umicom/distribution/runtime/impact.h"
#include "umicom/distribution/runtime/source_observation.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiObservedImpactFallback {
    UMI_OBSERVED_IMPACT_BASELINE_MISSING = 1U,
    UMI_OBSERVED_IMPACT_INVENTORY_CHANGED = 2U,
    UMI_OBSERVED_IMPACT_OUTSIDE_ROOT = 4U
} UmiObservedImpactFallback;

typedef struct UmiObservedImpactSummary {
    size_t observedFiles;
    size_t changedFiles;
    uint32_t fallbackReasons;
    bool changesKnown; /* False when the two observations cannot be compared. */
} UmiObservedImpactSummary;

/** Compare already captured bytes and feed changed paths into the existing
 * dependency/test-owner selector. Current must belong to inventory exactly;
 * a stale current observation returns INVALID_STATE without publishing outputs.
 * Baseline may be NULL. An absent baseline or changed inventory selects all
 * configured targets/tests and records why; changedFiles then remains unknown,
 * represented by zero storage with changesKnown false. A changed input outside
 * the exact source root also widens coverage rather than dropping the edit.
 *
 * Both observations borrow nothing from the original inventory. This call
 * borrows inputs until return; the successful plan owns its names. *outPlan
 * must be NULL and outSummary is mandatory. Every failure preserves both.
 * The existing planner's malformed metadata, limits and graph fallbacks apply.
 * No files, processes, Git state or historical test results are read here.
 *
 * Equality covers inventory-listed bytes only, including comment edits. Hosts
 * must reconfigure after file membership/configuration changes, account for
 * unlisted inputs and toolchain/environment changes, and separately schedule
 * earlier failures or missing qualification. A no-work plan is not a pass or
 * permission to reuse artifacts. Before/after reads are not an atomic snapshot.
 * This API does not persist baselines, launch work or implement automatic retries.
 */
UmiStatus UmiReleaseImpactFromObservations(const UmiReleaseInventory *inventory,
    const UmiReleaseSourceObservation *baseline,
    const UmiReleaseSourceObservation *current,
    UmiReleaseImpactPlan **outPlan, UmiObservedImpactSummary *outSummary);

/** Combine observed source changes with required retry identities. requiredTests
 * obeys CreateWithTests, including exact names, bounded input and conservative
 * fallback for an unknown test. The observation summary still counts source
 * changes only; a zero changedFiles value does not imply an empty test selection.
 * No historical report is trusted or loaded here. The host supplies the retry
 * list from separately checked evidence and must also handle unknown coverage.
 * All ownership, fallback and output-preservation rules above remain in force. */
UmiStatus UmiReleaseImpactFromObservationsWithTests(
    const UmiReleaseInventory *inventory,
    const UmiReleaseSourceObservation *baseline,
    const UmiReleaseSourceObservation *current,
    const char *const *requiredTests, size_t requiredCount,
    UmiReleaseImpactPlan **outPlan, UmiObservedImpactSummary *outSummary);
#ifdef __cplusplus
}
#endif
#endif
