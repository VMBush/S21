#ifndef GRAPH_H
#define GRAPH_H

struct elem {
    char type;
    double data;
    char lex;
    struct elem* next;
};

struct stack_lex {
    char type;
    int prior;
    struct stack_lex* next;
};

struct stack_nums {
    double data;
    struct stack_nums* next;
};

//все ф-ции, работающие с очередью elem, возвращают ссылку на новый пустой элемент очереди
char* get_user_input();
struct elem* get_polish(char* source);
//в cur лежит следующий пустой элемент
void out_insert(struct elem** cur, double data, char type);
//в cur лежит следующий пустой элемент
void stack_insert(struct stack_lex** lexemms, struct elem** cur, char data);
void read_elem(char** p_source, struct elem** p_cur, struct stack_lex** p_lex_stak);
//в cur лежит следующий пустой элемент
void read_num(char** source, struct elem** cur);
void read_lex(char** source, struct elem** cur, struct stack_lex** lex_stack);
char pop_lexem(struct stack_lex** p_lexemms);
void make_graph(struct elem* polish);
void normalize_data(double* y_data);
double solve_polish(struct elem* polish, double x);
void do_action(char action, struct stack_nums** p_num_stack);
void destroy_elem(struct elem* root);

#endif