/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/validation_gate.h
 *
 * PURPOSE:
 *   Define one validation requirement with severity, threshold and blocking semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_VALIDATION_GATE_H
#define UMICOM_DEVELOPER_HELIX_VALIDATION_GATE_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix validation gate data shared with callers of this public contract.
 */
typedef struct UmiHelixValidationGate {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double threshold; bool blocking;
} UmiHelixValidationGate;
/* Initialise a deterministic, empty validation gate record. */
void umi_helix_validation_gate_init(UmiHelixValidationGate *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_validation_gate_validate(const UmiHelixValidationGate *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_validation_gate_score(const UmiHelixValidationGate *record);
#ifdef __cplusplus
}
#endif
#endif
