/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/hardware_bus.c
 *
 * PURPOSE:
 *   Describe discoverable hardware buses and address/interrupt translation capabilities.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/hardware_bus.h"
#include "../../base/value_archive_internal.h"

/* Check that ct hardware bus satisfies its contract before another service relies on it. */
UmiStatus umi_ct_hardware_bus_validate(const UmiCtHardwareBus*b){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (b == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(b->bus_id, '\0', sizeof(b->bus_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||!umi_ct_id_valid(b->bus_id)||b->type<UMI_CT_BUS_PLATFORM||b->type>UMI_CT_BUS_SPI||(b->address_bits!=32U&&b->address_bits!=64U))return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtHardwareBusArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd5447348fc917777);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtHardwareBus *)0)->bus_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCtHardwareBusArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCtHardwareBus *)0)->bus_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtHardwareBusArchiveWrite(UmiArchiveWriter *writer, const UmiCtHardwareBus *value)
{
    UmiArchiveWriteText(writer, value->bus_id, sizeof(value->bus_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->type);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enumerable);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->hotplug);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->dma);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->address_bits);
}
static void UmiCtHardwareBusArchiveRead(UmiArchiveReader *reader, UmiCtHardwareBus *value)
{
    UmiArchiveReadText(reader, value->bus_id, sizeof(value->bus_id));
    value->type = (UmiCtBusType)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enumerable = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->hotplug = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->dma = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->address_bits = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiCtHardwareBusArchiveValidate(const UmiCtHardwareBus *value)
{
    return umi_ct_hardware_bus_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_hardware_bus_archive_encode, umi_ct_hardware_bus_archive_decode,
    UmiCtHardwareBus, UmiCtHardwareBusArchiveSchema, UmiCtHardwareBusArchiveBound, UmiCtHardwareBusArchiveWrite, UmiCtHardwareBusArchiveRead, UmiCtHardwareBusArchiveValidate)
