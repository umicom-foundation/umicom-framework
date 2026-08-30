/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/theme_token.c
 *
 * PURPOSE:
 *   semantic theme token representation independent of GTK CSS, Qt stylesheets or browser CSS.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/theme_token.h"

static double fc_abs(double v){return v<0.0?-v:v;}
UmiStatus umi_fc_theme_token_make(const char *id,double r,double g,double b,double a,bool required,UmiFcThemeToken *out_token){UmiStatus st;if(out_token==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(r<0.0||r>1.0||g<0.0||g>1.0||b<0.0||b>1.0||a<0.0||a>1.0)return UMI_STATUS_INVALID_ARGUMENT;*out_token=(UmiFcThemeToken){0};st=umi_fc_copy_text(out_token->id,sizeof(out_token->id),id);if(st!=UMI_STATUS_OK)return st;out_token->r=r;out_token->g=g;out_token->b=b;out_token->a=a;out_token->required=required;return UMI_STATUS_OK;}
double umi_fc_theme_token_distance(const UmiFcThemeToken *a,const UmiFcThemeToken *b){if(a==NULL||b==NULL)return 1.0;return (fc_abs(a->r-b->r)+fc_abs(a->g-b->g)+fc_abs(a->b-b->b)+fc_abs(a->a-b->a))/4.0;}
