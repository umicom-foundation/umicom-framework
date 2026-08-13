/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/spin_hint.h
 *
 * PURPOSE:
 *   Expose a tiny audited processor hint for bounded developer-runtime wait
 *   loops while preserving a portable C23 fallback on every architecture.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_SPIN_HINT_H
#define UMICOM_DEVELOPER_SPIN_HINT_H

#ifdef __cplusplus
extern "C" {
#endif

void umi_developer_spin_hint(unsigned iterations);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_SPIN_HINT_H */
