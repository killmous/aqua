#include "node.h"
#include "codegen.h"

static IRBuilder<> Builder(getGlobalContext());

void CodeGenContext::generateCode(NBlock& root) {
    std::cout << "Generating code...\n";

    /* Create the top level interpreter function to call as entry */
    FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), false);
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
    return ConstantInt::get(Type::getInt64Ty(getGlobalContext()), value, true);
}

Value* NDouble::codeGen(CodeGenContext& context) {
    std::cout << "Creating double: " << value << std::endl;
    return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), value);
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
    std::cout << "Creating variable declaration " << id->name << " : " << type->name << std::endl;
    AllocaInst *alloc = new AllocaInst(typeOf(*type), id->name.c_str(), context.currentBlock());
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
