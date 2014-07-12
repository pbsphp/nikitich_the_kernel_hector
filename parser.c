#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef enum
{
    T_UNDEFINED = 0,
    T_TEXT,
    T_CHOISE,
    T_LIST
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



int main()
{
    char *tokens[] = { "text", "(", "first", "second", ")", "ending" };

    Node *root = (Node *) malloc(sizeof(Node));
    root->parent = NULL;
    root->number_of_childs = 0;
    root->type = T_LIST;

    Node *current = root;


    int i;
    for (i = 0; i < 6; ++i) {
        char *token = tokens[i];

        if (strcmp(token, "(") == 0) {
            current = create_node(current, T_CHOISE, "");
        }
        else if (strcmp(token, ")") == 0) {
            current = current->parent;
        }
        else {
            create_node(current, T_TEXT, token);
        }
    }


    recursive_print(root, 0);


    destroy_node_with_childs(root);

    return 0;
}

