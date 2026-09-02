/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/change_set.c
 *
 * PURPOSE:
 *   Collect file changes and compute deterministic aggregate change risk and fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/change_set.h"
#include <string.h>
/* Clear change-set state before composing candidate mutations. */
void umi_helix_change_set_init(UmiHelixChangeSet *s){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL) memset(s,0,sizeof(*s)); }
/* Copy a validated file-change record and fold path/kind into deterministic evidence. */
UmiStatus umi_helix_change_set_add(UmiHelixChangeSet *s,const UmiHelixFileChange *c,UmiHelixChangeKind kind,const char *path){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||c==NULL||path==NULL||umi_helix_file_change_validate(c)!=UMI_STATUS_OK){ return UMI_STATUS_INVALID_ARGUMENT; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_HELIX_MAX_ITEMS){ return UMI_STATUS_CAPACITY_EXCEEDED; } s->items[s->count++]=*c; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(kind==UMI_HELIX_CHANGE_DELETE||kind==UMI_HELIX_CHANGE_RENAME) s->destructive_count++; s->fingerprint^=umi_helix_hash_text(path)+((uint64_t)kind+1U)*1099511628211ULL; return UMI_STATUS_OK; }
/* Blend mean file risk with destructive-operation density. */
double umi_helix_change_set_risk(const UmiHelixChangeSet *s){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->count==0U){ return 0.0; } double total=0.0; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<s->count;i++) total+=1.0-umi_helix_file_change_score(&s->items[i]); double mean=total/(double)s->count; double destructive=(double)s->destructive_count/(double)s->count; return umi_helix_clamp_score((mean*0.7)+(destructive*0.3)); }
