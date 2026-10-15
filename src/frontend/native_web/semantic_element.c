/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/semantic_element.c
 *
 * PURPOSE:
 *   Define keyed semantic browser elements independent of any browser or desktop toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/semantic_element.h"
#include "../../base/value_construction_internal.h"

#include <string.h>

/*
 * Initialise native web semantic element from caller-provided values so later operations
 * receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_native_web_semantic_element_init(UmiNativeWebSemanticElement *element, const char *element_id, const char *tag)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (element == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(element, 0, sizeof(*element));
    status = umi_native_web_copy_text(element->element_id, sizeof(element->element_id), element_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_native_web_copy_text(element->tag, sizeof(element->tag), tag);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    element->revision = 1U;
    return umi_native_web_semantic_element_validate(element);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_native_web_semantic_element_init_build_value(UmiNativeWebSemanticElement *element, const char *element_id, const char *tag)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (element == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(element, 0, sizeof(*element));
    status = umi_native_web_copy_text(element->element_id, sizeof(element->element_id), element_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_native_web_copy_text(element->tag, sizeof(element->tag), tag);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    element->revision = 1U;
    return umi_native_web_semantic_element_validate(element);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_native_web_semantic_element_init, UmiNativeWebSemanticElement, element,
    (UmiNativeWebSemanticElement *element, const char *element_id, const char *tag),
    umi_native_web_semantic_element_init_build_value(candidate, element_id, tag))


/*
 * Check that native web semantic element satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_native_web_semantic_element_validate(const UmiNativeWebSemanticElement *element)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (element == NULL || element->element_id[0] == '\0' || element->tag[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (element->attribute_count > UMI_NATIVE_WEB_MAX_ATTRIBUTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

