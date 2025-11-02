#include <bits/stdc++.h>
#include "assembler.h"
#include "DATA_PATH_1.h"

using namespace std;

int main() {
    initOpCodes();
    initFuncCodes();
    initOpTypes();
    
    ifstream in("input_sum_N.txt");
    if(!in) {
        cerr << "Error : File not found ...";
        return 1;
    }
    
    string line;
    while(getline(in, line)) {
        vector<string> instruction = parse(line);
        int opCode = ins_opCode_map.at(instruction[0]);
        // instructions is the global vector<unsigned int> declared in datapath.cpp
        instructions.push_back(opTypes[opCode](instruction));
    }

    // print instructions in binary (MSB first)
    for(unsigned int inst: instructions) {
        for(int i = 0; i < 32; i++) {
            cout << (((inst & (1u << (31 - i))) != 0) ? '1' : '0') << ' ';
        }
        cout << endl;
    }

    int cc = 0;
    InstructionFetch IF;
    InstructionDecode ID;
    InstructionExecute IE;
    MemoryOperation MEM;
    RegisterWriteback WB;

    instruction_fetch_info IFI;
    instruction_decode_info IDI;
    instruction_execution_info IEI;
    memory_operation_info MOI, MOI_temp;
    register_writeback_info RWI;

    if(PC < (int)instructions.size()) IFI.valid = 1;
    else IFI.valid = 0;

    while(IFI.valid || IDI.valid || IEI.valid || MOI.valid || RWI.valid) {
        // if(cc > 50) break;
        // cout << IFI.valid << ' ' << IDI.valid << ' ' << IEI.valid << ' ' << MOI.valid << ' ' << RWI.valid << endl;
        if(RWI.valid && !RWI.stall) {
            WB.begin(RWI);
        }

        if(MOI.valid && !MOI.stall) {
            RWI = MEM.begin(MOI);
        } else RWI.valid = 0;

        if(IEI.valid && !IEI.stall) {
            MOI = IE.begin(IEI);
        } else MOI.valid = 0;

        // if(!stall_decode_global && !stall_fetch_global) {
        //     if(IDI.valid && !IDI.stall) {
        //         IEI = ID.begin(IDI);
        //     } else IEI.valid = 0;

        //     if(IFI.valid && !IFI.stall) {
        //         IDI = IF.begin(IFI);
        //     } else IDI.valid = 0;
        // } else if(stall_decode_global && !stall_fetch_global) {
        //     IEI.valid = 0;       
        // } else if(!stall_decode_global && stall_fetch_global) {
        //     IFI.valid = 0;

        //     IEI = ID.begin(IDI);
        // } else {
        //     //TODO
        // }

        if(IDI.valid && !IDI.stall) {
            IEI = ID.begin(IDI);
        } else IEI.valid = 0;

        if(stall_decode_global) {
            // cout << "hello" << endl;
            // stall_decode_global = 0;
        } else {
            if(stall_fetch_global) {

            } else {
                if(IFI.valid && !IFI.stall) {
                    IDI = IF.begin(IFI);
                } else IDI.valid = 0;
            }
        }

        if(PC < (int)instructions.size()) IFI.valid = 1;
        else IFI.valid = 0;


        cout << "cycle: " << cc << '\n';
        // cout << PC << ' ' << IDI.pc.dpc << ' ' << IEI.pc.dpc << '-' << IEI.pc.bpc << '-' << IEI.pc.npc << '-' << IEI.pc.jpc << ' ' << MOI.pc.dpc << ' ' << RWI.pc.dpc << endl;
        for(int i = 0; i < 9; i++) {
            cout << 'x' << i << ": " << registers[i].in_use << ' ';
        }
        cout << endl;
        for(int i = 0; i < 9; i++) {
            cout << 'x' << i << ": " << registers[i].value << ' ';
        }
        cout << endl << endl << endl;
        cc++;
    }

    // final register dump
    for(int i = 0; i < 9; i++) {
        cout << 'x' << i << ": " << registers[i].value << ' ';
    }
    cout << endl;
    
    for(int i = 0; i < 10; i++) {
        cout << "mem " << i << ": " << memory[i] << endl;
    }
    return 0;
}