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

/*
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_CATALOGUE_H
#define UMICOM_ABI_CATALOGUE_H
#include <stddef.h>
#include "umicom/abi/descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the abi catalogue data shared with callers of this public contract.
 */
typedef struct UmiAbiCatalogue UmiAbiCatalogue;
/**
 * Initialise abi catalogue from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_abi_catalogue_create(UmiAbiCatalogue **out_catalogue);
/**
 * Release or reset state held by abi catalogue so the same storage can be reused safely.
 */
void umi_abi_catalogue_destroy(UmiAbiCatalogue *catalogue);
/**
 * Add abi catalogue only after its inputs and available capacity have been checked.
 */
UmiStatus umi_abi_catalogue_register(UmiAbiCatalogue *catalogue, const UmiAbiDescriptor *descriptor);
/**
 * Find abi catalogue while leaving the underlying catalogue or model owned by this module.
 */
const UmiAbiDescriptor *umi_abi_catalogue_find(const UmiAbiCatalogue *catalogue, const char *abi_id);
/**
 * Return the number of records represented by abi catalogue without changing their state.
 */
size_t umi_abi_catalogue_count(const UmiAbiCatalogue *catalogue);
#ifdef __cplusplus
}
#endif
#endif
