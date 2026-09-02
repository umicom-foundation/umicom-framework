/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/engine/catalogue.h
 *
 * PURPOSE:
 *   Publish a truthful catalogue of reusable Framework engines and validate
 *   the engine requirements declared by Umicom applications and extensions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_ENGINE_CATALOGUE_H
#define UMICOM_ENGINE_CATALOGUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/engine/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Descriptor strings are immutable Framework-owned text and remain valid for
 * the duration of the process. Callers must not free or modify them. */
typedef struct UmiEngineDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEngineKind kind;
    const char *engine_id;
    const char *display_name;
    const char *summary;
    const char *primary_header;
    UmiEngineMaturity maturity;
    UmiEngineCapabilityMask capabilities;
} UmiEngineDescriptor;

/**
 * Represent the engine requirement data shared with callers of this public contract.
 */
typedef struct UmiEngineRequirement {
    UmiEngineKind kind;
    UmiEngineMaturity minimum_maturity;
    UmiEngineCapabilityMask required_capabilities;
} UmiEngineRequirement;

/**
 * Represent the engine requirement report data shared with callers of this public
 * contract.
 */
typedef struct UmiEngineRequirementReport {
    uint32_t struct_size;
    uint32_t api_version;
    size_t requirement_count;
    size_t satisfied_count;
    size_t missing_count;
    UmiEngineKind first_missing_kind;
    UmiEngineCapabilityMask first_missing_capabilities;
    bool ready;
} UmiEngineRequirementReport;

/* Return the number of canonical reusable engines published by Framework. */
size_t umi_engine_catalogue_count(void);

/* Return a borrowed descriptor by position, or NULL outside the catalogue. */
const UmiEngineDescriptor *umi_engine_catalogue_at(size_t index);

/* Find a borrowed descriptor by stable enumeration kind. */
const UmiEngineDescriptor *umi_engine_catalogue_find(UmiEngineKind kind);

/* Find a borrowed descriptor by its persisted textual identifier. */
const UmiEngineDescriptor *umi_engine_catalogue_find_id(const char *engine_id);

/* Test whether one descriptor implements every requested capability bit. */
bool umi_engine_descriptor_supports(
    const UmiEngineDescriptor *descriptor,
    UmiEngineCapabilityMask capabilities);

/* Validation returns a status for malformed input. A valid but unsatisfied
 * request returns OK with report.ready set to false, allowing a UI to explain
 * missing engines without converting a normal compatibility result to an
 * internal error. */
UmiStatus umi_engine_catalogue_validate(
    const UmiEngineRequirement *requirements,
    size_t requirement_count,
    UmiEngineRequirementReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
