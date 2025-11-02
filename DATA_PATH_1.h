#pragma once
#include <bits/stdc++.h>
#include "CONTROL_PATH_1.h"

typedef struct {
    int value = 0;
    bool in_use = 0;
} GPR;

typedef struct {
    unsigned int npc,jpc,bpc,dpc;
} TPC;

GPR registers[32];
int memory[65536] = {0};

// global program counter used by pipeline
int PC = 0;

// global stalls for fetch/decode (stalled when RAW or branch/jump need to stop fetch)
bool stall_fetch_global = 0;
bool stall_decode_global = 0;

std::vector<unsigned int> instructions;

// --- pipeline stage infos ---
typedef struct {
    bool stall = 0;
    bool valid = 0;
} instruction_fetch_info;

typedef struct {
    bool stall = 0;
    bool valid = 0;
    TPC pc;
    unsigned int ir;
} instruction_decode_info;

typedef struct {
    bool stall = 0;
    bool valid = 0;
    TPC pc;
    int imm;
    ControlWord cw;
    unsigned int data1, data2, rd, rs2;
    unsigned int func3, func7;
} instruction_execution_info;

typedef struct {
    bool stall = 0;
    bool valid = 0;
    TPC pc;
    ControlWord cw;
    unsigned int ALUOut;
    unsigned int rd;
    unsigned int rs2;
} memory_operation_info;

typedef struct {
    bool stall = 0;
    bool valid = 0;
    TPC pc;
    ControlWord cw;
    unsigned int ALUOut;
    unsigned int rd;
    unsigned int LDOut;
} register_writeback_info;

// --- Instruction Fetch ---
class InstructionFetch {
public:
    instruction_decode_info begin(instruction_fetch_info IFI) {
        instruction_decode_info IDI;
        // cout << "Fetch PC: " << PC << ' ' << stall_fetch_global << endl;

        if(!IFI.valid) return IDI;

        IDI.ir = instructions[PC];
        
        IDI.pc.npc = PC + 1;
        IDI.pc.dpc = PC;
        PC++;
        
        int opCode = (IDI.ir) & 0b1111111;
        if(opCode == 103 || opCode == 111 || opCode == 99) {
            stall_fetch_global = 1;
        }

        IDI.valid = 1;

        return IDI;
    }
};

// --- Instruction Decode ---
class InstructionDecode {
    typedef struct {
        unsigned int opCode, rs1, rs2, rd, func3, func7;
        int imm;
        bool has_rs1 = 1, has_rs2 = 1;
    } INS_SEGS;
    
    ControlPath CP;
    
    int sign_extend_12(int x) {
        x &= 0xFFF;
        return (x << 20) >> 20;
    }
    
    int sign_extend_20(int x) {
        x &= 0xFFFFF;
        return (x << 12) >> 12;
    }
    
    INS_SEGS instructionSegregation(unsigned int ins) {
        INS_SEGS INS;
        INS.opCode = ins & 0b1111111u;
        ins >>= 7;
        
        INS.rd = ins & 0b11111u;
        ins >>= 5;
        
        INS.func3 = ins & 0b111u;
        ins >>= 3;
        
        INS.rs1 = ins & 0b11111u;
        ins >>= 5;
        
        INS.rs2 = ins & 0b11111u;
        ins >>= 5;
        
        INS.func7 = ins;
        
        INS.imm = 0;
        
        switch(INS.opCode) {
            case 19: // I-type (ADDI etc)
            case 103: // JALR (I-type)
            case 3:  // load (I-type)
            {
                INS.imm = INS.func7;
                INS.imm <<= 5;
                INS.imm = INS.imm | INS.rs2;
                INS.imm = sign_extend_12(INS.imm);
                INS.has_rs2 = 0;
                break;
            }
            
            case 35: // S-type (store)
            {
                INS.imm = INS.func7;
                INS.imm <<= 5;
                INS.imm = INS.imm | INS.rd; // note: after earlier shifts rd contained lower bits
                INS.imm = sign_extend_12(INS.imm);
                INS.has_rs1 = 0;
                break;
            }
            
            case 99: // B-type (branch)
            {
                INS.imm = INS.func7;
                INS.imm <<= 5;
                INS.imm = INS.imm | INS.rd;
                INS.imm = sign_extend_12(INS.imm);
                break;
            }
            
            case 111: // J-type (JAL)
            {
                INS.imm = INS.func7;
                INS.imm <<= 5;
                INS.imm = INS.imm | INS.rs2;
                INS.imm <<= 5;
                INS.imm = INS.imm | INS.rs1;
                INS.imm <<= 3;
                INS.imm = INS.imm | INS.func3;
                INS.imm = sign_extend_20(INS.imm);
                INS.has_rs2 = 0;
                INS.has_rs1 = 0;
                break;
            }
            
            default:
                break;
        }

        // cout << INS.imm << ' ' << INS.func7 << ' ' << INS.rs2 << ' ' << INS.rs1 << ' ' << INS.func3 << ' ' << INS.rd << ' ' << INS.opCode << endl;
        return INS;
    }
    
public:
    instruction_execution_info begin(instruction_decode_info IDI) {
        instruction_execution_info IEI;
        // cout << "Decode PC: " << IDI.pc.dpc << ' ' << stall_decode_global << endl;
        
        if(!IDI.valid) return IEI;

        INS_SEGS INS = instructionSegregation(IDI.ir);

        if((INS.has_rs1 && registers[INS.rs1].in_use) || (INS.has_rs2 && registers[INS.rs2].in_use)) {
            // cout << registers[INS.rs1].in_use << ' ' << registers[INS.rs2].in_use << ' ';
            stall_decode_global = 1;
            // cout << "decode stalled\n";
            return IEI;
        }

        if(INS.opCode != 35 && INS.opCode != 99) {
            registers[INS.rd].in_use = 1;
            // cout << INS.rd << " hfoashfouehouf" << endl;
        }
        
        IEI.pc = IDI.pc;
        IEI.pc.jpc = IEI.pc.npc + INS.imm;
        
        IEI.imm = INS.imm;
        
        IEI.cw = CP.generateControlWord(INS.func3, INS.func7, INS.opCode);
        
        if(IEI.cw.regRead) {
            IEI.data1 = registers[INS.rs1].value;
            IEI.data2 = registers[INS.rs2].value;
        }
        // cout << IEI.data1 << ' ' << IEI.data2 << endl;
        IEI.func3 = INS.func3;
        IEI.func7 = INS.func7;
        
        IEI.rd = INS.rd;
        IEI.rs2 = INS.rs2;
        IEI.valid = 1;
        
        return IEI;
    }    
};

