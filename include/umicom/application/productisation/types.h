/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/types.h
 *
 * PURPOSE:
 *   Define bounded identifiers and status vocabulary for Framework-owned
 *   application productisation evidence, gaps and completion planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_TYPES_H
#define UMICOM_APPLICATION_PRODUCTISATION_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRODUCTISATION_ID_CAPACITY 160U
#define UMI_PRODUCTISATION_TEXT_CAPACITY 320U
#define UMI_PRODUCTISATION_REFERENCE_CAPACITY 256U
#define UMI_PRODUCTISATION_MAX_APPLICATIONS 64U
#define UMI_PRODUCTISATION_MAX_CAPABILITIES 256U
#define UMI_PRODUCTISATION_MAX_EVIDENCE 768U
#define UMI_PRODUCTISATION_MAX_GAPS 768U
#define UMI_PRODUCTISATION_MAX_STEPS 768U

/**
 * List the named productisation asset kind values accepted by this public contract.
 */
typedef enum UmiProductisationAssetKind {
    UMI_PRODUCTISATION_ASSET_FEATURE = 1,
    UMI_PRODUCTISATION_ASSET_PANEL = 2,
    UMI_PRODUCTISATION_ASSET_LAYOUT = 3,
    UMI_PRODUCTISATION_ASSET_COMPONENT = 4,
    UMI_PRODUCTISATION_ASSET_CAPABILITY = 5
} UmiProductisationAssetKind;

/**
 * List the named productisation evidence state values accepted by this public contract.
 */
typedef enum UmiProductisationEvidenceState {
    UMI_PRODUCTISATION_EVIDENCE_DECLARED = 1,
    UMI_PRODUCTISATION_EVIDENCE_FOUNDATION = 2,
    UMI_PRODUCTISATION_EVIDENCE_IMPLEMENTED = 3,
    UMI_PRODUCTISATION_EVIDENCE_ADOPTED = 4,
    UMI_PRODUCTISATION_EVIDENCE_TESTED = 5,
    UMI_PRODUCTISATION_EVIDENCE_ACCEPTED = 6,
    UMI_PRODUCTISATION_EVIDENCE_BLOCKED = 7
} UmiProductisationEvidenceState;

/**
 * List the named productisation gap kind values accepted by this public contract.
 */
typedef enum UmiProductisationGapKind {
    UMI_PRODUCTISATION_GAP_MISSING_CAPABILITY = 1,
    UMI_PRODUCTISATION_GAP_CAPABILITY_MATURITY = 2,
    UMI_PRODUCTISATION_GAP_MISSING_COMPONENT = 3,
    UMI_PRODUCTISATION_GAP_APPLICATION_ADOPTION = 4,
    UMI_PRODUCTISATION_GAP_ACCEPTANCE_EVIDENCE = 5,
    UMI_PRODUCTISATION_GAP_EXTERNAL_ADAPTER = 6
} UmiProductisationGapKind;

/**
 * List the named productisation severity values accepted by this public contract.
 */
typedef enum UmiProductisationSeverity {
    UMI_PRODUCTISATION_SEVERITY_LOW = 1,
    UMI_PRODUCTISATION_SEVERITY_MEDIUM = 2,
    UMI_PRODUCTISATION_SEVERITY_HIGH = 3,
    UMI_PRODUCTISATION_SEVERITY_BLOCKER = 4
} UmiProductisationSeverity;

/**
 * List the named productisation owner values accepted by this public contract.
 */
typedef enum UmiProductisationOwner {
    UMI_PRODUCTISATION_OWNER_FRAMEWORK = 1,
    UMI_PRODUCTISATION_OWNER_APPLICATION = 2,
    UMI_PRODUCTISATION_OWNER_EXTERNAL_ADAPTER = 3
} UmiProductisationOwner;

/**
 * List the named productisation stage values accepted by this public contract.
 */
typedef enum UmiProductisationStage {
    UMI_PRODUCTISATION_STAGE_FRAMEWORK = 1,
    UMI_PRODUCTISATION_STAGE_STUDIO = 2,
    UMI_PRODUCTISATION_STAGE_TRADER = 3,
    UMI_PRODUCTISATION_STAGE_REMAINING_APPLICATIONS = 4,
    UMI_PRODUCTISATION_STAGE_ACCEPTANCE = 5
} UmiProductisationStage;

/**
 * Check whether an asset-kind value belongs to the public enumeration.
 *
 * @param kind Value to validate before it is stored or displayed.
 * @return Non-zero for a supported kind; otherwise zero.
 */
int umi_productisation_asset_kind_valid(UmiProductisationAssetKind kind);

/**
 * Check whether an evidence-state value belongs to the public enumeration.
 *
 * @param state Value to validate before evidence processing.
 * @return Non-zero for a supported state; otherwise zero.
 */
int umi_productisation_evidence_state_valid(
    UmiProductisationEvidenceState state);

/**
 * Return a stable display label for an asset kind.
 *
 * @param kind Asset kind to describe.
 * @return Framework-owned text; the caller must not change or free it.
 */
const char *umi_productisation_asset_kind_text(
    UmiProductisationAssetKind kind);

/**
 * Return a stable display label for an evidence state.
 *
 * @param state Evidence state to describe.
 * @return Framework-owned text; the caller must not change or free it.
 */
const char *umi_productisation_evidence_state_text(
    UmiProductisationEvidenceState state);

/**
 * Return a stable display label for a gap kind.
 *
 * @param kind Gap kind to describe.
 * @return Framework-owned text; the caller must not change or free it.
 */
const char *umi_productisation_gap_kind_text(UmiProductisationGapKind kind);

/**
 * Return a stable display label for a productisation severity.
 *
 * @param severity Severity to describe.
 * @return Framework-owned text; the caller must not change or free it.
 */
const char *umi_productisation_severity_text(UmiProductisationSeverity severity);

/**
 * Return a stable display label for a productisation owner.
 *
 * @param owner Owner category to describe.
 * @return Framework-owned text; the caller must not change or free it.
 */
const char *umi_productisation_owner_text(UmiProductisationOwner owner);

/**
 * Return a stable display label for a completion stage.
 *
 * @param stage Completion stage to describe.
 * @return Framework-owned text; the caller must not change or free it.
 */
const char *umi_productisation_stage_text(UmiProductisationStage stage);

#ifdef __cplusplus
}
#endif

#endif
