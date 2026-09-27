/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer_native/native_project.c
 *
 * PURPOSE:
 *   Validate native controls and emit ordinary C23 without interpolating code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/native_project.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The document remains canonical. These are temporary validation indices, not
 * a second editable tree or serialisation format. */
typedef struct CheckedTree {
    UmiDeclDocumentSnapshot snapshot;
    UmiDeclNode *nodes;
    size_t parents[UMI_DESIGNER_NATIVE_NODE_LIMIT];
    size_t children[UMI_DESIGNER_NATIVE_NODE_LIMIT];
    size_t root;
} CheckedTree;

typedef struct TextBuffer { char *text; size_t length; size_t capacity; } TextBuffer;
struct UmiDesignerNativeProject {
    TextBuffer files[UMI_DESIGNER_NATIVE_FILE_COUNT];
    UmiDesignerNativeProjectSummary summary;
};
static const char *const FILE_NAMES[UMI_DESIGNER_NATIVE_FILE_COUNT] = {
    "application.h", "application.c", "main.c", "CMakeLists.txt", "README.md", "EXPORT-COMPLETE.txt"
};

static UmiStatus Explain(char *out, size_t capacity, UmiStatus status,
    const char *node, const char *detail)
{
    if (out != NULL && capacity != 0U)
        (void)snprintf(out, capacity, "%s%s%s", node != NULL ? node : "",
            node != NULL && node[0] != '\0' ? ": " : "", detail);
    return status;
}

/* Reject malformed UTF-8 before any toolkit consumes a caption. This accepts
 * scalar values (including tab/newline) but not overlong encodings, surrogates
 * or values above U+10FFFF. NUL is the existing C-string terminator. */
static int Utf8(const char *text)
{
    const unsigned char *p = (const unsigned char *)text;
    size_t length = strlen(text), i = 0U;
    while (i < length) {
        unsigned char c = p[i++];
        unsigned remaining;
        uint32_t value, minimum;
        if (c < 0x80U) continue;
        if (c >= 0xc2U && c <= 0xdfU) { remaining = 1U; value = c & 0x1fU; minimum = 0x80U; }
        else if (c >= 0xe0U && c <= 0xefU) { remaining = 2U; value = c & 0x0fU; minimum = 0x800U; }
        else if (c >= 0xf0U && c <= 0xf4U) { remaining = 3U; value = c & 0x07U; minimum = 0x10000U; }
        else return 0;
        if ((size_t)remaining > length - i) return 0;
        while (remaining-- != 0U) {
            c = p[i++];
            if ((c & 0xc0U) != 0x80U) return 0;
            value = (value << 6U) | (uint32_t)(c & 0x3fU);
        }
        if (value < minimum || value > 0x10ffffU || (value >= 0xd800U && value <= 0xdfffU)) return 0;
    }
    return 1;
}

static int Is(const UmiDeclNode *node, const char *type)
{ return strcmp(node->component_type, type) == 0; }

static const UmiDeclValue *Property(const UmiDeclNode *node, const char *name)
{
    for (size_t i = 0U; i < node->attribute_count; ++i)
        if (strcmp(node->attributes[i].name, name) == 0) return &node->attributes[i].value;
    return NULL;
}

static size_t Index(const CheckedTree *tree, const char *id)
{
    for (size_t i = 0U; i < tree->snapshot.node_count; ++i)
        if (strcmp(tree->nodes[i].node_id, id) == 0) return i;
    return SIZE_MAX;
}

