/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/education.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Education Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/education.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/*
 * Education provides course-building and assessment-focused workspaces.
 * Lessons, resources and learner evidence remain data contracts; layouts only
 * decide how their reusable panels begin on screen. */

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "outline", "Course Outline",
      "Course/lesson hierarchy", "umicom.ai.authorengine", "education.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "lesson", "Lesson Editor",
      "Structured lesson content", "umicom.ai.authorengine", "education.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR },
    { sizeof(UmiExperiencePanelDefinition), "assets", "Assets",
      "Media/code/lab assets", "umicom.media", "education.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "ai-tutor", "AI Tutor",
      "Tutor/assistant configuration", "umicom.ai", "ai.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "preview", "Preview",
      "Learner preview", "umicom.frontend", "education.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "assessment", "Assessment",
      "Question/assessment authoring", "umicom.education", "education.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "lab", "Lab",
      "Interactive coding/trading labs", "umicom.education", "education.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "results", "Results",
      "Assessment/evaluation results", "umicom.data", "education.blue", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE }
};

static const char *const LAYOUT_COURSE[] = {"outline", "lesson", "assets", "ai-tutor", "preview"};
static const char *const LAYOUT_ASSESSMENT[] = {"assessment", "lab", "results", "ai-tutor"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "course", "Course",
      "Course/lesson authoring", LAYOUT_COURSE, COUNT_OF(LAYOUT_COURSE), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "assessment", "Assessment",
      "Questions, labs and learner evaluation", LAYOUT_ASSESSMENT, COUNT_OF(LAYOUT_ASSESSMENT), UMI_EXPERIENCE_LAYOUT_LOCKABLE }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "education.course", "Course authoring",
      "Course/lesson content model", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.authorengine" },
    { sizeof(UmiExperienceFeatureDefinition), "education.assessment", "Assessments",
      "Question/assessment engine", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.education" },
    { sizeof(UmiExperienceFeatureDefinition), "education.labs", "Labs",
      "Interactive labs", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.education" },
    { sizeof(UmiExperienceFeatureDefinition), "education.ai", "AI tutor",
      "AI tutoring and content support", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai" },
    { sizeof(UmiExperienceFeatureDefinition), "education.delivery", "Delivery",
      "Learner web/mobile delivery", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.frontend" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.education",
    "Umicom Education Studio",
    "course",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_education(void)
{
    return &DEFINITION;
}
