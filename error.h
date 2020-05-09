#ifndef ERROR_H
#define ERROR_H

struct Error {
    enum class ErrorType {SYNTAX, ERROR, WARN, VOID} type;
    enum class ErrorID {SYNTAX, NOVAR, CONV, EXPR, VOID,
                        REVAR, UNUSED, UNINIT, CONST} id;
    int row;
    int col;
};

#endif /*ERROR_H*/