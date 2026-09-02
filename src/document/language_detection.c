/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/language_detection.c
 *
 * PURPOSE:
 *   Infer common development-language identities from stable extension data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/language_detection.h"

#include <stdio.h>
#include <string.h>

typedef struct LanguageDefinition {
    const char *extension;
    const char *language_id;
    const char *mime_type;
    const char *icon_name;
} LanguageDefinition;

static const LanguageDefinition DEFINITIONS[] = {
    {"c", "c", "text/x-c", "text-x-csrc-symbolic"},
    {"h", "c", "text/x-chdr", "text-x-chdr-symbolic"},
    {"cc", "cpp", "text/x-c++", "text-x-c++src-symbolic"},
    {"cpp", "cpp", "text/x-c++", "text-x-c++src-symbolic"},
    {"hpp", "cpp", "text/x-c++hdr", "text-x-c++hdr-symbolic"},
    {"s", "asm", "text/x-asm", "text-x-generic-symbolic"},
    {"S", "asm", "text/x-asm", "text-x-generic-symbolic"},
    {"asm", "asm", "text/x-asm", "text-x-generic-symbolic"},
    {"py", "python3", "text/x-python", "text-x-python-symbolic"},
    {"js", "javascript", "application/javascript", "text-x-script-symbolic"},
    {"ts", "typescript", "application/typescript", "text-x-script-symbolic"},
    {"json", "json", "application/json", "text-x-generic-symbolic"},
    {"xml", "xml", "application/xml", "text-xml-symbolic"},
    {"html", "html", "text/html", "text-html-symbolic"},
    {"css", "css", "text/css", "text-css-symbolic"},
    {"md", "markdown", "text/markdown", "text-x-generic-symbolic"},
    {"cmake", "cmake", "text/x-cmake", "text-x-generic-symbolic"},
    {"sh", "sh", "application/x-shellscript", "text-x-script-symbolic"},
    {"ps1", "powershell", "text/x-powershell", "text-x-script-symbolic"},
    {"sql", "sql", "application/sql", "text-x-generic-symbolic"},
    {"yaml", "yaml", "application/yaml", "text-x-generic-symbolic"},
    {"yml", "yaml", "application/yaml", "text-x-generic-symbolic"}
};

/*
 * Provide the document language detect operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_language_detect(
    const char *path_or_name,
    UmiDocumentLanguageIdentity *out_identity)
{
    const char *extension;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path_or_name == NULL || out_identity == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_identity, 0, sizeof(*out_identity));
    (void)snprintf(out_identity->language_id, sizeof(out_identity->language_id), "%s", "plain");
    (void)snprintf(out_identity->mime_type, sizeof(out_identity->mime_type), "%s", "text/plain");
    (void)snprintf(out_identity->icon_name, sizeof(out_identity->icon_name), "%s", "text-x-generic-symbolic");
    out_identity->text = 1;
    extension = strrchr(path_or_name, '.');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (extension == NULL || extension[1] == '\0') return UMI_STATUS_OK;
    extension += 1U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(DEFINITIONS) / sizeof(DEFINITIONS[0]); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(DEFINITIONS[index].extension, extension) == 0) {
            (void)snprintf(out_identity->language_id, sizeof(out_identity->language_id),
                           "%s", DEFINITIONS[index].language_id);
            (void)snprintf(out_identity->mime_type, sizeof(out_identity->mime_type),
                           "%s", DEFINITIONS[index].mime_type);
            (void)snprintf(out_identity->icon_name, sizeof(out_identity->icon_name),
                           "%s", DEFINITIONS[index].icon_name);
            break;
        }
    }
    return UMI_STATUS_OK;
}
