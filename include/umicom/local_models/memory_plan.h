/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/local_models/memory_plan.h
 *
 * PURPOSE:
 *   Separate declared CPU model costs from native available-memory observations without launching an inference backend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_LOCAL_MODELS_MEMORY_PLAN_H
#define UMICOM_LOCAL_MODELS_MEMORY_PLAN_H
#include <stdbool.h>
#include <stdint.h>
#include "umicom/desktop_system/monitor.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_MODEL_MEMORY_WEIGHTS 1U
#define UMI_MODEL_MEMORY_CACHE 2U
#define UMI_MODEL_MEMORY_WORKSPACE 4U
#define UMI_MODEL_MEMORY_RUNTIME 8U
#define UMI_MODEL_MEMORY_ALL 15U

typedef struct UmiModelMemoryRequirements {
    uint64_t weights_bytes;
    uint64_t cache_bytes;
    uint64_t workspace_bytes;
    uint64_t runtime_bytes;
    unsigned int known_components;
} UmiModelMemoryRequirements;

typedef enum UmiModelMemoryVerdict {
    UMI_MODEL_MEMORY_INCOMPLETE = 1,
    UMI_MODEL_MEMORY_UNAVAILABLE = 2,
    UMI_MODEL_MEMORY_FIXTURE = 3,
    UMI_MODEL_MEMORY_STALE = 4,
    UMI_MODEL_MEMORY_INSUFFICIENT = 5,
    UMI_MODEL_MEMORY_WITHIN_DECLARED_BUDGET = 6
} UmiModelMemoryVerdict;

typedef struct UmiModelMemoryAssessment {
    UmiModelMemoryVerdict verdict;
    unsigned int known_components;
    bool requirements_complete;
    bool memory_known;
    bool observation_current;
    uint64_t known_payload_bytes;
    uint64_t host_reserve_bytes;
    uint64_t total_declared_bytes;
    uint64_t observed_total_bytes;
    uint64_t observed_available_bytes;
    uint64_t observation_age_millis;
    uint64_t remaining_bytes;
    uint64_t deficit_bytes;
} UmiModelMemoryAssessment;

/* Project declared CPU requirements against the existing desktop memory
 * observation. No hardware capture, allocation, reservation or model operation
 * occurs. An OK return means a complete assessment, NOT permission to launch.
 * The caller must inspect verdict; even WITHIN_DECLARED_BUDGET is only arithmetic
 * over supplied estimates and transient observations, not guaranteed peak fit.
 *
 * Supply resident weight costs for the selected backend, not simply file size.
 * CACHE covers that backend's context state; WORKSPACE covers temporary loading
 * and inference storage; RUNTIME covers additional engine/allocator overhead.
 * A known zero is allowed except for weights. Unknown fields must be zero and
 * their mask bits absent. Missing costs produce INCOMPLETE, never a guessed fit.
 * Shared GPU memory is part of system RAM, not extra CPU capacity. GPU admission
 * and device reservations need a separate backend-specific contract.
 *
 * Read memory only when snapshot.memoryStatus is OK, regardless of Capture's
 * overall status. A NULL snapshot or failed memory status means UNAVAILABLE.
 * OK memory requires totalBytes > 0 and availableBytes <= totalBytes. Fixture
 * observations cannot establish current host capacity. Nonzero fixture flags
 * keep the existing desktop snapshot's compatibility meaning.
 *
 * The host measures observation_age_millis with its own monotonic elapsed-time
 * clock from the capture; maximum_age_millis must be nonzero. Do not mix the
 * snapshot's platform clock origin with another clock. Age is supplied evidence,
 * not authenticated freshness. Recheck immediately before any later admission.
 * Verdict priority: incomplete, unavailable, fixture, stale, insufficient, within
 * declared budget. The report's flags expose secondary conditions independently.
 * Reserve is added once. Remaining/deficit are set only for a complete current
 * observation; all arithmetic overflow returns CAPACITY_EXCEEDED.
 *
 * Keep inputs stable and separate from output. Every refusal preserves output
 * bytes. No existing model run/session policy is changed by this new service. */
UmiStatus UmiModelMemoryAssess(const UmiModelMemoryRequirements *requirements,
    const UmiDesktopSystemSnapshot *snapshot,uint64_t observation_age_millis,
    uint64_t maximum_age_millis,uint64_t host_reserve_bytes,
    UmiModelMemoryAssessment *out_assessment);

