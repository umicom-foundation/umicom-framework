/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/bank_relationship.c
 *
 * PURPOSE:
 *   Implement represent customer-to-bank relationship ownership independent of presentation channels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/bank_relationship.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise banking bank relationship from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_bank_relationship_init(UmiBankingBankRelationship *value,
    const char *id,
    const char *customer_id,
    const char *relationship_manager,
    bool primary_relationship) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->customer_id,customer_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->relationship_manager,sizeof value->relationship_manager,relationship_manager);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->primary_relationship=primary_relationship;
    return umi_banking_bank_relationship_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking bank relationship satisfies its contract before another service
 * relies on it.
 */
bool umi_banking_bank_relationship_valid(const UmiBankingBankRelationship *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->customer_id.value, '\0', sizeof(value->customer_id.value)) == NULL) return 0;
    if (memchr(value->relationship_manager, '\0', sizeof(value->relationship_manager)) == NULL) return 0;

    return value!=NULL && (value->relationship_manager[0]!='\0');
}

/*
 * Provide the banking bank relationship primary operation used by this module and its
 * client applications.
 */
bool umi_banking_bank_relationship_primary(const UmiBankingBankRelationship *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->primary_relationship;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiBankingBankRelationshipArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3b017f2e9571a131);
    schema = (schema ^ (uint64_t)sizeof(((UmiBankingBankRelationship *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBankingBankRelationship *)0)->customer_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBankingBankRelationship *)0)->relationship_manager)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiBankingBankRelationshipArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiBankingBankRelationship *)0)->id.value) - 1U +
        8U + sizeof(((UmiBankingBankRelationship *)0)->customer_id.value) - 1U +
        8U + sizeof(((UmiBankingBankRelationship *)0)->relationship_manager) - 1U +
        8U;
}
static void UmiBankingBankRelationshipArchiveWrite(UmiArchiveWriter *writer, const UmiBankingBankRelationship *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->customer_id.value, sizeof(value->customer_id.value));
    UmiArchiveWriteText(writer, value->relationship_manager, sizeof(value->relationship_manager));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->primary_relationship);
}
static void UmiBankingBankRelationshipArchiveRead(UmiArchiveReader *reader, UmiBankingBankRelationship *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->customer_id.value, sizeof(value->customer_id.value));
    UmiArchiveReadText(reader, value->relationship_manager, sizeof(value->relationship_manager));
    value->primary_relationship = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiBankingBankRelationshipArchiveValidate(const UmiBankingBankRelationship *value)
{
    return umi_banking_bank_relationship_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_banking_bank_relationship_archive_encode, umi_banking_bank_relationship_archive_decode,
    UmiBankingBankRelationship, UmiBankingBankRelationshipArchiveSchema, UmiBankingBankRelationshipArchiveBound, UmiBankingBankRelationshipArchiveWrite, UmiBankingBankRelationshipArchiveRead, UmiBankingBankRelationshipArchiveValidate)
