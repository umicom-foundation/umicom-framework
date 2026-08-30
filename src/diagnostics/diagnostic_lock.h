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
typedef struct UmiDiagnosticLock { atomic_flag flag; } UmiDiagnosticLock;
static inline void umi_diagnostic_lock_init(UmiDiagnosticLock *lock) { atomic_flag_clear(&lock->flag); }
static inline void umi_diagnostic_lock_acquire(const UmiDiagnosticLock *lock)
{
    atomic_flag *flag = (atomic_flag *)&lock->flag;
    while (atomic_flag_test_and_set_explicit(flag, memory_order_acquire)) { }
}
static inline void umi_diagnostic_lock_release(const UmiDiagnosticLock *lock)
{
    atomic_flag *flag = (atomic_flag *)&lock->flag;
    atomic_flag_clear_explicit(flag, memory_order_release);
}
#endif
