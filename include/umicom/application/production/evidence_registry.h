/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/evidence_registry.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_EVIDENCE_REGISTRY_H
#define UMICOM_APPLICATION_PRODUCTION_EVIDENCE_REGISTRY_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/evidence_record.h"

/**
 * Represent the application production evidence registry data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionEvidenceRegistry {
    UmiApplicationProductionEvidenceRecord
        entries[UMI_APPLICATION_PRODUCTION_MAX_EVIDENCE];
    size_t count;
    uint64_t revision;
} UmiApplicationProductionEvidenceRegistry;

/**
 * Initialise application production evidence registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_production_evidence_registry_init(
    UmiApplicationProductionEvidenceRegistry *registry);
/**
 * Provide the application production evidence registry record operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_production_evidence_registry_record(
    UmiApplicationProductionEvidenceRegistry *registry,
    const char *evidence_id, UmiApplicationProductionEvidenceKind kind,
    UmiApplicationProductionEvidenceState state, const char *reference);
/**
 * Find application production evidence registry while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationProductionEvidenceRecord *
umi_application_production_evidence_registry_find(
    const UmiApplicationProductionEvidenceRegistry *registry,
    const char *evidence_id);

#ifdef __cplusplus
}
#endif
#endif
