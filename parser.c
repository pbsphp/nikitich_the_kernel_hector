#include <stdio.h>
#include <stdlib.h>
#include <string.h>



enum
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
