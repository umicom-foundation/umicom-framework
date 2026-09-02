/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/object_section.h
 *
 * PURPOSE:
 *   Describe object-file sections, flags, alignments and planned byte sizes before backend encoding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_OBJECT_SECTION_H
#define UMICOM_COMPILER_NATIVE_OBJECT_SECTION_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native section kind values accepted by this public contract.
 */
typedef enum UmiNativeSectionKind { UMI_NC_SECTION_TEXT=1, UMI_NC_SECTION_RODATA=2, UMI_NC_SECTION_DATA=3, UMI_NC_SECTION_BSS=4, UMI_NC_SECTION_DEBUG=5 } UmiNativeSectionKind;
/**
 * Represent the native object section data shared with callers of this public contract.
 */
typedef struct UmiNativeObjectSection { char name[UMI_NC_NAME_CAPACITY]; UmiNativeSectionKind kind; size_t size; size_t alignment; bool readable; bool writable; bool executable; } UmiNativeObjectSection;
/**
 * Initialise nc object section from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_object_section_init(UmiNativeObjectSection *section,const char *name,UmiNativeSectionKind kind,size_t alignment);
/**
 * Provide the nc object section reserve operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_object_section_reserve(UmiNativeObjectSection *section,size_t bytes);
/**
 * Return the number of records represented by nc object section aligned without changing
 * their state.
 */
size_t umi_nc_object_section_aligned_size(const UmiNativeObjectSection *section);
#ifdef __cplusplus
}
#endif
#endif
