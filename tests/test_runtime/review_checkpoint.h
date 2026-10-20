/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/review_checkpoint.h
 * PURPOSE: Retain the last completed GUI test phase when CTest terminates a hang.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_REVIEW_CHECKPOINT_H
#define UMICOM_TEST_REVIEW_CHECKPOINT_H
#include <stdio.h>
/* Flush before entering callbacks: a timeout cannot run test cleanup and must
 * still leave evidence. These markers do not change timeouts or test outcomes. */
static inline void UmiTestReviewCheckpoint(const char *scenario, const char *phase)
{
    (void)fprintf(stderr, "[review-test] %s: %s\n", scenario, phase);
    (void)fflush(stderr);
}
#endif
