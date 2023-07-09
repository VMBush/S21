#ifndef GRAPH_H
#define GRAPH_H

struct elem {
    char type;
    double data;
    char lex;
    struct elem *next;
};

struct stack_lex {
    char type;
    int prior;
    struct stack_lex *next;
};

struct stack_nums {
    double data;
    struct stack_nums *next;
};

#endif