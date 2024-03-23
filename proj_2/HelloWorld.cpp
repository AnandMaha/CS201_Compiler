#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
using namespace llvm;
using namespace std;

namespace
{
    std::map<Value *, int> operandTable;  // for operands (ex. a)
    std::map<std::string, int> exprTable; // for expressions (ex. a + b)
    int valueNum = 1;                     // global value number counter

    int updateOperandTable(const Value *operand, bool &exists, int val = -1)
    { // check if operand in operandTable
        auto result = operandTable.find(const_cast<Value *>(operand));
        if (result != operandTable.end())
        { // old operand
            exists = true;
            if (val > 0)
            { // explicit value num to set
                result->second = val;
            }
            return result->second; // already exists; use value num from before
        }
        else
        { // new operand
            exists = false;
            if (val > 0)
            {                                                            // explicit value num to set
                operandTable.emplace(const_cast<Value *>(operand), val); // add new operand to operandTable
                return val;
            }
            else
            {
                operandTable.emplace(const_cast<Value *>(operand), valueNum); // add new operand to operandTable
                return valueNum++;                                            // new operand means valueNum + 1 for later
            }
        }
    }

    int updateExprTable(const std::string expr, bool &exists)
    { // check if expr in exprTable
        auto result = exprTable.find(expr);
        if (result != exprTable.end())
        { // old expr
            exists = true;
            return result->second; // already exists; use value num from before
        }
        else
        {                                                                  // new expr
            exprTable.insert(std::pair<std::string, int>(expr, valueNum)); // add new expr to exprTable
            exists = false;
            return valueNum++; // new expr means valueNum + 1 for later
        }
    }

    void visitor(Function &fn)
    {
        auto functionName = fn.getName();
        errs() << "ValueNumbering: " << functionName << "\n";

        for (auto &basic_block : fn)
        {
            for (auto &inst : basic_block){
                bool exists = false; // for expr existence
                if (inst.getOpcode() == Instruction::Load ||
                    inst.getOpcode() == Instruction::Store ||
                    inst.isBinaryOp()){
                    errs() << formatv("{0,-40}", inst);
                }
                if (inst.getOpcode() == Instruction::Load){
                    int srcValueNum = updateOperandTable(inst.getOperand(0), exists);
                    int dstValueNum = updateOperandTable(&inst, exists, srcValueNum);
                    errs() << dstValueNum << " = " << srcValueNum << "\n";

                    // errs() << &inst << " " << inst.getOperand(0) << "\n";
                }
                if (inst.getOpcode() == Instruction::Store){
                    int srcValueNum = updateOperandTable(inst.getOperand(0), exists);
                    int dstValueNum = updateOperandTable(inst.getOperand(1), exists, srcValueNum); // same value num
                    errs() << dstValueNum << " = " << srcValueNum << "\n";

                    // errs() << inst.getOperand(0) << " " << inst.getOperand(1) << "\n";
                }
                if (inst.isBinaryOp()){
                    std::string op; // operator choice
                    switch (inst.getOpcode()){
                        case Instruction::Add:
                            op = "add";
                            break;
                        case Instruction::Sub:
                            op = "sub";
                            break;
                        case Instruction::Mul:
                            op = "mul";
                            break;
                        case Instruction::UDiv:
                            op = "udiv";
                            break;
                        case Instruction::SDiv:
                            op = "sdiv";
                            break;
                    }
                    int lhsValueNum = updateOperandTable(inst.getOperand(0), exists);
                    int rhsValueNum = updateOperandTable(inst.getOperand(1), exists);
                    std::string expr = std::to_string(lhsValueNum) + " " + op + " " + std::to_string(rhsValueNum);
                    int dstValueNum = updateExprTable(expr, exists); // use exprTable for expressions (string, NOT Value*)
                    errs() << dstValueNum << " = " << expr;
                    if (exists)
                    { // expr already exists
                        errs() << " (redundant)";
                    }
                    errs() << "\n";
                    updateOperandTable(&inst, exists, dstValueNum); // assigned variable, same value num as expr

                    // errs() << &inst << " " << inst.getOperand(0) << " " << inst.getOperand(1) << "\n";
                }
            }
        }
    }

    struct HelloWorld : PassInfoMixin<HelloWorld>
    {
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &)
        {
            visitor(F);
            return PreservedAnalyses::all();
        }
        static bool isRequired() { return true; }
    };
} // namespace

llvm::PassPluginLibraryInfo getHelloWorldPluginInfo()
{
    return {LLVM_PLUGIN_API_VERSION, "HelloWorld", LLVM_VERSION_STRING,
            [](PassBuilder &PB)
            {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>)
                    {
                        if (Name == "hello-world")
                        {
                            FPM.addPass(HelloWorld());
                            return true;
                        }
                        return false;
                    });
            }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo()
{
    return getHelloWorldPluginInfo();
}
