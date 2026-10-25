/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_messaging/simulation.h
 *
 * PURPOSE:
 *   Expose a deterministic financial messaging exercise for native application hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCIAL_MESSAGING_SIMULATION_H
#define UMICOM_FINANCIAL_MESSAGING_SIMULATION_H
#include "umicom/financial_messaging/progress.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUfmpSimulationReport {
  unsigned delivery_attempts;
  unsigned accepted_operations;
  unsigned replay_matches;
  int changed_payload_refused;
  int receipt_left_settlement_pending;
  int credit_left_settlement_pending;
  UmiUfmpProgressState final_state;
} UmiUfmpSimulationReport;
/* Run a deterministic, in-memory example using synthetic Bank.A, Bank.B and
 * Wallet.C identities. No files, sockets, certificates, real money, database
 * commits or cryptographic signatures are involved. This demonstrates only
 * the shared logical validation/replay/progress rules. Outputs publish together
 * on success; errors leave the caller's report unchanged. */
UmiStatus UmiUfmpSimulationRun(UmiUfmpSimulationReport *out);
/* Bounded educational text shared by native console hosts. The required size
 * excludes the terminator. NULL+zero measures; insufficient capacity leaves
 * out untouched. This runs the same side-effect-free simulation. */
UmiStatus UmiUfmpSimulationFormat(char *out, size_t capacity, size_t *required);
#ifdef __cplusplus
}
#endif
#endif
