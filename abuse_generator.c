#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dictionary.h"


#define MAX_TOKEN_TEXT_SIZE 200
#define MAX_CHILDS 10
#define MAX_TOKENS 100

#define MAX_PHRASE_LENGTH 1000
#define MAX_WORD_SIZE 100



/**
 * Types of tokens:
 */
typedef enum
{
    T_UNDEFINED = 0,
    T_TEXT,
    T_CHOISE,
    T_POSSIBILITY,
    T_LIST,
    T_SEPARATOR
} token_type;



/**
 * Node of syntax tree
 *
 * TODO: optimize!
 */
typedef struct Node
{
    struct Node *childs[MAX_CHILDS];
    int number_of_childs;

    struct Node *parent;


    token_type type;
    char content[MAX_TOKEN_TEXT_SIZE];

} Node;



/* Private */
static Node *create_node(Node *parent, token_type type, char *content);
static void destroy_node_with_childs(Node *node);
static unsigned int random_lt(unsigned int floor_);
static Node *choose_random(Node **nodes, unsigned int number_of_nodes);
static void recursive_print(Node *node, int nesting);
static void compile(Node *node, char *destination,
                    unsigned int *pos, unsigned int max_symbols);
static int is_separator(char character);
static void get_token(const char *expression, char *buffer, unsigned int *position);
static void tokenize(const char *pattern, char **tokens);
static void parse_syntax(const char *pattern, char *expression);
static void get_random_from_dict(char *buffer, char **dictionary);
static void get_random_word_by_type(char type, char *word);
static void replace_variables(const char *pattern, char *expression);

/* Public */
void get_random_phrase(char *destination);



/**
 * Creates node in syntax tree and append it to $parent
 * $parent can be NULL
 *
 * @param *parent   Pointer to parent.
 *                  This new node will be pushed to
 *                  parent->childs
 * @param type      One of token_type enum
 * @param *content  If type is T_TEXT it must be text,
 *                  else - empty string
 *
 * Fixme: not to push node to parent->childs,
 *        if number_of_childs >= MAX_CHILDS
 */
Node *create_node(Node *parent, token_type type, char *content)
{
    Node *node = (Node *) malloc(sizeof(Node));
    node->number_of_childs = 0;

    node->type = type;
    strncpy(node->content, content, MAX_TOKEN_TEXT_SIZE);

    node->parent = parent;
    if (parent) {
        parent->childs[parent->number_of_childs++] = node;
    }

    return node;
}


/**
 * Releases memory of node and all childs
 *
 * @param *node     Node for destorying
 */
void destroy_node_with_childs(Node *node)
{
    if (node) {
        for (int i = 0; i < node->number_of_childs; ++i) {
            destroy_node_with_childs(node->childs[i]);
        }

        free(node);
    }
}


/**
 * Returns unsigned random less then $floor_
 *
 * @param floor_    Random value guaranteed to be less then $floor_
 *                  Except $floot_ == 0!
 */
unsigned int random_lt(unsigned int floor_)
{
    static unsigned int addition = 0;
    if (floor_ == 0) {
        return 0;
    }
    srand(time(NULL) + addition++);
    return rand() % floor_;
}


/**
 * Chooses random from nodes
 * If number_of_nodes == 0 then returns NULL
 *
 * @param **nodes           Pointer to array of nodes
 * @param number_of_nodes   Number of nodes in array
 */
Node *choose_random(Node **nodes, unsigned int number_of_nodes)
{
    if (number_of_nodes == 0) {
        return NULL;
    }

    return nodes[random_lt(number_of_nodes)];
}


/**
 * DEBUG ONLY. TODO: Remove
 */
void recursive_print(Node *node, int nesting)
{
    if (node->type == T_TEXT) {
        int i;
        for (int i = 0; i < nesting; ++i) {
            printf(".");
        }
        printf("%s\n", node->content);
    }
    else if (node->type == T_CHOISE || node->type == T_LIST || node->type == T_POSSIBILITY) {
        int i;
        for (i = 0; i < node->number_of_childs; ++i) {
            recursive_print(node->childs[i], nesting + 1);
        }
    }
}


/**
 * If node->type is T_TEXT then appends append->content
 * to destination.
 *
 * @param *node         Node deals with
 * @param *destination  String to appending node->content
 * @param *pos          Pointer to current position in destination string
 * @param max_symbols   Length of buffer (destination)
 *
 * TODO: Rename
 */
void compile(Node *node, char *destination,
             unsigned int *pos, unsigned int max_symbols)
{
    char *dst = NULL;
    Node *random = NULL;
    int i;

    switch (node->type) {
    case T_TEXT:
        dst = node->content;
        while (*dst && *pos < max_symbols - 1) {
            destination[(*pos)++] = *dst++;
        }
        break;

    case T_CHOISE:
        random = choose_random(node->childs, node->number_of_childs);
        if (random) {
            compile(random, destination, pos, max_symbols);
        }
        break;

    case T_LIST:
        for (i = 0; i < node->number_of_childs; ++i) {
            compile(node->childs[i], destination, pos, max_symbols);
        }
        break;

    case T_POSSIBILITY:
        if (random_lt(2) == 1 && node->number_of_childs != 0) {
            compile(node->childs[0], destination, pos, max_symbols);
        }
        break;

    default:
        /* Ignore other types */
        /* TODO: Error on T_UNDEFINED */
        break;
    }

    /* Moved from T_TEXT section here for security reasons
    Error occurs While node->number_of_childs == 0 */
    destination[*pos] = '\0';
}


/**
 * Is character separator
 *
 * @param character     Character lol
 *
 * TODO: Rename
 */
