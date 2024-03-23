#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"     
#include "llvm/ADT/StringRef.h"
#include <map>
#include <set>
using namespace llvm;
using namespace std;

namespace
{
    std::set<Value*> setUnion(const std::set<Value*>& lhs, const std::set<Value*>& rhs){
        std::set<Value*> result = lhs;
        result.insert(rhs.begin(), rhs.end());
        return result;
    }

    std::set<Value*> setDiff(const std::set<Value*>& lhs, const std::set<Value*>& rhs){
        std::set<Value*> result = lhs;
        for (Value* v : rhs) {
            result.erase(v);
        }
        return result;
    }

    void visitor(Function &fn)
    {
        std::map<Value*, std::set<Value*>> liveOutMap; // block -> live out
        std::map<Value*, std::set<Value*>> varKillMap; // block -> var kill
        std::map<Value*, std::set<Value*>> ueVarMap; // block -> upward exposed var

        auto functionName = fn.getName();
        errs() << "Liveness analysis: " << functionName << "\n";

        // calculate varKill and ueVar
        for (auto &basic_block : fn)
        {
            //auto blockName = basic_block.getName();
            //errs() << " " << blockName << " " << "\n";

            std::set<Value*> varKillSet; // set variables each BB to be killed (defined in block)
            std::set<Value*> ueVarSet; // set variables each BB upward-exposed (used in block before redefinition)
        
            for (auto &inst : basic_block){
                //auto instName = inst.getName();
                //errs() << " " << instName << " " << "\n";

                if (inst.getOpcode() == Instruction::Load ||
                    inst.getOpcode() == Instruction::Store ||
                    inst.isBinaryOp()){
                    //errs() << formatv("{0,-40}", inst);
                }
                if (inst.getOpcode() == Instruction::Load){
                    Value* ueVar = inst.getOperand(0);
                    if(varKillSet.find(ueVar) == varKillSet.end()) // if ueVar NOT in varKillSet
                        ueVarSet.insert(ueVar);

                    //errs() << &inst << " " << inst.getOperand(0) << "\n";
                }
                if (inst.getOpcode() == Instruction::Store){
                    Value* varKill = inst.getOperand(1);
                    varKillSet.insert(varKill);

                    //errs() << inst.getOperand(0) << " " << inst.getOperand(1) << "\n";
                }
                if (inst.isBinaryOp()){
                    //errs() << &inst << " " << inst.getOperand(0) << " " << inst.getOperand(1) << "\n";
                }
            }

            varKillMap[&basic_block] = varKillSet;
            ueVarMap[&basic_block] = ueVarSet;
            liveOutMap[&basic_block] = {};
        }

        // calculate live out - iterative algorithm
        bool continueIter = true; // only loop while >= 1 live out set has changed in the iteration
        while(continueIter){
            continueIter = false;

            for (auto &basic_block : fn){
                std::set<Value*> liveOutSet;
                for(BasicBlock* succ : successors(&basic_block)){
                    std::set<Value*> liveInSuccSet = 
                        setUnion(
                            setDiff(liveOutMap[succ], varKillMap[succ]),
                            ueVarMap[succ]
                        );
                    liveOutSet = setUnion(liveOutSet, liveInSuccSet);
                }
                if(liveOutMap[&basic_block] != liveOutSet){
                    liveOutMap[&basic_block] = liveOutSet;
                    continueIter = true;
                }
            }
        }
        
        // print all block results
        for (auto &basic_block : fn){
            auto blockName = basic_block.getName();
            errs() << "------ " << blockName << " ------" << "\n";

            // print out var upward exposed 
            errs() << "UEVAR: ";
            for(auto c : ueVarMap[&basic_block]){ 
                errs() << c->getName() << " ";
            }
            errs() << "\n";

            // print out var killed
            errs() << "VARKILL: ";
            for(auto c : varKillMap[&basic_block]){ 
                errs() << c->getName() << " ";
            }
            errs() << "\n";

            // print out live out set
            errs() << "LIVEOUT: ";
            for(auto c : liveOutMap[&basic_block]){ 
                errs() << c->getName() << " ";
            }
            errs() << "\n";
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
