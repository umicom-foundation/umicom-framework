/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/test_policy.h
 * PURPOSE: Preserve explicit release-test decisions for one captured build.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/test_policy.h
 * Purpose: Preserve explicit release-test decisions for one captured build.
 * Author: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_TEST_POLICY_H
#define UMICOM_DISTRIBUTION_RUNTIME_TEST_POLICY_H
#include "umicom/distribution/runtime/inventory.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReleaseTestPolicy UmiReleaseTestPolicy;
typedef enum UmiReleaseTestRequirement {
    UMI_RELEASE_TEST_UNASSIGNED,
    UMI_RELEASE_TEST_REQUIRED,
    UMI_RELEASE_TEST_OPTIONAL
} UmiReleaseTestRequirement;

/* Borrowed strings remain valid until the owning policy is destroyed. Required
 * and optional decisions need both an owner and an engineering reason. Names
 * are literal and case-sensitive; wildcards, prefixes and regexes have no role. */
typedef struct UmiReleaseTestRule {
    const char *name;
    const char *owner;
    const char *reason;
    UmiReleaseTestRequirement requirement;
} UmiReleaseTestRule;

typedef enum UmiReleaseTestPolicyFinding {
    UMI_RELEASE_POLICY_MISSING_RULE,
    UMI_RELEASE_POLICY_ORPHAN_RULE,
    UMI_RELEASE_POLICY_UNASSIGNED,
    UMI_RELEASE_POLICY_REQUIRED_UNAVAILABLE,
    UMI_RELEASE_POLICY_OPTIONAL_UNAVAILABLE,
    UMI_RELEASE_POLICY_MISSING_REGISTRATION,
    UMI_RELEASE_POLICY_ADDED_REGISTRATION
} UmiReleaseTestPolicyFinding;
typedef void (*UmiReleaseTestPolicyFindingFn)(void *context,
    UmiReleaseTestPolicyFinding finding, const char *name);
typedef struct UmiReleaseTestPolicyAssessment {
    size_t expectedTests, policyRules, requiredTests, optionalTests;
    size_t unassignedTests, missingRules, orphanRules;
    size_t requiredUnavailable, optionalUnavailable;
    size_t missingRegistrations, addedRegistrations;
    bool readyForExecution;
} UmiReleaseTestPolicyAssessment;

/* Version-1 policy TSV has the inventory's bounds and hex string encoding.
 * Parse copies its input; *outPolicy must be NULL and is unchanged on failure.
 * Duplicate names, unknown requirements and incomplete decisions are errors.
 * Immutable objects support concurrent readers, but not concurrent destruction. */
UmiStatus UmiReleaseTestPolicyParse(const char *text, size_t length,
    UmiReleaseTestPolicy **outPolicy);
void UmiReleaseTestPolicyDestroy(UmiReleaseTestPolicy *policy);
size_t UmiReleaseTestPolicyCount(const UmiReleaseTestPolicy *policy);
const UmiReleaseTestRule *UmiReleaseTestPolicyAt(const UmiReleaseTestPolicy *policy,
    size_t index);

/* Draft every configured test as unassigned, without choosing for the owner.
 * An empty test population is UNAVAILABLE. Output text is caller-owned and must
 * be released with UmiReleaseTestPolicyTextDestroy. *outText must be NULL;
 * neither output is changed on failure. No file or network access occurs. */
UmiStatus UmiReleaseTestPolicyDraft(const UmiReleaseInventory *configured,
    char **outText, size_t *outLength);
void UmiReleaseTestPolicyTextDestroy(char *text);

/* Return a full new policy document with one exact-name decision replaced.
 * The original immutable object and every other rule are retained unchanged.
 * Missing names return NOT_FOUND. Setting UNASSIGNED allows an empty owner and
 * reason; REQUIRED and OPTIONAL require nonblank values. The generation/root/
 * configuration context is always retained. Output rules match Draft above. */
UmiStatus UmiReleaseTestPolicyEdit(const UmiReleaseTestPolicy *policy,
    const char *name, UmiReleaseTestRequirement requirement,
    const char *owner, const char *reason, char **outText, size_t *outLength);

/* Assess registration readiness, never execution outcomes or release approval.
 * All three inputs must describe the same generation, roots and configuration.
 * No test may lack a decision and at least one must be required. Optional
 * disabled/missing-command registrations are reported without blocking. Added
 * or missing names always block, including an absent optional registration.
 * Context/input errors leave the result untouched and invoke no callbacks.
 * Callback strings are borrowed; callbacks must not destroy the inputs.
 * Owner/reason fields are assertions, not authenticated approvals. */
UmiStatus UmiReleaseTestPolicyAssess(const UmiReleaseInventory *configured,
    const UmiReleaseInventory *observed, const UmiReleaseTestPolicy *policy,
    UmiReleaseTestPolicyFindingFn finding, void *context,
    UmiReleaseTestPolicyAssessment *outAssessment);
#ifdef __cplusplus
}
#endif
#endif
