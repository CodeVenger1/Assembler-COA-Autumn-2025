
#include "assembler.h"
#include "DATA_PATH_1.h"


int main() {
    initOpCodes();
    initFuncCodes();
    initOpTypes();
    
    // vector<unsigned int> instructions;
    
    ifstream in("output.txt");
    if(!in) {
        cerr << "Error : File not found ...";
        return 1;
    }
    
    string ins;
    while(getline(in, ins)) {
        vector<string> instruction = parse(ins);
        int opCode = ins_opCode_map[instruction[0]];
        instructions.push_back(opTypes[opCode](instruction));
    }

    for(int ins: instructions) {
        for(int i = 0; i < 32; i++) {
            cout << ((ins & (1 << (31-i))) != 0) << ' ';
        }
        cout << endl;
    }

    int cc = 0;
    InstructionFetch IF;
    InstructionDecode ID;
    InstructionExecute IE;
    MemoryOperation MEM;
    RegisterWriteback WB;

    instruction_fetch_info IFI, IFI_temp;
    instruction_decode_info IDI;
    instruction_execution_info IEI;
    memory_operation_info MOI;
    register_writeback_info RWI;

    // int PC = 0;
    IFI.pc = PC;
    if(PC < instructions.size()) IFI.valid = 1;
    while(IFI.valid) {
        if(!RWI.stall && RWI.valid) {
            IFI_temp = WB.begin(RWI);
        } else if(!MOI.stall && MOI.valid) {
            RWI = MEM.begin(MOI);
        } else if(!IEI.stall && IEI.valid) {
            MOI = IE.begin(IEI);
        } else if(!IDI.stall && IDI.valid) {
            IEI = ID.begin(IDI);
        } else if(!IFI.stall && IFI.valid) {
            IDI = IF.begin(IFI);
        }
        if(IFI_temp.valid) {
            IFI = IFI_temp;
            IFI_temp.valid = 0;
        } else if(!IFI_temp.valid) {
            PC++;
            IFI.pc = PC;
        }
        cc++;
        cout << cc << endl;
    }
    for(int i = 0; i < 9; i++) {
        cout << 'x' << i << ": " << registers[i].value << ' ';
    }
    cout << endl;
    
    for(int i = 0; i < 10; i++) {
        cout << "mem " << i << ": " << memory[i] << endl;
    }
}
