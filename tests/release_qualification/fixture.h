/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/fixture.h
 * Purpose: Compose synthetic qualification inputs without launching tools.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_QUALIFICATION_TEST_FIXTURE_H
#define UMICOM_QUALIFICATION_TEST_FIXTURE_H
#include "../release_impact/fixture.h"
#include "umicom/distribution/runtime/qualification.h"
#include "umicom/base/sha256.h"
typedef struct QualificationFixture {
    Fixture text;
    UmiReleaseInventory *inventory;
    UmiReleaseImpactPlan *plan;
    UmiReleaseSourceObservation *before;
    UmiReleaseQualification *run;
    UmiReleaseQualificationIdentity identity;
    char execution[65], toolchain[65];
} QualificationFixture;
UmiStatus QualificationRead(void *context, const char *path, size_t limit, size_t *outSize, char digest[65]);
int QualificationStart(QualificationFixture *fixture, size_t tests, bool noChanges);
void QualificationDestroy(QualificationFixture *fixture);
UmiReleaseQualificationResult QualificationResult(UmiReleaseQualificationOutcome outcome);
int QualificationRecordAll(QualificationFixture *fixture);
#endif
