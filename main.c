#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <io.h>
#define fileno _fileno
#define read _read
#else
#include <unistd.h>
#endif

// Элемент односвязного списка
struct Node {
    char symbol;
    struct Node* next;
};

// Элемент двусвязного списка
struct TwoWayNode {
    char symbol;
    struct TwoWayNode* prev;
    struct TwoWayNode* next;
};

// typedef позволяет не набирать [struct] Node/TwoWayNode
// Элемент односвязного списка
typedef struct Node Node;
// Элемент двусвязного списка
typedef struct TwoWayNode TwoWayNode;

// Создать новый элемент с указателем и значением
Node* newNode(char symbol, Node* next) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->symbol = symbol;
    node->next = next;
    return node;
}
// Создать новый элемент
Node* newEmptyNode() {
    Node* node = (Node*)malloc(sizeof(Node));
    node->next = NULL;
    return node;
}

// Вставить элемент после указанного
void insertToList(Node* previous, Node* node) {
    Node* next = previous->next;
    previous->next = node;
    node->next = next;
}
// Вставить символ после указанного
// Возвращает новый элемент
Node* insertCharToList(Node* previous, char symbol) {
    Node* node = newNode(symbol, previous->next);
    insertToList(previous, node);
    return node;
}
// Удалить следующий элемент
// Возвращает новый следующий элемент
Node* removeNextFromList(Node* previous) {
    Node* next = previous->next->next;
    free(previous->next);
    previous->next = next;
    return next;
}
// Удалить данный элемент
// Возвращает следующий элемент
Node* removeFromList(Node* node) {
    Node* next = node->next;
    free(node);
    return next;
}


// Создать новый элемент двусвязного списка с указателями
TwoWayNode* newTwoWayNode(char symbol, TwoWayNode* previous, TwoWayNode* next) {
    TwoWayNode* node = (TwoWayNode*)malloc(sizeof(TwoWayNode));
    node->symbol = symbol;
    node->prev = previous;
    node->next = next;
    return node;
}
// Создать новый элемент двусвязного списка
TwoWayNode* newEmptyTwoWayNode() {
    TwoWayNode* node = (TwoWayNode*)malloc(sizeof(TwoWayNode));
    node->prev = NULL;
    node->next = NULL;
    return node;
}

// Вставить элемент после указанного (двусвязный список)
void insertToTwoWayList(TwoWayNode* previous, TwoWayNode* node) {
    if (previous == NULL || node == NULL) return;
    TwoWayNode* next = previous->next;

    previous->next = node;
    node->prev = previous;

    next->prev = node;
    node->next = next;
}
// Вставить символ после указанного (двусвязный список)
// Возвращает новый элемент
TwoWayNode* insertCharToTwoWayList(TwoWayNode* previous, char symbol) {
    TwoWayNode* node = newTwoWayNode(symbol, previous, previous == NULL ? NULL : previous->next);
    if (previous != NULL) {
        node = newTwoWayNode(symbol, previous, previous->next);
        previous->next = node;
        node->next->prev = node;
    }
    else {
        node = newEmptyTwoWayNode();
        node->symbol = symbol;
    }
    return node;
}
// Удалить следующий элемент (двусвязный список)
// Возвращает новый следующий элемент
TwoWayNode* removeNextFromTwoWayList(TwoWayNode* previous) {
    if (previous == NULL || previous->next == NULL) return NULL;
    TwoWayNode* next = previous->next->next;
    free(previous->next);
    previous->next = next;
    if (next != NULL) next->prev = previous;
    return next;
}
// Удалить предыдущий элемент (двусвязный список)
// Возвращает новый предыдущий элемент
TwoWayNode* removePreviousFromTwoWayList(TwoWayNode* next) {
    if (next == NULL || next->prev == NULL) return NULL;
    TwoWayNode* previous = next->prev->prev;
    free(next->prev);
    if (previous != NULL) previous->next = next;
    next->prev = previous;
    return previous;
}
// Удалить данный элемент (двусвязный список)
void removeFromTwoWayList(TwoWayNode* node) {
    if (node == NULL) return;
    if (node->prev != NULL) node->prev->next = node->next;
    if (node->next != NULL) node->next->prev = node->prev;
    free(node);
}
// Удалить данный элемент
// Возвращает следующий элемент
TwoWayNode* removeFirstFromTwoWayList(TwoWayNode* node) {
    for (; node->prev != NULL; node = node->prev);
    Node* next = node->next;
    removeFromTwoWayList(node);
    return next;
}


