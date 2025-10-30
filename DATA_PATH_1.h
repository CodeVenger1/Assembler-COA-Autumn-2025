
#pragma once
#include <bits/stdc++.h>
#include "CONTROL_PATH_1.h"

typedef struct {
    int value = 0;
    bool in_use = 0;
} GPR;

GPR registers[32];
int memory[65536] = {0};

typedef struct {
    unsigned int npc,jpc,bpc,dpc;
} TPC;

int PC = 0;
vector<unsigned int> instructions;
// bool pc_valid = 0;

// typedef struct {
//     unsigned int rsl1, rsl2, rdl;
// } operand_forwarding_regs;

typedef struct {
    bool stall = 0;
    bool valid = 0;
    unsigned int pc;
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

class InstructionFetch {
    public:
    instruction_decode_info begin(instruction_fetch_info IFI) {
        instruction_decode_info IDI;
        
        IDI.ir = instructions[IFI.pc];
        
        IDI.pc.npc = IFI.pc + 1;
        IDI.pc.dpc = IFI.pc;
        
        int op = IDI.ir & 0b1111111;
        if(op == 111 || op == 99 || op == 103) {
            IFI.stall = 1;
            return IDI;
        }
        IDI.valid = 1;
        
        return IDI;
    }
};

class InstructionDecode {
    typedef struct {
        unsigned int opCode, rs1, rs2, rd, func3, func7;
        int imm;
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
        INS.opCode = ins & 0b1111111;
        ins >>= 7;
        
        INS.rd = ins & 0b11111;
        ins >>= 5;
        
        INS.func3 = ins & 0b111;
        ins >>= 3;
        
        INS.rs1 = ins & 0b11111;
        ins >>= 5;
        
        INS.rs2 = ins & 0b11111;
        ins >>= 5;
        
        INS.func7 = ins;
        
        INS.imm = 0;
        
        switch(INS.opCode) {
            case 19:
                INS.imm = INS.func7;
                INS.imm <<= 5;
                INS.imm = INS.imm | INS.rs2;
                INS.imm = sign_extend_12(INS.imm);
                break;
            
            case 103:
                INS.imm = INS.func7;
                INS.imm <<= 5;
                INS.imm = INS.imm | INS.rs2;
                INS.imm = sign_extend_12(INS.imm);
                break;
            
            case 3:
                INS.imm = INS.func7;
                INS.imm <<= 5;
                INS.imm = INS.imm | INS.rs2;
                INS.imm = sign_extend_12(INS.imm);
                break;
            
            case 35:
                INS.imm = INS.func7;
                INS.imm <<= 5;
                INS.imm = INS.imm | INS.rd;
                INS.imm = sign_extend_12(INS.imm);
                break;
            
            case 99:
                INS.imm = INS.func7;
                INS.imm <<= 5;
                INS.imm = INS.imm | INS.rd;
                INS.imm = sign_extend_12(INS.imm);
                break;
            
            case 111:
                INS.imm = INS.func7;
                INS.imm <<= 5;
                INS.imm = INS.imm | INS.rs2;
                INS.imm <<= 5;
                INS.imm = INS.imm | INS.rs1;
                INS.imm <<= 3;
                INS.imm = INS.imm | INS.func3;
                INS.imm = sign_extend_20(INS.imm);
                INS.imm = sign_extend_12(INS.imm);
                break;
            
            default:
                break;
        }

        // cout << INS.imm << ' ' << INS.func7 << ' ' << INS.rs2 << ' ' << INS.rs1 << ' ' << INS.func3 << ' ' << INS.rd << ' ' << INS.opCode << endl;
        
        return INS;
    }
    
public:
    instruction_execution_info begin(instruction_decode_info IDI) {
        instruction_execution_info IEI;
        
        INS_SEGS INS = instructionSegregation(IDI.ir);
        
        if(registers[INS.rd].in_use || registers[INS.rs1].in_use || registers[INS.rs2].in_use) {
            IDI.stall = 1;
            return IEI;
        }
        registers[INS.rd].in_use = 1;
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
        IDI.stall = 0;
        IEI.valid = 1;
        
        return IEI;
    }    
};

class InstructionExecute {
    ControlPath CP;
    
    int ALUOperation(unsigned int ALUOp, unsigned int func3, unsigned int func7, int src1, int src2) {
        unsigned int operation = CP.ALUControl(ALUOp, func3, func7);
        if(operation == 32) return 0;
        // cout << ALUOp << ' ' << src1 << ' ' << src2 << endl;
        
        switch(ALUOp) {
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
                return (src2 != 0) ? (src1/src2) : 0;
            case 12:
                return (src1 == src2) ? 1:0;
            case 13:
                return (src1 != src2) ? 1:0;
            case 14:
                return (src1 < src2) ? 1:0;
            case 15:
                return (src1 >= src2) ? 1:0;
            default:
                return 0;
        }
    }
    
public:
    memory_operation_info begin(instruction_execution_info IEI) {
        memory_operation_info MOI;
        
        int aluSrc1 = IEI.data1;
        int aluSrc2 = IEI.cw.ALUSrc ? IEI.imm : IEI.data2;
        
        int ALUOut = ALUOperation(IEI.cw.ALUOp, IEI.func3, IEI.func7, aluSrc1, aluSrc2);
        
        MOI.ALUOut = ALUOut;
        MOI.pc = IEI.pc;
        MOI.pc.bpc = IEI.pc.dpc + IEI.imm;
        MOI.cw = IEI.cw;
        MOI.rd = IEI.rd;
        MOI.rs2 = IEI.rs2;
        MOI.valid = 1;
        IEI.stall = 0;
        return MOI;
    }
};

class MemoryOperation {
public:
    register_writeback_info begin(memory_operation_info MOI) {
        register_writeback_info RWI;
        
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
        MOI.stall = 0;
        RWI.valid = 1;
        return RWI;
    }
};

class RegisterWriteback {
public:
    instruction_fetch_info begin(register_writeback_info RWI) {
        instruction_fetch_info IFI;
        
        if(RWI.cw.branch) {
            IFI.pc = RWI.ALUOut ? RWI.pc.bpc : RWI.pc.npc;
        } else if(RWI.cw.jump) {
            IFI.pc = RWI.pc.jpc;
        } else {
            IFI.pc = RWI.pc.npc;
        }
        
        if(IFI.pc >= instructions.size()) IFI.valid = 0;
        else IFI.valid = 1;
        
        if(RWI.cw.memToReg) {
            registers[RWI.rd].value = RWI.LDOut;
        } else if(RWI.cw.regWrite) {
            registers[RWI.rd].value = RWI.ALUOut;
        } else if(RWI.cw.jump) {
            registers[RWI.rd].value = RWI.pc.npc;
        }
        
        registers[RWI.rd].in_use = 0;
        return IFI;
    }
};
