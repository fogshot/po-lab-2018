#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>

#include "../util/util.h"
#include "branch_conditions.h"

using namespace llvm;
namespace pcpo {

BranchConditions::BranchConditions(std::map<BasicBlock *, State> &programPoints)
    : programPoints(programPoints), conditionCacheUsed(false) {}

bool BranchConditions::isBasicBlockReachable(BasicBlock *pred, BasicBlock *bb) {

  const auto bc = branchConditions.find(pred);
  if (bc != branchConditions.end())

    if (bc->second.find(bb) != bc->second.end()) {

      for (const auto &branchCondition : bc->second[bb])
        if (branchCondition.second->isBottom())
          return false;
    }
  return true;
}

bool BranchConditions::applyCondition(BasicBlock *pred, BasicBlock *bb) {

  assert(!conditionCacheUsed &&
         "ConditionCache has not been correctly unapplied last time!");

  bool isNotBottom = true;
  
  const auto bc = branchConditions.find(pred);
  if (bc != branchConditions.end())
    if (bc->second.find(bb) != bc->second.end()) {
      conditionCacheUsed = true;

      for (const auto &branchCondition : bc->second[bb]) {
        const auto value = branchCondition.first;
        /// buffer old value
        conditionCache[value] = programPoints[pred].getAbstractValue(value);
        /// overwrite value with condition
        programPoints[pred].vars[value] = branchCondition.second;
        if(branchCondition.second->isBottom()){
            isNotBottom &=false;
        }
      }
    }
  return isNotBottom;
}

void BranchConditions::unApplyCondition(BasicBlock *pred) {

  if (!conditionCacheUsed)
    return;

  for (const auto &condition : conditionCache)
    programPoints[pred].vars[condition.first] = condition.second;

  conditionCache.clear();
  conditionCacheUsed = false;
}

void BranchConditions::putBranchConditions(BasicBlock *pred, BasicBlock *bb,
                                           Value *val,
                                           std::shared_ptr<AbstractDomain> ad) {

  branchConditions[pred][bb][val] = ad;
}
}
