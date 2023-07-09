#include "graph.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int main() {
    char* source = get_user_input();

    struct elem* polish = get_polish(source);
    struct elem* cur_polish = polish;

    // while (cur_polish->next) {
    //     if (cur_polish->type == 'l') {
    //         printf("%c", cur_polish->lex);
    //     }
    //     if (cur_polish->type == 'd') {
    //         printf("%.0lf", cur_polish->data);
    //     }
    //     if (cur_polish->type == 'x') {
    //         printf("x");
    //     }
    //     cur_polish = cur_polish->next;
    // }

    make_graph(polish);
    //! free(polish);
    free(source);
    destroy_elem(polish);
    return 0;
}

char* get_user_input() {
    int size = 0;
    char ch = getchar();
    char* string = 0;

    while (ch != '\n') {
        size++;
        string = realloc(string, size + 1 * sizeof(char));
        string[size - 1] = ch;
        ch = getchar();
    }
    string[size] = '\0';

    return string;
}

struct elem* get_polish(char* source) {
    struct elem* root = malloc(sizeof(struct elem));
    struct elem* cur = root;
    struct stack_lex* lexemms = 0;

    if (*source == '-') {
        out_insert(&cur, 0, 'd');
        stack_insert(&lexemms, &cur, '-');
        source++;
    }

    while (*source != '\0') {
        read_elem(&source, &cur, &lexemms);
    }
    //плюнуть все лексеммы
    while (lexemms) {
        out_insert(&cur, pop_lexem(&lexemms), 'l');
    }

    //удалить последний элем
    struct elem* to_del = root;
    while (to_del->next != cur) {
        to_del = to_del->next;
    }
    free(to_del->next);
    to_del->next = 0;

    return root;
}

void read_elem(char** p_source, struct elem** p_cur, struct stack_lex** p_lex_stack) {
    char* source = *p_source;
    struct elem* cur = *p_cur;
    struct stack_lex* lex_stack = *p_lex_stack;

    if (*source == 'x') {
        out_insert(&cur, 0, 'x');
        source++;
    } else if (*source >= '0' && *source <= '9') {
        read_num(&source, &cur);
    } else {
        read_lex(&source, &cur, &lex_stack);
    }

    *p_source = source;
    *p_cur = cur;
    *p_lex_stack = lex_stack;
}

void read_num(char** p_source, struct elem** p_cur) {
    char* source = *p_source;
    struct elem* cur = *p_cur;

    double data = 0;
    while (*source >= '0' && *source <= '9') {
        data *= 10;
        data += *source - '0';
        source++;
    }
    if (*source == '.') {
        source++;

        int decs = 1;
        while (*source >= '0' && *source <= '9') {
            decs *= 10;
            data *= 10;
            data += *source - '0';
            source++;
        }
        data = data / decs;
    }
    out_insert(&cur, data, 'd');
    *p_source = source;
    *p_cur = cur;
}

void read_lex(char** p_source, struct elem** p_cur, struct stack_lex** p_lex_stack) {
    char* source = *p_source;
    struct elem* cur = *p_cur;
    struct stack_lex* lex_stack = *p_lex_stack;

    if (*source == '(') {
        stack_insert(&lex_stack, &cur, '(');
        source++;
        if (*source == '-') {
            out_insert(&cur, 0, 'd');
            stack_insert(&lex_stack, &cur, '-');
            source++;
        }
    } else if (strchr("+-*/)", *source)) {
        stack_insert(&lex_stack, &cur, *source);
        source++;
    } else if (*source == 's' && *(source + 1) == 'i') {
        stack_insert(&lex_stack, &cur, *source);
        source += 3;
    } else if (*source == 's') {
        stack_insert(&lex_stack, &cur, 'S');
        source += 4;
    } else if (*source == 'c' && *(source + 1) == 'o') {
        stack_insert(&lex_stack, &cur, *source);
        source += 3;
    } else if (*source == 'c') {
        stack_insert(&lex_stack, &cur, 'C');
        source += 3;
    } else if (*source == 't') {
        stack_insert(&lex_stack, &cur, 't');
        source += 3;
    } else {
        stack_insert(&lex_stack, &cur, 'l');
        source += 2;
    }
    *p_source = source;
    *p_cur = cur;
    *p_lex_stack = lex_stack;
}

void out_insert(struct elem** p_cur, double data, char type) {
    struct elem* cur = *p_cur;

    if (type == 'x') {
        cur->type = 'x';
    } else if (type == 'd') {
        cur->type = 'd';
        cur->data = data;
    } else if (type == 'l') {
        cur->type = 'l';
        cur->lex = data;
    }

    cur->next = malloc(sizeof(struct elem));
    cur = cur->next;

    *p_cur = cur;
}

