/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/gate.h
 *
 * PURPOSE:
 *   Represent one release gate and its evidence status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A gate turns a release requirement such as tests, signing or smoke checks into explicit machine-readable state.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_GATE_H
#define INCLUDE_UMICOM_DELIVERY_GATE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReleaseGate {
    char gate_id[UMI_DELIVERY_ID_CAPACITY];
    UmiEvidenceStatus status;
    int required;
    char evidence[UMI_DELIVERY_TEXT_CAPACITY];
} UmiReleaseGate;

UmiStatus umi_release_gate_init(UmiReleaseGate *gate,
                                const char *gate_id,
                                int required);
UmiStatus umi_release_gate_record(UmiReleaseGate *gate,
                                  UmiEvidenceStatus status,
                                  const char *evidence);
int umi_release_gate_passed(const UmiReleaseGate *gate);

#ifdef __cplusplus
}
#endif

#endif
