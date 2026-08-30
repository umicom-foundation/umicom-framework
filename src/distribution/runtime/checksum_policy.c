/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/checksum_policy.c
 *
 * PURPOSE:
 *   checksum algorithm and digest presence/match requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/checksum_policy.h"

#include <string.h>
void umi_dr_checksum_policy_init(UmiDrChecksumPolicy*p){if(p!=NULL){*p=(UmiDrChecksumPolicy){0};(void)umi_dr_copy_text(p->algorithm,sizeof(p->algorithm),"sha256");p->digest_required=true;p->match_required=true;}}bool umi_dr_checksum_policy_accept(const UmiDrChecksumPolicy*p,const char*e,const char*a){if(p==NULL)return false;if(p->digest_required&&(e==NULL||e[0]=='\0'||a==NULL||a[0]=='\0'))return false;if(p->match_required&&e!=NULL&&a!=NULL&&strcmp(e,a)!=0)return false;return true;}
