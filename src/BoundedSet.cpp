#include "BoundedSet.h"
#include "AbstractDomain.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/raw_os_ostream.h"
#include <iostream>
#include <iterator>
#include <memory>
#include <set>

namespace pcpo {

using namespace llvm;
using std::unique_ptr;

BoundedSet::BoundedSet(std::set<APInt, Comparator> vals) : values{vals} {}
BoundedSet::BoundedSet(APInt val) { values.insert(val); }
BoundedSet::BoundedSet(bool isTop) : top{isTop} {}

unique_ptr<AbstractDomain>
BoundedSet::compute(AbstractDomain &other,
                    std::function<APInt(APInt, APInt)> op) {
  if (BoundedSet *otherB = static_cast<BoundedSet *>(&other)) {
    int count = 0;
    std::set<APInt, Comparator> newValues{};
    if (top || otherB->top) {
      return unique_ptr<BoundedSet>{new BoundedSet(true)};
    }
    for (auto &leftVal : values) {
      for (auto &rightVal : otherB->values) {
        if (++count > SET_LIMIT) {
          unique_ptr<BoundedSet> topSet{new BoundedSet(true)};
        }

        APInt newVal = op(leftVal, rightVal);
        newValues.insert(newVal);
      }
    }
    unique_ptr<AbstractDomain> result{new BoundedSet(newValues)};
    return result;
  }
  return nullptr;
}

unique_ptr<AbstractDomain> BoundedSet::add(AbstractDomain &other) {
  auto opPlus = [](APInt left, APInt right) {
    APInt newValue{left};
    newValue += right;
    return newValue;
  };
  return compute(other, opPlus);
}
unique_ptr<AbstractDomain> BoundedSet::subtract(AbstractDomain &other) {
  auto opMinus = [](APInt left, APInt right) {
    APInt newValue{left};
    newValue -= right;
    return newValue;
  };
  return compute(other, opMinus);
}
unique_ptr<AbstractDomain> BoundedSet::multiply(AbstractDomain &other) {
  auto opMinus = [](APInt left, APInt right) {
    APInt newValue{left};
    newValue *= right;
    return newValue;
  };
  return compute(other, opMinus);
}
unique_ptr<AbstractDomain> BoundedSet::unaryMinus() {
  std::set<APInt, Comparator> newValues{};
  APInt tmp;
  for (auto &val : values) {
    tmp = APInt(val);
    tmp *= -1;
    newValues.insert(tmp);
  }
  return unique_ptr<BoundedSet>(new BoundedSet(newValues));
}
unique_ptr<AbstractDomain> BoundedSet::increment() { return nullptr; }
unique_ptr<AbstractDomain> BoundedSet::decrement() { return nullptr; }

unique_ptr<AbstractDomain> BoundedSet::leastUpperBound(AbstractDomain &other) {
  if (BoundedSet *otherB = static_cast<BoundedSet *>(&other)) {
    std::set<APInt, Comparator> result;
    for (auto &val : values) {
      result.insert(val);
    }
    int count = values.size();
    for (auto &val : otherB->values) {
      if (++count > SET_LIMIT) {
        return unique_ptr<BoundedSet>{new BoundedSet(true)};
      }
      result.insert(val);
    }
    unique_ptr<BoundedSet> res{new BoundedSet{result}};
    return res;
  }
  return nullptr;
}

// implements contains
bool BoundedSet::lessOrEqual(AbstractDomain &other) {
  if (BoundedSet *otherB = static_cast<BoundedSet *>(&other)) {
    auto end = otherB->values.end();
    for (auto &val : values) {
      if (otherB->values.find(val) == end) {
        // val is not contained int otherB->values
        return false;
      }
    }
    return true;
  }
  // TODO: throw exception if comparison is not implemented
  return false;
}

void BoundedSet::printOut() {
  //   errs() << "BoundedSet@" << this << std::endl;
  errs() << "BoundedSet@" << this << "\n";
  if (top) {
    errs() << "T\n";
    return;
  }
  for (auto &val : values) {
    errs() << val.toString(10, false) << "\n";
  }
}

bool BoundedSet::isTop() { return top; }
} // namespace pcpo