void stack_insert(struct stack_lex** p_lexemms, struct elem** p_cur, char data) {
    struct elem* cur = *p_cur;
    struct stack_lex* lexemms = *p_lexemms;

    struct stack_lex* income_lex = malloc(sizeof(struct stack_lex));
    income_lex->type = data;

    if (strchr("+-", data)) {
        income_lex->prior = 0;
    } else if (strchr("*/", data)) {
        income_lex->prior = 1;
    } else if (strchr("sctCSl", data)) {
        income_lex->prior = 2;
    } else if (strchr("()", data)) {
        income_lex->prior = 3;
    }

    if (data == '(') {
        income_lex->next = lexemms;
        lexemms = income_lex;
        
    } else if (data == ')') {
        //если элемент ), плюнуть все до ( и первую скобку
        while (lexemms->type != '(') {
            out_insert(&cur, pop_lexem(&lexemms), 'l');
        }
        free(income_lex);
    } else {
        while (lexemms && lexemms->type != '(' && lexemms->prior >= income_lex->prior) {
            out_insert(&cur, pop_lexem(&lexemms), 'l');
        }
        income_lex->next = lexemms;
        lexemms = income_lex;
    }

    *p_cur = cur;
    *p_lexemms = lexemms;
}

char pop_lexem(struct stack_lex** p_lexemms) {
    struct stack_lex* to_del = *p_lexemms;

    char data = to_del->type;
    *p_lexemms = to_del->next;
    free(to_del);

    return data;
}

void make_graph(struct elem* polish) {
    double y_data[80];
    double x = 0;
    double step = 4 * M_PI / 79;
    for (int i = 0; i < 80; i++) {
        x = i * step;
        y_data[i] = solve_polish(polish, x);
    }

    normalize_data(y_data);
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 80; j++) {
            if (round(y_data[j]) == 12 - i)
                printf("*");
            else
                printf(".");
        }
        if (i < 79) printf("\n");
    }
}

void normalize_data(double* data) {
    double max = fabs(data[0]);
    for (int i = 0; i < 80; i++) {
        if (fabs(data[i]) > max) {
            max = fabs(data[i]);
        }
    }
    for (int i = 0; i < 80; i++) {
        data[i] = data[i] / max * 12;
    }
}

double solve_polish(struct elem* polish, double x) {
    struct stack_nums* num_stack = 0;
    while (polish) {
        if (polish->type == 'x') {
            struct stack_nums* new = malloc(sizeof(struct stack_nums));
            new->next = num_stack;
            num_stack = new;
            new->data = x;
        } else if (polish->type == 'd') {
            struct stack_nums* new = malloc(sizeof(struct stack_nums));
            new->next = num_stack;
            num_stack = new;
            new->data = polish->data;
        } else if (polish->type == 'l') {
            do_action(polish->lex, &num_stack);
        }

        polish = polish->next;
    }
    double result = num_stack->data;
    free(num_stack);

    return result;
}

void do_action(char action, struct stack_nums** p_num_stack) {
    struct stack_nums* num_stack = *p_num_stack;

    if (action == '+') {
        num_stack->next->data += num_stack->data;
        struct stack_nums* del = num_stack;
        num_stack = num_stack->next;
        free(del);
    } else if (action == '-') {
        num_stack->next->data -= num_stack->data;
        struct stack_nums* del = num_stack;
        num_stack = num_stack->next;
        free(del);
    } else if (action == '*') {
        num_stack->next->data *= num_stack->data;
        struct stack_nums* del = num_stack;
        num_stack = num_stack->next;
        free(del);
    } else if (action == '/') {
        num_stack->next->data /= num_stack->data;
        struct stack_nums* del = num_stack;
        num_stack = num_stack->next;
        free(del);
    } else if (action == 's') {
        num_stack->data = sin(num_stack->data);
    } else if (action == 'c') {
        num_stack->data = cos(num_stack->data);
    } else if (action == 't') {
        num_stack->data = tan(num_stack->data);
    } else if (action == 'C') {
        num_stack->data = 1 / tan(num_stack->data);
    } else if (action == 'S') {
        num_stack->data = sqrt(num_stack->data);
    } else if (action == 'l') {
        num_stack->data = log(num_stack->data);
    }

    *p_num_stack = num_stack;
}

void destroy_elem(struct elem* root) {
    if (root->next) {
        destroy_elem(root->next);
    }
    free(root);
}