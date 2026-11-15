/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/conformance.c
 *
 * PURPOSE:
 *   Record conformance checks for an SDK consumer build.
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
#include "umicom/sdk/conformance.h"
#include <stddef.h>
#include <stdint.h>
/*
 * Provide the sdk conformance record operation used by this module and its client
 * applications.
 */
/* Check summary consistency and counter capacity before publishing another observation; the legacy void API latches refusal. The former implementation is retained for owner review. */
#if 0
void umi_sdk_conformance_record(UmiSdkConformance *s,int passed){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return;++s->checks_run;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(passed)++s->checks_passed;/* Use this fallback path when the earlier condition does not apply. */ else ++s->checks_failed;}
#endif
/* These counters describe caller-reported checks; they are not independent
 * proof of an SDK's ABI or runtime behavior. Subtraction avoids overflow while
 * checking the public structure's invariant. A zero summary is valid but empty. */
UmiStatus umi_sdk_conformance_validate(const UmiSdkConformance *state)
{
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (state->checks_passed > state->checks_run ||
        state->checks_failed != state->checks_run - state->checks_passed)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

UmiStatus umi_sdk_conformance_record_checked(UmiSdkConformance *state, int passed)
{
    UmiStatus status = umi_sdk_conformance_validate(state);
    if (status != UMI_STATUS_OK) return status;
    if (state->checks_run == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiSdkConformance next = *state;
    ++next.checks_run;
    if (passed) ++next.checks_passed;
    else ++next.checks_failed;
    *state = next;
    return UMI_STATUS_OK;
}

void umi_sdk_conformance_record(UmiSdkConformance *state, int passed)
{
    if (umi_sdk_conformance_record_checked(state, passed) != UMI_STATUS_OK && state != NULL) {
        /* This compatibility API cannot return refusal. Latch a nonzero failed
         * count instead of silently retaining a passing verdict after a lost
         * check. Keep the other counts for diagnosis; explicitly reinitialize
         * the whole summary before collecting a new set of observations. */
        state->checks_failed = SIZE_MAX;
    }
}
/*
 * Provide the sdk conformance passed operation used by this module and its client
 * applications.
 */
/* Require a consistent nonempty all-pass summary rather than interpreting a missing failure count as proof of success. The former implementation is retained for owner review. */
#if 0
int umi_sdk_conformance_passed(const UmiSdkConformance *s){return s!=NULL&&s->checks_run>0U&&s->checks_failed==0U;}
#endif
int umi_sdk_conformance_passed(const UmiSdkConformance *state)
{
    return umi_sdk_conformance_validate(state) == UMI_STATUS_OK &&
        state->checks_run != 0U && state->checks_failed == 0U;
}
