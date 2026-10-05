/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_ui_component_spec.c
 *
 * PURPOSE:
 *   Test one reusable toolkit-neutral UI component behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <string.h>
#include "umicom/ui/components/component.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/components/component.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiComponentSpecTransferEqual(const UmiUiComponentSpec *a, const UmiUiComponentSpec *b)
{
    return a->structure_size == b->structure_size &&
        a->kind == b->kind &&
        strcmp(a->id, b->id) == 0 &&
        strcmp(a->text, b->text) == 0 &&
        strcmp(a->css_class, b->css_class) == 0 &&
        strcmp(a->tooltip, b->tooltip) == 0 &&
        strcmp(a->accessible_name, b->accessible_name) == 0 &&
        a->orientation == b->orientation &&
        a->width == b->width &&
        a->height == b->height &&
        a->spacing == b->spacing &&
        a->numeric_value == b->numeric_value &&
        a->visible == b->visible &&
        a->sensitive == b->sensitive &&
        a->hexpand == b->hexpand &&
        a->vexpand == b->vexpand;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiComponentSpecTransferTails(UmiUiComponentSpec *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->text) + 1U;
        memset(value->text + used, 0xa5, sizeof(value->text) - used);
    }
    {
        size_t used = strlen(value->css_class) + 1U;
        memset(value->css_class + used, 0xa5, sizeof(value->css_class) - used);
    }
    {
        size_t used = strlen(value->tooltip) + 1U;
        memset(value->tooltip + used, 0xa5, sizeof(value->tooltip) - used);
    }
    {
        size_t used = strlen(value->accessible_name) + 1U;
        memset(value->accessible_name + used, 0xa5, sizeof(value->accessible_name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiComponentSpecTransferMalformed(const UmiUiComponentSpec *sample)
{
    (void)sample;
    {
        UmiUiComponentSpec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_component_spec_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_component_spec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiComponentSpec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.text, 'x', sizeof(invalid.text));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_component_spec_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_component_spec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated text was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiComponentSpec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.css_class, 'x', sizeof(invalid.css_class));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_component_spec_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_component_spec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated css_class was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiComponentSpec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.tooltip, 'x', sizeof(invalid.tooltip));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_component_spec_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_component_spec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated tooltip was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiComponentSpec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.accessible_name, 'x', sizeof(invalid.accessible_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_component_spec_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_component_spec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated accessible_name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiComponentSpecTransferCases, UmiUiComponentSpec,
    umi_ui_component_spec_archive_encode, umi_ui_component_spec_archive_decode,
    UmiUiComponentSpecTransferEqual, UmiUiComponentSpecTransferTails, UmiUiComponentSpecTransferMalformed)

int main(void){UmiUiComponentSpec s=umi_ui_component_spec_default(UMI_UI_COMPONENT_BUTTON);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_component_spec_set_id(&s,"save")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_component_spec_set_text(&s,"Save")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_component_spec_validate(&s)!=UMI_STATUS_OK)return 3;
    if (UmiUiComponentSpecTransferCases(&s) != 0) return 1;
return strcmp(umi_ui_component_kind_name(s.kind),"button")==0?0:4;}
