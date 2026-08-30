/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/diagnostic_signal.h
 *
 * PURPOSE:
 *   Represent a compiler, test, runtime or static-analysis signal suitable for failure reasoning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_DIAGNOSTIC_SIGNAL_H
#define UMICOM_DEVELOPER_HELIX_DIAGNOSTIC_SIGNAL_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixDiagnosticSignal {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double risk;
} UmiHelixDiagnosticSignal;
/* Initialise a deterministic, empty diagnostic signal record. */
void umi_helix_diagnostic_signal_init(UmiHelixDiagnosticSignal *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_diagnostic_signal_validate(const UmiHelixDiagnosticSignal *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_diagnostic_signal_score(const UmiHelixDiagnosticSignal *record);
#ifdef __cplusplus
}
#endif
#endif
