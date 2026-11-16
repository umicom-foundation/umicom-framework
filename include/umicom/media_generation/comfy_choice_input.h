/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_choice_input.h
 *
 * PURPOSE:
 *   Read captured ComfyUI string-choice input metadata through the immutable shared catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_CHOICE_INPUT_H
#define UMICOM_MEDIA_GENERATION_COMFY_CHOICE_INPUT_H
#include "umicom/media_generation/comfy_nodes.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiComfyChoiceInput {
    char name[129];
    size_t choice_count;
    size_t uninterpreted_options;
} UmiComfyChoiceInput;
/* Read a legacy COMBO descriptor: [["choice",...]] with an optional options
 * object. Every choice must be a string. Keep source order, duplicate labels,
 * empty strings and empty lists; no value is automatically selected.
 * All option keys remain opaque and are counted, not acted upon. A copied
 * report survives catalogue destruction; choice text is read separately.
 *
 * Object/custom descriptors, named types, mixed/non-string lists and extra
 * descriptor elements return NOT_IMPLEMENTED. Empty descriptors, invalid
 * type slots or non-object options return PARSE_ERROR. Missing node/input gives
 * NOT_FOUND. Index/group/owner admission matches the existing raw reader.
 * Failure leaves caller output unchanged. No I/O or custom node execution.
 * Serialized reads/destruction and non-overlapping caller storage are required.
 * Catalogue byte/node limits bound the linear list validation in each query. */
UmiStatus UmiComfyNodeCatalogueChoiceInput(const UmiComfyNodeCatalogue *catalogue,
    size_t node_index, UmiComfyInputGroup group, size_t input_index,
    UmiComfyChoiceInput *out);
/* Decode one string into caller-owned UTF-8 storage, including its final NUL.
 * Validate the complete string-list profile before returning any member.
 * A choice index outside the observed list gives NOT_FOUND; too little space
 * gives CAPACITY_EXCEEDED. NULL output or zero capacity gives INVALID_ARGUMENT.
 * All failures leave the whole buffer unchanged. Copied text survives owner
 * destruction; original schema JSON remains available through InputAt.
 * Treat text as untrusted display data, never a trusted path or executable.
 * An observed model filename does not prove model availability or freshness. */
UmiStatus UmiComfyNodeCatalogueChoiceAt(const UmiComfyNodeCatalogue *catalogue,
    size_t node_index, UmiComfyInputGroup group, size_t input_index,
    size_t choice_index, char *out, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