int is_separator(char character)
{
    return (character == '(' || character == ')' || character == '|' ||
            character == '[' || character == ']');
}


/**
 * Gets token from expression and writes it to buffer
 * If no more tokens then buffer will be empty string
 *
 * @param *expression       Expression with tokens
 * @param *buffer           Buffer for token
 * @param *position         Next token starts here
 */
void get_token(const char *expression, char *buffer, unsigned int *position)
{
    if (is_separator(expression[*position])) {
        *buffer++ = expression[*position];
        ++(*position);
    }
    else {
        while (expression[*position] && !is_separator(expression[*position])) {
            *buffer++ = expression[*position];
            ++(*position);
        }
    }
    *buffer = '\0';
}


/**
 * Splits pattern to tokens and write them to $tokens
 *
 * @param *pattern   pattern for splitting
 * @param **tokens      Tokens will be written here
 */
void tokenize(const char *pattern, char **tokens)
{
    char buffer[MAX_TOKEN_TEXT_SIZE];
    char *token = NULL;
    int token_number = 0;
    unsigned int position = 0;

    while (1) {
        token = buffer;
        get_token(pattern, token, &position);
        if (*token == '\0') {
            break;
        }
        strncpy(tokens[token_number++], token, MAX_TOKEN_TEXT_SIZE);
    }

    tokens[token_number] = NULL;
}


/**
 * Makes phrase by pattern and write it to expression
 *
 * @param *pattern      It will be interpretated
 * @param *expression   It will contain new expression
 * @param max_symbols   Max width of expression
 */
void parse_syntax(const char *pattern, char *expression)
{
    /* Allocate memory for tokens array */
    char **tokens = (char **) malloc(sizeof(char *) * MAX_TOKENS);
    for (int i = 0; i < MAX_TOKENS; ++i) {
        tokens[i] = (char *) malloc(sizeof(char) * MAX_TOKEN_TEXT_SIZE);
    }

    /* Split pattern to tokens */
    tokenize(pattern, tokens);

    /* Build syntax tree */
    Node *root = create_node(NULL, T_LIST, "");

    Node *current = root;

    for (int i = 0; tokens[i] != NULL; ++i) {
        char *token = tokens[i];

        if (strcmp(token, "(") == 0) {
            current = create_node(current, T_CHOISE, "");
        }
        else if (strcmp(token, "[") == 0) {
            current = create_node(current, T_POSSIBILITY, "");
        }
        else if (strcmp(token, ")") == 0 || strcmp(token, "]") == 0) {
            if (current && current->parent) {
                current = current->parent;
            }
            else {
                /* Unexpected parenthesis. TODO: Error */
            }
        }
        else if (strcmp(token, "|") == 0) {
            /* Separator, do nothing */
        }
        else {
            create_node(current, T_TEXT, token);
        }
    }

    if (current != root) {
        /* Unexpected parenthesis. TODO: Error */
    }


    /* Parse syntax tree and build expression */
    unsigned int position = 0;
    compile(root, expression, &position, MAX_PHRASE_LENGTH);


    /* Free memory */
    for (int i = 0; i < MAX_TOKENS; ++i) {
        free(tokens[i]);
    }

    free(tokens);

    /* Destroy syntax tree and free memory */
    destroy_node_with_childs(root);
}


/**
 * Chooses random word from dictionary
 *
 * @param *buffer       Buffer for word
 * @param **dictionary  Dictionary with words
 */
static void get_random_from_dict(char *buffer, char **dictionary)
{
    int dict_length = 0;
    int random_index = 0;
    while (dictionary[dict_length] != 0) {
        ++dict_length;
    }

    random_index = random_lt(dict_length);
    strncpy(buffer, dictionary[random_index], MAX_WORD_SIZE);
}


/**
 * Gets word type and places random word (of this type) to *word
 *
 * @param type      Type (character). For example, 'n' is noun.
 * @param *word     Buffer for word
 */
static void get_random_word_by_type(char type, char *word)
{
    char **dict = 0;

    switch (type) {
    case 'n':
        dict = nouns;
        break;
    case 'a':
        dict = adjectives;
        break;
    case 'e':
        dict = etc;
        break;
    case 'v':
        dict = verbs;
        break;
    default:
        strcpy(word, "%");
        return;
    }

    get_random_from_dict(word, dict);
}


/**
 * Replaces variables to words
 *
 * @param *pattern      String with variables
 * @param *expression   Expression with words
 */
void replace_variables(const char *pattern, char *expression)
{
    char word_buffer[MAX_WORD_SIZE];
    char *word = word_buffer;

    int i;

    while (*pattern != '\0') {
        word = word_buffer;
        if (*pattern == '%' && *(pattern + 1) != '\0') {
            ++pattern;

            get_random_word_by_type(*pattern, word);

            i = MAX_PHRASE_LENGTH;

            /* TODO: Maybe strcat? */
            while (*word != '\0' && i--) {
                *expression++ = *word++;
            }
        }
        else {
            *expression++ = *pattern;
        }

        ++pattern;
    }
    *expression = '\0';
}



/* Public */

/**
 * Puts random (compiled) phrase to destination string
 *
 * @param *destination       Places random phrase here
 */
void get_random_phrase(char *destination)
{
    char pattern[MAX_PHRASE_LENGTH];
    char buffer[MAX_PHRASE_LENGTH];

    get_random_from_dict(pattern, patterns);
    parse_syntax(pattern, buffer);
    replace_variables(buffer, destination);
}


int main()
{
    char buffer[1000];
    for (int i = 0; i < 100; ++i) {
        get_random_phrase(buffer);
        printf("%s\n", buffer);
    }

    return 0;
}
