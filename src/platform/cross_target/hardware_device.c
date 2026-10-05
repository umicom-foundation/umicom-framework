/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/hardware_device.c
 *
 * PURPOSE:
 *   Describe hardware devices using bus-neutral MMIO/IRQ metadata for Umicom OS driver matching.
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

#include "umicom/platform/cross_target/hardware_device.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that ct hardware device satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_hardware_device_validate(const UmiCtHardwareDevice*d){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (d == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(d->device_id, '\0', sizeof(d->device_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(d->compatible, '\0', sizeof(d->compatible)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||!umi_ct_id_valid(d->device_id)||d->device_class<UMI_CT_DEVICE_CPU||d->device_class>UMI_CT_DEVICE_OTHER)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d->mmio_size!=0U&&d->mmio_base+d->mmio_size<d->mmio_base)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtHardwareDeviceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x52ce67e9b39d4ce0);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtHardwareDevice *)0)->device_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtHardwareDevice *)0)->compatible)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCtHardwareDeviceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCtHardwareDevice *)0)->device_id) - 1U +
        8U + sizeof(((UmiCtHardwareDevice *)0)->compatible) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtHardwareDeviceArchiveWrite(UmiArchiveWriter *writer, const UmiCtHardwareDevice *value)
{
    UmiArchiveWriteText(writer, value->device_id, sizeof(value->device_id));
    UmiArchiveWriteText(writer, value->compatible, sizeof(value->compatible));
    UmiArchiveWriteSigned(writer, (int64_t)value->device_class);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->mmio_base);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->mmio_size);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->irq);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->present);
}
static void UmiCtHardwareDeviceArchiveRead(UmiArchiveReader *reader, UmiCtHardwareDevice *value)
{
    UmiArchiveReadText(reader, value->device_id, sizeof(value->device_id));
    UmiArchiveReadText(reader, value->compatible, sizeof(value->compatible));
    value->device_class = (UmiCtDeviceClass)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->mmio_base = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->mmio_size = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->irq = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->present = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCtHardwareDeviceArchiveValidate(const UmiCtHardwareDevice *value)
{
    return umi_ct_hardware_device_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_hardware_device_archive_encode, umi_ct_hardware_device_archive_decode,
    UmiCtHardwareDevice, UmiCtHardwareDeviceArchiveSchema, UmiCtHardwareDeviceArchiveBound, UmiCtHardwareDeviceArchiveWrite, UmiCtHardwareDeviceArchiveRead, UmiCtHardwareDeviceArchiveValidate)
