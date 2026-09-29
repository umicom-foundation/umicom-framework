/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_RELEASE_TEST_POLICY_INTERNAL_H
#define UMICOM_RELEASE_TEST_POLICY_INTERNAL_H
#include "umicom/distribution/runtime/test_policy.h"
struct UmiReleaseTestPolicy {
    char *storage;
    const char *generation, *sourceRoot, *buildRoot, *configuration;
    UmiReleaseTestRule *rules;
    size_t count;
};
bool UmiReleaseTestPolicyNonblank(const char *text);
const UmiReleaseTestRule *UmiReleaseTestPolicyFind(const UmiReleaseTestPolicy *policy,
    const char *name);
#endif
