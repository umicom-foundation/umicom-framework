/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/types.h
 *
 * PURPOSE:
 *   Define stable identifiers, capacities, versions and enums shared by the declarative application system.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The declarations below describe semantic application data and behaviour.
 * They deliberately avoid GUI-toolkit types so the same contract can be used
 * by GTK4, web, headless tests and future frontend adapters.
 */

#ifndef UMICOM_DECLARATIVE_TYPES_H
#define UMICOM_DECLARATIVE_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DECL_ID_CAPACITY 128U
#define UMI_DECL_NAME_CAPACITY 128U
#define UMI_DECL_TEXT_CAPACITY 512U
#define UMI_DECL_PATH_CAPACITY 512U
#define UMI_DECL_MAX_ATTRIBUTES 32U
#define UMI_DECL_MAX_DIAGNOSTICS 128U
#define UMI_DECL_MAX_COMPONENTS 512U
#define UMI_DECL_MAX_PROPERTIES 32U
#define UMI_DECL_MAX_TEMPLATES 64U

/* Values remain toolkit neutral so the same template can feed GTK4, web or headless renderers. */
typedef enum UmiDeclValueKind {
    UMI_DECL_VALUE_STRING = 1,
    UMI_DECL_VALUE_BOOLEAN = 2,
    UMI_DECL_VALUE_INTEGER = 3,
    UMI_DECL_VALUE_REAL = 4
} UmiDeclValueKind;

typedef enum UmiDeclNodeKind {
    UMI_DECL_NODE_COMPONENT = 1,
    UMI_DECL_NODE_RESOURCE = 2
} UmiDeclNodeKind;

typedef enum UmiDeclDiagnosticSeverity {
    UMI_DECL_DIAGNOSTIC_INFO = 1,
    UMI_DECL_DIAGNOSTIC_WARNING = 2,
    UMI_DECL_DIAGNOSTIC_ERROR = 3
} UmiDeclDiagnosticSeverity;

typedef struct UmiDeclVersion {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
} UmiDeclVersion;

UmiDeclVersion umi_decl_version_current(void);
int umi_decl_version_equal(UmiDeclVersion left, UmiDeclVersion right);
UmiStatus umi_decl_copy_text(char *destination, size_t capacity, const char *source);
int umi_decl_id_is_valid(const char *identifier);
const char *umi_decl_value_kind_text(UmiDeclValueKind kind);

#ifdef __cplusplus
}
#endif

#endif
