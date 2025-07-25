/*
 * SPDX-License-Identifier: Apache-2.0
 */

// ATTENTION: The code in this file is highly EXPERIMENTAL.
// Adventurous users should note that the APIs will probably change.

#pragma once

#include <unordered_set>
#include <vector>
#include <map>

#include "onnx/common/ir.h"
#include "onnx/common/ir_pb_converter.h"
#include "onnx/proto_utils.h"
#include "onnxoptimizer/passes/adjust_add.h"
#include "onnxoptimizer/passes/adjust_slice_and_matmul.h"
#include "onnxoptimizer/passes/eliminate_consecutive_idempotent_ops.h"
#include "onnxoptimizer/passes/eliminate_deadend.h"
#include "onnxoptimizer/passes/eliminate_duplicate_initializer.h"
#include "onnxoptimizer/passes/eliminate_identity.h"
#include "onnxoptimizer/passes/eliminate_if_with_const_cond.h"
#include "onnxoptimizer/passes/eliminate_nop_cast.h"
#include "onnxoptimizer/passes/eliminate_nop_concat.h"
#include "onnxoptimizer/passes/eliminate_nop_dropout.h"
#include "onnxoptimizer/passes/eliminate_nop_expand.h"
#include "onnxoptimizer/passes/eliminate_nop_flatten.h"
#include "onnxoptimizer/passes/eliminate_nop_monotone_argmax.h"
#include "onnxoptimizer/passes/eliminate_nop_pad.h"
#include "onnxoptimizer/passes/eliminate_nop_reshape.h"
#include "onnxoptimizer/passes/eliminate_nop_split.h"
#include "onnxoptimizer/passes/eliminate_nop_transpose.h"
#include "onnxoptimizer/passes/eliminate_shape_gather.h"
#include "onnxoptimizer/passes/eliminate_shape_op.h"
#include "onnxoptimizer/passes/eliminate_slice_after_shape.h"
#include "onnxoptimizer/passes/eliminate_unused_initializer.h"
#include "onnxoptimizer/passes/extract_constant_to_initializer.h"
#include "onnxoptimizer/passes/fuse_add_bias_into_conv.h"
#include "onnxoptimizer/passes/fuse_bn_into_conv.h"
#include "onnxoptimizer/passes/fuse_concat_into_reshape.h"
#include "onnxoptimizer/passes/fuse_consecutive_concats.h"
#include "onnxoptimizer/passes/fuse_consecutive_log_softmax.h"
#include "onnxoptimizer/passes/fuse_consecutive_reduce_unsqueeze.h"
#include "onnxoptimizer/passes/fuse_consecutive_squeezes.h"
#include "onnxoptimizer/passes/fuse_consecutive_transposes.h"
#include "onnxoptimizer/passes/fuse_matmul_add_bias_into_gemm.h"
#include "onnxoptimizer/passes/fuse_pad_into_conv.h"
#include "onnxoptimizer/passes/fuse_pad_into_pool.h"
#include "onnxoptimizer/passes/fuse_transpose_into_gemm.h"
#include "onnxoptimizer/passes/lift_lexical_references.h"
#include "onnxoptimizer/passes/nop.h"
#include "onnxoptimizer/passes/rename_input_output.h"
#include "onnxoptimizer/passes/replace_einsum_with_matmul.h"
#include "onnxoptimizer/passes/set_unique_name_for_nodes.h"
#include "onnxoptimizer/passes/split.h"
#include "onnxoptimizer/passes/fuse_consecutive_slices.h"
#include "onnxoptimizer/passes/eliminate_common_subexpression.h"
#include "onnxoptimizer/passes/fuse_qkv.h"
#include "onnxoptimizer/passes/fuse_consecutive_unsqueezes.h"
#include "onnxoptimizer/passes/eliminate_nop_with_unit.h"
#include "onnxoptimizer/passes/rewrite_input_dtype.h"
#include "onnxoptimizer/passes/rewrite_where.h"

namespace ONNX_NAMESPACE {
namespace optimization {

// Registry containing all passes available in ONNX.
struct GlobalPassRegistry {
  std::map<std::string, std::shared_ptr<Pass>> passes;
  std::vector<std::string> pass_names;

  GlobalPassRegistry() {
    // Register the optimization passes to the optimizer.
    registerPass<AdjustAdd>();
    registerPass<RenameInputOutput>();
    registerPass<SetUniqueNameForNodes>();
    registerPass<NopEmptyPass>();
    registerPass<EliminateNopCast>();
    registerPass<EliminateNopDropout>();
    registerPass<EliminateNopFlatten>();
    registerPass<ExtractConstantToInitializer>();
    registerPass<EliminateConsecutiveIdempotentOps>();
    registerPass<EliminateIfWithConstCond>();
    registerPass<EliminateNopMonotoneArgmax>();
    registerPass<EliminateNopPad>();
    registerPass<EliminateNopConcat>();
    registerPass<EliminateNopSplit>();
    registerPass<EliminateNopExpand>();
    registerPass<EliminateShapeGather>();
    registerPass<EliminateSliceAfterShape>();
    registerPass<EliminateNopTranspose>();
    registerPass<FuseAddBiasIntoConv>();
    registerPass<FuseBNIntoConv>();
    registerPass<FuseConsecutiveConcats>();
    registerPass<FuseConsecutiveLogSoftmax>();
    registerPass<FuseConsecutiveReduceUnsqueeze>();
    registerPass<FuseConsecutiveSqueezes>();
    registerPass<FuseConsecutiveTransposes>();
    registerPass<FuseMatMulAddBiasIntoGemm>();
    registerPass<FusePadIntoConv>();
    registerPass<FusePadIntoPool>();
    registerPass<FuseTransposeIntoGemm>();
    registerPass<ReplaceEinsumWithMatmul>();
    registerPass<LiftLexicalReferences>();
    registerPass<SplitInit>();
    registerPass<SplitPredict>();
    registerPass<FuseConcatIntoReshape>();
    registerPass<EliminateNopReshape>();
    registerPass<EliminateOpWithUnit>();
    registerPass<EliminateCommonSubexpression>();
    registerPass<FuseQKV>();
    registerPass<FuseConsecutiveUnsqueezes>();
    registerPass<EliminateDeadEnd>();
    registerPass<EliminateIdentity>();
    registerPass<EliminateShapeOp>();
    registerPass<FuseConsecutiveSlices>();
    registerPass<EliminateUnusedInitializer>();
    registerPass<EliminateDuplicateInitializer>();
    registerPass<AdjustSliceAndMatmul>();
    registerPass<RewriteInputDtype>();
    registerPass<RewriteWhere>();
  }

  ~GlobalPassRegistry() {
    this->passes.clear();
  }

  std::shared_ptr<Pass> find(std::string pass_name) {
    auto it = this->passes.find(pass_name);
    ONNX_ASSERTM(it != this->passes.end(), "pass %s is unknown.",
                 pass_name.c_str());
    return it->second;
  }
  const std::vector<std::string> GetAvailablePasses() {
    return pass_names;
  }

  const std::vector<std::string> GetFuseAndEliminationPass();

  template <typename T>
  void registerPass() {
    static_assert(std::is_base_of<Pass, T>::value, "T must inherit from Pass");
    std::shared_ptr<Pass> pass(new T());
    passes[pass->getPassName()] = pass;
    pass_names.emplace_back(pass->getPassName());
  }
};
}  // namespace optimization
}  // namespace ONNX_NAMESPACE
