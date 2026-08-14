/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/source_generation_v2.h
 *
 * PURPOSE:
 *   Generate a transparent, inspectable source bundle from a visual document.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Generated output remains ordinary text. Studio may show, save and diff every
 * file, so visual authoring never hides what the application will build.
 */
#ifndef UMICOM_DESIGNER_SOURCE_GENERATION_V2_H
#define UMICOM_DESIGNER_SOURCE_GENERATION_V2_H

#include "umicom/designer/builder_types_v2.h"
#include "umicom/designer/document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerGeneratedFileV2 {
    char path[UMI_DECL_PATH_CAPACITY];
    char content[UMI_DESIGNER_V2_GENERATED_CONTENT_CAPACITY];
    size_t length;
} UmiDesignerGeneratedFileV2;

typedef struct UmiDesignerGeneratedSourceV2 {
    UmiDesignerGeneratedFileV2 files[UMI_DESIGNER_V2_MAX_GENERATED_FILES];
    size_t file_count;
    uint64_t source_revision;
} UmiDesignerGeneratedSourceV2;

UmiStatus umi_designer_source_generation_v2_generate(const UmiDesignerDocument *document,
                                                      UmiDesignerGeneratedSourceV2 *out_source);
const UmiDesignerGeneratedFileV2 *umi_designer_source_generation_v2_find(
    const UmiDesignerGeneratedSourceV2 *source,
    const char *path);

#ifdef __cplusplus
}
#endif
#endif
