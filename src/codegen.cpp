#include "node.h"
#include "codegen.h"

static IRBuilder<> Builder(getGlobalContext());

void CodeGenContext::generateCode(NBlock& root) {
    /* Create the top level interpreter function to call as entry */
    llvm::ArrayRef<Type*> argTypes;
    FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), argTypes, false);
    mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
    BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);

    /* Push a new variable/block context */
    pushBlock(bblock);
    root.codeGen(*this); /* emit bytecode for the toplevel block */
    ReturnInst::Create(getGlobalContext(), bblock);
    popBlock();

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
        return FunctionType::get(Type::getInt64Ty(getGlobalContext()), false);
    } else if (type.name.compare("Double") == 0) {
        return FunctionType::get(Type::getDoubleTy(getGlobalContext()), false);
    } else {
        return FunctionType::get(Type::getVoidTy(getGlobalContext()), false);
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
    if (context.locals().find(name) == context.locals().end()) {
        std::cerr << "undeclared variable " << name << std::endl;
        return NULL;
    }
    return new LoadInst(context.locals()[name], "", false, context.currentBlock());
}

Value* NVariableDeclaration::codeGen(CodeGenContext& context) {
    std::cout << "Creating variable declaration "
        << id->name << " : " << type->name << std::endl;
    AllocaInst *alloc = new AllocaInst(PointerType::getUnqual(typeOf(*type)), id->name.c_str(), context.currentBlock());
    context.locals()[id->name] = alloc;
    if (expr != NULL) {
        new StoreInst(expr->codeGen(context), context.locals()[id->name], false, context.currentBlock());
    }
    return alloc;
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
