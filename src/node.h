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
typedef std::vector<NExpression*> ExpressionList;
class NStatement : public Node {};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(std::string name)
        : name(name) {};

    virtual llvm::Value* codeGen(CodeGenContext& context);
};
typedef std::vector<NIdentifier*> TypeSignature;

class NVariableDeclaration : public NStatement {
public:
    NIdentifier* id;
    NIdentifier* type;
    NExpression* expr;
    NVariableDeclaration(NIdentifier* id, NIdentifier* type, NExpression* expr)
        : id(id), type(type), expr(expr) {}

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionDefinition : public NStatement {
public:
    NIdentifier* id;
    TypeSignature typesig;
    NFunctionDefinition(NIdentifier* id, TypeSignature typesig)
        : id(id), typesig(typesig) {}

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NExpressionStatement : public NStatement {
public:
    NExpression* expression;
    NExpressionStatement(NExpression* expression)
        : expression(expression) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NMethodCall : public NExpression {
public:
    NIdentifier* id;
    ExpressionList arguments;
    NMethodCall(NIdentifier* id, ExpressionList arguments)
        : id(id), arguments(arguments) { }

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

class NChar : public NExpression {
public:
    char value;
    NChar(char value)
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