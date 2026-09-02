/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/diagnostic_lock.h
 *
 * PURPOSE:
 *   Provide a private atomic spin lock so the diagnostics library remains
 *   independent from the higher-level platform library.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_DIAGNOSTIC_LOCK_H
#define UMICOM_DIAGNOSTICS_DIAGNOSTIC_LOCK_H
#include <stdatomic.h>
/**
 * Represent the diagnostic lock data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticLock { atomic_flag flag; } UmiDiagnosticLock;
/**
 * Initialise diagnostic lock from caller-provided values so later operations receive a
 * known state.
 */
static inline void umi_diagnostic_lock_init(UmiDiagnosticLock *lock) { atomic_flag_clear(&lock->flag); }
/**
 * Provide the diagnostic lock acquire operation used by this module and its client
 * applications.
 */
static inline void umi_diagnostic_lock_acquire(const UmiDiagnosticLock *lock)
{
    atomic_flag *flag = (atomic_flag *)&lock->flag;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (atomic_flag_test_and_set_explicit(flag, memory_order_acquire)) { }
}
/**
 * Release or reset state held by diagnostic lock so the same storage can be reused safely.
 */
static inline void umi_diagnostic_lock_release(const UmiDiagnosticLock *lock)
{
    atomic_flag *flag = (atomic_flag *)&lock->flag;
    atomic_flag_clear_explicit(flag, memory_order_release);
}
#endif
