#include "node.h"
#include "codegen.h"

static IRBuilder<> Builder(getGlobalContext());

void CodeGenContext::generateCode(NBlock& root) {
    /* Push a new variable/block context */
    root.codeGen(*this); /* emit bytecode for the toplevel block */

    /* Print the bytecode in a human-readable format
       to see if our program compiled properly
     */
    std::cout << "Code is generated.\n";
    PassManager pm;
    pm.add(createPrintModulePass(&outs()));
    pm.run(*module);
}

GenericValue CodeGenContext::runCode() {
    std::cout << "Running code...\n";
    mainFunction = module->getFunction("main");
    std::vector<GenericValue> noargs;
    LLVMInitializeNativeTarget();
    ee = llvm::EngineBuilder(module).create();
    GenericValue v = ee->runFunction(mainFunction, noargs);
    std::cout << "Code was run.\n";
    return v;
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(const NIdentifier& type) {
    if (type.name.compare("Int") == 0) {
        return Type::getInt64Ty(getGlobalContext());
    } else if (type.name.compare("Double") == 0) {
        return Type::getDoubleTy(getGlobalContext());
    } else {
        return Type::getVoidTy(getGlobalContext());
    }
}

Value* NInteger::codeGen(CodeGenContext& context) {
    std::cout << "Creating integer: " << value << std::endl;

    FunctionType *ftype = FunctionType::get(Type::getInt64Ty(getGlobalContext()), false);
    Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, "", context.module);
    BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);

    Builder.SetInsertPoint(bblock);
    Builder.CreateRet(ConstantInt::get(Type::getInt64Ty(getGlobalContext()), value, true));

    return function;
}

Value* NDouble::codeGen(CodeGenContext& context) {
    std::cout << "Creating double: " << value << std::endl;

    FunctionType *ftype = FunctionType::get(Type::getDoubleTy(getGlobalContext()), false);
    Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, "", context.module);
    BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);

    Builder.SetInsertPoint(bblock);
    Builder.CreateRet(ConstantFP::get(Type::getDoubleTy(getGlobalContext()), value));

    return function;
}

Value* NIdentifier::codeGen(CodeGenContext& context) {
    std::cout << "Creating identifier reference: " << name << std::endl;
    /*if (context.locals().find(name) == context.locals().end()) {
        std::cerr << "undeclared variable " << name << std::endl;
        return NULL;
    }*/
    return context.module->getFunction(name);
}

Value* NDeclaration::codeGen(CodeGenContext& context) {
    std::cout << "Creating variable declaration "
        << id->name << " : " << (*typesig)[0]->name << std::endl;
    FunctionType *ftype = FunctionType::get(typeOf(*(*typesig)[0]), false);
    Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, id->name, context.module);
    BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);
    Function* ret = (Function *)expr->codeGen(context);
    Builder.SetInsertPoint(bblock);
    Builder.CreateRet(Builder.CreateCall(ret));

    return function;
}

Value* NBlock::codeGen(CodeGenContext& context) {
    Value *last = NULL;
    for(auto it : statementlist) {
        std::cout << "Generating code for " << typeid(*it).name() << std::endl;
        last = it->codeGen(context);
    }
    std::cout << "Creating block" << std::endl;
    return last;
}
