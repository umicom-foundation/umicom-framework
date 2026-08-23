/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/outline.h
 *
 * PURPOSE:
 *   Project symbols from one document into a toolkit-neutral outline tree/list.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_OUTLINE_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_OUTLINE_H

#include "umicom/developer_productivity/symbols.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_OUTLINE_CAPACITY 1024U

typedef struct UmiDeveloperOutlineModel {
    UmiDeveloperSymbol symbols[UMI_DEVELOPER_OUTLINE_CAPACITY];
    size_t count;
    char document_uri[UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY];
    uint64_t revision;
} UmiDeveloperOutlineModel;

UmiStatus umi_developer_outline_build(
    const UmiDeveloperSymbolIndex *index,
    const char *document_uri,
    UmiDeveloperOutlineModel *out_outline);

#ifdef __cplusplus
}
#endif

#endif
