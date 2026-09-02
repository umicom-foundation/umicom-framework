/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/sbom.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_SBOM_H
#define INCLUDE_UMICOM_DELIVERY_SBOM_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the sbom component data shared with callers of this public contract.
 */
typedef struct UmiSbomComponent {
    char name[UMI_DELIVERY_ID_CAPACITY];
    char version[UMI_DELIVERY_VERSION_CAPACITY];
    char supplier[UMI_DELIVERY_ID_CAPACITY];
    char license_id[UMI_DELIVERY_ID_CAPACITY];
} UmiSbomComponent;

/**
 * Represent the sbom data shared with callers of this public contract.
 */
typedef struct UmiSbom {
    UmiSbomComponent components[UMI_DELIVERY_MAX_COMPONENTS];
    size_t count;
} UmiSbom;

/**
 * Initialise sbom from caller-provided values so later operations receive a known state.
 */
void umi_sbom_init(UmiSbom *sbom);
/**
 * Add sbom only after its inputs and available capacity have been checked.
 */
UmiStatus umi_sbom_add(UmiSbom *sbom,
                       const char *name,
                       const char *version,
                       const char *supplier,
                       const char *license_id);
/**
 * Find sbom while leaving the underlying catalogue or model owned by this module.
 */
const UmiSbomComponent *umi_sbom_find(const UmiSbom *sbom,
                                      const char *name);

#ifdef __cplusplus
}
#endif

#endif