/* Explicit dense key/value geometry for a backend that really retains these
 * arrays. This is not a universal GGUF architecture estimator: compressed,
 * sliding-window, recurrent, paged and hybrid caches may have different layouts.
 * key/value element widths are 1, 2, 4 or 8 bytes; alignment and allocator costs
 * belong in the caller's separate overhead. All geometry values are positive.
 * The function checks every product and sum without allocation or file I/O. */
typedef struct UmiModelDenseKvShape {
    uint64_t token_capacity;
    uint64_t sequence_capacity;
    uint64_t layers;
    uint64_t key_heads;
    uint64_t value_heads;
    uint64_t key_dimensions;
    uint64_t value_dimensions;
    uint32_t key_element_bytes;
    uint32_t value_element_bytes;
} UmiModelDenseKvShape;
typedef struct UmiModelKvFootprint {
    uint64_t key_bytes;
    uint64_t value_bytes;
    uint64_t total_bytes;
} UmiModelKvFootprint;
/* Keep shape and output separate and stable. Every failure preserves output. */
UmiStatus UmiModelDenseKvCacheBytes(const UmiModelDenseKvShape *shape,
    UmiModelKvFootprint *out_footprint);

/* Solve the same dense KV geometry for a caller-selected cache budget.
 * shape.token_capacity is the requested upper bound, not an unbounded search.
 * Budget covers KV payload only: weights, runtime, allocator alignment and
 * workspace costs must be budgeted separately through UmiModelMemoryAssess.
 * Zero tokens is a successful arithmetic answer when one token cannot fit.
 * No memory is reserved and no model/context configuration is changed. */
typedef struct UmiModelKvTokenPlan {
    uint64_t requested_tokens;
    uint64_t affordable_tokens;
    uint64_t bytes_per_token;
    uint64_t cache_bytes;
    uint64_t unused_budget_bytes;
    bool request_fits;
} UmiModelKvTokenPlan;
/* Invalid geometry or arithmetic overflow preserves output. Inputs/output
 * must be stable and disjoint. Reuses DenseKvCacheBytes with one token so a
 * huge requested capacity can be reduced without overflowing its full cost. */
UmiStatus UmiModelDenseKvPlanTokens(const UmiModelDenseKvShape *shape,
    uint64_t cache_budget_bytes, UmiModelKvTokenPlan *out_plan);

/* Compose a host observation with dense KV payload geometry. Non-cache costs
 * must exclude CACHE: cache_bytes must be zero and its known bit absent, so the
 * same cost cannot be counted twice. Missing other costs remain INCOMPLETE.
 * The implementation uses a known-zero cache for the fixed-cost assessment,
 * then spends only its remaining bytes on the existing dense-token planner.
 *
 * Read selected_memory and cache only when cache_plan_available is true. False
 * means fixed_memory explains why no trustworthy arithmetic budget is available
 * (incomplete, unavailable, fixture, stale or already insufficient). A true
 * flag can still select zero tokens; it is not a usable-context or launch grant.
 * No host capture, allocation, reservation, device admission or model operation
 * occurs. The estimates must include loading peaks and non-KV overhead; a
 * changing host observation can invalidate them immediately after this call.
 *
 * Invalid geometry, duplicate cache accounting and arithmetic overflow preserve
 * output. Inputs must remain stable and disjoint from the output. Geometry is
 * checked even when host memory is unavailable. Existing memory-assessment age,
 * fixture and status rules apply; reserve is accounted for exactly once. */
typedef struct UmiModelHostKvPlan {
    UmiModelMemoryAssessment fixed_memory;
    bool cache_plan_available;
    UmiModelKvTokenPlan cache;
    UmiModelMemoryAssessment selected_memory;
} UmiModelHostKvPlan;
UmiStatus UmiModelDenseKvPlanHostMemory(const UmiModelMemoryRequirements *non_cache,
    const UmiModelDenseKvShape *shape, const UmiDesktopSystemSnapshot *snapshot,
    uint64_t observation_age_millis, uint64_t maximum_age_millis,
    uint64_t host_reserve_bytes, UmiModelHostKvPlan *out_plan);
#ifdef __cplusplus
}
#endif
#endif
