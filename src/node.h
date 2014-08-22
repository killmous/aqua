#ifndef __NODE_H__
#define __NODE_H__

#include <iostream>
#include <vector>

class Node {
public:
    virtual ~Node() {}
};

class NExpression : public Node {};
class NStatement : public Node {};

class NIdentifier : public NExpression {
public:
    std::string* name;
    NIdentifier(std::string* name)
        : name(name) {};
};

class NDeclaration : public NStatement {
public:
    NIdentifier* id;
    NExpression* expr;
    NDeclaration(NIdentifier* id, NExpression* expr)
        : id(id), expr(expr) {}
};

class NInteger : public NExpression {
public:
    long long num;
    NInteger(long long num)
        : num(num) {}
};

typedef std::vector<NStatement*> NStatementList;

class NBlock : public NStatement {
public:
    NStatementList statementlist;

    NBlock(void) {};
    void addStatement(NStatement* statement) {
        this->statementlist.push_back(statement);
    }

    void exec(void) {}
};

#endif