static UmiStatus CheckProperty(const UmiDeclNode *node, const UmiDeclAttribute *attribute)
{
    const char *name = attribute->name;
    const UmiDeclValue *v = &attribute->value;
    UmiDeclValue parsed;
    int kind = 0;
    int64_t minimum = 0, maximum = 0;
    if (strcmp(name, "title") == 0 || strcmp(name, "tooltip") == 0) kind = UMI_DECL_VALUE_STRING;
    else if (strcmp(name, "visible") == 0 || strcmp(name, "enabled") == 0) kind = UMI_DECL_VALUE_BOOLEAN;
    else if (Is(node, "window") && (strcmp(name, "width") == 0 || strcmp(name, "height") == 0)) {
        kind = UMI_DECL_VALUE_INTEGER; minimum = 240; maximum = 8192;
    } else if ((Is(node, "pane") || Is(node, "split")) && strcmp(name, "orientation") == 0) {
        kind = UMI_DECL_VALUE_STRING;
        if (strcmp(v->text, "horizontal") != 0 && strcmp(v->text, "vertical") != 0)
            return UMI_STATUS_INVALID_ARGUMENT;
    } else if (Is(node, "pane") && strcmp(name, "spacing") == 0) {
        kind = UMI_DECL_VALUE_INTEGER; maximum = 64;
    } else if (Is(node, "split") && strcmp(name, "position") == 0) {
        kind = UMI_DECL_VALUE_INTEGER; maximum = 8192;
    } else if ((Is(node, "label") || Is(node, "text") || Is(node, "editor")) && strcmp(name, "text") == 0)
        kind = UMI_DECL_VALUE_STRING;
    else if (Is(node, "text") && strcmp(name, "placeholder") == 0) kind = UMI_DECL_VALUE_STRING;
    else if (Is(node, "editor") && (strcmp(name, "wrap") == 0 || strcmp(name, "read_only") == 0))
        kind = UMI_DECL_VALUE_BOOLEAN;
    else if (Is(node, "button") && (strcmp(name, "action") == 0 ||
             strcmp(name, "source") == 0 || strcmp(name, "target") == 0)) kind = UMI_DECL_VALUE_STRING;
    else return UMI_STATUS_NOT_IMPLEMENTED;
    if ((int)v->kind != kind || !Utf8(v->text)) return UMI_STATUS_INVALID_ARGUMENT;
    /* The public record carries both input text and typed values. Do not emit
     * a constructor which reparses contradictory fields into a different value. */
    if (umi_decl_value_from_text(v->kind, v->text, &parsed) != UMI_STATUS_OK ||
        !umi_decl_value_equal(v, &parsed)) return UMI_STATUS_INVALID_STATE;
    if (kind == UMI_DECL_VALUE_INTEGER && (v->integer_value < minimum || v->integer_value > maximum))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

static UmiStatus CheckTree(const UmiDeclDocument *document, CheckedTree *tree,
    char *explanation, size_t capacity)
{
    UmiStatus status;
    size_t roots = 0U, count;
    memset(tree, 0, sizeof *tree);
    if (explanation != NULL && capacity != 0U) explanation[0] = '\0';
    if (document == NULL) return Explain(explanation, capacity, UMI_STATUS_INVALID_ARGUMENT, NULL, "A document is required.");
    status = umi_decl_document_snapshot(document, &tree->snapshot);
    if (status != UMI_STATUS_OK) return status;
    count = tree->snapshot.node_count;
    if (count == 0U || count > UMI_DESIGNER_NATIVE_NODE_LIMIT)
        return Explain(explanation, capacity, UMI_STATUS_CAPACITY_EXCEEDED, NULL, "Use between 1 and 128 components.");
    if (!umi_decl_version_equal(tree->snapshot.version, umi_decl_version_current()))
        return Explain(explanation, capacity, UMI_STATUS_NOT_IMPLEMENTED, NULL, "This native profile supports template version 1.0.0.");
    tree->nodes = calloc(count, sizeof *tree->nodes);
    if (tree->nodes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < count; ++i) {
        UmiDeclNode *node = &tree->nodes[i];
        status = umi_decl_document_node_at(document, i, node);
        if (status != UMI_STATUS_OK) return status;
        if (UmiDeclNodeValidate(node) != UMI_STATUS_OK)
            return Explain(explanation, capacity, UMI_STATUS_INVALID_STATE, NULL, "The document contains a malformed node.");
        if (node->kind != UMI_DECL_NODE_COMPONENT ||
            !(Is(node,"window") || Is(node,"pane") || Is(node,"split") || Is(node,"tabs") ||
              Is(node,"label") || Is(node,"text") || Is(node,"editor") || Is(node,"button")))
            return Explain(explanation, capacity, UMI_STATUS_NOT_IMPLEMENTED, node->node_id, "This component is outside the native controls profile.");
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(node->node_id, tree->nodes[j].node_id) == 0)
                return Explain(explanation, capacity, UMI_STATUS_INVALID_STATE, node->node_id, "Duplicate component identity.");
        for (size_t p = 0U; p < node->attribute_count; ++p) {
            status = CheckProperty(node, &node->attributes[p]);
            if (status != UMI_STATUS_OK)
                return Explain(explanation, capacity, status, node->node_id, node->attributes[p].name);
        }
        const UmiDeclValue *visible = Property(node, "visible");
        if (Is(node, "window") && visible != NULL && !visible->boolean_value)
            return Explain(explanation, capacity, UMI_STATUS_INVALID_ARGUMENT, node->node_id, "A standalone root window must be visible.");
        if (node->parent_id[0] == '\0') { ++roots; tree->root = i; }
    }
    if (roots != 1U || !Is(&tree->nodes[tree->root], "window"))
        return Explain(explanation, capacity, UMI_STATUS_INVALID_STATE, NULL, "Use exactly one root window.");
    for (size_t i = 0U; i < count; ++i) {
        const UmiDeclNode *node = &tree->nodes[i];
        tree->parents[i] = node->parent_id[0] == '\0' ? SIZE_MAX : Index(tree, node->parent_id);
        if (i != tree->root && (tree->parents[i] == SIZE_MAX || Is(node, "window")))
            return Explain(explanation, capacity, UMI_STATUS_INVALID_STATE, node->node_id, "Missing parent or nested window.");
        if (tree->parents[i] != SIZE_MAX) ++tree->children[tree->parents[i]];
    }
    for (size_t i = 0U; i < count; ++i) {
        const UmiDeclNode *node = &tree->nodes[i];
        size_t children = tree->children[i], steps = 0U, p = i;
        while (p != SIZE_MAX) {
            if (++steps > count) return Explain(explanation, capacity, UMI_STATUS_INVALID_STATE, node->node_id, "A parent cycle is not a layout.");
            if (steps > UMI_DESIGNER_NATIVE_DEPTH_LIMIT)
                return Explain(explanation, capacity, UMI_STATUS_CAPACITY_EXCEEDED, node->node_id, "The native layout depth limit is 32.");
            p = tree->parents[p];
        }
        if ((Is(node,"window") && children != 1U) ||
            (Is(node,"split") && children != 2U) ||
            (Is(node,"tabs") && (children == 0U || children > 16U)) ||
            (!(Is(node,"window") || Is(node,"split") || Is(node,"pane") || Is(node,"tabs")) && children != 0U))
            return Explain(explanation, capacity, UMI_STATUS_INVALID_STATE, node->node_id, "Invalid child count for this component.");
        if (Is(node,"button")) {
            const UmiDeclValue *action=Property(node,"action"), *source=Property(node,"source"), *target=Property(node,"target");
            if (action == NULL && source == NULL && target == NULL) continue;
            if (action == NULL || source == NULL || target == NULL || strcmp(action->text,"text.count") != 0)
                return Explain(explanation, capacity, UMI_STATUS_NOT_IMPLEMENTED, node->node_id, "An action needs text.count, source and target.");
            size_t si=Index(tree,source->text), ti=Index(tree,target->text);
            if (si==SIZE_MAX || ti==SIZE_MAX || !(Is(&tree->nodes[si],"text") || Is(&tree->nodes[si],"editor")) || !Is(&tree->nodes[ti],"label"))
                return Explain(explanation, capacity, UMI_STATUS_INVALID_STATE, node->node_id, "The count action needs a text/editor source and label target.");
        }
    }
    return Explain(explanation, capacity, UMI_STATUS_OK, NULL, "Native controls are valid.");
}

