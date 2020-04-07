//
//  SharedArray.cpp
//  DoubleConversion
//
//  Created by Szymon Kapala on 19/03/2020.
//

#include "SharedArray.h"
#include "SharedValueRegistry.h"

SharedArray::SharedArray(int id, std::vector<std::shared_ptr<SharedValue>> svs) {
  this->id = id;
  this->svs = svs;
  this->dirty = false;
  this->shouldBeSentToJava = false;
  this->type = SharedValueType::shared_array;
}

SharedArray::~SharedArray() {
  //noop
}

jsi::Value SharedArray::asValue(jsi::Runtime &rt) const {
  return jsi::Value::undefined();
}

void SharedArray::setNewValue(std::shared_ptr<SharedValue> sv) {
  SharedArray * other = (SharedArray*) sv.get();
  this->svs = other->svs;
}

jsi::Value SharedArray::asParameter(jsi::Runtime &rt) {
  jsi::Array array(rt, svs.size());
  for (int i = 0; i < svs.size(); ++i) {
    array.setValueAtIndex(rt, i, svs[i]->asParameter(rt));
  }
  return array;
}

std::vector<int> SharedArray::getSharedValues() {
  std::vector<int> res;
  for (auto &sv : svs) {
    std::vector<int> innerRes = sv->getSharedValues();
    for (auto id : innerRes) {
      res.push_back(id);
    }
  }
  return res;
}

std::shared_ptr<SharedValue> SharedArray::copy() {
  std::vector<std::shared_ptr<SharedValue>> copiedSvs;
  for (auto &sv : svs) {
    copiedSvs.push_back(sv->copy());
  }
  int id = SharedValueRegistry::NEXT_SHARED_VALUE_ID--;
  return std::make_shared<SharedArray>(id, copiedSvs);
}
