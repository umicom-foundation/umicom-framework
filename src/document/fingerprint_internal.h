/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/fingerprint_internal.h
 *
 * PURPOSE:
 *   Share the private native fingerprint backend symbol with the C dispatcher.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_FINGERPRINT_INTERNAL_H
#define UMICOM_DOCUMENT_FINGERPRINT_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

#if defined(UMICOM_DOCUMENT_HASH_X86_64_ASM) || \
    defined(UMICOM_DOCUMENT_HASH_AARCH64_ASM) || \
    defined(UMICOM_DOCUMENT_HASH_RISCV64_ASM)
uint64_t umi_document_hash_bytes_asm(const unsigned char *bytes,
                                     size_t byte_count,
                                     uint64_t initial_hash);
#endif

#endif /* UMICOM_DOCUMENT_FINGERPRINT_INTERNAL_H */
