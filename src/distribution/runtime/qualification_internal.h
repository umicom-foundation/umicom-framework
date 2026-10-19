/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_internal.h
 * Purpose: Share private owned qualification state between recording and assessment.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_QUALIFICATION_INTERNAL_H
#define UMICOM_QUALIFICATION_INTERNAL_H
#include "umicom/distribution/runtime/qualification.h"
#include "umicom/base/sha256.h"
typedef struct UmiQualificationTest {
    UmiReleaseQualificationResult result;
    bool recorded;
} UmiQualificationTest;
struct UmiReleaseQualification {
    UmiReleaseQualificationIdentity identity;
    UmiReleaseImpactSummary selection;
    UmiReleaseImpactItem *items;
    size_t count, testStart;
    UmiQualificationTest *tests;
    UmiReleaseQualificationResult build, testProcess;
    char inventory[65], source[65], toolchain[65];
    char afterInventory[65], afterSource[65], afterToolchain[65];
    uint32_t closeBlockers;
    UmiStatus executionStatus;
    bool buildRecorded, closed, testProcessRecorded;
};
bool UmiQualificationDigestValid(const char *text);
UmiStatus UmiQualificationPlanFingerprint(UmiReleaseQualification *run);
UmiStatus UmiQualificationMatch(const UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity);
size_t UmiQualificationFindTest(const UmiReleaseQualification *run, const char *name);
UmiStatus UmiQualificationResultValid(const UmiReleaseQualificationResult *result);
struct UmiQualificationView;
void UmiQualificationViewMarkSaved(struct UmiQualificationView *view);
#endif
