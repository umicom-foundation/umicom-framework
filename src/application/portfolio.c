/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/portfolio.c
 *
 * PURPOSE:
 *   Define the long-lived Umicom application portfolio as declarative,
 *   Framework-owned composition data.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/portfolio.h"

#include <string.h>

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))
#define FRONTENDS_DESKTOP (UMI_FRONTEND_GTK4 | UMI_FRONTEND_QT | UMI_FRONTEND_WT)
#define FLAGS_DESKTOP (UMI_APPLICATION_STANDALONE | UMI_APPLICATION_FEDERATED | \
    UMI_APPLICATION_MULTI_WINDOW | UMI_APPLICATION_MULTI_MONITOR | \
    UMI_APPLICATION_HEADLESS_SAFE | UMI_APPLICATION_SANDBOX_RECOMMENDED)

static const char *const CAP_PLATFORM[] = {
    "umicom.runtime", "umicom.ui", "umicom.messaging", "umicom.security",
    "umicom.application.federation"
};
static const char *const CAP_DEVELOPMENT[] = {
    "umicom.ui", "umicom.workspace", "umicom.build", "umicom.protocol",
    "umicom.vcs", "umicom.debug", "umicom.application.federation"
};
static const char *const CAP_TRADING[] = {
    "umicom.ui", "umicom.trading", "umicom.chart", "umicom.data",
    "umicom.security", "umicom.application.federation"
};
static const char *const CAP_TMS[] = {
    "umicom.ui", "umicom.tms", "umicom.data", "umicom.integration",
    "umicom.workflow", "umicom.audit", "umicom.application.federation"
};
static const char *const CAP_MEDIA[] = {
    "umicom.ui", "umicom.media", "umicom.data", "umicom.tasks",
    "umicom.application.federation"
};
static const char *const CAP_AI_CREATOR[] = {
    "umicom.ui", "umicom.ai", "umicom.ai.media-generation", "umicom.media",
    "umicom.application.federation"
};
static const char *const CAP_RAG[] = {
    "umicom.ui", "umicom.ai.retrieval", "umicom.knowledge", "umicom.data",
    "umicom.application.federation"
};
static const char *const CAP_LLM[] = {
    "umicom.ui", "umicom.ai", "umicom.llm", "umicom.ai.tools",
    "umicom.application.federation"
};
static const char *const CAP_GAMES[] = {
    "umicom.ui", "umicom.games", "umicom.media", "umicom.ai",
    "umicom.application.federation"
};
static const char *const CAP_DESIGN[] = {
    "umicom.ui", "umicom.designer", "umicom.cad", "umicom.data",
    "umicom.application.federation"
};
static const char *const CAP_AUTHOR[] = {
    "umicom.ui", "umicom.ai.authorengine", "umicom.ai.retrieval",
    "umicom.media", "umicom.delivery", "umicom.application.federation"
};
static const char *const CAP_WEB[] = {
    "umicom.ui", "umicom.web", "umicom.frontend", "umicom.designer",
    "umicom.application.federation"
};
static const char *const CAP_DATABASE[] = {
    "umicom.ui", "umicom.data", "umicom.data.migrations",
    "umicom.data.repositories", "umicom.application.federation"
};
static const char *const CAP_OPERATIONS[] = {
    "umicom.ui", "umicom.observability.readiness", "umicom.metrics",
    "umicom.tracing", "umicom.resilience", "umicom.application.federation"
};
static const char *const CAP_OS[] = {
    "umicom.runtime", "umicom.ui", "umicom.os", "umicom.process",
    "umicom.security", "umicom.application.federation"
};

static const char *const OPTIONAL_AI[] = {"umicom.ai", "umicom.ai.retrieval"};
static const char *const OPTIONAL_WEB[] = {"umicom.web", "umicom.frontend"};
static const char *const OPTIONAL_TRADING[] = {"umicom.trading", "umicom.chart"};
static const char *const OPTIONAL_NONE[] = {"umicom.plugin.contributions"};

