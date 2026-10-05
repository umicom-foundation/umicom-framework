/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_component_descriptor.c
 *
 * PURPOSE:
 *   Verify semantic descriptors augment canonical component kinds without replacing them.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
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
#include "umicom/ui/design/component_descriptor.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/component_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignComponentDescriptorTransferEqual(const UmiDesignComponentDescriptor *a, const UmiDesignComponentDescriptor *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->display_name, b->display_name) == 0 &&
        a->kind == b->kind &&
        a->default_role == b->default_role &&
        a->capability_flags == b->capability_flags &&
        a->interactive == b->interactive;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignComponentDescriptorTransferTails(UmiDesignComponentDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->display_name) + 1U;
        memset(value->display_name + used, 0xa5, sizeof(value->display_name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignComponentDescriptorTransferMalformed(const UmiDesignComponentDescriptor *sample)
{
    (void)sample;
    {
        UmiDesignComponentDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_design_component_descriptor_valid(&invalid)) ||
            umi_design_component_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDesignComponentDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.display_name, 'x', sizeof(invalid.display_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_design_component_descriptor_valid(&invalid)) ||
            umi_design_component_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated display_name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignComponentDescriptorTransferCases, UmiDesignComponentDescriptor,
    umi_design_component_descriptor_archive_encode, umi_design_component_descriptor_archive_decode,
    UmiDesignComponentDescriptorTransferEqual, UmiDesignComponentDescriptorTransferTails, UmiDesignComponentDescriptorTransferMalformed)

int main(void){UmiDesignComponentDescriptor d;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_component_descriptor_init(&d,"button.primary","Primary Button",UMI_UI_COMPONENT_BUTTON,UMI_DESIGN_ROLE_PRIMARY,1)!=UMI_STATUS_OK)return 1;
    if (UmiDesignComponentDescriptorTransferCases(&d) != 0) return 1;
return umi_design_component_descriptor_valid(&d)?0:2;}
