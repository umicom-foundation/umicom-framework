/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/path_policy.c
 *
 * PURPOSE:
 *   portable path validation rejecting traversal and unsafe absolute package paths.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/path_policy.h"

#include <string.h>
bool umi_dr_path_policy_has_traversal(const char*p){const char*s;if(p==NULL)return true;for(s=p;*s!='\0';++s){if(s[0]=='.'&&s[1]=='.'&&(s[2]=='/'||s[2]=='\\'||s[2]=='\0')&&(s==p||s[-1]=='/'||s[-1]=='\\'))return true;}return false;}
bool umi_dr_path_policy_relative_safe(const char*p){if(p==NULL||p[0]=='\0')return false;if(p[0]=='/'||p[0]=='\\')return false;if(((p[0]>='A'&&p[0]<='Z')||(p[0]>='a'&&p[0]<='z'))&&p[1]==':')return false;return !umi_dr_path_policy_has_traversal(p);}
