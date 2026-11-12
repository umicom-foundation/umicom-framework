/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/cell_value.c
 *
 * PURPOSE:
 *   Implement typed cell values and equality.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/cell_value.h"
#include <string.h>

UmiStatus umi_ui_ent_cell_value_validate(const UmiUiEntCellValue *value)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    switch (value->kind) {
        case UMI_UI_ENT_VALUE_EMPTY:
        case UMI_UI_ENT_VALUE_INTEGER:
        case UMI_UI_ENT_VALUE_REAL:
            return UMI_STATUS_OK;
        case UMI_UI_ENT_VALUE_TEXT:
            return memchr(value->text, '\0', sizeof(value->text)) != NULL
                ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
        case UMI_UI_ENT_VALUE_BOOLEAN:
            return value->boolean_value == 0 || value->boolean_value == 1
                ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }
}

/*
 * Release or reset state held by ui ent cell value so the same storage can be reused
 * safely.
 */
void umi_ui_ent_cell_value_clear(UmiUiEntCellValue *v){/* Apply this branch only when its contract condition is satisfied. */ if(v){memset(v,0,sizeof *v);v->kind=UMI_UI_ENT_VALUE_EMPTY;}}
/*
 * Provide the ui ent cell value set text operation used by this module and its client
 * applications.
 */
/* Capture bounded borrowed text before replacing its cell owner. Preserve the old clear-before-copy path to document why failed edits previously destroyed state. The previous implementation is retained for review. */
#if 0
UmiStatus umi_ui_ent_cell_value_set_text(UmiUiEntCellValue *v,const char *t){UmiStatus s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!v)return UMI_STATUS_INVALID_ARGUMENT;umi_ui_ent_cell_value_clear(v);s=umi_ui_ent_copy_text(v->text,sizeof v->text,t);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)v->kind=UMI_UI_ENT_VALUE_TEXT;return s;}
#endif
UmiStatus umi_ui_ent_cell_value_set_text(UmiUiEntCellValue *value, const char *text)
{
    UmiUiEntCellValue staged = {0};
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_ui_ent_copy_text(staged.text, sizeof(staged.text), text);
    if (status != UMI_STATUS_OK) return status;
    staged.kind = UMI_UI_ENT_VALUE_TEXT;
    *value = staged;
    return UMI_STATUS_OK;
}
/*
 * Provide the ui ent cell value set integer operation used by this module and its client
 * applications.
 */
void umi_ui_ent_cell_value_set_integer(UmiUiEntCellValue *v,int64_t n){/* Apply this branch only when its contract condition is satisfied. */ if(v){umi_ui_ent_cell_value_clear(v);v->kind=UMI_UI_ENT_VALUE_INTEGER;v->integer_value=n;}}
/*
 * Provide the ui ent cell value set real operation used by this module and its client
 * applications.
 */
void umi_ui_ent_cell_value_set_real(UmiUiEntCellValue *v,double n){/* Apply this branch only when its contract condition is satisfied. */ if(v){umi_ui_ent_cell_value_clear(v);v->kind=UMI_UI_ENT_VALUE_REAL;v->real_value=n;}}
/*
 * Provide the ui ent cell value set boolean operation used by this module and its client
 * applications.
 */
void umi_ui_ent_cell_value_set_boolean(UmiUiEntCellValue *v,int e){/* Apply this branch only when its contract condition is satisfied. */ if(v){umi_ui_ent_cell_value_clear(v);v->kind=UMI_UI_ENT_VALUE_BOOLEAN;v->boolean_value=e?1:0;}}
/*
 * Provide the ui ent cell value equal operation used by this module and its client
 * applications.
 */
/* Validate active public fields before equality; unknown kinds and unterminated text cannot reach strcmp. Keep the earlier equality logic for review. The previous implementation is retained for review. */
#if 0
int umi_ui_ent_cell_value_equal(const UmiUiEntCellValue *a,const UmiUiEntCellValue *b){/* Use the stable identifier comparison to choose the matching record or policy. */ if(!a||!b||a->kind!=b->kind)return 0;/* Select the behaviour associated with the requested command or state value. */ switch(a->kind){case UMI_UI_ENT_VALUE_EMPTY:return 1;case UMI_UI_ENT_VALUE_TEXT:return strcmp(a->text,b->text)==0;case UMI_UI_ENT_VALUE_INTEGER:return a->integer_value==b->integer_value;case UMI_UI_ENT_VALUE_REAL:return a->real_value==b->real_value;case UMI_UI_ENT_VALUE_BOOLEAN:return a->boolean_value==b->boolean_value;default:return 0;}}
#endif
int umi_ui_ent_cell_value_equal(const UmiUiEntCellValue *left,
                                const UmiUiEntCellValue *right)
{
    /* Inactive fields are irrelevant. A malformed active field is never read
     * through an unbounded string operation or reported as a matching value. */
    if (umi_ui_ent_cell_value_validate(left) != UMI_STATUS_OK ||
        umi_ui_ent_cell_value_validate(right) != UMI_STATUS_OK ||
        left->kind != right->kind) return 0;
    switch (left->kind) {
        case UMI_UI_ENT_VALUE_EMPTY: return 1;
        case UMI_UI_ENT_VALUE_TEXT: return strcmp(left->text, right->text) == 0;
        case UMI_UI_ENT_VALUE_INTEGER: return left->integer_value == right->integer_value;
        case UMI_UI_ENT_VALUE_REAL: return left->real_value == right->real_value;
        case UMI_UI_ENT_VALUE_BOOLEAN: return left->boolean_value == right->boolean_value;
        default: return 0;
    }
}
