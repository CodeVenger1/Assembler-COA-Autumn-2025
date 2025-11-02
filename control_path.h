// #include <bits/stdc++.h>
#include "assembler.h"
// using namespace std;

class ControlSignal {
public:
    int regRead , regWrite, ALUSrc, memRead, memWrite, mem2reg, branch, jump, ALUOp;
    ControlSignal() : regRead(0) , regWrite(0), ALUSrc(0), memRead(0), memWrite(0), mem2reg(0), branch(0), jump(0), ALUOp(0b00) {};
};

unordered_map<int,ControlSignal*> controlMapping;

void initControlMapping() {
    controlMapping[51] = new ControlSignal();
    controlMapping[51] -> regRead = 1;
    controlMapping[51] -> regWrite = 1;
    controlMapping[51] -> ALUOp = 0b10; 
    
    controlMapping[19] = new ControlSignal();
    controlMapping[19] -> regRead = 1;
    controlMapping[19] -> regWrite = 1;
    controlMapping[19] -> ALUSrc = 1;
    controlMapping[19] -> ALUOp = 0b10;
    
    controlMapping[99] = new ControlSignal();
    controlMapping[99] -> regRead = 1;
    controlMapping[99] -> branch = 1;
    controlMapping[99] -> ALUOp = 0b01;
    
    controlMapping[35] = new ControlSignal();
    controlMapping[35] -> regRead = 1;
    controlMapping[35] -> memWrite = 1;
    controlMapping[35] -> ALUSrc = 1;
    controlMapping[35] -> ALUOp = 0b00;
    
    controlMapping[3] = new ControlSignal();
    controlMapping[3] -> regRead = 1;
    controlMapping[3] -> regWrite = 1;
    controlMapping[3] -> ALUSrc = 1;
    controlMapping[3] -> memRead = 1;
    controlMapping[3] -> mem2reg = 1;
    controlMapping[3] -> ALUOp = 0b00;
    
    controlMapping[111] = new ControlSignal();
    controlMapping[111] -> jump = 1;
    controlMapping[111] -> regWrite = 1;
    controlMapping[111] -> ALUOp = 0b11;
    
    
    controlMapping[103] = new ControlSignal();
    controlMapping[103] -> jump = 1;
    controlMapping[103] -> regWrite = 1;
    controlMapping[103] -> ALUOp = 0b11;
}

int ALUControl(int ALUOP,int func7, int func3) {
    switch(ALUOP) {
        case 0b00 : 
            return 0b0010;
        
        case 0b01 :
            return 0b0110;
        
        case 0b10 :
            if(func7 == 32) return 0b0110;
            if(func3 == 0) return 0b0010;
            if(func3 == 3) return 0b0000;
            if(func3 == 4) return 0b0001;
        default:
            return 0b1111;
    }
    return 0b1111;
}



// int main() {
    // initOpCodes();
//     initFuncCodes();
//     initOpTypes();
    
//     initControlMapping(); 
//     vector<int> machineCodes;
    
//     ifstream in("input_max_N.txt");
//     // ofstream out("output.txt");
//     if(!in) {
//         cerr << "Error : File not found ...\n";
//         return 1;
//     }
//     if(!out) {
//         cerr << "Error : File not found ...\n";
//         return 1;
//     }
    
//     string ins;
//     while(getline(in, ins)) {
//         vector<string> instruction = parse(ins);
//         int opCode = ins_opCode_map[instruction[0]];
//         // out << D2B(opTypes[opCode](instruction)) << '\n';
//         // cout << D2B(opTypes[opCode](instruction)) << '\n';
//         machineCodes.push_back(opTypes[opCode](instruction));
//     }
    
//     in.close();
//     out.close();
    
//     return 0;
// }