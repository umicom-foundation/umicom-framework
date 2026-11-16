/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_literal_input.h
 *
 * PURPOSE:
 *   Observe text and Boolean node input defaults and declared flags without creating widgets or executing nodes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_LITERAL_INPUT_H
#define UMICOM_MEDIA_GENERATION_COMFY_LITERAL_INPUT_H
#include "umicom/media_generation/comfy_nodes.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiComfyLiteralKind {
    UMI_COMFY_LITERAL_STRING = 1,
    UMI_COMFY_LITERAL_BOOLEAN = 2
} UmiComfyLiteralKind;
#define UMI_COMFY_LITERAL_FORCE_INPUT 1U
#define UMI_COMFY_LITERAL_MULTILINE 2U
#define UMI_COMFY_LITERAL_DYNAMIC_PROMPTS 4U
typedef struct UmiComfyLiteralInput {
    char name[129];
    UmiComfyLiteralKind kind;
    bool default_known;
    /* Meaningful only for BOOLEAN with default_known=true. */
    bool boolean_default;
    unsigned known_flags, enabled_flags;
    size_t uninterpreted_options;
} UmiComfyLiteralInput;
/* Supported descriptors are ["STRING"] or ["BOOLEAN"], optionally followed
 * by an options object. Observe a correctly typed default and forceInput.
 * STRING additionally observes multiline and dynamicPrompts. Other options,
 * including string flags on BOOLEAN, remain opaque and are counted.
 * Known bits distinguish missing flags from explicit false. Missing default
 * never invents false or empty text. A copied report outlives the catalogue.
 *
 * Non-array/custom descriptors, other named types, choices and extra elements
 * return NOT_IMPLEMENTED. Empty arrays, invalid type slots, non-object options
 * or wrong types for interpreted fields give PARSE_ERROR. Missing indices give
 * NOT_FOUND; invalid arguments give INVALID_ARGUMENT. Failure preserves out.
 * Read source/connection identity separately through CatalogueInspect.
 *
 * No input is submitted, expanded or rendered. In particular dynamicPrompts
 * is an observed flag, not permission to evaluate text. The raw InputAt API
 * remains available for unsupported descriptors and uninterpreted options.
 * Serialize reads/destruction and keep caller output separate from owned data. */
UmiStatus UmiComfyNodeCatalogueLiteralInput(const UmiComfyNodeCatalogue *catalogue,
    size_t node_index, UmiComfyInputGroup group, size_t input_index,
    UmiComfyLiteralInput *out);
/* Copy an observed STRING default, including its final NUL. Missing default
 * gives NOT_FOUND; a BOOLEAN descriptor gives NOT_IMPLEMENTED. Empty text is
 * a successful one-byte copy. Too little capacity gives CAPACITY_EXCEEDED;
 * NULL output or zero capacity gives INVALID_ARGUMENT. Decode only after the
 * entire supported profile passes. Every failure leaves all output bytes
 * unchanged. Text is untrusted, not markup, a command or a template to execute.
 * Catalogue limits bound its size; no extra truncating text limit is applied. */
UmiStatus UmiComfyNodeCatalogueStringDefault(const UmiComfyNodeCatalogue *catalogue,
    size_t node_index, UmiComfyInputGroup group, size_t input_index,
    char *out, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
