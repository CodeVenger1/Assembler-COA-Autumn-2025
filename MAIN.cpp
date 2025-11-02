
#include "assembler.h"
#include "DATA_PATH_1.h"


int main() {
    initOpCodes();
    initFuncCodes();
    initOpTypes();
    
    vector<unsigned int> instructions;
    
    ifstream in("input_fact.txt");
    // ofstream out("output.txt");
    if(!in) {
        cerr << "Error : File not found ...";
        return 1;
    }
    // if(!out) {
    //     cerr << "Error : File not found ...";
    //     return 1;
    // }
    
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

    
    InstructionFetch IF;
    InstructionDecode ID;
    InstructionExecute IE;
    MemoryOperation MEM;
    RegisterWriteback WB;
    
    instruction_fetch_info IFI = {0};
    IFI.pc = 0;
    IFI.halted = 0;
    
    while(true) {
        auto IDI = IF.begin(instructions, IFI);
        auto IEI = ID.begin(IDI);
        auto MOI = IE.begin(IEI);
        auto RWI = MEM.begin(MOI);
        IFI = WB.begin(RWI);
        
        if(IFI.halted) break;
        for(int i = 0; i < 9; i++) {
            cout << 'x' << i << ": " << registers[i] << ' ';
        }
        cout << endl;
    }
    
    for(int i = 0; i < 9; i++) {
        cout << 'x' << i << ": " << registers[i] << ' ';
    }
    cout << endl;
    
    for(int i = 0; i < 10; i++) {
        cout << "mem " << i << ": " << memory[i] << endl;
    }
    
    in.close();
    // out.close();
    
    return 0;
}
