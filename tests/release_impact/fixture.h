/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_impact/fixture.h
 * Purpose: Build small synthetic inventories for impact regression scenarios.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IMPACT_TEST_FIXTURE_H
#define UMICOM_IMPACT_TEST_FIXTURE_H
#include "umicom/distribution/runtime/impact.h"
#include <stdio.h>
#define REQUIRE(expr) do { if (!(expr)) { (void)fprintf(stderr, "Failed at line %d: %s\n", __LINE__, #expr); return 1; } } while (0)
typedef struct Fixture { char *text; size_t used, capacity; } Fixture;
bool FixtureStart(Fixture *fixture);
bool FixtureRow(Fixture *fixture, const char *kind, const char *name, const char *owner, const char *state, const char *detail);
bool FixtureTarget(Fixture *fixture, const char *name, const char *dependency);
bool FixtureTest(Fixture *fixture, const char *name, const char *target, const char *helper, bool always);
/* Versioned related-test metadata uses exact owned names, not command parsing. */
bool FixtureRelatedTest(Fixture *fixture, const char *name, const char *target,
    const char *const *related, size_t relatedCount, bool always);
bool FixtureSource(Fixture *fixture, const char *name, const char *target);
UmiStatus FixtureParse(Fixture *fixture, UmiReleaseInventory **inventory);
void FixtureDestroy(Fixture *fixture);
bool PlanContains(const UmiReleaseImpactPlan *plan, const char *name, uint32_t reason);
#endif
