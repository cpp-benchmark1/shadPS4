// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "common/config.h"
#include "common/io_file.h"
#include "common/path_util.h"
#include "shader_recompiler/exception.h"
#include "shader_recompiler/frontend/fetch_shader.h"
#include "shader_recompiler/frontend/translate/translate.h"
#include "shader_recompiler/info.h"
#include "shader_recompiler/runtime_info.h"
#include "video_core/amdgpu/resource.h"
#include "video_core/amdgpu/types.h"

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 1515
#include <magic_enum/magic_enum.hpp>

namespace Shader::Gcn {

Translator::Translator(IR::Block* block_, Info& info_, const RuntimeInfo& runtime_info_,
                       const Profile& profile_)
    : ir{*block_, block_->begin()}, info{info_}, runtime_info{runtime_info_}, profile{profile_} {}

void Translator::EmitPrologue() {
    ir.Prologue();
    ir.SetExec(ir.Imm1(true));

    // Initialize user data.
    IR::ScalarReg dst_sreg = IR::ScalarReg::S0;
    for (u32 i = 0; i < runtime_info.num_user_data; i++) {
        ir.SetScalarReg(dst_sreg, ir.GetUserData(dst_sreg));
        ++dst_sreg;
    }

    IR::VectorReg dst_vreg = IR::VectorReg::V0;
    switch (info.stage) {
    case Stage::Vertex:
    case Stage::Export:
        // v0: vertex ID, always present
        ir.SetVectorReg(dst_vreg++, ir.GetAttributeU32(IR::Attribute::VertexId));
        // v1: instance ID, step rate 0
        if (runtime_info.num_input_vgprs > 0) {
            ir.SetVectorReg(dst_vreg++, ir.GetAttributeU32(IR::Attribute::InstanceId0));
        }
        // v2: instance ID, step rate 1
        if (runtime_info.num_input_vgprs > 1) {
            ir.SetVectorReg(dst_vreg++, ir.GetAttributeU32(IR::Attribute::InstanceId1));
        }
        // v3: instance ID, plain
        if (runtime_info.num_input_vgprs > 2) {
            ir.SetVectorReg(dst_vreg++, ir.GetAttributeU32(IR::Attribute::InstanceId));
        }
        break;
    case Stage::Fragment:
        dst_vreg = IR::VectorReg::V0;
        if (runtime_info.fs_info.addr_flags.persp_sample_ena) {
            ++dst_vreg; // I
            ++dst_vreg; // J
        }
        if (runtime_info.fs_info.addr_flags.persp_center_ena) {
            ++dst_vreg; // I
            ++dst_vreg; // J
        }
        if (runtime_info.fs_info.addr_flags.persp_centroid_ena) {
            ++dst_vreg; // I
            ++dst_vreg; // J
        }
        if (runtime_info.fs_info.addr_flags.persp_pull_model_ena) {
            ++dst_vreg; // I/W
            ++dst_vreg; // J/W
            ++dst_vreg; // 1/W
        }
        if (runtime_info.fs_info.addr_flags.linear_sample_ena) {
            ++dst_vreg; // I
            ++dst_vreg; // J
        }
        if (runtime_info.fs_info.addr_flags.linear_center_ena) {
            ++dst_vreg; // I
            ++dst_vreg; // J
        }
        if (runtime_info.fs_info.addr_flags.linear_centroid_ena) {
            ++dst_vreg; // I
            ++dst_vreg; // J
        }
        if (runtime_info.fs_info.addr_flags.line_stipple_tex_ena) {
            ++dst_vreg;
        }
        if (runtime_info.fs_info.addr_flags.pos_x_float_ena) {
            if (runtime_info.fs_info.en_flags.pos_x_float_ena) {
                ir.SetVectorReg(dst_vreg++, ir.GetAttribute(IR::Attribute::FragCoord, 0));
            } else {
                ir.SetVectorReg(dst_vreg++, ir.Imm32(0.0f));
            }
        }
        if (runtime_info.fs_info.addr_flags.pos_y_float_ena) {
            if (runtime_info.fs_info.en_flags.pos_y_float_ena) {
                ir.SetVectorReg(dst_vreg++, ir.GetAttribute(IR::Attribute::FragCoord, 1));
            } else {
                ir.SetVectorReg(dst_vreg++, ir.Imm32(0.0f));
            }
        }
        if (runtime_info.fs_info.addr_flags.pos_z_float_ena) {
            if (runtime_info.fs_info.en_flags.pos_z_float_ena) {
                ir.SetVectorReg(dst_vreg++, ir.GetAttribute(IR::Attribute::FragCoord, 2));
            } else {
                ir.SetVectorReg(dst_vreg++, ir.Imm32(0.0f));
            }
        }
        if (runtime_info.fs_info.addr_flags.pos_w_float_ena) {
            if (runtime_info.fs_info.en_flags.pos_w_float_ena) {
                ir.SetVectorReg(dst_vreg++, ir.GetAttribute(IR::Attribute::FragCoord, 3));
            } else {
                ir.SetVectorReg(dst_vreg++, ir.Imm32(0.0f));
            }
        }
        if (runtime_info.fs_info.addr_flags.front_face_ena) {
            if (runtime_info.fs_info.en_flags.front_face_ena) {
                ir.SetVectorReg(dst_vreg++, ir.GetAttributeU32(IR::Attribute::IsFrontFace));
            } else {
                ir.SetVectorReg(dst_vreg++, ir.Imm32(0));
            }
        }
        break;
    case Stage::Compute:
        ir.SetVectorReg(dst_vreg++, ir.GetAttributeU32(IR::Attribute::LocalInvocationId, 0));
        ir.SetVectorReg(dst_vreg++, ir.GetAttributeU32(IR::Attribute::LocalInvocationId, 1));
        ir.SetVectorReg(dst_vreg++, ir.GetAttributeU32(IR::Attribute::LocalInvocationId, 2));

        if (runtime_info.cs_info.tgid_enable[0]) {
            ir.SetScalarReg(dst_sreg++, ir.GetAttributeU32(IR::Attribute::WorkgroupId, 0));
        }
        if (runtime_info.cs_info.tgid_enable[1]) {
            ir.SetScalarReg(dst_sreg++, ir.GetAttributeU32(IR::Attribute::WorkgroupId, 1));
        }
        if (runtime_info.cs_info.tgid_enable[2]) {
            ir.SetScalarReg(dst_sreg++, ir.GetAttributeU32(IR::Attribute::WorkgroupId, 2));
        }
        break;
    case Stage::Geometry:
        switch (runtime_info.gs_info.out_primitive[0]) {
        case AmdGpu::GsOutputPrimitiveType::TriangleStrip:
            ir.SetVectorReg(IR::VectorReg::V3, ir.Imm32(2u)); // vertex 2
            [[fallthrough]];
        case AmdGpu::GsOutputPrimitiveType::LineStrip:
            ir.SetVectorReg(IR::VectorReg::V1, ir.Imm32(1u)); // vertex 1
            [[fallthrough]];
        default:
            ir.SetVectorReg(IR::VectorReg::V0, ir.Imm32(0u)); // vertex 0
            break;
        }
        ir.SetVectorReg(IR::VectorReg::V2, ir.GetAttributeU32(IR::Attribute::PrimitiveId));
        break;
    default:
        throw NotImplementedException("Unknown shader stage");
    }
}

template <typename T>
T Translator::GetSrc(const InstOperand& operand) {
    constexpr bool is_float = std::is_same_v<T, IR::F32>;

    const auto get_imm = [&](auto value) -> T {
        if constexpr (is_float) {
            return ir.Imm32(std::bit_cast<float>(value));
        } else {
            return ir.Imm32(std::bit_cast<u32>(value));
        }
    };

    T value{};
    switch (operand.field) {
    case OperandField::ScalarGPR:
        value = ir.GetScalarReg<T>(IR::ScalarReg(operand.code));
        break;
    case OperandField::VectorGPR:
        value = ir.GetVectorReg<T>(IR::VectorReg(operand.code));
        break;
    case OperandField::ConstZero:
        value = get_imm(0U);
        break;
    case OperandField::SignedConstIntPos:
        value = get_imm(operand.code - SignedConstIntPosMin + 1);
        break;
    case OperandField::SignedConstIntNeg:
        value = get_imm(-s32(operand.code) + SignedConstIntNegMin - 1);
        break;
    case OperandField::LiteralConst:
        value = get_imm(operand.code);
        break;
    case OperandField::ConstFloatPos_1_0:
        value = get_imm(1.f);
        break;
    case OperandField::ConstFloatPos_0_5:
        value = get_imm(0.5f);
        break;
    case OperandField::ConstFloatPos_2_0:
        value = get_imm(2.0f);
        break;
    case OperandField::ConstFloatPos_4_0:
        value = get_imm(4.0f);
        break;
    case OperandField::ConstFloatNeg_0_5:
        value = get_imm(-0.5f);
        break;
    case OperandField::ConstFloatNeg_1_0:
        value = get_imm(-1.0f);
        break;
    case OperandField::ConstFloatNeg_2_0:
        value = get_imm(-2.0f);
        break;
    case OperandField::ConstFloatNeg_4_0:
        value = get_imm(-4.0f);
        break;
    case OperandField::VccLo:
        if constexpr (is_float) {
            value = ir.BitCast<IR::F32>(ir.GetVccLo());
        } else {
            value = ir.GetVccLo();
        }
        break;
    case OperandField::VccHi:
        if constexpr (is_float) {
            value = ir.BitCast<IR::F32>(ir.GetVccHi());
        } else {
            value = ir.GetVccHi();
        }
        break;
    case OperandField::M0:
        if constexpr (is_float) {
            value = ir.BitCast<IR::F32>(ir.GetM0());
        } else {
            value = ir.GetM0();
        }
        break;
    case OperandField::Scc:
        if constexpr (is_float) {
            UNREACHABLE();
        } else {
            value = ir.BitCast<IR::U32>(ir.GetScc());
        }
        break;
    default:
        UNREACHABLE();
    }

    if constexpr (is_float) {
        if (operand.input_modifier.abs) {
            value = ir.FPAbs(value);
        }
        if (operand.input_modifier.neg) {
            value = ir.FPNeg(value);
        }
    } else {
        if (operand.input_modifier.abs) {
            value = ir.IAbs(value);
        }
        if (operand.input_modifier.neg) {
            value = ir.INeg(value);
        }
    }
    return value;
}

template IR::U32 Translator::GetSrc<IR::U32>(const InstOperand&);
template IR::F32 Translator::GetSrc<IR::F32>(const InstOperand&);

template <typename T>
T Translator::GetSrc64(const InstOperand& operand) {
    constexpr bool is_float = std::is_same_v<T, IR::F64>;

    const auto get_imm = [&](auto value) -> T {
        if constexpr (is_float) {
            return ir.Imm64(std::bit_cast<double>(value));
        } else {
            return ir.Imm64(std::bit_cast<u64>(value));
        }
    };

    T value{};
    switch (operand.field) {
    case OperandField::ScalarGPR: {
        const auto value_lo = ir.GetScalarReg(IR::ScalarReg(operand.code));
        const auto value_hi = ir.GetScalarReg(IR::ScalarReg(operand.code + 1));
        if constexpr (is_float) {
            UNREACHABLE();
        } else {
            value = ir.PackUint2x32(ir.CompositeConstruct(value_lo, value_hi));
        }
        break;
    }
    case OperandField::VectorGPR: {
        const auto value_lo = ir.GetVectorReg(IR::VectorReg(operand.code));
        const auto value_hi = ir.GetVectorReg(IR::VectorReg(operand.code + 1));
        if constexpr (is_float) {
            value = ir.PackFloat2x32(ir.CompositeConstruct(value_lo, value_hi));
        } else {
            value = ir.PackUint2x32(ir.CompositeConstruct(value_lo, value_hi));
        }
        break;
    }
    case OperandField::ConstZero:
        value = get_imm(0ULL);
        break;
    case OperandField::SignedConstIntPos:
        value = get_imm(s64(operand.code) - SignedConstIntPosMin + 1);
        break;
    case OperandField::SignedConstIntNeg:
        value = get_imm(-s64(operand.code) + SignedConstIntNegMin - 1);
        break;
    case OperandField::LiteralConst:
        value = get_imm(u64(operand.code));
        break;
    case OperandField::ConstFloatPos_1_0:
        value = get_imm(1.0);
        break;
    case OperandField::ConstFloatPos_0_5:
        value = get_imm(0.5);
        break;
    case OperandField::ConstFloatPos_2_0:
        value = get_imm(2.0);
        break;
    case OperandField::ConstFloatPos_4_0:
        value = get_imm(4.0);
        break;
    case OperandField::ConstFloatNeg_0_5:
        value = get_imm(-0.5);
        break;
    case OperandField::ConstFloatNeg_1_0:
        value = get_imm(-1.0);
        break;
    case OperandField::ConstFloatNeg_2_0:
        value = get_imm(-2.0);
        break;
    case OperandField::ConstFloatNeg_4_0:
        value = get_imm(-4.0);
        break;
    case OperandField::VccLo:
        if constexpr (is_float) {
            UNREACHABLE();
        } else {
            value = ir.PackUint2x32(ir.CompositeConstruct(ir.GetVccLo(), ir.GetVccHi()));
        }
        break;
    case OperandField::VccHi:
    default:
        UNREACHABLE();
    }

    if constexpr (is_float) {
        if (operand.input_modifier.abs) {
            value = ir.FPAbs(value);
        }
        if (operand.input_modifier.neg) {
            value = ir.FPNeg(value);
        }
    }
    return value;
}

template IR::U64 Translator::GetSrc64<IR::U64>(const InstOperand&);
template IR::F64 Translator::GetSrc64<IR::F64>(const InstOperand&);

void Translator::SetDst(const InstOperand& operand, const IR::U32F32& value) {
    IR::U32F32 result = value;
    if (value.Type() == IR::Type::F32) {
        if (operand.output_modifier.multiplier != 0.f) {
            result = ir.FPMul(result, ir.Imm32(operand.output_modifier.multiplier));
        }
        if (operand.output_modifier.clamp) {
            result = ir.FPSaturate(value);
        }
    }

    switch (operand.field) {
    case OperandField::ScalarGPR:
        return ir.SetScalarReg(IR::ScalarReg(operand.code), result);
    case OperandField::VectorGPR:
        return ir.SetVectorReg(IR::VectorReg(operand.code), result);
    case OperandField::VccLo:
        return ir.SetVccLo(result);
    case OperandField::VccHi:
        return ir.SetVccHi(result);
    case OperandField::M0:
        return ir.SetM0(result);
    default:
        UNREACHABLE();
    }
}

void Translator::SetDst64(const InstOperand& operand, const IR::U64F64& value_raw) {
    IR::U64F64 value_untyped = value_raw;

    const bool is_float = value_raw.Type() == IR::Type::F64 || value_raw.Type() == IR::Type::F32;
    if (is_float) {
        if (operand.output_modifier.multiplier != 0.f) {
            value_untyped =
                ir.FPMul(value_untyped, ir.Imm64(f64(operand.output_modifier.multiplier)));
        }
        if (operand.output_modifier.clamp) {
            value_untyped = ir.FPSaturate(value_raw);
        }
    }
    const IR::U64 value =
        is_float ? ir.BitCast<IR::U64>(IR::F64{value_untyped}) : IR::U64{value_untyped};

    const IR::Value unpacked{ir.UnpackUint2x32(value)};
    const IR::U32 lo{ir.CompositeExtract(unpacked, 0U)};
    const IR::U32 hi{ir.CompositeExtract(unpacked, 1U)};
    switch (operand.field) {
    case OperandField::ScalarGPR:
        ir.SetScalarReg(IR::ScalarReg(operand.code + 1), hi);
        return ir.SetScalarReg(IR::ScalarReg(operand.code), lo);
    case OperandField::VectorGPR:
        ir.SetVectorReg(IR::VectorReg(operand.code + 1), hi);
        return ir.SetVectorReg(IR::VectorReg(operand.code), lo);
    case OperandField::VccLo:
        UNREACHABLE();
    case OperandField::VccHi:
        UNREACHABLE();
    case OperandField::M0:
        break;
    default:
        UNREACHABLE();
    }
}

void Translator::EmitFetch(const GcnInst& inst) {
    // Read the pointer to the fetch shader assembly.
    info.has_fetch_shader = true;
    info.fetch_shader_sgpr_base = inst.src[0].code;

    const auto fetch_data = ParseFetchShader(info);
    ASSERT(fetch_data.has_value());

    if (Config::dumpShaders()) {
        using namespace Common::FS;
        const auto dump_dir = GetUserPath(PathType::ShaderDir) / "dumps";
        if (!std::filesystem::exists(dump_dir)) {
            std::filesystem::create_directories(dump_dir);
        }
        const auto filename = fmt::format("vs_{:#018x}.fetch.bin", info.pgm_hash);
        const auto file = IOFile{dump_dir / filename, FileAccessMode::Write};
        file.WriteRaw<u8>(fetch_data->code, fetch_data->size);
    }

    for (const auto& attrib : fetch_data->attributes) {
        const IR::Attribute attr{IR::Attribute::Param0 + attrib.semantic};
        IR::VectorReg dst_reg{attrib.dest_vgpr};

        // Read the V# of the attribute to figure out component number and type.
        const auto buffer = info.ReadUdReg<AmdGpu::Buffer>(attrib.sgpr_base, attrib.dword_offset);
        for (u32 i = 0; i < 4; i++) {
            const IR::F32 comp = [&] {
                switch (buffer.GetSwizzle(i)) {
                case AmdGpu::CompSwizzle::One:
                    return ir.Imm32(1.f);
                case AmdGpu::CompSwizzle::Zero:
                    return ir.Imm32(0.f);
                case AmdGpu::CompSwizzle::Red:
                    return ir.GetAttribute(attr, 0);
                case AmdGpu::CompSwizzle::Green:
                    return ir.GetAttribute(attr, 1);
                case AmdGpu::CompSwizzle::Blue:
                    return ir.GetAttribute(attr, 2);
                case AmdGpu::CompSwizzle::Alpha:
                    return ir.GetAttribute(attr, 3);
                default:
                    UNREACHABLE();
                }
            }();
            ir.SetVectorReg(dst_reg++, comp);
        }

        // In case of programmable step rates we need to fallback to instance data pulling in
        // shader, so VBs should be bound as regular data buffers
        if (attrib.UsesStepRates()) {
            info.buffers.push_back({
                .sharp_idx = info.srt_info.ReserveSharp(attrib.sgpr_base, attrib.dword_offset, 4),
                .used_types = IR::Type::F32,
                .is_instance_data = true,
                .instance_attrib = attrib.semantic,
            });
        }
    }
}

void Translator::LogMissingOpcode(const GcnInst& inst) {
    LOG_ERROR(Render_Recompiler, "Unknown opcode {} ({}, category = {})",
              magic_enum::enum_name(inst.opcode), u32(inst.opcode),
              magic_enum::enum_name(inst.category));
    info.translation_failed = true;
}

void Translate(IR::Block* block, u32 pc, std::span<const GcnInst> inst_list, Info& info,
               const RuntimeInfo& runtime_info, const Profile& profile) {
    if (inst_list.empty()) {
        return;
    }
    Translator translator{block, info, runtime_info, profile};
    for (const auto& inst : inst_list) {
        pc += inst.length;

        // Special case for emitting fetch shader.
        if (inst.opcode == Opcode::S_SWAPPC_B64) {
            ASSERT(info.stage == Stage::Vertex || info.stage == Stage::Export);
            translator.EmitFetch(inst);
            continue;
        }

        // Emit instructions for each category.
        switch (inst.category) {
        case InstCategory::DataShare:
            translator.EmitDataShare(inst);
            break;
        case InstCategory::VectorInterpolation:
            translator.EmitVectorInterpolation(inst);
            break;
        case InstCategory::ScalarMemory:
            translator.EmitScalarMemory(inst);
            break;
        case InstCategory::VectorMemory:
            translator.EmitVectorMemory(inst);
            break;
        case InstCategory::Export:
            translator.EmitExport(inst);
            break;
        case InstCategory::FlowControl:
            translator.EmitFlowControl(pc, inst);
            break;
        case InstCategory::ScalarALU:
            translator.EmitScalarAlu(inst);
            break;
        case InstCategory::VectorALU:
            translator.EmitVectorAlu(inst);
            break;
        case InstCategory::DebugProfile:
            break;
        default:
            UNREACHABLE();
        }
    }
}

} // namespace Shader::Gcn