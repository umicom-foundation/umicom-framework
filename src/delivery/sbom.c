/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/sbom.c
 *
 * PURPOSE:
 *   Build a bounded software bill of materials describing release components and suppliers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * An SBOM lets a user inspect what libraries and components were included in a package.
 */

#include "umicom/delivery/sbom.h"
#include "delivery_internal.h"
#include <string.h>

void umi_sbom_init(UmiSbom *sbom)
{
    if (sbom != NULL) (void)memset(sbom, 0, sizeof(*sbom));
}

UmiStatus umi_sbom_add(UmiSbom *sbom,
                       const char *name,
                       const char *version,
                       const char *supplier,
                       const char *license_id)
{
    UmiSbomComponent *component;
    if (sbom == NULL || name == NULL || version == NULL ||
        supplier == NULL || license_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (sbom->count >= UMI_DELIVERY_MAX_COMPONENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    component = &sbom->components[sbom->count];
    (void)memset(component, 0, sizeof(*component));
    if (umi_delivery_copy_text(component->name, sizeof(component->name), name) != UMI_STATUS_OK ||
        umi_delivery_copy_text(component->version, sizeof(component->version), version) != UMI_STATUS_OK ||
        umi_delivery_copy_text(component->supplier, sizeof(component->supplier), supplier) != UMI_STATUS_OK ||
        umi_delivery_copy_text(component->license_id, sizeof(component->license_id), license_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++sbom->count;
    return UMI_STATUS_OK;
}

const UmiSbomComponent *umi_sbom_find(const UmiSbom *sbom,
                                      const char *name)
{
    size_t i;
    if (sbom == NULL || name == NULL) return NULL;
    for (i = 0U; i < sbom->count; ++i) {
        if (strcmp(sbom->components[i].name, name) == 0) return &sbom->components[i];
    }
    return NULL;
}
