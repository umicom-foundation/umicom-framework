/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/form_validation.h
 *
 * PURPOSE:
 *   Represent form validation outcome separately from rendering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_FORM_VALIDATION_H
#define UMICOM_UI_COMPONENTS_FORM_VALIDATION_H
#include <stdbool.h>
#define UMI_UI_FORM_MESSAGE_CAPACITY 256U
typedef struct UmiUiFormValidation { bool valid; char message[UMI_UI_FORM_MESSAGE_CAPACITY]; } UmiUiFormValidation;
UmiUiFormValidation umi_ui_form_validation_ok(void);
UmiUiFormValidation umi_ui_form_validation_error(const char *message);
#endif