UmiStatus UmiDesignerNativeValidate(const UmiDeclDocument *document, char *explanation, size_t capacity)
{
    CheckedTree tree;
    UmiStatus status = CheckTree(document, &tree, explanation, capacity);
    free(tree.nodes);
    return status;
}

static UmiStatus Append(TextBuffer *buffer, const char *text, size_t length)
{
    if (length > UMI_DESIGNER_NATIVE_SOURCE_LIMIT - buffer->length)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t required = buffer->length + length + 1U;
    if (required > buffer->capacity) {
        size_t capacity = buffer->capacity == 0U ? 1024U : buffer->capacity;
        while (capacity < required && capacity < UMI_DESIGNER_NATIVE_SOURCE_LIMIT + 1U) {
            size_t next = capacity * 2U;
            capacity = next > UMI_DESIGNER_NATIVE_SOURCE_LIMIT + 1U ? UMI_DESIGNER_NATIVE_SOURCE_LIMIT + 1U : next;
        }
        char *grown = realloc(buffer->text, capacity);
        if (grown == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        buffer->text = grown; buffer->capacity = capacity;
    }
    memcpy(buffer->text + buffer->length, text, length);
    buffer->length += length; buffer->text[buffer->length] = '\0';
    return UMI_STATUS_OK;
}
static UmiStatus Add(TextBuffer *b, const char *text) { return Append(b,text,strlen(text)); }
static UmiStatus Format(TextBuffer *b, const char *format, ...)
{
    char text[2048];
    va_list args;
    va_start(args,format); int count = vsnprintf(text,sizeof text,format,args); va_end(args);
    if (count < 0 || (size_t)count >= sizeof text) return UMI_STATUS_CAPACITY_EXCEEDED;
    return Append(b,text,(size_t)count);
}

/* Exact three-digit octal escapes cannot absorb a following hex digit, and
 * escaping '?' also avoids trigraph-sensitive consumers. Each model string
 * is at most 511 bytes, below C's minimum supported literal length. */
static UmiStatus Quoted(TextBuffer *b, const char *text)
{
    UmiStatus status=Add(b,"\"");
    for (const unsigned char *p=(const unsigned char *)text; *p!='\0' && status==UMI_STATUS_OK; ++p) {
        if (*p>=32U && *p<=126U && *p!='"' && *p!='\\' && *p!='?') status=Append(b,(const char *)p,1U);
        else { char escape[5]; (void)snprintf(escape,sizeof escape,"\\%03o",(unsigned)*p); status=Add(b,escape); }
    }
    return status==UMI_STATUS_OK ? Add(b,"\"") : status;
}
static int ProjectName(const char *name)
{
    if (name == NULL || name[0] < 'a' || name[0] > 'z') return 0;
    for (size_t i=0U; name[i]!='\0'; ++i)
        if (i>=63U || !((name[i]>='a' && name[i]<='z') || (name[i]>='0' && name[i]<='9') || name[i]=='_')) return 0;
    return 1;
}
#define TRY(call) do { status=(call); if(status!=UMI_STATUS_OK) goto finish; } while(0)

UmiStatus UmiDesignerNativeProjectCreate(const UmiDeclDocument *document,
    const char *projectName, UmiDesignerNativeProject **outProject,
    char *explanation, size_t capacity)
{
    CheckedTree tree={0};
    UmiDesignerNativeProject *project=NULL;
    UmiStatus status;
    if (outProject == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outProject=NULL;
    if (!ProjectName(projectName)) return Explain(explanation,capacity,UMI_STATUS_INVALID_ARGUMENT,NULL,"Use a lower-case target name, up to 63 letters, digits or underscores.");
    status=CheckTree(document,&tree,explanation,capacity);
    if (status!=UMI_STATUS_OK) goto finish;
    project=calloc(1U,sizeof *project);
    if (project==NULL) { status=UMI_STATUS_OUT_OF_MEMORY; goto finish; }
    project->summary.sourceRevision=tree.snapshot.revision;
    project->summary.nodeCount=tree.snapshot.node_count;
    TextBuffer *c=&project->files[1];
    TRY(Add(&project->files[0],
        "/* Umicom native project | Sammy Hegab, Umicom Foundation | MIT */\n"
        "#ifndef UMICOM_GENERATED_APPLICATION_H\n#define UMICOM_GENERATED_APPLICATION_H\n"
        "#include <umicom/declarative/document.h>\n"
        "UmiStatus UmiGeneratedApplicationCreate(UmiDeclDocument **outDocument);\n#endif\n"));
    TRY(Add(c,"/* Generated model construction. Review before building.\n"
        " * Reusable behaviour remains in Umicom Framework.\n"
        " * Sammy Hegab, Umicom Foundation | MIT */\n#include \"application.h\"\n\n"
        "UmiStatus UmiGeneratedApplicationCreate(UmiDeclDocument **outDocument)\n{\n"
        "    UmiDeclDocument *document = NULL;\n    UmiDeclNode node;\n    UmiStatus status;\n"
        "    if (outDocument == NULL) return UMI_STATUS_INVALID_ARGUMENT;\n    *outDocument = NULL;\n"
        "    status = umi_decl_document_create("));
    TRY(Quoted(c,tree.snapshot.application_id));
    TRY(Add(c,", &document);\n    if (status != UMI_STATUS_OK) return status;\n"));
    TRY(Format(c,"    status = umi_decl_document_set_version(document, (UmiDeclVersion){%uU,%uU,%uU});\n    if (status != UMI_STATUS_OK) goto failure;\n",
        (unsigned)tree.snapshot.version.major,(unsigned)tree.snapshot.version.minor,(unsigned)tree.snapshot.version.patch));
    for(size_t i=0U;i<tree.snapshot.node_count;++i) {
        const UmiDeclNode *node=&tree.nodes[i];
        TRY(Add(c,"    status = umi_decl_node_init(&node, ")); TRY(Quoted(c,node->node_id));
        TRY(Add(c,", ")); TRY(Quoted(c,node->component_type)); TRY(Add(c,", ")); TRY(Quoted(c,node->parent_id));
        TRY(Add(c,");\n    if (status != UMI_STATUS_OK) goto failure;\n"));
        for(size_t p=0U;p<node->attribute_count;++p) {
            const UmiDeclAttribute *a=&node->attributes[p];
            TRY(Add(c,"    status = umi_decl_node_set_attribute(&node, ")); TRY(Quoted(c,a->name));
            TRY(Format(c,", (UmiDeclValueKind)%u, ",(unsigned)a->value.kind)); TRY(Quoted(c,a->value.text));
            TRY(Add(c,");\n    if (status != UMI_STATUS_OK) goto failure;\n"));
        }
        TRY(Add(c,"    status = umi_decl_document_add_node(document, &node);\n    if (status != UMI_STATUS_OK) goto failure;\n"));
    }
    TRY(Add(c,"    *outDocument = document;\n    return UMI_STATUS_OK;\nfailure:\n    umi_decl_document_destroy(document);\n    return status;\n}\n"));
    TRY(Add(&project->files[2],
        "/* Thin application entry point | Sammy Hegab, Umicom Foundation | MIT */\n"
        "#include \"application.h\"\n#include <umicom/designer/native_project.h>\n"
        "#include <stdio.h>\n#include <string.h>\n"
        "#ifdef UMICOM_NATIVE_PROJECT_GUI\n#include <umicom/designer/native_gtk4.h>\n#endif\n"
        "int main(int argc, char **argv)\n{\n    UmiDeclDocument *document = NULL;\n    char message[256];\n"
        "    if (argc > 2 || (argc == 2 && strcmp(argv[1], \"--check\") != 0)) {\n"
        "        fputs(\"Use --check for a headless model check, or no arguments for the GUI.\\n\", stderr); return 2;\n    }\n"
        "    UmiStatus status = UmiGeneratedApplicationCreate(&document);\n"
        "    if (status == UMI_STATUS_OK) status = UmiDesignerNativeValidate(document, message, sizeof message);\n"
        "    if (status == UMI_STATUS_OK && argc == 2) {\n"
        "        printf(\"Model valid: %zu components. No window or file was opened.\\n\", umi_decl_document_node_count(document));\n"
        "    } else if (status == UMI_STATUS_OK) {\n#ifdef UMICOM_NATIVE_PROJECT_GUI\n"
        "        status = UmiDesignerNativeGtkRun(document);\n#else\n"
        "        fputs(\"This build has no GTK frontend. Build with UMICOM_NATIVE_PROJECT_GUI=ON.\\n\", stderr);\n"
        "        status = UMI_STATUS_UNAVAILABLE;\n#endif\n    }\n"
        "    if (status != UMI_STATUS_OK) fprintf(stderr, \"Application did not complete: %s\\n\", umi_status_text(status));\n"
        "    umi_decl_document_destroy(document);\n    return status == UMI_STATUS_OK ? 0 : 1;\n}\n"));
    TextBuffer *cm=&project->files[3];
    TRY(Add(cm,"# Umicom native project | Sammy Hegab, Umicom Foundation | MIT\ncmake_minimum_required(VERSION 3.24)\n"));
    TRY(Format(cm,"project(%s LANGUAGES C)\n",projectName));
    TRY(Add(cm,"include(CTest)\noption(UMICOM_NATIVE_PROJECT_GUI \"Build the GTK frontend\" ON)\n"
        "if(NOT TARGET Umicom::designer_native)\n"
        "    find_package(UmicomDesignerNative CONFIG QUIET)\n"
        "    if(NOT UmicomDesignerNative_FOUND)\n        find_package(UmicomFramework CONFIG REQUIRED)\n    endif()\nendif()\n"));
    TRY(Format(cm,"add_executable(%s main.c application.c)\nset_target_properties(%s PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)\n"
        "target_link_libraries(%s PRIVATE Umicom::designer_native)\n",projectName,projectName,projectName));
    TRY(Add(cm,"if(UMICOM_NATIVE_PROJECT_GUI)\n    if(NOT TARGET Umicom::designer_native_gtk4)\n"
        "        message(FATAL_ERROR \"The selected Framework SDK has no native GTK adapter. Select a GTK-enabled SDK or set UMICOM_NATIVE_PROJECT_GUI=OFF.\")\n    endif()\n"));
    TRY(Format(cm,"    target_link_libraries(%s PRIVATE Umicom::designer_native_gtk4)\n    target_compile_definitions(%s PRIVATE UMICOM_NATIVE_PROJECT_GUI=1)\nendif()\n"
        "if(BUILD_TESTING)\n    add_test(NAME application.model COMMAND %s --check)\nendif()\n",projectName,projectName,projectName));
    TRY(Add(&project->files[4],"# Native Umicom application\n\n"
        "The model is reconstructed by application.c through the canonical Framework document API.\n"
        "The window, layouts, controls and text.count action are provided by Framework.\n"
        "No document is loaded relative to the current working directory.\n\n"
        "Review every file before building. Supply the installed Framework SDK via CMAKE_PREFIX_PATH.\n"
        "Configure with UMICOM_NATIVE_PROJECT_GUI=OFF for a headless model check.\n"
        "With the GTK adapter installed, leave that option ON and run the executable without arguments.\n"
        "The --check switch validates the model only; it is not a graphical acceptance test.\n\n"
        "The Notes example keeps draft text in memory only. Closing the window discards edits.\n"
        "There is no file-save action, external command, database, shell or network binding.\n"
        "A button without an action is disabled. Only the explicit text.count action is supported.\n\n"
        "Do not export into this directory again. Generate into a new directory and review a merge;\n"
        "handwritten changes are never automatically replaced. This is source, not an installer.\n"
        "Windows runtime DLLs still need normal development PATH or a qualified release package.\n"));
    TextBuffer *record=&project->files[5];
    TRY(Format(record,"Umicom native source export\nproject=%s\nsource_revision=%llu\nnodes=%zu\n",
        projectName,(unsigned long long)tree.snapshot.revision,tree.snapshot.node_count));
    for(size_t i=0U;i<5U;++i) TRY(Format(record,"file=%s bytes=%zu\n",FILE_NAMES[i],project->files[i].length));
    TRY(Add(record,"Source preparation completed. No build, test, installation or guest launch is implied.\n"));
    project->summary.fileCount=UMI_DESIGNER_NATIVE_FILE_COUNT;
    for(size_t i=0U;i<UMI_DESIGNER_NATIVE_FILE_COUNT;++i) project->summary.totalBytes+=project->files[i].length;
    *outProject=project; project=NULL;
    status=Explain(explanation,capacity,UMI_STATUS_OK,NULL,"Native source plan prepared. No files written.");
finish:
    free(tree.nodes);
    UmiDesignerNativeProjectDestroy(project);
    return status;
}
#undef TRY

void UmiDesignerNativeProjectDestroy(UmiDesignerNativeProject *project)
{
    if(project==NULL) return;
    for(size_t i=0U;i<UMI_DESIGNER_NATIVE_FILE_COUNT;++i) free(project->files[i].text);
    free(project);
}
UmiStatus UmiDesignerNativeProjectGetSummary(const UmiDesignerNativeProject *project,
    UmiDesignerNativeProjectSummary *outSummary)
{
    if(project==NULL || outSummary==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSummary=project->summary; return UMI_STATUS_OK;
}
UmiStatus UmiDesignerNativeProjectFile(const UmiDesignerNativeProject *project,
    size_t index,UmiDesignerNativeFileView *outFile)
{
    if(outFile==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outFile=(UmiDesignerNativeFileView){0};
    if(project==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=UMI_DESIGNER_NATIVE_FILE_COUNT) return UMI_STATUS_NOT_FOUND;
    *outFile=(UmiDesignerNativeFileView){FILE_NAMES[index],project->files[index].text,project->files[index].length};
    return UMI_STATUS_OK;
}

/* The example is an ordinary canonical document, not an alternate GUI model. */
UmiStatus UmiDesignerNativeNotesDocument(UmiDeclDocument **outDocument)
{
    UmiDeclDocument *doc=NULL;
    UmiDeclNode node;
    UmiStatus status;
    const char *const ids[]={"window","content","heading","title","notes","count","result"};
    const char *const types[]={"window","pane","label","text","editor","button","label"};
    if(outDocument==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDocument=NULL;
    status=umi_decl_document_create("umicom.notes.practice",&doc);
    if(status!=UMI_STATUS_OK) return status;
#define STEP(call) do { status=(call); if(status!=UMI_STATUS_OK) goto failure; } while(0)
    for(size_t i=0U;i<7U;++i) {
        STEP(umi_decl_node_init(&node,ids[i],types[i],i==0U ? "" : i==1U ? "window" : "content"));
        if(i==0U) {
            STEP(umi_decl_node_set_attribute(&node,"title",UMI_DECL_VALUE_STRING,"Umicom Notes - practice workspace"));
            STEP(umi_decl_node_set_attribute(&node,"width",UMI_DECL_VALUE_INTEGER,"800"));
            STEP(umi_decl_node_set_attribute(&node,"height",UMI_DECL_VALUE_INTEGER,"600"));
        } else if(i==1U) STEP(umi_decl_node_set_attribute(&node,"spacing",UMI_DECL_VALUE_INTEGER,"12"));
        else if(i==2U) STEP(umi_decl_node_set_attribute(&node,"text",UMI_DECL_VALUE_STRING,"Write a draft, then count its characters. Closing discards the draft."));
        else if(i==3U) {
            STEP(umi_decl_node_set_attribute(&node,"text",UMI_DECL_VALUE_STRING,"Workshop plan"));
            STEP(umi_decl_node_set_attribute(&node,"placeholder",UMI_DECL_VALUE_STRING,"Note title"));
        } else if(i==4U) {
            STEP(umi_decl_node_set_attribute(&node,"text",UMI_DECL_VALUE_STRING,"Prepare the Notes exercise."));
            STEP(umi_decl_node_set_attribute(&node,"wrap",UMI_DECL_VALUE_BOOLEAN,"true"));
        } else if(i==5U) {
            STEP(umi_decl_node_set_attribute(&node,"title",UMI_DECL_VALUE_STRING,"Count characters"));
            STEP(umi_decl_node_set_attribute(&node,"action",UMI_DECL_VALUE_STRING,"text.count"));
            STEP(umi_decl_node_set_attribute(&node,"source",UMI_DECL_VALUE_STRING,"notes"));
            STEP(umi_decl_node_set_attribute(&node,"target",UMI_DECL_VALUE_STRING,"result"));
        } else STEP(umi_decl_node_set_attribute(&node,"text",UMI_DECL_VALUE_STRING,"Choose Count characters after editing the draft."));
        STEP(umi_decl_document_add_node(doc,&node));
    }
    *outDocument=doc; return UMI_STATUS_OK;
failure:
    umi_decl_document_destroy(doc); return status;
#undef STEP
}
