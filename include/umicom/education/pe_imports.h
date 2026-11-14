/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/pe_imports.h
 *
 * PURPOSE:
 *   Inspect ordinary and delayed Windows DLL imports without loading executable code.
 *
 * EDUCATIONAL OVERVIEW:
 *   A data-directory size can include import names and thunks, not just descriptors.
 *   Bound the descriptors actually visited and require their zero terminator. The
 *   result owns its names and is published only after every validation succeeds.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_PE_IMPORTS_H
#define UMICOM_EDUCATION_PE_IMPORTS_H
#include "umicom/education/binary_inspect.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PE_IMPORT_CAPACITY 256U
#define UMI_PE_IMPORT_NAME_CAPACITY 256U
#define UMI_PE_IMPORT_IMAGE_LIMIT (512U * 1024U * 1024U)

/* This value is an observation, not permission to load any listed DLL. Names
 * are unique case-insensitively and remain in first-observed descriptor order. */
typedef struct UmiEducationPeImports {
    uint16_t machine;
    uint8_t address_bits;
    size_t count;
    char names[UMI_PE_IMPORT_CAPACITY][UMI_PE_IMPORT_NAME_CAPACITY];
} UmiEducationPeImports;

/* Inspect one complete file byte span up to IMAGE_LIMIT. Reuse the canonical
 * executable-header inspector, then validate PE32/PE32+ import tables and names.
 * No filesystem, loader, network or process operation occurs. Null arguments
 * fail with INVALID_ARGUMENT; malformed data fails with PARSE_ERROR; excessive
 * descriptor/name counts return CAPACITY_EXCEEDED. Allocation may fail.
 * Every failure preserves all bytes of out_imports. Caller serialises access. */
UmiStatus UmiEducationPeImportsInspectBytes(const unsigned char *bytes,
    size_t size, UmiEducationPeImports *out_imports);
#ifdef __cplusplus
}
#endif
#endif
