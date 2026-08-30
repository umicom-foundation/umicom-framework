/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/evolution_variant.h
 *
 * PURPOSE:
 *   Represent a generated implementation variant and its lineage, fitness and validation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_EVOLUTION_VARIANT_H
#define UMICOM_DEVELOPER_HELIX_EVOLUTION_VARIANT_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixEvolutionVariant {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double quality; double regression;
} UmiHelixEvolutionVariant;
/* Initialise a deterministic, empty evolution variant record. */
void umi_helix_evolution_variant_init(UmiHelixEvolutionVariant *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_evolution_variant_validate(const UmiHelixEvolutionVariant *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_evolution_variant_score(const UmiHelixEvolutionVariant *record);
#ifdef __cplusplus
}
#endif
#endif
