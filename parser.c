#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


typedef enum
{
    T_UNDEFINED = 0,
    T_TEXT,
    T_CHOISE,
    T_LIST,
    T_SEPARATOR
} token_type;



typedef struct Node
{
    struct Node *childs[1000];
    int number_of_childs;

    struct Node *parent;


    token_type type;
    char content[1000];

} Node;



Node *create_node(Node *parent, token_type type, char *content)
{
    Node *node = (Node *) malloc(sizeof(Node));
    node->number_of_childs = 0;

    node->type = type;
    strcpy(node->content, content);

    node->parent = parent;
    if (parent) {
        parent->childs[parent->number_of_childs++] = node;
    }

    return node;
}


void destroy_node_with_childs(Node *node)
{
    if (node) {
        int i;
        for (i = 0; i < node->number_of_childs; ++i) {
            destroy_node_with_childs(node->childs[i]);
        }

        free(node);
    }
}


Node *choose_random(Node **nodes, unsigned int number_of_nodes)
{
    if (number_of_nodes == 0) {
        return NULL;
    }

    srand(time(NULL));
    return nodes[rand() % number_of_nodes];
}


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


void compile(Node *node, char *destination, int *pos)
{
    char *p = NULL;
    Node *random = NULL;
    int i;

    switch (node->type) {
    case T_TEXT:
        p = node->content;
        while (*p) {
            destination[(*pos)++] = *p++;
        }
        destination[*pos] = '\0';
        break;

    case T_CHOISE:
        random = choose_random(node->childs, node->number_of_childs);
        compile(random, destination, pos);
        break;

    case T_LIST:
        for (i = 0; i < node->number_of_childs; ++i) {
            compile(node->childs[i], destination, pos);
        }

    default:
    break;
    }
}


int is_separator(char character)
{
    return (character == '(' || character == ')' || character == '|');
}


void get_token(const char *expression, char *buffer)
{
    static unsigned int position = 0;

    if (is_separator(expression[position])) {
        *buffer++ = expression[position++];
    }
    else {
        while (expression[position] && !is_separator(expression[position])) {
            *buffer++ = expression[position++];
        }
    }
    *buffer = '\0';
}


void tokenize(const char *expression, char **tokens)
{
    char buffer[1000];
    char *token = NULL;
    int token_number = 0;

    while (1) {
        token = buffer;
        get_token(expression, token);

        if (*token == '\0') break;
        strcpy(tokens[token_number++], token);
    }

    tokens[token_number] = NULL;
}


void make_phrase(const char *expression, char *destination)
{
    char **tokens = (char **) malloc(sizeof(char *) * 100);
    for (int i = 0; i < 100; ++i) {
        tokens[i] = (char *) malloc(sizeof(char) * 100);
    }

    tokenize(expression, tokens);

    Node *root = create_node(NULL, T_LIST, "");

    Node *current = root;


    for (int i = 0; tokens[i] != NULL; ++i) {
        char *token = tokens[i];

        if (strcmp(token, "(") == 0) {
            current = create_node(current, T_CHOISE, "");
        }
        else if (strcmp(token, ")") == 0) {
            current = current->parent;
        }
        else if (strcmp(token, "|") == 0) {
            /* Separator, do nothing */
        }
        else {
            create_node(current, T_TEXT, token);
        }
    }


    int position = 0;
    compile(root, destination, &position);


    for (int i = 0; i < 100; ++i) {
        free(tokens[i]);
    }

    free(tokens);

    destroy_node_with_childs(root);
}



int main()
{
    char *expression = "begin (first|( A| B)) end";
    char buffer[1000];

    char *ptr = buffer;
    make_phrase(expression, ptr);

    printf("%s\n", ptr);

    return 0;
}
