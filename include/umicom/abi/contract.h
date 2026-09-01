/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/contract.h
 *
 * PURPOSE:
 *   Create stable fingerprints from public ABI descriptors for conformance evidence.
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
#ifndef UMICOM_ABI_CONTRACT_H
#define UMICOM_ABI_CONTRACT_H
#include <stdint.h>
#include "umicom/abi/descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif
uint64_t umi_abi_contract_fingerprint(const UmiAbiDescriptor *descriptor);
#ifdef __cplusplus
}
#endif
#endif
