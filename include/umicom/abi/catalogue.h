/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/catalogue.h
 *
 * PURPOSE:
 *   Maintain a bounded catalogue of public ABI descriptors for tools and products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_CATALOGUE_H
#define UMICOM_ABI_CATALOGUE_H
#include <stddef.h>
#include "umicom/abi/descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAbiCatalogue UmiAbiCatalogue;
UmiStatus umi_abi_catalogue_create(UmiAbiCatalogue **out_catalogue);
void umi_abi_catalogue_destroy(UmiAbiCatalogue *catalogue);
UmiStatus umi_abi_catalogue_register(UmiAbiCatalogue *catalogue, const UmiAbiDescriptor *descriptor);
const UmiAbiDescriptor *umi_abi_catalogue_find(const UmiAbiCatalogue *catalogue, const char *abi_id);
size_t umi_abi_catalogue_count(const UmiAbiCatalogue *catalogue);
#ifdef __cplusplus
}
#endif
#endif