// Прочитать файл в односвязный список, используя буфер
Node* readToList(FILE* file, const size_t buffer_size, const char stop) {
    Node* start = newEmptyNode();
    Node* end = start;
    Node* second;
    int file_desc = fileno(file);
    char* buffer = (char*)malloc(sizeof(char) * buffer_size);
    int found_stop = 0;
    int read_size;

    while ((read_size = read(file_desc, buffer, buffer_size)) > 0) {
        for (size_t i = 0; i < read_size; i++) {
            if (buffer[i] == stop) {
                found_stop = 1;
                break;
            }
            end = insertCharToList(end, buffer[i]);
        }
        if (found_stop)
            break;
    }

    free(buffer);

    if (start->next == NULL) {
        // Строка нулевой длины, ничего не прочитано
        return start;
    }
    // Строка не нулевой длины, первый элемент пустой, строка начинается с следующего элемента
    second = start->next;
    free(start);
    return second;
}

// Прочитать файл в двусвязный список, используя буфер
TwoWayNode* readToTwoWayList(FILE* file, const size_t buffer_size, const char stop) {
    TwoWayNode* start = newEmptyTwoWayNode();
    TwoWayNode* end = start;
    TwoWayNode* second;
    int file_desc = fileno(file);
    char* buffer = (char*)malloc(sizeof(char) * buffer_size);
    int found_stop = 0;
    int read_size;

    while ((read_size = read(file_desc, buffer, buffer_size)) > 0) {
        for (size_t i = 0; i < read_size; i++) {
            if (buffer[i] == stop) {
                found_stop = 1;
                break;
            }
            insertCharToTwoWayList(end, buffer[i]);
        }
        if (found_stop)
            break;
    }

    free(buffer);

    if (start->next == NULL) {
        // Строка нулевой длины, ничего не прочитано
        return start;
    }
    // Строка не нулевой длины, первый элемент пустой, строка начинается с следующего элемента
    second = start->next;
    second->prev = NULL;
    free(start);
    return second;
}


// Удалить из строки (односвязного списка) цифры
// Возвращает указатель на начало списка
Node* removeDigitsFromList(Node* start) {
    Node* node;
    while (start != NULL && isdigit(start->symbol)) {
        node = start->next;
        free(start);
        start = node;
    }
    if (start == NULL) return NULL;
    for (node = start; node->next != NULL; node = node->next) {
        if (isdigit(node->next->symbol)) {
            node = removeNextFromList(node);
        }    
    }
    return start;
}
// Удалить из строки (двусвязного списка) цифры
// Возвращает указатель на начало списка
TwoWayNode* removeDigitsFromTwoWayList(TwoWayNode* start) {
    while (start != NULL && isdigit(start->symbol)) {
        start = start->next;
        removeFromTwoWayList(start->prev);
    }
    if (start == NULL) return NULL;
    for (TwoWayNode* node = start; node->next != NULL; node = node->next) {
        if (isdigit(node->next->symbol)) {
            node = removeNextFromTwoWayList(node);
        }
    }
    return start;
}

// Удвоить в строке (односвязного списка) символы +/-
// Возвращает указатель на начало списка
Node* doubleSignsInList(Node* start) {
    for (Node* node = start; node != NULL; node = node->next) {
        if (node->symbol == '+' || node->symbol == '-') {
            node = insertCharToList(node, node->symbol);
        }
    }
    return start;
}

// Преобразование односвязного списка в строку
char* listToChar(Node* node) {
    size_t alloc_size = 4;
    size_t str_size = 0;
    char* str = (char*)malloc(sizeof(char) * alloc_size);
    for (; node != NULL; node = node->next) {
        str[str_size++] = node->symbol;
        if (str_size >= alloc_size) {
            alloc_size *= 2;
            str = (char*)realloc(str, alloc_size);
        }
    }
    str[str_size++] = '\0';
    return realloc(str, str_size);
}

int main() {
    printf("Введите строку:\n");
    Node* str = readToList(stdin, 5, '\n');
    Node* new_str = removeDigitsFromList(doubleSignsInList(str));
    char* c_str = listToChar(new_str);
    printf("Новая строка:\n%s\n", c_str);
    free(str);
    free(new_str);
    free(c_str);


    return 0;
}
