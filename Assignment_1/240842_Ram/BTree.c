#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

// Given that the order of the B-tree is 4
// Each node has atmost 4 key-val pairs (at-least 2) and atmost 5 children

typedef struct {
    int key;
    char value[100];
} key_val;

typedef struct node{
    int current_vals;
    key_val* values;
    int num_children;
    struct node** children; //pointer to an array of pointers each of which will point to a child node
    bool leaf;
} node;

key_val* search_within_tree(node* head, int element){
    if(head == NULL)
        return NULL;
    // Initially in the recursive approach I was trying I was recursively going to each child node to look for the element if it was not in the parent node
    // I later realised that the point of the sorted elements in the binary tree is to prevent such silly exorbitant computational costs :)
    int i = 0;
    while(i < head->current_vals && element > head->values[i].key)
        i++;
    if(i < head->current_vals && element == head->values[i].key)
        return &(head->values[i]);
    if(head->leaf == true)
        return NULL;
    return search_within_tree(head->children[i], element);
}

// Do not need a double pointer here since I am not altering the pointer itself, just manipulating what the pointer points to
void split_node(node* parent, int i, node* y) {
    node* z = (node*)malloc(sizeof(node)); 
    z->values = (key_val*)malloc(4 * sizeof(key_val));
    z->children = (node**)malloc(5 * sizeof(node*));
    z->leaf = y->leaf;
    z->current_vals = 2;
    for (int j = 0; j < 2; j++)
        z->values[j] = y->values[j+3];
    if (y->leaf == false) {
        z->num_children = 3;
        for (int j = 0; j < 3; j++)
            z->children[j] = y->children[j+3];
    } else {
        z->num_children = 0;
    }
    y->current_vals = 2;
    if (y->leaf == false) {
        y->num_children = 3;
    }
   for (int j = parent->num_children; j > i+1; j--)
    parent->children[j] = parent->children[j-1];
    parent->children[i+1] = z;
    for (int j = parent->current_vals - 1; j >= i; j--)
        parent->values[j + 1] = parent->values[j];
    parent->values[i] = y->values[2];
    parent->current_vals = parent->current_vals + 1;
    parent->num_children = parent->num_children + 1;
}

void insert_non_full(node* head, key_val element) {
    int i = head->current_vals - 1;
    if (head->leaf == true) {
        while (i >= 0 && head->values[i].key > element.key) {
            head->values[i+1] = head->values[i];
            i--;
        }
        head->values[i+1] = element;
        head->current_vals = head->current_vals + 1;
    } else {
        while (i >= 0 && head->values[i].key > element.key)
            i--;
        if (head->children[i+1]->current_vals == 4) {
            split_node(head, i, head->children[i+1]);
            if (head->values[i].key < element.key)
                i++;
        }
        insert_non_full(head->children[i+1], element);
    }
}


// Was initially using node* head but then I realized that this would result in the pointer passing by value. Now, sending the address of the pointer to the node.
void insert_element(node** head, key_val element) {
    if (*head == NULL) {
        // Empty tree, create the first node
        *head = (node*)malloc(sizeof(node));
        (*head)->values = (key_val*)malloc(4 * sizeof(key_val));
        (*head)->children = (node**)malloc(5 * sizeof(node*));
        (*head)->leaf = true;
        (*head)->values[0] = element;
        (*head)->current_vals = 1;
        (*head)->num_children = 0;
    } else {
        if ((*head)->current_vals == 4) {
            // Node is full, split it
            node* s = (node*)malloc(sizeof(node));
            s->values = (key_val*)malloc(4 * sizeof(key_val));
            s->children = (node**)malloc(5 * sizeof(node*));
            s->leaf = false;
            s->current_vals = 0;
            s->num_children = 1;
            s->children[0] = *head;
            split_node(s, 0, *head);
            int i = 0;
            if (s->values[0].key < element.key)
                i++;
            insert_non_full(s->children[i], element);
            *head = s;
        } else {
            insert_non_full(*head, element);
        }
    }
}

void delete_node(node** head, int del_key){
    
}

void print_tree(node* head, int level) {
    if (head == NULL) return;
    static int current_level = -1;
    static bool first_node_in_level = true;
    if (level > current_level) {
        if (current_level >= 0) printf("\n");
        current_level = level;
        first_node_in_level = true;
    }
    if (!first_node_in_level) printf("  ");
    first_node_in_level = false;
    printf("[");
    for (int i = 0; i < head->current_vals; i++) {
        if (i > 0) printf(" ");
        printf("[%d,%s]", head->values[i].key, head->values[i].value);
    }
    printf("]");
    if (head->num_children > 0) {
        for (int i = 0; i < head->num_children; i++) {
            print_tree(head->children[i], level + 1);
        }
    }
}

int main() {
    node* root = NULL;
    int choice, key;
    char value[100];
    key_val element;
    key_val* result;
    while (1) {
        printf("\nWelcome to the Ram Daftari Database\n");
        printf("1. Insert a key-value pair\n");
        printf("2. Search for a value based on a key\n");
        printf("3. Display the B-Tree\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                printf("Enter key: ");
                scanf("%d", &key);
                result = search_within_tree(root, key);
                if (result != NULL) {
                    printf("Key already exists in the tree : %d : %s\n", result->key, result->value);
                    break;
                }
                printf("Enter value: ");
                scanf("%s", value);
                element.key = key;
                strcpy(element.value, value);
                insert_element(&root, element);
                printf("Inserted successfully!\n");
                break;
            case 2:
                printf("Enter key to search: ");
                scanf("%d", &key);
                result = search_within_tree(root, key);
                if (result != NULL) {
                    printf("Found: %d -> %s\n", result->key, result->value);
                } else {
                    printf("Key not found!\n");
                }
                break;
            case 3:
                printf("\nB-Tree Structure:\n");
                if (root == NULL) {
                    printf("Tree is empty!\n");
                } else {
                    print_tree(root, 0);
                }
                break;
            case 4:
                exit(0);
            default:
                printf("Choose a valid operation!\n");
        }
    }
    return 0;
}
