/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/alignment.h
 *
 * PURPOSE:
 *   Define deterministic alignment and distribution operations for visual design surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiDesignerAlignmentRegistry UmiDesignerAlignmentRegistry;

UmiStatus umi_designer_alignment_registry_create(UmiDesignerAlignmentRegistry **out_registry);
void umi_designer_alignment_registry_destroy(UmiDesignerAlignmentRegistry *registry);
UmiStatus umi_designer_alignment_registry_upsert(UmiDesignerAlignmentRegistry *registry, const UmiDesignerAlignmentSnapshot *item);
UmiStatus umi_designer_alignment_registry_remove(UmiDesignerAlignmentRegistry *registry, const char *id);
UmiStatus umi_designer_alignment_registry_find(const UmiDesignerAlignmentRegistry *registry, const char *id, UmiDesignerAlignmentSnapshot *out_item);
UmiStatus umi_designer_alignment_registry_at(const UmiDesignerAlignmentRegistry *registry, size_t index, UmiDesignerAlignmentSnapshot *out_item);
size_t umi_designer_alignment_registry_count(const UmiDesignerAlignmentRegistry *registry);
uint64_t umi_designer_alignment_registry_revision(const UmiDesignerAlignmentRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
