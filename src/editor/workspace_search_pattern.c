/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workspace_search_pattern.c
 *
 * PURPOSE:
 *   Implement a bounded, portable regular-expression and literal-pattern
 *   engine for Framework workspace search. The implementation compiles an
 *   expression to a Thompson-style non-deterministic finite automaton (NFA),
 *   avoiding the catastrophic recursive backtracking behaviour associated
 *   with many traditional regex engines.
 *
 * SUPPORTED REGEX CONSTRUCTS:
 *   Literals, escaped literals, ., ^, $, \b, \B, \d, \D, \w, \W, \s, \S,
 *   character classes and ranges, capturing groups, non-capturing groups,
 *   alternation, ?, *, + and bounded repetitions {m}, {m,}, {m,n}.
 *
 * DELIBERATE LIMITS:
 *   Back-references and look-around assertions are not accepted. They require
 *   non-regular matching semantics and would weaken the deterministic resource
 *   limits required by a reusable IDE, server and remote-worker foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workspace_search_pattern.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_PROGRAM_CAPACITY 2048U
#define DEFAULT_EXECUTION_STEPS 5000000ULL
#define MAX_BOUNDED_REPEAT 1024U
#define CLASS_BITMAP_BYTES 32U
#define CAPTURE_SLOT_COUNT (UMI_EDITOR_WORKSPACE_SEARCH_MAX_CAPTURES * 2U)
#define CAPTURE_UNSET UINT64_MAX

typedef enum RegexNodeType {
    REGEX_NODE_EMPTY = 1,
    REGEX_NODE_LITERAL = 2,
    REGEX_NODE_ANY = 3,
    REGEX_NODE_CLASS = 4,
    REGEX_NODE_BEGIN = 5,
    REGEX_NODE_END = 6,
    REGEX_NODE_WORD_BOUNDARY = 7,
    REGEX_NODE_SEQUENCE = 8,
    REGEX_NODE_ALTERNATION = 9,
    REGEX_NODE_REPEAT = 10,
    REGEX_NODE_GROUP = 11
} RegexNodeType;

typedef struct RegexClass {
    unsigned char bitmap[CLASS_BITMAP_BYTES];
    int negated;
} RegexClass;

typedef struct RegexNode RegexNode;

struct RegexNode {
    RegexNodeType type;
    union {
        unsigned char literal;
        RegexClass character_class;
        struct {
            RegexNode **items;
            size_t count;
            size_t capacity;
        } list;
        struct {
            RegexNode *child;
            uint32_t minimum;
            uint32_t maximum;
            int greedy;
        } repeat;
        struct {
            RegexNode *child;
            uint32_t group_index;
        } group;
        struct {
            int negated;
        } boundary;
    } data;
};

typedef struct RegexParser {
    const char *expression;
    size_t length;
    size_t position;
    size_t capture_count;
    UmiEditorWorkspaceSearchPatternDiagnostic *diagnostic;
    UmiStatus status;
} RegexParser;

typedef enum RegexInstructionType {
    REGEX_INSTRUCTION_CHARACTER = 1,
    REGEX_INSTRUCTION_ANY = 2,
    REGEX_INSTRUCTION_CLASS = 3,
    REGEX_INSTRUCTION_BEGIN = 4,
    REGEX_INSTRUCTION_END = 5,
    REGEX_INSTRUCTION_WORD_BOUNDARY = 6,
    REGEX_INSTRUCTION_SPLIT = 7,
    REGEX_INSTRUCTION_JUMP = 8,
    REGEX_INSTRUCTION_SAVE = 9,
    REGEX_INSTRUCTION_MATCH = 10
} RegexInstructionType;

typedef struct RegexInstruction {
    RegexInstructionType type;
    size_t first;
    size_t second;
    uint32_t capture_slot;
    unsigned char character;
    RegexClass character_class;
    int negated;
} RegexInstruction;

typedef struct RegexProgram {
    RegexInstruction *instructions;
    size_t count;
    size_t capacity;
    size_t maximum;
} RegexProgram;

typedef struct RegexThread {
    size_t program_counter;
    uint64_t captures[CAPTURE_SLOT_COUNT];
} RegexThread;

typedef struct RegexThreadList {
    RegexThread *items;
    size_t count;
    size_t capacity;
} RegexThreadList;

typedef struct RegexExecution {
    const UmiEditorWorkspaceSearchPattern *pattern;
    const char *text;
    size_t text_length;
    uint64_t steps;
    uint64_t maximum_steps;
    uint32_t *visited_generation;
    uint32_t generation;
    UmiStatus status;
} RegexExecution;

struct UmiEditorWorkspaceSearchPattern {
    RegexInstruction *instructions;
    size_t instruction_count;
    size_t instruction_capacity;
    size_t capture_count;
    uint64_t revision;
    uint64_t maximum_execution_steps;
    UmiEditorWorkspaceSearchPatternMode mode;
    UmiEditorWorkspaceSearchCaseMode case_mode;
    int effective_case_sensitive;
    int whole_word;
    int multiline;
    int dot_matches_newline;
    int compiled;
    char expression[UMI_EDITOR_WORKSPACE_SEARCH_EXPRESSION_CAPACITY];
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static void copy_text(char *destination,
                      size_t capacity,
                      const char *source)
{
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) {
        destination[0] = '\0';
        return;
    }
    (void)snprintf(destination, capacity, "%s", source);
}

static void diagnostic_clear(
    UmiEditorWorkspaceSearchPatternDiagnostic *diagnostic)
{
    if (diagnostic == NULL) return;
    (void)memset(diagnostic, 0, sizeof(*diagnostic));
    diagnostic->struct_size = (uint32_t)sizeof(*diagnostic);
    diagnostic->api_version =
        UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_API_VERSION;
    diagnostic->status = UMI_STATUS_OK;
}

static void diagnostic_set(
    RegexParser *parser,
    UmiStatus status,
    size_t position,
    const char *message)
{
    if (parser->status != UMI_STATUS_OK) return;
    parser->status = status;
    if (parser->diagnostic != NULL) {
        parser->diagnostic->status = status;
        parser->diagnostic->expression_offset = position;
        copy_text(parser->diagnostic->message,
                  sizeof(parser->diagnostic->message),
                  message);
    }
}

static RegexNode *node_create(RegexNodeType type)
{
    RegexNode *node = (RegexNode *)calloc(1U, sizeof(*node));
    if (node != NULL) node->type = type;
    return node;
}

static void node_destroy(RegexNode *node)
{
    size_t index;
    if (node == NULL) return;
    if (node->type == REGEX_NODE_SEQUENCE ||
        node->type == REGEX_NODE_ALTERNATION) {
        for (index = 0U; index < node->data.list.count; ++index) {
            node_destroy(node->data.list.items[index]);
        }
        free(node->data.list.items);
        node->data.list.items = NULL;
    } else if (node->type == REGEX_NODE_REPEAT) {
        node_destroy(node->data.repeat.child);
        node->data.repeat.child = NULL;
    } else if (node->type == REGEX_NODE_GROUP) {
        node_destroy(node->data.group.child);
        node->data.group.child = NULL;
    }
    free(node);
}

