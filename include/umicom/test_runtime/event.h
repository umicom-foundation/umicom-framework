/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/event.h
 *
 * PURPOSE:
 *   Describe typed test-runtime events published after state changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EVENT
#define UMICOM_TEST_RUNTIME_EVENT

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime event data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeEvent {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t kind;
    uint64_t sequence;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeEvent;

/**
 * Initialise test runtime event from caller-provided values so later operations receive a
 * known state.
 */
void umi_test_runtime_event_init(UmiTestRuntimeEvent *value, const char *id);
/**
 * Check that test runtime event satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_test_runtime_event_validate(const UmiTestRuntimeEvent *value);
/**
 * Provide the test runtime event set name operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_event_set_name(UmiTestRuntimeEvent *value, const char *name);
/**
 * Provide the test runtime event set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_event_set_detail(UmiTestRuntimeEvent *value, const char *detail);
/**
 * Provide the test runtime event set kind operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_event_set_kind(UmiTestRuntimeEvent *value, uint64_t number);
/**
 * Provide the test runtime event set sequence operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_event_set_sequence(UmiTestRuntimeEvent *value, uint64_t number);
/**
 * Provide the test runtime event touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_event_touch(UmiTestRuntimeEvent *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime event same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_event_same_identity(const UmiTestRuntimeEvent *left, const UmiTestRuntimeEvent *right);


/** Publish a complete reviewed value while expected_revision still matches.
 * Copy the live record, edit that copy with the ordinary setters, and retain
 * the revision observed before editing. Both values must validate and have
 * the same ID. A stale review returns INVALID_STATE; exhausted revision space
 * returns CAPACITY_EXCEEDED. All failures leave the live value unchanged.
 * Success copies every proposal field and assigns one new live revision;
 * the proposal's own revision does not control publication. Self-assignment
 * is allowed and also advances once. Other overlapping storage is unsupported.
 * Serialize access on the owner. This value operation allocates nothing and
 * does not perform I/O, authenticate evidence or run the described workflow. */
UmiStatus umi_test_runtime_event_replace_if_current(UmiTestRuntimeEvent *value,
    uint64_t expected_revision, const UmiTestRuntimeEvent *proposal);

#ifdef __cplusplus
}
#endif
#endif
