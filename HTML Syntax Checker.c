#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STACK_SIZE 100
#define MAX_TAG_SIZE 100
#define MAX_CODE_SIZE 10000

typedef struct {
    char* items[MAX_STACK_SIZE];
    int top;
    char* fullTags[MAX_STACK_SIZE];
} Stack;

void initStack(Stack* stack) {
    stack->top = -1;
    int i;
    for ( i = 0; i < MAX_STACK_SIZE; i++) {
        stack->fullTags[i] = NULL;
    }
}

bool isStackEmpty(Stack* stack) {
    return stack->top == -1;
}

bool isStackFull(Stack* stack) {
    return stack->top == MAX_STACK_SIZE - 1;
}

void push(Stack* stack, const char* item, const char* fullTag) {
    if (!isStackFull(stack)) {
        stack->items[++stack->top] = strdup(item);
        stack->fullTags[stack->top] = strdup(fullTag);
    }
}

char* pop(Stack* stack) {
    if (!isStackEmpty(stack)) {
        free(stack->fullTags[stack->top]);
        return stack->items[stack->top--];
    }
    return NULL;
}

char* peek(Stack* stack) {
    if (!isStackEmpty(stack)) {
        return stack->items[stack->top];
    }
    return NULL;
}

void freeStack(Stack* stack) {
    while (!isStackEmpty(stack)) {
        free(pop(stack));
    }
}

void extractTagName(const char* tag, char* tagName) {
    int i = 1;
    int j = 0;
    while (tag[i] != '>' && tag[i] != '\0') {
        if (tag[i] != '/') {
            tagName[j++] = tag[i];
        }
        i++;
    }
    tagName[j] = '\0';
}

void checkHTMLCode(const char* code) {
    Stack stack;
    initStack(&stack);
    
    const char* p = code;
    char tag[MAX_TAG_SIZE];
    char tagName[MAX_TAG_SIZE];
    bool insideTag = false;
    int tagIndex = 0;

    while (*p != '\0') {
        if (*p == '<') {
            insideTag = true;
            tagIndex = 0;
            tag[tagIndex++] = *p;
        } else if (*p == '>') {
            if (insideTag) {
                tag[tagIndex++] = *p;
                tag[tagIndex] = '\0';
                extractTagName(tag, tagName);

                if (tag[1] == '/') {
                    if (isStackEmpty(&stack)) {
                        printf("error %s\n", tag);
                        freeStack(&stack);
                        return;
                    }
                    char* topTag = pop(&stack);
                    if (strcmp(topTag, tagName) != 0) {
                        printf("error %s\n", tag);
                        free(topTag);
                        freeStack(&stack);
                        return;
                    }
                    free(topTag);
                } else {
                    push(&stack, tagName, tag);
                }
                insideTag = false;
            }
        } else if (insideTag) {
            tag[tagIndex++] = *p;
        }
        p++;
    }

    if (!isStackEmpty(&stack)) {
        printf("error %s\n", stack.fullTags[stack.top]);
    } else {
        printf("correct\n");
    }
    
    freeStack(&stack);
}

int main() {
    char code[MAX_CODE_SIZE];

    size_t length = 0;
    while (fgets(code + length, sizeof(code) - length, stdin)) {
        length += strlen(code + length);
        if (length >= MAX_CODE_SIZE - 1 || code[length - 1] == '\n') {
            break;
        }
    }

    checkHTMLCode(code);
    return 0;
}