static UmiStatus node_list_append(RegexNode *node, RegexNode *child)
{
    RegexNode **replacement;
    size_t capacity;
    if (node == NULL || child == NULL ||
        (node->type != REGEX_NODE_SEQUENCE &&
         node->type != REGEX_NODE_ALTERNATION)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (node->data.list.count < node->data.list.capacity) {
        node->data.list.items[node->data.list.count++] = child;
        return UMI_STATUS_OK;
    }
    capacity = node->data.list.capacity == 0U
        ? 4U : node->data.list.capacity * 2U;
    if (capacity < node->data.list.capacity ||
        capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (RegexNode **)realloc(
        node->data.list.items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    node->data.list.items = replacement;
    node->data.list.capacity = capacity;
    node->data.list.items[node->data.list.count++] = child;
    return UMI_STATUS_OK;
}

static int parser_at_end(const RegexParser *parser)
{
    return parser->position >= parser->length;
}

static char parser_peek(const RegexParser *parser)
{
    return parser_at_end(parser) ? '\0' : parser->expression[parser->position];
}

static char parser_take(RegexParser *parser)
{
    return parser_at_end(parser) ? '\0'
                                 : parser->expression[parser->position++];
}

static void class_set(RegexClass *character_class, unsigned char value)
{
    character_class->bitmap[value >> 3U] |=
        (unsigned char)(1U << (value & 7U));
}

static int class_has(const RegexClass *character_class, unsigned char value)
{
    return (character_class->bitmap[value >> 3U] &
            (unsigned char)(1U << (value & 7U))) != 0U;
}

static void class_add_range(RegexClass *character_class,
                            unsigned char first,
                            unsigned char last)
{
    unsigned int value;
    if (first > last) {
        unsigned char temporary = first;
        first = last;
        last = temporary;
    }
    for (value = first; value <= (unsigned int)last; ++value) {
        class_set(character_class, (unsigned char)value);
    }
}

static void class_add_digits(RegexClass *character_class)
{
    class_add_range(character_class, (unsigned char)'0', (unsigned char)'9');
}

static void class_add_word(RegexClass *character_class)
{
    class_add_range(character_class, (unsigned char)'0', (unsigned char)'9');
    class_add_range(character_class, (unsigned char)'A', (unsigned char)'Z');
    class_add_range(character_class, (unsigned char)'a', (unsigned char)'z');
    class_set(character_class, (unsigned char)'_');
}

static void class_add_space(RegexClass *character_class)
{
    class_set(character_class, (unsigned char)' ');
    class_set(character_class, (unsigned char)'\t');
    class_set(character_class, (unsigned char)'\n');
    class_set(character_class, (unsigned char)'\r');
    class_set(character_class, (unsigned char)'\f');
    class_set(character_class, (unsigned char)'\v');
}

static int hex_value(char character)
{
    if (character >= '0' && character <= '9') return character - '0';
    if (character >= 'a' && character <= 'f') {
        return character - 'a' + 10;
    }
    if (character >= 'A' && character <= 'F') {
        return character - 'A' + 10;
    }
    return -1;
}

static UmiStatus parser_escape_character(RegexParser *parser,
                                         unsigned char *out_character)
{
    char escaped;
    int high;
    int low;
    if (parser_at_end(parser) || out_character == NULL) {
        diagnostic_set(parser,
                       UMI_STATUS_PARSE_ERROR,
                       parser->position,
                       "Trailing escape sequence");
        return parser->status;
    }
    escaped = parser_take(parser);
    switch (escaped) {
        case 'n': *out_character = (unsigned char)'\n'; return UMI_STATUS_OK;
        case 'r': *out_character = (unsigned char)'\r'; return UMI_STATUS_OK;
        case 't': *out_character = (unsigned char)'\t'; return UMI_STATUS_OK;
        case 'f': *out_character = (unsigned char)'\f'; return UMI_STATUS_OK;
        case 'v': *out_character = (unsigned char)'\v'; return UMI_STATUS_OK;
        case 'x':
            if (parser->position + 2U > parser->length) {
                diagnostic_set(parser,
                               UMI_STATUS_PARSE_ERROR,
                               parser->position - 1U,
                               "Hex escape requires exactly two digits");
                return parser->status;
            }
            high = hex_value(parser_take(parser));
            low = hex_value(parser_take(parser));
            if (high < 0 || low < 0) {
                diagnostic_set(parser,
                               UMI_STATUS_PARSE_ERROR,
                               parser->position - 2U,
                               "Invalid hexadecimal escape");
                return parser->status;
            }
            *out_character = (unsigned char)((high << 4) | low);
            return UMI_STATUS_OK;
        default:
            *out_character = (unsigned char)escaped;
            return UMI_STATUS_OK;
    }
}

static RegexNode *parse_alternation(RegexParser *parser);

static RegexNode *parse_character_class(RegexParser *parser)
{
    RegexNode *node;
    RegexClass character_class;
    int have_value = 0;
    int first_position = 1;
    unsigned char previous = 0U;

    (void)memset(&character_class, 0, sizeof(character_class));
    if (parser_peek(parser) == '^') {
        character_class.negated = 1;
        (void)parser_take(parser);
    }
    while (!parser_at_end(parser)) {
        char current = parser_take(parser);
        unsigned char value;
        if (current == ']' && !first_position) {
            node = node_create(REGEX_NODE_CLASS);
            if (node == NULL) {
                diagnostic_set(parser,
                               UMI_STATUS_OUT_OF_MEMORY,
                               parser->position,
                               "Unable to allocate character class");
                return NULL;
            }
            node->data.character_class = character_class;
            return node;
        }
        first_position = 0;
        if (current == '\\') {
            char category = parser_peek(parser);
            if (category == 'd' || category == 'D' ||
                category == 'w' || category == 'W' ||
                category == 's' || category == 'S') {
                RegexClass category_class;
                size_t index;
                int category_negated;
                (void)memset(&category_class, 0, sizeof(category_class));
                category = parser_take(parser);
                category_negated = isupper((unsigned char)category) != 0;
                switch ((char)tolower((unsigned char)category)) {
                    case 'd': class_add_digits(&category_class); break;
                    case 'w': class_add_word(&category_class); break;
                    default: class_add_space(&category_class); break;
                }
                for (index = 0U; index < 256U; ++index) {
                    int present = class_has(&category_class,
                                            (unsigned char)index);
                    if (category_negated) present = !present;
                    if (present) class_set(&character_class,
                                           (unsigned char)index);
                }
                have_value = 0;
                continue;
            }
            if (parser_escape_character(parser, &value) != UMI_STATUS_OK) {
                return NULL;
            }
        } else {
            value = (unsigned char)current;
        }
        if (value == (unsigned char)'-' && have_value &&
            parser_peek(parser) != ']' && !parser_at_end(parser)) {
            unsigned char range_end;
            char end_character = parser_take(parser);
            if (end_character == '\\') {
                if (parser_escape_character(parser, &range_end) !=
                    UMI_STATUS_OK) {
                    return NULL;
                }
            } else {
                range_end = (unsigned char)end_character;
            }
            class_add_range(&character_class, previous, range_end);
            have_value = 0;
            continue;
        }
        class_set(&character_class, value);
        previous = value;
        have_value = 1;
    }
    diagnostic_set(parser,
                   UMI_STATUS_PARSE_ERROR,
                   parser->position,
                   "Unterminated character class");
    return NULL;
}

static RegexNode *parse_escape_atom(RegexParser *parser)
{
    char category;
    RegexNode *node;
    unsigned char value;
    if (parser_at_end(parser)) {
        diagnostic_set(parser,
                       UMI_STATUS_PARSE_ERROR,
                       parser->position,
                       "Trailing escape sequence");
        return NULL;
    }
    category = parser_peek(parser);
    if (category == 'd' || category == 'D' ||
        category == 'w' || category == 'W' ||
        category == 's' || category == 'S') {
        category = parser_take(parser);
        node = node_create(REGEX_NODE_CLASS);
        if (node == NULL) {
            diagnostic_set(parser,
                           UMI_STATUS_OUT_OF_MEMORY,
                           parser->position,
                           "Unable to allocate character category");
            return NULL;
        }
        node->data.character_class.negated =
            isupper((unsigned char)category) != 0;
        switch ((char)tolower((unsigned char)category)) {
            case 'd': class_add_digits(&node->data.character_class); break;
            case 'w': class_add_word(&node->data.character_class); break;
            default: class_add_space(&node->data.character_class); break;
        }
        return node;
    }
    if (category == 'b' || category == 'B') {
        category = parser_take(parser);
        node = node_create(REGEX_NODE_WORD_BOUNDARY);
        if (node == NULL) {
            diagnostic_set(parser,
                           UMI_STATUS_OUT_OF_MEMORY,
                           parser->position,
                           "Unable to allocate word boundary");
            return NULL;
        }
        node->data.boundary.negated = category == 'B';
        return node;
    }
    if (parser_escape_character(parser, &value) != UMI_STATUS_OK) return NULL;
    node = node_create(REGEX_NODE_LITERAL);
    if (node == NULL) {
        diagnostic_set(parser,
                       UMI_STATUS_OUT_OF_MEMORY,
                       parser->position,
                       "Unable to allocate escaped literal");
        return NULL;
    }
    node->data.literal = value;
    return node;
}

static RegexNode *parse_atom(RegexParser *parser)
{
    char current;
    RegexNode *node;
    RegexNode *child;
    uint32_t group_index;
    int capturing = 1;

    if (parser_at_end(parser)) return node_create(REGEX_NODE_EMPTY);
    current = parser_take(parser);
    switch (current) {
        case '(':
            if (parser_peek(parser) == '?') {
                size_t marker = parser->position;
                (void)parser_take(parser);
                if (parser_peek(parser) != ':') {
                    diagnostic_set(parser,
                                   UMI_STATUS_PARSE_ERROR,
                                   marker,
                                   "Only non-capturing (?:...) groups are supported after (?"
                    );
                    return NULL;
                }
                (void)parser_take(parser);
                capturing = 0;
            }
            group_index = 0U;
            if (capturing) {
                if (parser->capture_count + 1U >=
                    UMI_EDITOR_WORKSPACE_SEARCH_MAX_CAPTURES) {
                    diagnostic_set(parser,
                                   UMI_STATUS_CAPACITY_EXCEEDED,
                                   parser->position - 1U,
                                   "Too many capturing groups");
                    return NULL;
                }
                group_index = (uint32_t)(++parser->capture_count);
            }
            child = parse_alternation(parser);
            if (child == NULL) return NULL;
            if (parser_peek(parser) != ')') {
                node_destroy(child);
                diagnostic_set(parser,
                               UMI_STATUS_PARSE_ERROR,
                               parser->position,
                               "Unterminated group");
                return NULL;
            }
            (void)parser_take(parser);
            if (!capturing) return child;
            node = node_create(REGEX_NODE_GROUP);
            if (node == NULL) {
                node_destroy(child);
                diagnostic_set(parser,
                               UMI_STATUS_OUT_OF_MEMORY,
                               parser->position,
                               "Unable to allocate capturing group");
                return NULL;
            }
            node->data.group.child = child;
            node->data.group.group_index = group_index;
            return node;
        case '[':
            return parse_character_class(parser);
        case '.':
            return node_create(REGEX_NODE_ANY);
        case '^':
            return node_create(REGEX_NODE_BEGIN);
        case '$':
            return node_create(REGEX_NODE_END);
        case '\\':
            return parse_escape_atom(parser);
        case ')':
        case '|':
            --parser->position;
            return node_create(REGEX_NODE_EMPTY);
        case '*':
        case '+':
        case '?':
        case '{':
            diagnostic_set(parser,
                           UMI_STATUS_PARSE_ERROR,
                           parser->position - 1U,
                           "Quantifier has no preceding atom");
            return NULL;
        default:
            node = node_create(REGEX_NODE_LITERAL);
            if (node == NULL) {
                diagnostic_set(parser,
                               UMI_STATUS_OUT_OF_MEMORY,
                               parser->position,
                               "Unable to allocate literal");
                return NULL;
            }
            node->data.literal = (unsigned char)current;
            return node;
    }
}

static UmiStatus parse_unsigned(RegexParser *parser, uint32_t *out_value)
{
    uint64_t value = 0U;
    size_t start = parser->position;
    if (out_value == NULL || parser_at_end(parser) ||
        !isdigit((unsigned char)parser_peek(parser))) {
        diagnostic_set(parser,
                       UMI_STATUS_PARSE_ERROR,
                       parser->position,
                       "Expected repetition count");
        return parser->status;
    }
    while (!parser_at_end(parser) &&
           isdigit((unsigned char)parser_peek(parser))) {
        unsigned int digit = (unsigned int)(parser_take(parser) - '0');
        value = value * 10U + digit;
        if (value > MAX_BOUNDED_REPEAT) {
            diagnostic_set(parser,
                           UMI_STATUS_CAPACITY_EXCEEDED,
                           start,
                           "Bounded repetition exceeds the Framework limit");
            return parser->status;
        }
    }
    *out_value = (uint32_t)value;
    return UMI_STATUS_OK;
}

static RegexNode *parse_repetition(RegexParser *parser)
{
    RegexNode *atom = parse_atom(parser);
    RegexNode *repeat;
    uint32_t minimum;
    uint32_t maximum;
    int greedy = 1;
    char quantifier;

    if (atom == NULL || parser->status != UMI_STATUS_OK) return atom;
    if (parser_at_end(parser)) return atom;
    quantifier = parser_peek(parser);
    if (quantifier != '*' && quantifier != '+' &&
        quantifier != '?' && quantifier != '{') {
        return atom;
    }
    (void)parser_take(parser);
    switch (quantifier) {
        case '*':
            minimum = 0U;
            maximum = UMI_EDITOR_WORKSPACE_SEARCH_UNBOUNDED_REPEAT;
            break;
        case '+':
            minimum = 1U;
            maximum = UMI_EDITOR_WORKSPACE_SEARCH_UNBOUNDED_REPEAT;
            break;
        case '?':
            minimum = 0U;
            maximum = 1U;
            break;
        default:
            if (parse_unsigned(parser, &minimum) != UMI_STATUS_OK) {
                node_destroy(atom);
                return NULL;
            }
            maximum = minimum;
            if (parser_peek(parser) == ',') {
                (void)parser_take(parser);
                if (parser_peek(parser) == '}') {
                    maximum = UMI_EDITOR_WORKSPACE_SEARCH_UNBOUNDED_REPEAT;
                } else if (parse_unsigned(parser, &maximum) != UMI_STATUS_OK) {
                    node_destroy(atom);
                    return NULL;
                }
            }
            if (parser_peek(parser) != '}') {
                node_destroy(atom);
                diagnostic_set(parser,
                               UMI_STATUS_PARSE_ERROR,
                               parser->position,
                               "Unterminated bounded repetition");
                return NULL;
            }
            (void)parser_take(parser);
            if (maximum != UMI_EDITOR_WORKSPACE_SEARCH_UNBOUNDED_REPEAT &&
                minimum > maximum) {
                node_destroy(atom);
                diagnostic_set(parser,
                               UMI_STATUS_PARSE_ERROR,
                               parser->position,
                               "Repetition minimum exceeds maximum");
                return NULL;
            }
            break;
    }
    if (parser_peek(parser) == '?') {
        (void)parser_take(parser);
        greedy = 0;
    }
    if (parser_peek(parser) == '*' || parser_peek(parser) == '+' ||
        parser_peek(parser) == '?' || parser_peek(parser) == '{') {
        node_destroy(atom);
        diagnostic_set(parser,
                       UMI_STATUS_PARSE_ERROR,
                       parser->position,
                       "Multiple quantifiers cannot target the same atom");
        return NULL;
    }
    repeat = node_create(REGEX_NODE_REPEAT);
    if (repeat == NULL) {
        node_destroy(atom);
        diagnostic_set(parser,
                       UMI_STATUS_OUT_OF_MEMORY,
                       parser->position,
                       "Unable to allocate repetition");
        return NULL;
    }
    repeat->data.repeat.child = atom;
    repeat->data.repeat.minimum = minimum;
    repeat->data.repeat.maximum = maximum;
    repeat->data.repeat.greedy = greedy;
    return repeat;
}

static RegexNode *parse_sequence(RegexParser *parser)
{
    RegexNode *sequence = node_create(REGEX_NODE_SEQUENCE);
    if (sequence == NULL) {
        diagnostic_set(parser,
                       UMI_STATUS_OUT_OF_MEMORY,
                       parser->position,
                       "Unable to allocate sequence");
        return NULL;
    }
    while (!parser_at_end(parser) && parser_peek(parser) != ')' &&
           parser_peek(parser) != '|') {
        RegexNode *item = parse_repetition(parser);
        UmiStatus status;
        if (item == NULL) {
            node_destroy(sequence);
            return NULL;
        }
        status = node_list_append(sequence, item);
        if (status != UMI_STATUS_OK) {
            node_destroy(item);
            node_destroy(sequence);
            diagnostic_set(parser,
                           status,
                           parser->position,
                           "Unable to grow sequence");
            return NULL;
        }
    }
    if (sequence->data.list.count == 0U) {
        node_destroy(sequence);
        return node_create(REGEX_NODE_EMPTY);
    }
    if (sequence->data.list.count == 1U) {
        RegexNode *single = sequence->data.list.items[0];
        free(sequence->data.list.items);
        sequence->data.list.items = NULL;
        free(sequence);
        return single;
    }
    return sequence;
}

static RegexNode *parse_alternation(RegexParser *parser)
{
    RegexNode *first = parse_sequence(parser);
    RegexNode *alternation;
    UmiStatus status;
    if (first == NULL) return NULL;
    if (parser_peek(parser) != '|') return first;
    alternation = node_create(REGEX_NODE_ALTERNATION);
    if (alternation == NULL) {
        node_destroy(first);
        diagnostic_set(parser,
                       UMI_STATUS_OUT_OF_MEMORY,
                       parser->position,
                       "Unable to allocate alternation");
        return NULL;
    }
    status = node_list_append(alternation, first);
    if (status != UMI_STATUS_OK) {
        node_destroy(first);
        node_destroy(alternation);
        diagnostic_set(parser,
                       status,
                       parser->position,
                       "Unable to grow alternation");
        return NULL;
    }
    while (parser_peek(parser) == '|') {
        RegexNode *branch;
        (void)parser_take(parser);
        branch = parse_sequence(parser);
        if (branch == NULL) {
            node_destroy(alternation);
            return NULL;
        }
        status = node_list_append(alternation, branch);
        if (status != UMI_STATUS_OK) {
            node_destroy(branch);
            node_destroy(alternation);
            diagnostic_set(parser,
                           status,
                           parser->position,
                           "Unable to grow alternation");
            return NULL;
        }
    }
    return alternation;
}

static RegexNode *parse_regex(RegexParser *parser)
{
    RegexNode *root = parse_alternation(parser);
    if (root == NULL) return NULL;
    if (!parser_at_end(parser)) {
        char unexpected = parser_peek(parser);
        node_destroy(root);
        if (unexpected == ')') {
            diagnostic_set(parser,
                           UMI_STATUS_PARSE_ERROR,
                           parser->position,
                           "Unmatched closing parenthesis");
        } else {
            diagnostic_set(parser,
                           UMI_STATUS_PARSE_ERROR,
                           parser->position,
                           "Unexpected expression token");
        }
        return NULL;
    }
    return root;
}

static RegexNode *parse_literal_expression(RegexParser *parser)
{
    RegexNode *sequence = node_create(REGEX_NODE_SEQUENCE);
    size_t index;
    if (sequence == NULL) {
        diagnostic_set(parser,
                       UMI_STATUS_OUT_OF_MEMORY,
                       0U,
                       "Unable to allocate literal sequence");
        return NULL;
    }
    for (index = 0U; index < parser->length; ++index) {
        RegexNode *literal = node_create(REGEX_NODE_LITERAL);
        UmiStatus status;
        if (literal == NULL) {
            node_destroy(sequence);
            diagnostic_set(parser,
                           UMI_STATUS_OUT_OF_MEMORY,
                           index,
                           "Unable to allocate literal");
            return NULL;
        }
        literal->data.literal = (unsigned char)parser->expression[index];
        status = node_list_append(sequence, literal);
        if (status != UMI_STATUS_OK) {
            node_destroy(literal);
            node_destroy(sequence);
            diagnostic_set(parser,
                           status,
                           index,
                           "Unable to grow literal sequence");
            return NULL;
        }
    }
    if (sequence->data.list.count == 1U) {
        RegexNode *single = sequence->data.list.items[0];
        free(sequence->data.list.items);
        free(sequence);
        return single;
    }
    return sequence;
}

static UmiStatus program_reserve(RegexProgram *program, size_t required)
{
    RegexInstruction *replacement;
    size_t capacity;
    if (required <= program->capacity) return UMI_STATUS_OK;
    capacity = program->capacity == 0U ? 32U : program->capacity * 2U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > program->maximum ||
        capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (RegexInstruction *)realloc(
        program->instructions, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    program->instructions = replacement;
    program->capacity = capacity;
    return UMI_STATUS_OK;
}

static UmiStatus program_emit(RegexProgram *program,
                              RegexInstruction instruction,
                              size_t *out_position)
{
    UmiStatus status;
    if (program == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (program->count >= program->maximum) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = program_reserve(program, program->count + 1U);
    if (status != UMI_STATUS_OK) return status;
    if (out_position != NULL) *out_position = program->count;
    program->instructions[program->count++] = instruction;
    return UMI_STATUS_OK;
}

static RegexInstruction instruction_make(RegexInstructionType type)
{
    RegexInstruction instruction;
    (void)memset(&instruction, 0, sizeof(instruction));
    instruction.type = type;
    return instruction;
}

static UmiStatus compile_node(RegexProgram *program, const RegexNode *node);

static UmiStatus compile_alternatives(RegexProgram *program,
                                      RegexNode *const *items,
                                      size_t index,
                                      size_t count)
{
    RegexInstruction split;
    RegexInstruction jump;
    size_t split_position;
    size_t jump_position;
    UmiStatus status;
    if (index >= count) return UMI_STATUS_OK;
    if (index + 1U == count) return compile_node(program, items[index]);
    split = instruction_make(REGEX_INSTRUCTION_SPLIT);
    status = program_emit(program, split, &split_position);
    if (status != UMI_STATUS_OK) return status;
    program->instructions[split_position].first = program->count;
    status = compile_node(program, items[index]);
    if (status != UMI_STATUS_OK) return status;
    jump = instruction_make(REGEX_INSTRUCTION_JUMP);
    status = program_emit(program, jump, &jump_position);
    if (status != UMI_STATUS_OK) return status;
    program->instructions[split_position].second = program->count;
    status = compile_alternatives(program, items, index + 1U, count);
    if (status != UMI_STATUS_OK) return status;
    program->instructions[jump_position].first = program->count;
    return UMI_STATUS_OK;
}

static UmiStatus compile_optional(RegexProgram *program,
                                  const RegexNode *child,
                                  int greedy)
{
    RegexInstruction split = instruction_make(REGEX_INSTRUCTION_SPLIT);
    size_t split_position;
    size_t child_start;
    size_t after;
    UmiStatus status = program_emit(program, split, &split_position);
    if (status != UMI_STATUS_OK) return status;
    child_start = program->count;
    status = compile_node(program, child);
    if (status != UMI_STATUS_OK) return status;
    after = program->count;
    if (greedy) {
        program->instructions[split_position].first = child_start;
        program->instructions[split_position].second = after;
    } else {
        program->instructions[split_position].first = after;
        program->instructions[split_position].second = child_start;
    }
    return UMI_STATUS_OK;
}

static UmiStatus compile_star(RegexProgram *program,
                              const RegexNode *child,
                              int greedy)
{
    RegexInstruction split = instruction_make(REGEX_INSTRUCTION_SPLIT);
    RegexInstruction jump = instruction_make(REGEX_INSTRUCTION_JUMP);
    size_t split_position;
    size_t child_start;
    size_t after;
    UmiStatus status = program_emit(program, split, &split_position);
    if (status != UMI_STATUS_OK) return status;
    child_start = program->count;
    status = compile_node(program, child);
    if (status != UMI_STATUS_OK) return status;
    jump.first = split_position;
    status = program_emit(program, jump, NULL);
    if (status != UMI_STATUS_OK) return status;
    after = program->count;
    if (greedy) {
        program->instructions[split_position].first = child_start;
        program->instructions[split_position].second = after;
    } else {
        program->instructions[split_position].first = after;
        program->instructions[split_position].second = child_start;
    }
    return UMI_STATUS_OK;
}

static UmiStatus compile_node(RegexProgram *program, const RegexNode *node)
{
    RegexInstruction instruction;
    UmiStatus status;
    size_t index;
    if (program == NULL || node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    switch (node->type) {
        case REGEX_NODE_EMPTY:
            return UMI_STATUS_OK;
        case REGEX_NODE_LITERAL:
            instruction = instruction_make(REGEX_INSTRUCTION_CHARACTER);
            instruction.character = node->data.literal;
            return program_emit(program, instruction, NULL);
        case REGEX_NODE_ANY:
            instruction = instruction_make(REGEX_INSTRUCTION_ANY);
            return program_emit(program, instruction, NULL);
        case REGEX_NODE_CLASS:
            instruction = instruction_make(REGEX_INSTRUCTION_CLASS);
            instruction.character_class = node->data.character_class;
            return program_emit(program, instruction, NULL);
        case REGEX_NODE_BEGIN:
            instruction = instruction_make(REGEX_INSTRUCTION_BEGIN);
            return program_emit(program, instruction, NULL);
        case REGEX_NODE_END:
            instruction = instruction_make(REGEX_INSTRUCTION_END);
            return program_emit(program, instruction, NULL);
        case REGEX_NODE_WORD_BOUNDARY:
            instruction = instruction_make(REGEX_INSTRUCTION_WORD_BOUNDARY);
            instruction.negated = node->data.boundary.negated;
            return program_emit(program, instruction, NULL);
        case REGEX_NODE_SEQUENCE:
            for (index = 0U; index < node->data.list.count; ++index) {
                status = compile_node(program, node->data.list.items[index]);
                if (status != UMI_STATUS_OK) return status;
            }
            return UMI_STATUS_OK;
        case REGEX_NODE_ALTERNATION:
            return compile_alternatives(program,
                                        node->data.list.items,
                                        0U,
                                        node->data.list.count);
        case REGEX_NODE_GROUP:
            instruction = instruction_make(REGEX_INSTRUCTION_SAVE);
            instruction.capture_slot = node->data.group.group_index * 2U;
            status = program_emit(program, instruction, NULL);
            if (status != UMI_STATUS_OK) return status;
            status = compile_node(program, node->data.group.child);
            if (status != UMI_STATUS_OK) return status;
            instruction = instruction_make(REGEX_INSTRUCTION_SAVE);
            instruction.capture_slot =
                node->data.group.group_index * 2U + 1U;
            return program_emit(program, instruction, NULL);
        case REGEX_NODE_REPEAT:
            for (index = 0U; index < node->data.repeat.minimum; ++index) {
                status = compile_node(program, node->data.repeat.child);
                if (status != UMI_STATUS_OK) return status;
            }
            if (node->data.repeat.maximum ==
                UMI_EDITOR_WORKSPACE_SEARCH_UNBOUNDED_REPEAT) {
                return compile_star(program,
                                    node->data.repeat.child,
                                    node->data.repeat.greedy);
            }
            for (index = node->data.repeat.minimum;
                 index < node->data.repeat.maximum;
                 ++index) {
                status = compile_optional(program,
                                          node->data.repeat.child,
                                          node->data.repeat.greedy);
                if (status != UMI_STATUS_OK) return status;
            }
            return UMI_STATUS_OK;
        default:
            return UMI_STATUS_INTERNAL_ERROR;
    }
}

static int request_valid(
    const UmiEditorWorkspaceSearchPatternRequest *request)
{
    return request != NULL &&
           request->struct_size == (uint32_t)sizeof(*request) &&
           request->api_version ==
               UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_API_VERSION &&
           request->mode >= UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_LITERAL &&
           request->mode <= UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_REGEX &&
           request->case_mode >=
               UMI_EDITOR_WORKSPACE_SEARCH_CASE_INSENSITIVE &&
           request->case_mode <= UMI_EDITOR_WORKSPACE_SEARCH_CASE_SMART &&
           terminated(request->expression, sizeof(request->expression)) &&
           request->expression[0] != '\0';
}

static int expression_has_uppercase(const char *expression)
{
    size_t index;
    for (index = 0U; expression[index] != '\0'; ++index) {
        if (isupper((unsigned char)expression[index])) return 1;
    }
    return 0;
}

void umi_editor_workspace_search_pattern_request_init(
    UmiEditorWorkspaceSearchPatternRequest *request,
    const char *expression)
{
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->struct_size = (uint32_t)sizeof(*request);
    request->api_version = UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_API_VERSION;
    request->mode = UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_LITERAL;
    request->case_mode = UMI_EDITOR_WORKSPACE_SEARCH_CASE_SMART;
    request->maximum_program_instructions = DEFAULT_PROGRAM_CAPACITY;
    request->maximum_execution_steps = DEFAULT_EXECUTION_STEPS;
    copy_text(request->expression, sizeof(request->expression), expression);
}

UmiStatus umi_editor_workspace_search_pattern_create(
    UmiEditorWorkspaceSearchPattern **out_pattern)
{
    UmiEditorWorkspaceSearchPattern *pattern;
    if (out_pattern == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_pattern = NULL;
    pattern = (UmiEditorWorkspaceSearchPattern *)calloc(1U, sizeof(*pattern));
    if (pattern == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    pattern->revision = 1U;
    *out_pattern = pattern;
    return UMI_STATUS_OK;
}

void umi_editor_workspace_search_pattern_destroy(
    UmiEditorWorkspaceSearchPattern *pattern)
{
    if (pattern == NULL) return;
    free(pattern->instructions);
    pattern->instructions = NULL;
    free(pattern);
}

UmiStatus umi_editor_workspace_search_pattern_compile(
    UmiEditorWorkspaceSearchPattern *pattern,
    const UmiEditorWorkspaceSearchPatternRequest *request,
    UmiEditorWorkspaceSearchPatternDiagnostic *out_diagnostic)
{
    RegexParser parser;
    RegexProgram program;
    RegexNode *root;
    RegexInstruction match;
    UmiStatus status;
    size_t maximum_program;

    diagnostic_clear(out_diagnostic);
    if (pattern == NULL || !request_valid(request)) {
        if (out_diagnostic != NULL) {
            out_diagnostic->status = UMI_STATUS_INVALID_ARGUMENT;
            copy_text(out_diagnostic->message,
                      sizeof(out_diagnostic->message),
                      "Invalid search-pattern request");
        }
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    maximum_program = request->maximum_program_instructions == 0U
        ? DEFAULT_PROGRAM_CAPACITY
        : request->maximum_program_instructions;
    if (maximum_program < 2U || maximum_program > 65536U) {
        if (out_diagnostic != NULL) {
            out_diagnostic->status = UMI_STATUS_INVALID_ARGUMENT;
            copy_text(out_diagnostic->message,
                      sizeof(out_diagnostic->message),
                      "Program-instruction limit is outside the supported range");
        }
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&parser, 0, sizeof(parser));
    parser.expression = request->expression;
    parser.length = strlen(request->expression);
    parser.diagnostic = out_diagnostic;
    parser.status = UMI_STATUS_OK;
    root = request->mode == UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_LITERAL
        ? parse_literal_expression(&parser)
        : parse_regex(&parser);
    if (root == NULL || parser.status != UMI_STATUS_OK) {
        node_destroy(root);
        return parser.status == UMI_STATUS_OK
            ? UMI_STATUS_PARSE_ERROR : parser.status;
    }

    (void)memset(&program, 0, sizeof(program));
    program.maximum = maximum_program;
    status = compile_node(&program, root);
    if (status == UMI_STATUS_OK) {
        match = instruction_make(REGEX_INSTRUCTION_MATCH);
        status = program_emit(&program, match, NULL);
    }
    node_destroy(root);
    if (status != UMI_STATUS_OK) {
        free(program.instructions);
        if (out_diagnostic != NULL) {
            out_diagnostic->status = status;
            copy_text(out_diagnostic->message,
                      sizeof(out_diagnostic->message),
                      status == UMI_STATUS_CAPACITY_EXCEEDED
                          ? "Compiled expression exceeds the instruction limit"
                          : "Unable to compile expression");
        }
        return status;
    }

    free(pattern->instructions);
    pattern->instructions = program.instructions;
    pattern->instruction_count = program.count;
    pattern->instruction_capacity = program.capacity;
    pattern->capture_count = parser.capture_count + 1U;
    pattern->mode = request->mode;
    pattern->case_mode = request->case_mode;
    pattern->effective_case_sensitive =
        request->case_mode == UMI_EDITOR_WORKSPACE_SEARCH_CASE_SENSITIVE ||
        (request->case_mode == UMI_EDITOR_WORKSPACE_SEARCH_CASE_SMART &&
         expression_has_uppercase(request->expression));
    pattern->whole_word = request->whole_word != 0;
    pattern->multiline = request->multiline != 0;
    pattern->dot_matches_newline = request->dot_matches_newline != 0;
    pattern->maximum_execution_steps = request->maximum_execution_steps == 0U
        ? DEFAULT_EXECUTION_STEPS : request->maximum_execution_steps;
    copy_text(pattern->expression,
              sizeof(pattern->expression),
              request->expression);
    pattern->compiled = 1;
    pattern->revision = next_revision(pattern->revision);
    if (out_diagnostic != NULL) out_diagnostic->status = UMI_STATUS_OK;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_search_pattern_reset(
    UmiEditorWorkspaceSearchPattern *pattern)
{
    if (pattern == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    free(pattern->instructions);
    pattern->instructions = NULL;
    pattern->instruction_count = 0U;
    pattern->instruction_capacity = 0U;
    pattern->capture_count = 0U;
    pattern->maximum_execution_steps = 0U;
    pattern->expression[0] = '\0';
    pattern->compiled = 0;
    pattern->revision = next_revision(pattern->revision);
    return UMI_STATUS_OK;
}

static int is_word_byte(unsigned char value)
{
    return isalnum(value) != 0 || value == (unsigned char)'_';
}

static int character_equal(unsigned char left,
                           unsigned char right,
                           int case_sensitive)
{
    if (case_sensitive) return left == right;
    return tolower(left) == tolower(right);
}

static int class_matches(const RegexClass *character_class,
                         unsigned char value,
                         int case_sensitive)
{
    int present = class_has(character_class, value);
    if (!case_sensitive) {
        present = present ||
                  class_has(character_class,
                            (unsigned char)tolower(value)) ||
                  class_has(character_class,
                            (unsigned char)toupper(value));
    }
    return character_class->negated ? !present : present;
}

static int at_beginning(const UmiEditorWorkspaceSearchPattern *pattern,
                        const char *text,
                        size_t position)
{
    return position == 0U ||
           (pattern->multiline && position > 0U &&
            text[position - 1U] == '\n');
}

static int at_end(const UmiEditorWorkspaceSearchPattern *pattern,
                  const char *text,
                  size_t text_length,
                  size_t position)
{
    return position == text_length ||
           (pattern->multiline && position < text_length &&
            text[position] == '\n');
}

static int at_word_boundary(const char *text,
                            size_t text_length,
                            size_t position)
{
    int left_word = position > 0U
        ? is_word_byte((unsigned char)text[position - 1U]) : 0;
    int right_word = position < text_length
        ? is_word_byte((unsigned char)text[position]) : 0;
    return left_word != right_word;
}

static UmiStatus thread_list_reserve(RegexThreadList *list, size_t required)
{
    RegexThread *replacement;
    size_t capacity;
    if (required <= list->capacity) return UMI_STATUS_OK;
    capacity = list->capacity == 0U ? 32U : list->capacity * 2U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (RegexThread *)realloc(
        list->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->items = replacement;
    list->capacity = capacity;
    return UMI_STATUS_OK;
}

static int same_thread(const RegexThread *left, const RegexThread *right)
{
    return left->program_counter == right->program_counter &&
           memcmp(left->captures,
                  right->captures,
                  sizeof(left->captures)) == 0;
}

static UmiStatus thread_list_add(RegexThreadList *list,
                                 const RegexThread *thread)
{
    size_t index;
    UmiStatus status;
    for (index = 0U; index < list->count; ++index) {
        if (same_thread(&list->items[index], thread)) return UMI_STATUS_OK;
    }
    status = thread_list_reserve(list, list->count + 1U);
    if (status != UMI_STATUS_OK) return status;
    list->items[list->count++] = *thread;
    return UMI_STATUS_OK;
}

static uint32_t next_generation(RegexExecution *execution)
{
    ++execution->generation;
    if (execution->generation == 0U) {
        (void)memset(execution->visited_generation,
                     0,
                     execution->pattern->instruction_count *
                         sizeof(*execution->visited_generation));
        execution->generation = 1U;
    }
    return execution->generation;
}

static UmiStatus add_thread(RegexExecution *execution,
                            RegexThreadList *list,
                            size_t program_counter,
                            const uint64_t captures[CAPTURE_SLOT_COUNT],
                            size_t position,
                            uint32_t generation)
{
    const RegexInstruction *instruction;
    uint64_t updated_captures[CAPTURE_SLOT_COUNT];
    RegexThread thread;
    UmiStatus status;

    if (execution->status != UMI_STATUS_OK) return execution->status;
    if (++execution->steps > execution->maximum_steps) {
        execution->status = UMI_STATUS_TIMEOUT;
        return execution->status;
    }
    if (program_counter >= execution->pattern->instruction_count) {
        execution->status = UMI_STATUS_INTERNAL_ERROR;
        return execution->status;
    }
    if (execution->visited_generation[program_counter] == generation) {
        return UMI_STATUS_OK;
    }
    execution->visited_generation[program_counter] = generation;
    instruction = &execution->pattern->instructions[program_counter];
    switch (instruction->type) {
        case REGEX_INSTRUCTION_JUMP:
            return add_thread(execution,
                              list,
                              instruction->first,
                              captures,
                              position,
                              generation);
        case REGEX_INSTRUCTION_SPLIT:
            status = add_thread(execution,
                                list,
                                instruction->first,
                                captures,
                                position,
                                generation);
            if (status != UMI_STATUS_OK) return status;
            return add_thread(execution,
                              list,
                              instruction->second,
                              captures,
                              position,
                              generation);
        case REGEX_INSTRUCTION_SAVE:
            if (instruction->capture_slot >= CAPTURE_SLOT_COUNT) {
                execution->status = UMI_STATUS_INTERNAL_ERROR;
                return execution->status;
            }
            (void)memcpy(updated_captures,
                         captures,
                         sizeof(updated_captures));
            updated_captures[instruction->capture_slot] = (uint64_t)position;
            return add_thread(execution,
                              list,
                              program_counter + 1U,
                              updated_captures,
                              position,
                              generation);
        case REGEX_INSTRUCTION_BEGIN:
            if (!at_beginning(execution->pattern,
                              execution->text,
                              position)) {
                return UMI_STATUS_OK;
            }
            return add_thread(execution,
                              list,
                              program_counter + 1U,
                              captures,
                              position,
                              generation);
        case REGEX_INSTRUCTION_END:
            if (!at_end(execution->pattern,
                        execution->text,
                        execution->text_length,
                        position)) {
                return UMI_STATUS_OK;
            }
            return add_thread(execution,
                              list,
                              program_counter + 1U,
                              captures,
                              position,
                              generation);
        case REGEX_INSTRUCTION_WORD_BOUNDARY: {
            int boundary = at_word_boundary(execution->text,
                                            execution->text_length,
                                            position);
            if (instruction->negated) boundary = !boundary;
            if (!boundary) return UMI_STATUS_OK;
            return add_thread(execution,
                              list,
                              program_counter + 1U,
                              captures,
                              position,
                              generation);
        }
        default:
            (void)memset(&thread, 0, sizeof(thread));
            thread.program_counter = program_counter;
            (void)memcpy(thread.captures,
                         captures,
                         sizeof(thread.captures));
            return thread_list_add(list, &thread);
    }
}

static int instruction_consumes(
    const UmiEditorWorkspaceSearchPattern *pattern,
    const RegexInstruction *instruction,
    unsigned char value)
{
    switch (instruction->type) {
        case REGEX_INSTRUCTION_CHARACTER:
            return character_equal(instruction->character,
                                   value,
                                   pattern->effective_case_sensitive);
        case REGEX_INSTRUCTION_ANY:
            return pattern->dot_matches_newline || value != (unsigned char)'\n';
        case REGEX_INSTRUCTION_CLASS:
            return class_matches(&instruction->character_class,
                                 value,
                                 pattern->effective_case_sensitive);
        default:
            return 0;
    }
}

static int whole_word_accepts(const char *text,
                              size_t text_length,
                              size_t start,
                              size_t end)
{
    int left = start > 0U
        ? is_word_byte((unsigned char)text[start - 1U]) : 0;
    int first = start < text_length
        ? is_word_byte((unsigned char)text[start]) : 0;
    int last = end > start
        ? is_word_byte((unsigned char)text[end - 1U]) : first;
    int right = end < text_length
        ? is_word_byte((unsigned char)text[end]) : 0;
    return left != first && last != right;
}

static void capture_match(
    const UmiEditorWorkspaceSearchPattern *pattern,
    const RegexThread *thread,
    size_t start,
    size_t end,
    uint64_t execution_steps,
    UmiEditorWorkspaceSearchPatternMatch *out_match)
{
    size_t index;
    (void)memset(out_match, 0, sizeof(*out_match));
    out_match->struct_size = (uint32_t)sizeof(*out_match);
    out_match->api_version =
        UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_API_VERSION;
    out_match->start_byte_offset = (uint64_t)start;
    out_match->end_byte_offset = (uint64_t)end;
    out_match->capture_count = pattern->capture_count;
    out_match->execution_steps = execution_steps;
    out_match->empty = start == end;
    out_match->captures[0].matched = 1;
    out_match->captures[0].start_byte_offset = (uint64_t)start;
    out_match->captures[0].end_byte_offset = (uint64_t)end;
    for (index = 1U; index < pattern->capture_count; ++index) {
        uint64_t capture_start = thread->captures[index * 2U];
        uint64_t capture_end = thread->captures[index * 2U + 1U];
        if (capture_start != CAPTURE_UNSET &&
            capture_end != CAPTURE_UNSET &&
            capture_start <= capture_end) {
            out_match->captures[index].matched = 1;
            out_match->captures[index].start_byte_offset = capture_start;
            out_match->captures[index].end_byte_offset = capture_end;
        }
    }
}

static UmiStatus find_from_start(
    const UmiEditorWorkspaceSearchPattern *pattern,
    const char *text,
    size_t text_length,
    size_t start,
    UmiEditorWorkspaceSearchPatternMatch *out_match,
    int *out_found)
{
    RegexExecution execution;
    RegexThreadList current;
    RegexThreadList next;
    uint64_t initial_captures[CAPTURE_SLOT_COUNT];
    size_t position;
    size_t index;
    int candidate_found = 0;
    size_t candidate_end = start;
    RegexThread candidate_thread;
    UmiStatus status;

    *out_found = 0;
    (void)memset(&execution, 0, sizeof(execution));
    (void)memset(&current, 0, sizeof(current));
    (void)memset(&next, 0, sizeof(next));
    (void)memset(&candidate_thread, 0, sizeof(candidate_thread));
    for (index = 0U; index < CAPTURE_SLOT_COUNT; ++index) {
        initial_captures[index] = CAPTURE_UNSET;
    }
    initial_captures[0] = (uint64_t)start;
    execution.pattern = pattern;
    execution.text = text;
    execution.text_length = text_length;
    execution.maximum_steps = pattern->maximum_execution_steps;
    execution.status = UMI_STATUS_OK;
    execution.visited_generation = (uint32_t *)calloc(
        pattern->instruction_count,
        sizeof(*execution.visited_generation));
    if (execution.visited_generation == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    status = add_thread(&execution,
                        &current,
                        0U,
                        initial_captures,
                        start,
                        next_generation(&execution));
    if (status != UMI_STATUS_OK) goto cleanup;

    /*
     * Thread order is semantic. SPLIT adds its preferred branch first, so the
     * list represents alternation and greedy/lazy priority. A MATCH behind
     * higher-priority consuming threads becomes a pending candidate; all
     * lower-priority threads can be discarded because they can never beat it.
     * When no higher-priority thread remains, the candidate is accepted. This
     * is the ordered-Thompson rule used to provide deterministic Perl-style
     * leftmost-first matching without recursive backtracking.
     */
    for (position = start; position <= text_length; ++position) {
        size_t first_match = SIZE_MAX;

        for (index = 0U; index < current.count; ++index) {
            const RegexThread *thread = &current.items[index];
            const RegexInstruction *instruction =
                &pattern->instructions[thread->program_counter];
            if (instruction->type == REGEX_INSTRUCTION_MATCH &&
                (!pattern->whole_word ||
                 whole_word_accepts(text,
                                    text_length,
                                    start,
                                    position))) {
                first_match = index;
                break;
            }
        }

        if (first_match != SIZE_MAX) {
            candidate_found = 1;
            candidate_end = position;
            candidate_thread = current.items[first_match];
            if (first_match == 0U) {
                capture_match(pattern,
                              &candidate_thread,
                              start,
                              candidate_end,
                              execution.steps,
                              out_match);
                *out_found = 1;
                status = UMI_STATUS_OK;
                goto cleanup;
            }

            /* Retain only paths with higher priority than the candidate. */
            current.count = first_match;
        }

        if (position == text_length) break;
        next.count = 0U;
        for (index = 0U; index < current.count; ++index) {
            const RegexThread *thread = &current.items[index];
            const RegexInstruction *instruction =
                &pattern->instructions[thread->program_counter];
            if (instruction_consumes(pattern,
                                     instruction,
                                     (unsigned char)text[position])) {
                status = add_thread(&execution,
                                    &next,
                                    thread->program_counter + 1U,
                                    thread->captures,
                                    position + 1U,
                                    next_generation(&execution));
                if (status != UMI_STATUS_OK) goto cleanup;
            }
        }
        {
            RegexThreadList temporary = current;
            current = next;
            next = temporary;
        }
        if (current.count == 0U) break;
    }

    if (candidate_found) {
        capture_match(pattern,
                      &candidate_thread,
                      start,
                      candidate_end,
                      execution.steps,
                      out_match);
        *out_found = 1;
    }
    status = UMI_STATUS_OK;

cleanup:
    free(execution.visited_generation);
    free(current.items);
    free(next.items);
    return status;
}

UmiStatus umi_editor_workspace_search_pattern_find(
    const UmiEditorWorkspaceSearchPattern *pattern,
    const char *text,
    size_t text_length,
    size_t start_byte_offset,
    UmiEditorWorkspaceSearchPatternMatch *out_match)
{
    size_t start;
    UmiStatus status;
    if (out_match != NULL) {
        (void)memset(out_match, 0, sizeof(*out_match));
    }
    if (pattern == NULL || text == NULL || out_match == NULL ||
        !pattern->compiled || pattern->instructions == NULL ||
        pattern->instruction_count == 0U || start_byte_offset > text_length) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (start = start_byte_offset; start <= text_length; ++start) {
        int found = 0;
        status = find_from_start(pattern,
                                 text,
                                 text_length,
                                 start,
                                 out_match,
                                 &found);
        if (status != UMI_STATUS_OK) return status;
        if (found) return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_editor_workspace_search_pattern_snapshot(
    const UmiEditorWorkspaceSearchPattern *pattern,
    UmiEditorWorkspaceSearchPatternSnapshot *out_snapshot)
{
    if (pattern == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_API_VERSION;
    out_snapshot->mode = pattern->mode;
    out_snapshot->case_mode = pattern->case_mode;
    out_snapshot->expression_length = strlen(pattern->expression);
    out_snapshot->instruction_count = pattern->instruction_count;
    out_snapshot->capture_count = pattern->capture_count;
    out_snapshot->revision = pattern->revision;
    out_snapshot->maximum_execution_steps = pattern->maximum_execution_steps;
    out_snapshot->whole_word = pattern->whole_word;
    out_snapshot->multiline = pattern->multiline;
    out_snapshot->dot_matches_newline = pattern->dot_matches_newline;
    out_snapshot->compiled = pattern->compiled;
    return UMI_STATUS_OK;
}

uint64_t umi_editor_workspace_search_pattern_revision(
    const UmiEditorWorkspaceSearchPattern *pattern)
{
    return pattern != NULL ? pattern->revision : 0U;
}