static const char *const DOMAIN_PLATFORM[] = {"shell", "operations", "security"};
static const char *const DOMAIN_DEVELOPMENT[] = {"shell", "development", "ai", "rag"};
static const char *const DOMAIN_STUDIO[] = {"shell", "development", "ai", "rag", "operations"};
static const char *const DOMAIN_TRADING[] = {"shell", "trading", "ai"};
static const char *const DOMAIN_TMS[] = {"shell", "treasury", "operations"};
static const char *const DOMAIN_MEDIA[] = {"shell", "media", "ai"};
static const char *const DOMAIN_MUSIC[] = {"shell", "music", "ai"};
static const char *const DOMAIN_CREATOR[] = {"shell", "creator", "media", "ai"};
static const char *const DOMAIN_RAG[] = {"shell", "rag", "ai"};
static const char *const DOMAIN_LLM[] = {"shell", "llm", "ai"};
static const char *const DOMAIN_GAMES[] = {"shell", "games", "media", "ai"};
static const char *const DOMAIN_KITCHEN[] = {"shell", "kitchen", "cad", "ai"};
static const char *const DOMAIN_CAD[] = {"shell", "cad", "design"};
static const char *const DOMAIN_AUTHOR[] = {"shell", "author", "rag", "media", "ai"};
static const char *const DOMAIN_WEB[] = {"shell", "web", "development", "design"};
static const char *const DOMAIN_DATABASE[] = {"shell", "database", "operations"};
static const char *const DOMAIN_INTEGRATION[] = {"shell", "integration", "operations"};
static const char *const DOMAIN_OPERATIONS[] = {"shell", "operations", "security"};
static const char *const DOMAIN_OS[] = {"shell", "os", "operations", "security"};
static const char *const DOMAIN_EDUCATION[] = {"shell", "education", "author", "media"};

static const char *const PROFILE_STUDIO[] = {"Development", "Review", "AI Assisted"};
static const char *const PROFILE_TRADER[] = {"Trading", "Research", "Strategy Development"};
static const char *const PROFILE_TMS[] = {"Front Office", "Middle Office", "Back Office"};
static const char *const PROFILE_MEDIA[] = {"Edit", "Review", "Export"};
static const char *const PROFILE_MUSIC[] = {"Compose", "Arrange", "Master"};
static const char *const PROFILE_CREATOR[] = {"Image", "Video", "Storyboard"};
static const char *const PROFILE_AI[] = {"Chat", "Models", "Evaluation"};
static const char *const PROFILE_DESIGN[] = {"Design", "Inspect", "Render"};
static const char *const PROFILE_GAMES[] = {"World", "Gameplay", "Debug"};
static const char *const PROFILE_AUTHOR[] = {"Research", "Author", "Publish"};
static const char *const PROFILE_PLATFORM[] = {"Applications", "System", "Operations"};
static const char *const PROFILE_EDUCATION[] = {"Course", "Lesson", "Assessment"};

#define APP(id, name, repo, exe, purpose_text, family_value, maturity_value, \
            frontend_value, capability_values, optional_values, domain_values, \
            profile_values) \
    { sizeof(UmiApplicationDefinition), (id), (name), (repo), (exe), \
      (purpose_text), (family_value), (maturity_value), (frontend_value), \
      FLAGS_DESKTOP, (capability_values), COUNT_OF(capability_values), \
      (optional_values), COUNT_OF(optional_values), (domain_values), \
      COUNT_OF(domain_values), (profile_values), COUNT_OF(profile_values) }