// --- Instruction Execute ---
class InstructionExecute {
    ControlPath CP;
    
    int ALUOperation(unsigned int ALUOp, unsigned int func3, unsigned int func7, int src1, int src2) {
        unsigned int operation = CP.ALUControl(ALUOp, func3, func7);
        if(operation == 32) return 0;
        
        switch(operation) { // switch on resolved operation (not on the ALUOp index)
            case 0:
                return src1 + src2;
            case 1:
                return (src1 < src2) ? 1 : 0;
            case 2:
                return ((unsigned int)src1 < (unsigned int)src2) ? 1 : 0;
            case 3:
                return src1 & src2;
            case 4:
                return src1 | src2;
            case 5:
                return src1 ^ src2;
            case 6:
                return src1 << (src2 & 0x1F);
            case 7:
                return (int)((unsigned int)src1 << (src2 & 0x1F));
            case 8:
                return src1 - src2;
            case 9:
                return src1 >> (src2 & 0x1F);
            case 10:
                return src1 * src2;
            case 11:
                return (src2 != 0) ? (src1 / src2) : 0;
            case 12:
                return (src1 == src2) ? 1 : 0;
            case 13:
                return (src1 != src2) ? 1 : 0;
            case 14:
                return (src1 < src2) ? 1 : 0;
            case 15:
                return (src1 >= src2) ? 1 : 0;
            default:
                return 0;
        }
    }
    
public:
    memory_operation_info begin(instruction_execution_info IEI) {
        memory_operation_info MOI;
        // cout << "Execution PC: " << IEI.pc.dpc << ' ';

        if(!IEI.valid) return MOI;
        
        int aluSrc1 = IEI.data1;
        int aluSrc2 = IEI.cw.ALUSrc ? IEI.imm : IEI.data2;

        cout << aluSrc1 << ' ' << aluSrc2 << endl;
        
        int ALUOut = ALUOperation(IEI.cw.ALUOp, IEI.func3, IEI.func7, aluSrc1, aluSrc2);
        
        MOI.ALUOut = ALUOut;
        MOI.pc = IEI.pc;
        MOI.pc.bpc = IEI.pc.dpc + IEI.imm;
        MOI.cw = IEI.cw;
        MOI.rd = IEI.rd;
        MOI.rs2 = IEI.rs2;
        MOI.valid = 1;
        
        if(IEI.cw.branch) {
            PC = ALUOut ? MOI.pc.bpc : MOI.pc.npc;
            stall_fetch_global = 0;
        }
        return MOI;
    }
};

// --- Memory Operation ---
class MemoryOperation {
public:
    register_writeback_info begin(memory_operation_info MOI) {
        register_writeback_info RWI;
        // cout << "Memory PC: " << MOI.pc.dpc << endl;
        
        if(!MOI.valid) return RWI;

        RWI.pc = MOI.pc;
        RWI.cw = MOI.cw;
        RWI.ALUOut = MOI.ALUOut;
        RWI.rd = MOI.rd;
        
        int loadedData = 0;
        if(MOI.cw.memRead) {
            loadedData = memory[MOI.ALUOut];
            RWI.LDOut = loadedData;
        }
        
        if(MOI.cw.memWrite) {
            memory[MOI.ALUOut] = registers[MOI.rs2].value;
        }

        RWI.valid = 1;
        
        return RWI;
    }
};

// --- Register Writeback ---
class RegisterWriteback {
public:
    void begin(register_writeback_info RWI) {
        // cout << "Writeback PC: " << RWI.pc.dpc << ' ' << stall_fetch_global << endl;

        if(!RWI.valid) return;
        
        if(RWI.cw.memToReg) {
            registers[RWI.rd].value = RWI.LDOut;
        } else if(RWI.cw.regWrite) {
            registers[RWI.rd].value = RWI.ALUOut;
        } else if(RWI.cw.jump) {
            registers[RWI.rd].value = RWI.pc.npc;
            stall_fetch_global = 0;
        }
        registers[RWI.rd].in_use = 0;

        
        stall_decode_global = 0;
        // cout << "decode unstalled" << endl;
    }
};