/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/form_body.h
 * PURPOSE: Decode bounded browser form bodies without truncation or duplicate controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_FORM_BODY_H
#define UMICOM_WEB_FORM_BODY_H
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WEB_FORM_BODY_MAX_BYTES 131072U
#define UMI_WEB_FORM_CONTROL_CAPACITY 261U
#define UMI_WEB_FORM_NAME_CAPACITY 224U
#define UMI_WEB_FORM_VALUE_CAPACITY 1024U
typedef struct UmiWebFormControl {
  char name[UMI_WEB_FORM_NAME_CAPACITY];
  char value[UMI_WEB_FORM_VALUE_CAPACITY];
} UmiWebFormControl;
/* Allocate this collection on the heap. It owns decoded UTF-8 text. */
typedef struct UmiWebFormBody {
  size_t count;
  UmiWebFormControl controls[UMI_WEB_FORM_CONTROL_CAPACITY];
} UmiWebFormBody;
/* application/x-www-form-urlencoded only; caller validates method/content type.
 * Exactly length bytes are read. Reject malformed escapes, NUL, invalid UTF-8,
 * missing '=', empty names/segments and duplicate decoded names. '+' means space.
 * Empty body is valid. Failure preserves out. No authentication is performed. */
UmiStatus umi_web_form_body_parse(const void *bytes, size_t length, UmiWebFormBody *out);
/* Returned text is borrowed. Malformed collections or duplicate names return
 * NULL, as does absence. Call parse to obtain a complete validated collection. */
const char *umi_web_form_body_get(const UmiWebFormBody *body, const char *name);
#ifdef __cplusplus
}
#endif
#endif
