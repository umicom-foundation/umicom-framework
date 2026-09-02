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

/*
 * An SBOM lets a user inspect what libraries and components were included in a package.
 */

#include "umicom/delivery/sbom.h"
#include "delivery_internal.h"
#include <string.h>

/* Initialise sbom from caller-provided values so later operations receive a known state. */
void umi_sbom_init(UmiSbom *sbom)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sbom != NULL) (void)memset(sbom, 0, sizeof(*sbom));
}

/* Add sbom only after its inputs and available capacity have been checked. */
UmiStatus umi_sbom_add(UmiSbom *sbom,
                       const char *name,
                       const char *version,
                       const char *supplier,
                       const char *license_id)
{
    UmiSbomComponent *component;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sbom == NULL || name == NULL || version == NULL ||
        supplier == NULL || license_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (sbom->count >= UMI_DELIVERY_MAX_COMPONENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    component = &sbom->components[sbom->count];
    (void)memset(component, 0, sizeof(*component));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(component->name, sizeof(component->name), name) != UMI_STATUS_OK ||
        umi_delivery_copy_text(component->version, sizeof(component->version), version) != UMI_STATUS_OK ||
        umi_delivery_copy_text(component->supplier, sizeof(component->supplier), supplier) != UMI_STATUS_OK ||
        umi_delivery_copy_text(component->license_id, sizeof(component->license_id), license_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++sbom->count;
    return UMI_STATUS_OK;
}

/* Find sbom while leaving the underlying catalogue or model owned by this module. */
const UmiSbomComponent *umi_sbom_find(const UmiSbom *sbom,
                                      const char *name)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sbom == NULL || name == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < sbom->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(sbom->components[i].name, name) == 0) return &sbom->components[i];
    }
    return NULL;
}
