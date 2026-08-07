#ifndef UMICOM_PLATFORM_DOCUMENT_H
#define UMICOM_PLATFORM_DOCUMENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDocument {
    char path[512];
    char *text;
    size_t length;
    uint64_t revision;
    int dirty;
} UmiDocument;

void umi_document_init(UmiDocument *document);
void umi_document_dispose(UmiDocument *document);
UmiStatus umi_document_open(UmiDocument *document, const char *path);
UmiStatus umi_document_set_text(UmiDocument *document, const char *text);
UmiStatus umi_document_append(UmiDocument *document, const char *text);
UmiStatus umi_document_save(UmiDocument *document);

#ifdef __cplusplus
}
#endif

#endif
