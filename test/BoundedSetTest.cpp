#include "../src/abstract_domain/BoundedSet.h"
#include "llvm/Support/raw_os_ostream.h"
#include <set>
using namespace llvm;
namespace pcpo {

APInt apint0{32, 0, false};
APInt apint1{32, 1, false};
APInt apint2{32, 2, false};
APInt apint3{32, 3, false};
APInt apint5{32, 5, false};
BoundedSet top{32, true};
BoundedSet bot{32, false};
BoundedSet set0{apint1};
BoundedSet set1_3{32, std::set<APInt, Comparator>{apint1, apint3}};
BoundedSet set2_5{32, std::set<APInt, Comparator>{apint2, apint5}};

void testConstructor() {
  errs() << "[testConstructor] top: ";
  errs() << top << "\n";
  errs() << "[testConstructor] bot: ";
  errs() << bot << "\n";
  errs() << "[testConstructor] singleton: ";
  errs() << set0 << "\n";
  errs() << "[testConstructor] multi: ";
  errs() << set1_3 << "\n";
}

void testLeastUpperBound() {
  auto result = set2_5.leastUpperBound(set1_3);
  BoundedSet res = *(static_cast<BoundedSet *>(result.get()));
  BoundedSet expRes{32, {1, 2, 3, 5}};
  if (!(res == expRes)) {
    errs() << "testLeastUpperBound failed\n";
    errs() << res << "\n";
  }
  result = top.leastUpperBound(set2_5);
  res = *(static_cast<BoundedSet *>(result.get()));
  errs() << res << "\n";
  if (!(res == top)) {
    errs() << "testLeastUpperBound failed\n";
    errs() << res << "\n";
  }
}

void testEquals() {
  BoundedSet set0_4{32, {0, 1, 2, 3, 4}};
  BoundedSet set0_4P{32, {0, 1, 2, 3, 4}};
  if (!(set0_4 == set0_4P)) {
    errs() << "testEquals failed\n";
  }
}

void testICompEquals() {
  BoundedSet set1{32, {0, 1, 2, 3, 4, 14}};
  BoundedSet set2{32, {14, 21, 332}};

  auto resultPair = set1.icmp(CmpInst::Predicate::ICMP_EQ, 32, set2);
  errs() << *resultPair.first << "\n";
  errs() << *resultPair.second << "\n";
  resultPair = set1.icmp(CmpInst::Predicate::ICMP_UGE, 32, set2);
  errs() << *resultPair.first << "\n";
  errs() << *resultPair.second << "\n";
}

void testICompLess() {
  BoundedSet set1{32, {49}};
  auto result = set1.icmp(CmpInst::Predicate::ICMP_ULE, 32, top);
  errs() << (*result.first.get()) << "\n";
  errs() << (*result.second.get()) << "\n";
}

void testLeastUpperBoundUnique() {
  BoundedSet set0_4{32, {0, 1, 2, 3, 4}};
  BoundedSet set0_4P{32, {0, 1, 2, 3, 4}};
  auto result = set0_4.leastUpperBound(set0_4P);
  BoundedSet res = *(static_cast<BoundedSet *>(result.get()));
  if (!(res == set0_4P)) {
    errs() << "testLeastUpperBoundUnique failed\n";
    errs() << "[testLeastUpperBoundUnique]: ";
    errs() << res << "\n";
  }
}

void testAdd() {
  auto result = set2_5.add(32, set1_3, false, false);
  BoundedSet res = *(static_cast<BoundedSet *>(result.get()));
  BoundedSet expRes{32, {3, 5, 6, 8}};
  if (!(res == expRes)) {
    errs() << "testAdd failed\n";
  }
}

void testAdd2() {
  BoundedSet a{32, {5}};
  BoundedSet b{32, {4294967294}};
  auto result = a.add(32, b, true, false);
  BoundedSet res = *(static_cast<BoundedSet *>(result.get()));
  errs() << res << "\n";
}

void testLeastUpperBoundTop() {
  APInt first{32, 13, false};
  APInt second{32, 42, false};

  BoundedSet left{first};
  BoundedSet right{second};
  auto result = left.leastUpperBound(right);
  APInt tmpInt;
  for (int i = 0; i < SET_LIMIT; i++) {
    tmpInt = APInt(32, i, false);
    BoundedSet tmp(tmpInt);
    result = result->leastUpperBound(tmp);
  }
  BoundedSet res = *(static_cast<BoundedSet *>(result.get()));
  if (!(res == top)) {
    errs() << "testLeastUpperBoundTop failed\n";
    errs() << *result << "\n";
  }
}

void testUDiv() {
  BoundedSet dividend{32, {1, 2, 4}};
  BoundedSet divisorZero{32, {0, 1}};
  errs()
      << "Next Line should contain a warning for possible division by zero.\n";
  auto result = dividend.udiv(32, divisorZero);
  if (!(*(static_cast<BoundedSet *>(result.get())) == dividend)) {
    errs() << "testUDiv failed\n";
    errs() << (*result.get()) << "\n";
  }
  errs() << "----\n";
}

void testSDiv() {
  BoundedSet minSValue{4, {8}};
  BoundedSet setWithMinusOne{4, {15, 3}};
  errs()
      << "Next Line should contain a warning for possible division overflow.\n";
  auto result = minSValue.srem(4, setWithMinusOne);
  if (!(*(static_cast<BoundedSet *>(result.get())) == BoundedSet{4, {14}})) {
    errs() << "testSDiv failed\n";
    errs() << (*result.get()) << "\n";
  }
  errs() << "----\n";
}

void testSRem() {
  BoundedSet minSValue{4, {8}};
  BoundedSet setWithMinusOne{4, {15, 3}};
  errs()
      << "Next Line should contain a warning for possible division overflow.\n";
  auto result = minSValue.srem(4, setWithMinusOne);
  if (!(*(static_cast<BoundedSet *>(result.get())) == BoundedSet{4, {14}})) {
    errs() << "testSRem failed\n";
    errs() << *result.get() << "\n";
  }
  errs() << "----\n";
}

void testIsBottom() {
  if (!BoundedSet{32, false}.isBottom()) {
    errs() << "testIsBottom failed\n";
  }
  if (top.isBottom()) {
    errs() << "testIsBottom failed\n";
  }
  if (set1_3.isBottom()) {
    errs() << "testIsBottom failed\n";
  }
}

void testLeastUpperBoundWithAdd() {
  errs() << "[testLeastUpperBoundWithAdd]\n";
  BoundedSet a{32, {3}};
  shared_ptr<AbstractDomain> result{new BoundedSet{32, {1, 2}}};
  shared_ptr<AbstractDomain> tmp{new BoundedSet{32, false}};

  BoundedSet res{32, false};
  for (int i = 0; i < 4; i++) {
    tmp = result->add(32, a, false, false);
    res = *(static_cast<BoundedSet *>(tmp.get()));
    errs() << res << "\n";
    result = result->leastUpperBound(res);
  }
}

void assertBottomBs(shared_ptr<AbstractDomain> ob, std::string test) {
    if(!ob->isBottom())
      errs() << test << "  isNotBottom";

    if(ob->isTop())
        errs() << test << "  isTop";
}

void assertTopBs(shared_ptr<AbstractDomain> ob, std::string test) {
    if(ob->isBottom())
      errs() << test << "  isBottom\n";

    if(!ob->isTop())
        errs() << test << "  isNotTop\n";
}

void testContainsRandomBs() {
/**
  APInt begin{32,0};
  APInt end{32,3};
  APInt stride{32,3};
  StridedInterval interval1{begin,end,stride};
  auto bottom = StridedInterval::create_bottom(32);

  auto result = bottom->leastUpperBound(interval1);
  result = bottom->leastUpperBound(*result);
  errs() << "\n\n" << *result; */


  const std::string testName = "[containsRandom] ";
  unsigned bitWidth = 32;

  auto bottom = BoundedSet::create_bottom(bitWidth);
  auto top = BoundedSet::create_top(bitWidth);

  assertBottomBs(bottom, "bottom");
  assertTopBs(top, "top");

  assertBottomBs(bottom->leastUpperBound(*bottom), "bottom LUB bottom");

  assertTopBs(bottom->leastUpperBound(*top), "bottom LUB top");
  assertTopBs(top->leastUpperBound(*bottom), "top LUB bottom");

  for(unsigned stride=3; stride < 10; stride++) {
    auto previousIteration = bottom;
    unsigned insertCount = 0;

    for(unsigned i=0; i < 100; i++) {
      if(i % stride != 0)
        continue;

      insertCount++;

      APInt other(bitWidth,i);
      BoundedSet newSI(other);

      auto thisIteration = previousIteration->leastUpperBound(newSI);
      auto thisIterationRev = newSI.leastUpperBound(*previousIteration);

      if(!(*reinterpret_cast<BoundedSet*>(thisIteration.get()) == *reinterpret_cast<BoundedSet*>(thisIterationRev.get()))) {
          errs() << "== not symmetric";
          return;
      }

      if (thisIteration->size() != insertCount) {
          // errs() << thisIteration->size() << " should be  " << insertCount << " size wrong\n";
          // errs() << *thisIteration;
          // return;
      }


      if(!(*reinterpret_cast<BoundedSet*>(thisIteration.get())
        == *reinterpret_cast<BoundedSet*>(thisIteration->leastUpperBound(*thisIteration).get()))) {
          errs() << "LUB(this,this) != this 1\n";
          break;
      }


      if(!(*reinterpret_cast<BoundedSet*>(thisIteration.get())
        == *reinterpret_cast<BoundedSet*>(thisIteration->leastUpperBound(*previousIteration).get()))) {
          errs() << "LUB(prev,this) != this 2\n";
          errs() << *thisIteration << " lub " << *previousIteration << " "
          << *thisIteration->leastUpperBound(*previousIteration);
          break;
      }

      if(!(*reinterpret_cast<BoundedSet*>(thisIteration.get())
        == *reinterpret_cast<BoundedSet*>(previousIteration->leastUpperBound(*thisIteration).get()))) {
          errs() << "LUB(prev,this) != this 3\n";
          errs() << *thisIteration << " lub " << *previousIteration << " = "
          << *previousIteration->leastUpperBound(*thisIteration) << "\n";
          break;
      }

      if(*thisIteration<=(*previousIteration)) {
        errs() << *thisIteration << " " << *previousIteration << "Less or equal failed";
        return;
      }

      if(!(*previousIteration<=(*thisIteration))) {
        errs() << "Less or equal failed";
        return;
      }

      // todo max, min, etc.

      previousIteration = thisIteration;
    }
  }
}
void runBoundedSet() {
  testContainsRandomBs();
  testConstructor();
  testLeastUpperBound();
  testLeastUpperBoundWithAdd();
  testLeastUpperBoundUnique();
  testAdd();
  testLeastUpperBoundTop();
  testICompEquals();
  testUDiv();
  testICompLess();
  testIsBottom();
  testSDiv();
  testSRem();
  testAdd2();
}
} // namespace pcpo
