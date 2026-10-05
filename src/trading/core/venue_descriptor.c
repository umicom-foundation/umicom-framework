/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/venue_descriptor.c
 *
 * PURPOSE:
 *   Define exchange and execution-venue identity and capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/venue_descriptor.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Initialise a venue descriptor with bounded identifiers and text. */
UmiStatus umi_trading_venue_descriptor_init(UmiTradingVenueDescriptor *venue,const char *id,const char *mic,const char *name,bool auctions,bool hidden,uint32_t priority) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(venue==NULL||id==NULL||mic==NULL||name==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(venue,0,sizeof *venue);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_core_id_assign(&venue->venue_id,id)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_core_copy_text(venue->mic,sizeof venue->mic,mic)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_core_copy_text(venue->name,sizeof venue->name,name)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    venue->supports_auctions=auctions; venue->supports_hidden_liquidity=hidden; venue->priority=priority;
    return umi_trading_venue_descriptor_valid(venue)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the required venue identity fields. */
bool umi_trading_venue_descriptor_valid(const UmiTradingVenueDescriptor *venue) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (venue == NULL) return 0;
    if (memchr(venue->venue_id.value, '\0', sizeof(venue->venue_id.value)) == NULL) return 0;
    if (memchr(venue->mic, '\0', sizeof(venue->mic)) == NULL) return 0;
    if (memchr(venue->name, '\0', sizeof(venue->name)) == NULL) return 0;
 return venue!=NULL&&venue->venue_id.value[0]!='\0'&&venue->mic[0]!='\0'&&venue->name[0]!='\0'; }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingVenueDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x58ba720228e3d0f2);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradingVenueDescriptor *)0)->venue_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradingVenueDescriptor *)0)->mic)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradingVenueDescriptor *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTradingVenueDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTradingVenueDescriptor *)0)->venue_id.value) - 1U +
        8U + sizeof(((UmiTradingVenueDescriptor *)0)->mic) - 1U +
        8U + sizeof(((UmiTradingVenueDescriptor *)0)->name) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTradingVenueDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiTradingVenueDescriptor *value)
{
    UmiArchiveWriteText(writer, value->venue_id.value, sizeof(value->venue_id.value));
    UmiArchiveWriteText(writer, value->mic, sizeof(value->mic));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->supports_auctions);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->supports_hidden_liquidity);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
}
static void UmiTradingVenueDescriptorArchiveRead(UmiArchiveReader *reader, UmiTradingVenueDescriptor *value)
{
    UmiArchiveReadText(reader, value->venue_id.value, sizeof(value->venue_id.value));
    UmiArchiveReadText(reader, value->mic, sizeof(value->mic));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->supports_auctions = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->supports_hidden_liquidity = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTradingVenueDescriptorArchiveValidate(const UmiTradingVenueDescriptor *value)
{
    return umi_trading_venue_descriptor_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_venue_descriptor_archive_encode, umi_trading_venue_descriptor_archive_decode,
    UmiTradingVenueDescriptor, UmiTradingVenueDescriptorArchiveSchema, UmiTradingVenueDescriptorArchiveBound, UmiTradingVenueDescriptorArchiveWrite, UmiTradingVenueDescriptorArchiveRead, UmiTradingVenueDescriptorArchiveValidate)
