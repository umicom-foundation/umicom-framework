/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/hash.h
 *
 * PURPOSE:
 *   Provide a deterministic 64-bit fingerprint for ABI contracts and SDK evidence.
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
#ifndef UMICOM_ABI_HASH_H
#define UMICOM_ABI_HASH_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the abi hash bytes operation used by this module and its client applications.
 */
uint64_t umi_abi_hash_bytes(const void *data, size_t size);
/**
 * Provide the abi hash text operation used by this module and its client applications.
 */
uint64_t umi_abi_hash_text(const char *text);
/**
 * Provide the abi hash combine operation used by this module and its client applications.
 */
uint64_t umi_abi_hash_combine(uint64_t left, uint64_t right);
#ifdef __cplusplus
}
#endif
#endif
