/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/diagnostic.c
 *
 * PURPOSE:
 *   Implement bounded diagnostic retention and severity queries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/diagnostic.h"

#include <string.h>

/* Public records are caller-owned values. Validate only live entries and their
 * inline strings before a UI, summary or retained history consumes them. Empty
 * text is valid for diagnostics that have no source location or producer code. */
static int DiagnosticSeverityValid(UmiBuildDiagnosticSeverity severity)
{
    return severity==UMI_BUILD_DIAGNOSTIC_NOTE||severity==UMI_BUILD_DIAGNOSTIC_WARNING||
        severity==UMI_BUILD_DIAGNOSTIC_ERROR||severity==UMI_BUILD_DIAGNOSTIC_FATAL;
}

UmiStatus UmiBuildDiagnosticValidate(const UmiBuildDiagnostic *diagnostic)
{
    if (diagnostic==NULL||!DiagnosticSeverityValid(diagnostic->severity)||
        memchr(diagnostic->file,0,sizeof diagnostic->file)==NULL||
        memchr(diagnostic->code,0,sizeof diagnostic->code)==NULL||
        memchr(diagnostic->message,0,sizeof diagnostic->message)==NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus UmiBuildDiagnosticListValidate(const UmiBuildDiagnosticList *list)
{
    if (list==NULL||list->count>UMI_BUILD_MAX_DIAGNOSTICS)return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index=0;index<list->count;++index)
        if (UmiBuildDiagnosticValidate(&list->items[index])!=UMI_STATUS_OK)
            return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

void umi_build_diagnostic_list_init(UmiBuildDiagnosticList *list)
{
    if (list != NULL) {
        (void)memset(list, 0, sizeof(*list));
    }
}

/* Retain the earlier append for review. Incoming record admission and a
 * saturating loss count replace unchecked storage and wrapping evidence. */
#if 0
UmiStatus umi_build_diagnostic_list_add(UmiBuildDiagnosticList *list,
                                        const UmiBuildDiagnostic *diagnostic)
{
    if (list == NULL || diagnostic == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (list->count >= UMI_BUILD_MAX_DIAGNOSTICS) {
        list->dropped += 1U;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    list->items[list->count++] = *diagnostic;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_build_diagnostic_list_add(UmiBuildDiagnosticList *list,
                                        const UmiBuildDiagnostic *diagnostic)
{
    if (list==NULL||list->count>UMI_BUILD_MAX_DIAGNOSTICS||
        UmiBuildDiagnosticValidate(diagnostic)!=UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (list->count==UMI_BUILD_MAX_DIAGNOSTICS) {
        /* Saturation keeps missing coverage visible even when a producer has
         * exhausted the loss counter; wrapping to zero would imply no loss. */
        if (list->dropped!=SIZE_MAX)++list->dropped;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    list->items[list->count++]=*diagnostic;
    return UMI_STATUS_OK;
}

/* The former accessor is preserved for review. A claimed count cannot
 * extend the fixed array, and an admitted entry must contain bounded text. */
#if 0
const UmiBuildDiagnostic *umi_build_diagnostic_list_at(
    const UmiBuildDiagnosticList *list,
    size_t index)
{
    return list != NULL && index < list->count
        ? &list->items[index]
        : NULL;
}
#endif
const UmiBuildDiagnostic *umi_build_diagnostic_list_at(
    const UmiBuildDiagnosticList *list,size_t index)
{
    if (list==NULL||list->count>UMI_BUILD_MAX_DIAGNOSTICS||index>=list->count)
        return NULL;
    return UmiBuildDiagnosticValidate(&list->items[index])==UMI_STATUS_OK
        ? &list->items[index] : NULL;
}

/* Retain the original traversal for review. The shared validator bounds
 * iteration and distinguishes invalid imported evidence before counting. */
#if 0
size_t umi_build_diagnostic_list_count_severity(
    const UmiBuildDiagnosticList *list,
    UmiBuildDiagnosticSeverity minimum)
{
    size_t index;
    size_t count = 0U;
    if (list == NULL) {
        return 0U;
    }
    for (index = 0U; index < list->count; ++index) {
        if (list->items[index].severity >= minimum) {
            count += 1U;
        }
    }
    return count;
}
#endif
size_t umi_build_diagnostic_list_count_severity(
    const UmiBuildDiagnosticList *list,UmiBuildDiagnosticSeverity minimum)
{
    if (!DiagnosticSeverityValid(minimum)||UmiBuildDiagnosticListValidate(list)!=UMI_STATUS_OK)
        return 0U;
    size_t count=0U;
    for (size_t index=0;index<list->count;++index)
        if (list->items[index].severity>=minimum)++count;
    return count;
}
