/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/alignment.h
 *
 * PURPOSE:
 *   Define deterministic alignment and distribution operations for visual design surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_DESIGNER_ALIGNMENT_H
#define UMICOM_DESIGNER_ALIGNMENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_ALIGNMENT_CAPACITY 256U

/**
 * Represent the designer alignment snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerAlignmentSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char operation[64];
    char selection_id[128];
    double spacing;
    int horizontal;
    int vertical;
    int distribute;
    uint64_t revision;
} UmiDesignerAlignmentSnapshot;

/**
 * Represent the designer alignment registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerAlignmentRegistry UmiDesignerAlignmentRegistry;

/**
 * Initialise designer alignment registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_alignment_registry_create(UmiDesignerAlignmentRegistry **out_registry);
/**
 * Release or reset state held by designer alignment registry so the same storage can be
 * reused safely.
 */
void umi_designer_alignment_registry_destroy(UmiDesignerAlignmentRegistry *registry);
/**
 * Provide the designer alignment registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_alignment_registry_upsert(UmiDesignerAlignmentRegistry *registry, const UmiDesignerAlignmentSnapshot *item);
/**
 * Remove designer alignment registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_designer_alignment_registry_remove(UmiDesignerAlignmentRegistry *registry, const char *id);
/**
 * Find designer alignment registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_alignment_registry_find(const UmiDesignerAlignmentRegistry *registry, const char *id, UmiDesignerAlignmentSnapshot *out_item);
/**
 * Find designer alignment registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_alignment_registry_at(const UmiDesignerAlignmentRegistry *registry, size_t index, UmiDesignerAlignmentSnapshot *out_item);
/**
 * Return the number of records represented by designer alignment registry without changing
 * their state.
 */
size_t umi_designer_alignment_registry_count(const UmiDesignerAlignmentRegistry *registry);
/**
 * Provide the designer alignment registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_designer_alignment_registry_revision(const UmiDesignerAlignmentRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
