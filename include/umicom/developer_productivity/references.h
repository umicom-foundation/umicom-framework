/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/references.h
 *
 * PURPOSE:
 *   Retain provider-neutral symbol reference locations for Find References,
 *   rename/refactor preview and AI context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_REFERENCES_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_REFERENCES_H

#include "umicom/developer_productivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_REFERENCE_CAPACITY 128U

typedef struct UmiDeveloperReference {
    char symbol_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    UmiDeveloperProductivityLocation location;
    int declaration;
    int definition;
    int write_reference;
} UmiDeveloperReference;

typedef struct UmiDeveloperReferenceSet {
    UmiDeveloperReference references[UMI_DEVELOPER_REFERENCE_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiDeveloperReferenceSet;

void umi_developer_reference_set_init(
    UmiDeveloperReferenceSet *set);

UmiStatus umi_developer_reference_set_add(
    UmiDeveloperReferenceSet *set,
    const UmiDeveloperReference *reference);

UmiStatus umi_developer_reference_set_for_symbol(
    const UmiDeveloperReferenceSet *set,
    const char *symbol_id,
    UmiDeveloperReference *out_references,
    size_t capacity,
    size_t *out_count);

#ifdef __cplusplus
}
#endif

#endif
