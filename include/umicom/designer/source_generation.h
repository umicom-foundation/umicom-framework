/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/source_generation.h
 *
 * PURPOSE:
 *   Generate a transparent, inspectable source bundle from a visual document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Generated output remains ordinary text. Studio may show, save and diff every
 * file, so visual authoring never hides what the application will build.
 */
#ifndef UMICOM_DESIGNER_SOURCE_GENERATION_H
#define UMICOM_DESIGNER_SOURCE_GENERATION_H

#include "umicom/designer/builder_types.h"
#include "umicom/designer/document.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the designer generated file data shared with callers of this public contract.
 */
typedef struct UmiDesignerGeneratedFile {
    char path[UMI_DECL_PATH_CAPACITY];
    char content[UMI_DESIGNER_GENERATED_CONTENT_CAPACITY];
    size_t length;
} UmiDesignerGeneratedFile;

/**
 * Represent the designer generated source data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerGeneratedSource {
    UmiDesignerGeneratedFile files[UMI_DESIGNER_MAX_GENERATED_FILES];
    size_t file_count;
    uint64_t source_revision;
} UmiDesignerGeneratedSource;

/**
 * Provide the designer source generation generate operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_source_generation_generate(const UmiDesignerDocument *document,
                                                      UmiDesignerGeneratedSource *out_source);
/**
 * Find designer source generation while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiDesignerGeneratedFile *umi_designer_source_generation_find(
    const UmiDesignerGeneratedSource *source,
    const char *path);

#ifdef __cplusplus
}
#endif
#endif
