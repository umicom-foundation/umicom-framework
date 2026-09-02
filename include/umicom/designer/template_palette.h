/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/template_palette.h
 *
 * PURPOSE:
 *   Define reusable visual templates and starter compositions for the Studio designer.
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
#ifndef UMICOM_DESIGNER_TEMPLATE_PALETTE_H
#define UMICOM_DESIGNER_TEMPLATE_PALETTE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_TEMPLATE_PALETTE_CAPACITY 1024U

/**
 * Represent the designer template palette snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerTemplatePaletteSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char category[128];
    char description[512];
    char template_id[128];
    char preview_uri[512];
    int32_t order;
    uint64_t revision;
} UmiDesignerTemplatePaletteSnapshot;

/**
 * Represent the designer template palette registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerTemplatePaletteRegistry UmiDesignerTemplatePaletteRegistry;

/**
 * Initialise designer template palette registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_designer_template_palette_registry_create(UmiDesignerTemplatePaletteRegistry **out_registry);
/**
 * Release or reset state held by designer template palette registry so the same storage
 * can be reused safely.
 */
void umi_designer_template_palette_registry_destroy(UmiDesignerTemplatePaletteRegistry *registry);
/**
 * Provide the designer template palette registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_designer_template_palette_registry_upsert(UmiDesignerTemplatePaletteRegistry *registry, const UmiDesignerTemplatePaletteSnapshot *item);
/**
 * Remove designer template palette registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_designer_template_palette_registry_remove(UmiDesignerTemplatePaletteRegistry *registry, const char *id);
/**
 * Find designer template palette registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_template_palette_registry_find(const UmiDesignerTemplatePaletteRegistry *registry, const char *id, UmiDesignerTemplatePaletteSnapshot *out_item);
/**
 * Find designer template palette registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_template_palette_registry_at(const UmiDesignerTemplatePaletteRegistry *registry, size_t index, UmiDesignerTemplatePaletteSnapshot *out_item);
/**
 * Return the number of records represented by designer template palette registry without
 * changing their state.
 */
size_t umi_designer_template_palette_registry_count(const UmiDesignerTemplatePaletteRegistry *registry);
/**
 * Provide the designer template palette registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_designer_template_palette_registry_revision(const UmiDesignerTemplatePaletteRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
