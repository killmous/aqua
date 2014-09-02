#ifndef __NODE_H__
#define __NODE_H__

#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

class CodeGenContext;

class Node {
public:
    virtual ~Node() {}
    virtual llvm::Value* codeGen(CodeGenContext& context) {}
};

class NExpression : public Node {};
class NStatement : public Node {};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(std::string name)
        : name(name) {};

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement {
public:
    NIdentifier* id;
    NIdentifier* type;
    NExpression* expr;
    NVariableDeclaration(NIdentifier* id, NIdentifier* type, NExpression* expr)
        : id(id), type(type), expr(expr) {}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value)
        : value(value) {}

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NDouble : public NExpression {
public:
    double value;
    NDouble(double value)
        : value(value) {}

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

typedef std::vector<NStatement*> NStatementList;

class NBlock : public NStatement {
public:
    NStatementList statementlist;

    NBlock(void) {};
    void addStatement(NStatement* statement) {
        this->statementlist.push_back(statement);
    }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

#endif