/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_inspector_property.c
 *
 * PURPOSE:
 *   Exercise the inspector property enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/ui/enterprise/inspector_property.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/enterprise/inspector_property.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiEntInspectorPropertyTransferEqual(const UmiUiEntInspectorProperty *a, const UmiUiEntInspectorProperty *b)
{
    return strcmp(a->property_id, b->property_id) == 0 &&
        strcmp(a->section_id, b->section_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        strcmp(a->value, b->value) == 0 &&
        strcmp(a->value_type, b->value_type) == 0 &&
        a->editable == b->editable &&
        a->required == b->required;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiEntInspectorPropertyTransferTails(UmiUiEntInspectorProperty *value)
{
    (void)value;
    {
        size_t used = strlen(value->property_id) + 1U;
        memset(value->property_id + used, 0xa5, sizeof(value->property_id) - used);
    }
    {
        size_t used = strlen(value->section_id) + 1U;
        memset(value->section_id + used, 0xa5, sizeof(value->section_id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->value) + 1U;
        memset(value->value + used, 0xa5, sizeof(value->value) - used);
    }
    {
        size_t used = strlen(value->value_type) + 1U;
        memset(value->value_type + used, 0xa5, sizeof(value->value_type) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiEntInspectorPropertyTransferMalformed(const UmiUiEntInspectorProperty *sample)
{
    (void)sample;
    {
        UmiUiEntInspectorProperty invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.property_id, 'x', sizeof(invalid.property_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_inspector_property_validate(&invalid)) ||
            umi_ui_ent_inspector_property_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated property_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiEntInspectorProperty invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.section_id, 'x', sizeof(invalid.section_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_inspector_property_validate(&invalid)) ||
            umi_ui_ent_inspector_property_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated section_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiEntInspectorProperty invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_inspector_property_validate(&invalid)) ||
            umi_ui_ent_inspector_property_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiEntInspectorProperty invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value, 'x', sizeof(invalid.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_inspector_property_validate(&invalid)) ||
            umi_ui_ent_inspector_property_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiEntInspectorProperty invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value_type, 'x', sizeof(invalid.value_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_inspector_property_validate(&invalid)) ||
            umi_ui_ent_inspector_property_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value_type was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiEntInspectorPropertyTransferCases, UmiUiEntInspectorProperty,
    umi_ui_ent_inspector_property_archive_encode, umi_ui_ent_inspector_property_archive_decode,
    UmiUiEntInspectorPropertyTransferEqual, UmiUiEntInspectorPropertyTransferTails, UmiUiEntInspectorPropertyTransferMalformed)

int main(void){UmiUiEntInspectorProperty v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_inspector_property_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.property_id,sizeof v.property_id,"id")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.section_id,sizeof v.section_id,"general")!=UMI_STATUS_OK)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.label,sizeof v.label,"Name")!=UMI_STATUS_OK)return 4;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_inspector_property_validate(&v))return 9;
    if (UmiUiEntInspectorPropertyTransferCases(&v) != 0) return 1;
puts("ok");return 0;}
