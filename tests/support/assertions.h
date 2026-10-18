/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/support/assertions.h
 * PURPOSE: Keep required fixture work and contract checks active in every build.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_SUPPORT_ASSERTIONS_H
#define UMICOM_TEST_SUPPORT_ASSERTIONS_H

/* These legacy fixtures perform some required setup inside assert expressions.
 * A Release build must execute that setup and check failures, too. Re-including
 * the standard header after clearing NDEBUG restores active C assertions.
 * This affects this test translation unit, never the separately compiled
 * production libraries. New tests should use explicit setup with cleanup.
 *
 * Include this before fixture helpers so their assertions are active when
 * they are defined. Reusing one policy keeps independent test builds honest. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#endif
