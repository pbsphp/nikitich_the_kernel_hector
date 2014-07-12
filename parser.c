#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define MAX_TOKEN_TEXT_SIZE 200

#define MAX_CHILDS 10

#define MAX_TOKENS 100


/**
 * Types of tokens:
 */
typedef enum
{
    T_UNDEFINED = 0,
    T_TEXT,
    T_CHOISE,
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

    srand(time(NULL));
    return nodes[rand() % number_of_nodes];
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
    else if (node->type == T_CHOISE || node->type == T_LIST) {
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
    return (character == '(' || character == ')' || character == '|');
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
void make_phrase(const char *pattern, char *expression, int max_symbols)
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
        else if (strcmp(token, ")") == 0) {
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
    compile(root, expression, &position, max_symbols);


    /* Free memory */
    for (int i = 0; i < MAX_TOKENS; ++i) {
        free(tokens[i]);
    }

    free(tokens);

    /* Destroy syntax tree and free memory */
    destroy_node_with_childs(root);
}



int main()
{
    char *pattern = "";

    char buffer[1000];

    char *ptr = buffer;
    make_phrase(pattern, ptr, 1000);

    printf("'%s'\n", ptr);

    return 0;
}
