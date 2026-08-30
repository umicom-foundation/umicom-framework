/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/drag_drop_equivalence.h
 *
 * PURPOSE:
 *   drag/drop parity scoring with keyboard-accessible fallback requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_DRAG_DROP_EQUIVALENCE_H
#define UMICOM_FRONTEND_CONFORMANCE_DRAG_DROP_EQUIVALENCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcDragDropEquivalence { uint64_t required; uint64_t supported; bool keyboard_fallback; } UmiFcDragDropEquivalence;
double umi_fc_drag_drop_equivalence_evaluate(const UmiFcDragDropEquivalence *item);
UmiFcOutcome umi_fc_drag_drop_equivalence_outcome(const UmiFcDragDropEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
