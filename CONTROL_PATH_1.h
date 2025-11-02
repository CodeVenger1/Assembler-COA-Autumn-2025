typedef struct {
    bool regRead = false, regWrite = false;
    bool memRead = false, memWrite = false, memToReg = false;
    bool ALUSrc = false;
    unsigned int ALUOp = 32;
    bool branch = false, jump = false;
} ControlWord;

class ControlPath {
    unsigned int generateALUOp(unsigned int func3, unsigned int func7,unsigned int opCode);
    public:
        ControlWord generateControlWord(unsigned int func3, unsigned int func7, unsigned int opCode);
        unsigned int ALUControl(unsigned int ALUOp, unsigned int func3, unsigned int func7);
};

unsigned int ControlPath::generateALUOp(unsigned int func3, unsigned int func7,unsigned int opCode) {
    if (opCode == 51) {  
        if (func7 == 0) {
            switch (func3) {
                case 0: return 0;   // ADD
                case 1: return 1;   // SLT
                case 2: return 2;   // SLTU
                case 3: return 3;   // AND
                case 4: return 4;   // OR
                case 5: return 5;   // XOR
                case 6: return 6;   // SLL
                case 7: return 9;   // SRL
                default: return 32;
            }
        } 
        else if (func7 == 32) {
            switch (func3) {
                case 0: return 8;   // SUB
                case 1: return 9;   // SRA
                default: return 32;
            }
        } 
        else if (func7 == 31) {
            switch (func3) {
                case 0: return 10;  // MUL
                case 1: return 11;  // DIV
                default: return 32;
            }
        }
    }

    else if (opCode == 19) {
        switch (func3) {
            case 0: return 0;   // ADDI
            case 1: return 1;   // SLTI
            case 2: return 2;   // SLTIU
            case 3: return 3;   // ANDI
            case 4: return 4;   // ORI
            case 5: return 5;   // XORI
            case 6: return 10;  // MULI (custom)
            case 7: return 11;  // DIVI (custom)
            default:
                if (func7 == 0 && func3 == 0) return 6;    // SLLI
                if (func7 == 0 && func3 == 1) return 9;    // SRLI
                if (func7 == 32 && func3 == 0) return 9;   // SRAI
                return 0;
        }
    }

    else if (opCode == 99) {
        switch (func3) {
            case 0: return 12;  // BEQ
            case 1: return 13;  // BNE
            case 2: return 14;  // BLT
            case 4: return 15;  // BGE or BGT
            default: return 0;
        }
    }

    else if (opCode == 3 || opCode == 35) {
        return 0;  // base + offset (add)
    }

    else if (opCode == 111 || opCode == 103) {
        return 0;
    }

    return 32;
}

ControlWord ControlPath::generateControlWord(unsigned int func3, unsigned int func7,unsigned int opCode) {
    ControlWord cw;
    
    cw.ALUOp = generateALUOp(func3,func7,opCode);
    
    switch(opCode) {
        case 51:
            cw.regRead = true;
            cw.regWrite = true;
            cw.ALUSrc = false;
            break;
        
        case 19:
            cw.regRead = true;
            cw.regWrite = true;
            cw.ALUSrc = true;
            break;
        
        case 35:
            cw.regRead = true;
            cw.ALUSrc = true;
            cw.memWrite = true;
            break;
        
        case 3:
            cw.regRead = true;
            cw.regWrite = true;
            cw.ALUSrc = true;
            cw.memRead = true;
            cw.memToReg = true;
            break;
        
        case 99:
            cw.regRead = true;
            cw.branch = true;
            break;
        
        case 111:
            cw.regWrite = true;
            cw.jump = true;
            break;
            
        case 103:
            cw.regRead = true;
            cw.regWrite = true;
            cw.jump = true;
            break;
        default:
            break;
    }
    
    return cw;
}

unsigned int ControlPath::ALUControl(unsigned int ALUOp, unsigned int func3, unsigned int func7) {
    if(ALUOp != 32) return ALUOp;
    return 32;
}