/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/form.h
 *
 * PURPOSE:
 *   Define common form-field metadata shared by settings pages, TMS forms and OS control panels.
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

#ifndef UMICOM_UI_COMPONENTS_FORM_H
#define UMICOM_UI_COMPONENTS_FORM_H
#include <stdbool.h>
#include "umicom/ui/components/component.h"
typedef struct UmiUiFormField { char id[UMI_UI_COMPONENT_ID_CAPACITY]; char label[UMI_UI_COMPONENT_TEXT_CAPACITY]; char placeholder[UMI_UI_COMPONENT_TEXT_CAPACITY]; bool required; bool secret; } UmiUiFormField;
UmiStatus umi_ui_form_field_init(UmiUiFormField *field,const char *id,const char *label);
#endif
