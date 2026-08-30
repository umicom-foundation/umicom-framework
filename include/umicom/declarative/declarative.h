/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/declarative.h
 *
 * PURPOSE:
 *   Provide one aggregate include for the complete declarative application language, compiler and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The declarations below describe semantic application data and behaviour.
 * They deliberately avoid GUI-toolkit types so the same contract can be used
 * by GTK4, web, headless tests and future frontend adapters.
 */

#ifndef UMICOM_DECLARATIVE_DECLARATIVE_H
#define UMICOM_DECLARATIVE_DECLARATIVE_H
#include "umicom/declarative/types.h"
#include "umicom/declarative/value.h"
#include "umicom/declarative/attribute.h"
#include "umicom/declarative/node.h"
#include "umicom/declarative/document.h"
#include "umicom/declarative/diagnostic.h"
#include "umicom/declarative/lexer.h"
#include "umicom/declarative/parser.h"
#include "umicom/declarative/serializer.h"
#include "umicom/declarative/formatter.h"
#include "umicom/declarative/property.h"
#include "umicom/declarative/component.h"
#include "umicom/declarative/component_registry.h"
#include "umicom/declarative/catalogue.h"
#include "umicom/declarative/binding.h"
#include "umicom/declarative/command.h"
#include "umicom/declarative/event.h"
#include "umicom/declarative/frontend.h"
#include "umicom/declarative/starter.h"
#include "umicom/declarative/schema.h"
#include "umicom/declarative/validator.h"
#include "umicom/declarative/plan.h"
#include "umicom/declarative/compiler.h"
#include "umicom/declarative/migration.h"
#include "umicom/declarative/template_store.h"
#include "umicom/declarative/renderer.h"
#include "umicom/declarative/generator.h"
#endif
