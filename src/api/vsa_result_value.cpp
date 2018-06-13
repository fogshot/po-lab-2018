#include <llvm/Analysis/LazyValueInfo.h>

#include "vsa_result_value.h"
using namespace llvm;

namespace pcpo {

LazyValueInfo::Tristate VsaResultValue::testIf(CmpInst::Predicate predicate,
                                               Constant *C) {

  // only comparison with ConstantInt is implemented
  if (ConstantInt::classof(C)) {
    // create abstract domain for constant
    auto temp = AD_TYPE(reinterpret_cast<ConstantInt *>(C)->getValue());
    
    // perform comparison
    auto result = abstractValue->icmp(predicate,
                                      C->getType()->getIntegerBitWidth(), temp);

    // temp. bools
    bool b_t = !result.first->isBottom(); // predicate might be true
    bool b_f = !result.second->isBottom(); // predicate might not be true

    if (b_t && b_f) // predicate might be true or not
      return LazyValueInfo::Unknown;
    if (b_f) // predicate is not true
      return LazyValueInfo::False;
    else // predicate is true
      return LazyValueInfo::True;
  } else { // not implemented: top
    return LazyValueInfo::Unknown;
  }
}
}