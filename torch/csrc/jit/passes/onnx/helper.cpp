#include <torch/csrc/jit/passes/onnx/helper.h>
#include <torch/csrc/jit/jit_log.h>

namespace torch {
namespace jit {
namespace onnx {
using namespace ::c10::onnx;

}

ValueToParamPairMap buildValueToParamsMap(
    Block* b,
    const ParamMap& paramsDict) {
  ValueToParamPairMap valsToParamsMap;
  for (auto& input : b->inputs()) {
    auto it = paramsDict.find(input->debugName());
    if (it != paramsDict.end()) {
      valsToParamsMap.emplace(input, *it);
    }
  }
  return valsToParamsMap;
}

void eraseUnusedBlockInputs(Block* b) {
  for (size_t i_1 = b->inputs().size(); i_1 > 0; --i_1) {
    size_t i = i_1 - 1;
    if (!b->inputs().at(i)->hasUses()) {
      b->eraseInput(i);
    }
  }
}

void eraseUnusedValuesFromMap(ValueToParamPairMap& valsToParamsMap) {
  auto it = valsToParamsMap.begin();
  while (it != valsToParamsMap.end()) {
    if (!it->first->hasUses()) {
      it = valsToParamsMap.erase(it);
    } else {
      ++it;
    }
  }
}

void buildParamsMapFromValueToParamsMap(
    const ValueToParamPairMap& valsToParamsMap,
    ParamMap& paramsDict) {
  paramsDict.clear();
  for (const auto& nameTensorParamPair : valsToParamsMap) {
    paramsDict.insert(nameTensorParamPair.second);
  }
}

Node* addNodeToBlock(Block* block, Value* input, Symbol kind) {
  auto new_node = block->appendNode(block->owningGraph()->create(kind));
  auto new_input = new_node->addInput(input);
  for (size_t i = 0; i < new_node->outputs().size(); i++) {
    auto output = new_node->outputs()[i];
    block->registerOutput(output);
  }
  return new_node;
}
} // namespace jit
} // namespace torch
