/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/source_generation.h
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
#ifndef UMICOM_DESIGNER_SOURCE_GENERATION_H
#define UMICOM_DESIGNER_SOURCE_GENERATION_H

#include "umicom/designer/builder_types.h"
#include "umicom/designer/document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerGeneratedFile {
    char path[UMI_DECL_PATH_CAPACITY];
    char content[UMI_DESIGNER_GENERATED_CONTENT_CAPACITY];
    size_t length;
} UmiDesignerGeneratedFile;

typedef struct UmiDesignerGeneratedSource {
    UmiDesignerGeneratedFile files[UMI_DESIGNER_MAX_GENERATED_FILES];
    size_t file_count;
    uint64_t source_revision;
} UmiDesignerGeneratedSource;

UmiStatus umi_designer_source_generation_generate(const UmiDesignerDocument *document,
                                                      UmiDesignerGeneratedSource *out_source);
const UmiDesignerGeneratedFile *umi_designer_source_generation_find(
    const UmiDesignerGeneratedSource *source,
    const char *path);

#ifdef __cplusplus
}
#endif
#endif
