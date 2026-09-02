/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/path_policy.c
 *
 * PURPOSE:
 *   portable path validation rejecting traversal and unsafe absolute package paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/path_policy.h"

#include <string.h>
/*
 * Provide the dr path policy has traversal operation used by this module and its client
 * applications.
 */
bool umi_dr_path_policy_has_traversal(const char*p){const char*s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return true;/* Visit each bounded item once so every record receives the same rule. */ for(s=p;*s!='\0';++s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s[0]=='.'&&s[1]=='.'&&(s[2]=='/'||s[2]=='\\'||s[2]=='\0')&&(s==p||s[-1]=='/'||s[-1]=='\\'))return true;}return false;}
/*
 * Provide the dr path policy relative safe operation used by this module and its client
 * applications.
 */
bool umi_dr_path_policy_relative_safe(const char*p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||p[0]=='\0')return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p[0]=='/'||p[0]=='\\')return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(((p[0]>='A'&&p[0]<='Z')||(p[0]>='a'&&p[0]<='z'))&&p[1]==':')return false;return !umi_dr_path_policy_has_traversal(p);}
