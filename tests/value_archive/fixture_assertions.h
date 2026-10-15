/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/fixture_assertions.h
 * PURPOSE: Keep state-establishing assertions active in value-integrity test fixtures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_FIXTURE_ASSERTIONS_H
#define UMICOM_TEST_FIXTURE_ASSERTIONS_H

/* Existing domain fixtures use assert calls both to prepare valid values and
 * to check results. Those calls must execute before the added integrity cases,
 * even when an optimized build defines NDEBUG. Re-including assert.h after
 * undefining NDEBUG is supported by C and restores its active definition.
 * This header is test-only; production targets keep their configured policy.
 * New fixtures should prefer explicit checked setup calls for required work. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#endif