static const UmiApplicationDefinition APPLICATIONS[] = {
    APP("org.umicom.studio", "Umicom Studio IDE", "umicom-studio-ide",
        "umicom-studio", "Composable professional development and application orchestration workbench.",
        UMI_APPLICATION_FAMILY_DEVELOPMENT, UMI_APPLICATION_AVAILABLE,
        FRONTENDS_DESKTOP, CAP_PLATFORM, OPTIONAL_AI, DOMAIN_STUDIO, PROFILE_STUDIO),
    APP("org.umicom.ide", "Umicom IDE", "umicom-ide", "umicom-ide",
        "Focused coding, build, debug, test and source-control environment.",
        UMI_APPLICATION_FAMILY_DEVELOPMENT, UMI_APPLICATION_FOUNDATION,
        FRONTENDS_DESKTOP, CAP_DEVELOPMENT, OPTIONAL_AI, DOMAIN_DEVELOPMENT, PROFILE_STUDIO),
    APP("org.umicom.trader", "Umicom Trader", "umicom-trader", "umicom-trader",
        "Multi-monitor trading, charting, execution, risk, research and strategy workspace.",
        UMI_APPLICATION_FAMILY_FINANCE, UMI_APPLICATION_FOUNDATION,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_TRADING, OPTIONAL_AI,
        DOMAIN_TRADING, PROFILE_TRADER),
    APP("org.umicom.tms", "Umicom TMS", "umicom-tms", "umicom-tms",
        "Treasury management across trade capture, workflows, risk, settlement and accounting.",
        UMI_APPLICATION_FAMILY_FINANCE, UMI_APPLICATION_FOUNDATION,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_TMS, OPTIONAL_AI,
        DOMAIN_TMS, PROFILE_TMS),
    APP("org.umicom.media-studio", "Umicom Media Studio", "umicom-media-studio",
        "umicom-media-studio", "Non-linear video, audio, compositing, review and delivery workspace.",
        UMI_APPLICATION_FAMILY_MEDIA, UMI_APPLICATION_ROADMAP,
        FRONTENDS_DESKTOP, CAP_MEDIA, OPTIONAL_AI, DOMAIN_MEDIA, PROFILE_MEDIA),
    APP("org.umicom.music-studio", "Umicom Music Studio", "umicom-music-studio",
        "umicom-music-studio", "AI-assisted song creation, recording, arrangement, mixing and mastering.",
        UMI_APPLICATION_FAMILY_MEDIA, UMI_APPLICATION_ROADMAP,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_MEDIA, OPTIONAL_AI,
        DOMAIN_MUSIC, PROFILE_MUSIC),
    APP("org.umicom.creator", "Umicom AI Creator", "umicom-ai-creator",
        "umicom-creator", "Governed image, video, animation and storyboard generation.",
        UMI_APPLICATION_FAMILY_AI, UMI_APPLICATION_ROADMAP,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_AI_CREATOR, OPTIONAL_NONE,
        DOMAIN_CREATOR, PROFILE_CREATOR),
    APP("org.umicom.rag", "Umicom RAG", "umicom-rag", "umicom-rag",
        "Ingest, govern, retrieve, cite and evaluate private knowledge collections.",
        UMI_APPLICATION_FAMILY_AI, UMI_APPLICATION_FOUNDATION,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_RAG, OPTIONAL_NONE,
        DOMAIN_RAG, PROFILE_AI),
    APP("org.umicom.llm", "Umicom LLM", "umicom-llm", "umicom-llm",
        "Local and remote model runtime, chat, tools, evaluation and model operations.",
        UMI_APPLICATION_FAMILY_AI, UMI_APPLICATION_FOUNDATION,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_LLM, OPTIONAL_NONE,
        DOMAIN_LLM, PROFILE_AI),
    APP("org.umicom.games", "Umicom Games", "umicom-games", "umicom-games",
        "Game creation, world editing, asset management, simulation and play testing.",
        UMI_APPLICATION_FAMILY_GAMES, UMI_APPLICATION_ROADMAP,
        FRONTENDS_DESKTOP, CAP_GAMES, OPTIONAL_NONE, DOMAIN_GAMES, PROFILE_GAMES),
    APP("org.umicom.kitchen-designer", "Umicom Kitchen Designer",
        "umicom-kitchen-designer", "umicom-kitchen-designer",
        "Parametric kitchen planning, catalogues, costing and photorealistic presentation.",
        UMI_APPLICATION_FAMILY_DESIGN, UMI_APPLICATION_ROADMAP,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_DESIGN, OPTIONAL_AI,
        DOMAIN_KITCHEN, PROFILE_DESIGN),
    APP("org.umicom.cad", "Umicom CAD", "umicom-cad", "umicom-cad",
        "Parametric 2D drafting, 3D modelling, constraints, assemblies and rendering.",
        UMI_APPLICATION_FAMILY_DESIGN, UMI_APPLICATION_ROADMAP,
        FRONTENDS_DESKTOP, CAP_DESIGN, OPTIONAL_AI, DOMAIN_CAD, PROFILE_DESIGN),
    APP("org.umicom.author", "Umicom Author", "umicom-authorengine-ai",
        "umicom-author", "Research-to-publication engine for books, courses, sites and media.",
        UMI_APPLICATION_FAMILY_MEDIA, UMI_APPLICATION_FOUNDATION,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_AUTHOR, OPTIONAL_NONE,
        DOMAIN_AUTHOR, PROFILE_AUTHOR),
    APP("org.umicom.web-studio", "Umicom Web Studio", "umicom-web-studio",
        "umicom-web-studio", "Visual and code-first website, portal and web-application creation.",
        UMI_APPLICATION_FAMILY_DEVELOPMENT, UMI_APPLICATION_ROADMAP,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_WEB, OPTIONAL_AI,
        DOMAIN_WEB, PROFILE_STUDIO),
    APP("org.umicom.mobile-studio", "Umicom Mobile Studio", "umicom-mobile-studio",
        "umicom-mobile-studio", "Multi-platform mobile application design, testing and delivery.",
        UMI_APPLICATION_FAMILY_DEVELOPMENT, UMI_APPLICATION_ROADMAP,
        FRONTENDS_DESKTOP | UMI_FRONTEND_MOBILE, CAP_DEVELOPMENT, OPTIONAL_WEB,
        DOMAIN_DEVELOPMENT, PROFILE_STUDIO),
    APP("org.umicom.database-studio", "Umicom Database Studio", "umicom-database-studio",
        "umicom-database-studio", "Schema, query, migration, lineage, backup and data-operations workspace.",
        UMI_APPLICATION_FAMILY_ENTERPRISE, UMI_APPLICATION_FOUNDATION,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_DATABASE, OPTIONAL_AI,
        DOMAIN_DATABASE, PROFILE_PLATFORM),
    APP("org.umicom.integration-studio", "Umicom Integration Studio",
        "umicom-integration-studio", "umicom-integration-studio",
        "APIs, mappings, messaging, workflows, connectors and integration monitoring.",
        UMI_APPLICATION_FAMILY_ENTERPRISE, UMI_APPLICATION_FOUNDATION,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_TMS, OPTIONAL_NONE,
        DOMAIN_INTEGRATION, PROFILE_PLATFORM),
    APP("org.umicom.operations", "Umicom Operations", "umicom-operations",
        "umicom-operations", "Observability, deployment, health, incidents and fleet operations.",
        UMI_APPLICATION_FAMILY_ENTERPRISE, UMI_APPLICATION_FOUNDATION,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_OPERATIONS, OPTIONAL_NONE,
        DOMAIN_OPERATIONS, PROFILE_PLATFORM),
    APP("org.umicom.security-centre", "Umicom Security Centre",
        "umicom-security-centre", "umicom-security-centre",
        "Identity, policy, secrets, trust, approvals and security evidence.",
        UMI_APPLICATION_FAMILY_ENTERPRISE, UMI_APPLICATION_FOUNDATION,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_PLATFORM, OPTIONAL_NONE,
        DOMAIN_OPERATIONS, PROFILE_PLATFORM),
    APP("org.umicom.marketplace", "Umicom Marketplace", "umicom-marketplace",
        "umicom-marketplace", "Discover, verify, install and update applications, modules and content packs.",
        UMI_APPLICATION_FAMILY_PLATFORM, UMI_APPLICATION_FOUNDATION,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB, CAP_PLATFORM, OPTIONAL_NONE,
        DOMAIN_PLATFORM, PROFILE_PLATFORM),
    APP("org.umicom.os", "Umicom OS", "umicom-os", "umicom-os-shell",
        "Portable Umicom desktop and runtime for hosted, x86-64 and RISC-V environments.",
        UMI_APPLICATION_FAMILY_OPERATING_SYSTEM, UMI_APPLICATION_ROADMAP,
        FRONTENDS_DESKTOP, CAP_OS, OPTIONAL_NONE, DOMAIN_OS, PROFILE_PLATFORM),
    APP("org.umicom.education", "Umicom Education Studio", "umicom-education-studio",
        "umicom-education", "Course, lesson, lab, assessment and tutorial production and delivery.",
        UMI_APPLICATION_FAMILY_EDUCATION, UMI_APPLICATION_ROADMAP,
        FRONTENDS_DESKTOP | UMI_FRONTEND_WEB | UMI_FRONTEND_MOBILE,
        CAP_AUTHOR, OPTIONAL_TRADING, DOMAIN_EDUCATION, PROFILE_EDUCATION)
};

size_t umi_application_portfolio_count(void)
{
    return COUNT_OF(APPLICATIONS);
}

const UmiApplicationDefinition *umi_application_portfolio_at(size_t index)
{
    return index < umi_application_portfolio_count() ? &APPLICATIONS[index]
                                                     : NULL;
}

const UmiApplicationDefinition *umi_application_portfolio_find(
    const char *application_id)
{
    size_t index;
    if (application_id == NULL) return NULL;
    for (index = 0U; index < umi_application_portfolio_count(); ++index) {
        if (strcmp(APPLICATIONS[index].application_id, application_id) == 0)
            return &APPLICATIONS[index];
    }
    return NULL;
}

size_t umi_application_portfolio_family_count(UmiApplicationFamily family)
{
    size_t index;
    size_t count = 0U;
    for (index = 0U; index < umi_application_portfolio_count(); ++index) {
        if (APPLICATIONS[index].family == family) count += 1U;
    }
    return count;
}

size_t umi_application_portfolio_maturity_count(
    UmiApplicationMaturity maturity)
{
    size_t index;
    size_t count = 0U;
    for (index = 0U; index < umi_application_portfolio_count(); ++index) {
        if (APPLICATIONS[index].maturity == maturity) count += 1U;
    }
    return count;
}
