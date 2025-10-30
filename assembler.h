#include <bits/stdc++.h>
using namespace std;

vector<string> parse(string& ins) {
    vector<string> parsed_ins;
    string curr_ins = "";
    for(char ch:ins) {
        if(ch == ' ' || ch == ',' || ch == '(' || ch == ')') {
            if(curr_ins.size()) parsed_ins.push_back(curr_ins);
            curr_ins = "";
        } else {
            curr_ins.push_back(ch);
        }
    }
    if(curr_ins.size()) parsed_ins.push_back(curr_ins);
    return parsed_ins;
}

string D2B(int n) {
    unsigned int x = static_cast<unsigned int>(n);
    string bin = "";
    for(int i = 31; i >= 0 ; i--) {
        bin.push_back((x & (1u << i)) ? '1' : '0');
    }
    return bin;
}

unordered_map<string, unsigned int> ins_opCode_map;
unordered_map<string, pair<int,int>> func_codes_map;
unsigned int (*opTypes[128])(vector<string>& parsed_int);
// unordered_map<string, int> reg_map;

unsigned int op51(vector<string>& parsed_ins) {
    int rd = stoi(parsed_ins[1].substr(1));
    int rs1 = stoi(parsed_ins[2].substr(1));
    int rs2 = stoi(parsed_ins[3].substr(1));
    
    auto [func7, func3] = func_codes_map[parsed_ins[0]];
    
    unsigned int machine_code = 51 | (rd << 7) | (func3 << 12) | (rs1 << 15) | (rs2 << 20) | (func7 << 25);
    return machine_code;
}

unsigned int op19(vector<string>& parsed_ins) {
    int rd = stoi(parsed_ins[1].substr(1));
    int rs1 = stoi(parsed_ins[2].substr(1));
    // cout << parsed_ins[3] << " "<<endl;
    int imm = stoi(parsed_ins[3]);
    // cout << imm
    auto [func7, func3] = func_codes_map[parsed_ins[0]];
    unsigned int machine_code;
    if(func7 != 1) 
        machine_code = ins_opCode_map[parsed_ins[0]] | (rd << 7) | (func3 << 12) | (rs1 << 15) | (imm << 20) | (func7 << 25);
    else 
        machine_code = ins_opCode_map[parsed_ins[0]] | (rd << 7) | (func3 << 12) | (rs1 << 15) | (imm << 20);
    return machine_code;
}

unsigned int op99(vector<string>& parsed_ins) {
    int rs1 = stoi(parsed_ins[1].substr(1));
    int rs2 = stoi(parsed_ins[2].substr(1));
    int imm = stoi(parsed_ins[3]);
    
    auto [_, func3] = func_codes_map[parsed_ins[0]];
    
    int imm1 = 0, imm2 = 0;
    for(int i = 0; i < 5; i++) imm1 |= (imm & (1 << i));
    for(int i = 5; i < 12; i++) imm2 |= (imm & (1 << i));
    imm2 >>= 5;
    
    unsigned int machine_code = 99 | (imm1 << 7) | (func3 << 12) | (rs1 << 15) | (rs2 << 20) | (imm2 << 25);
    return machine_code;
}

unsigned int op35(vector<string>& parsed_ins) {
    int data = stoi(parsed_ins[1].substr(1));
    int offset = stoi(parsed_ins[2]);
    int src = stoi(parsed_ins[3].substr(1));
    
    auto [_, func3] = func_codes_map[parsed_ins[0]];
    
    int imm1 = 0, imm2 = 0;
    for(int i = 0; i < 5; i++) imm1 |= (offset & (1 << i));
    for(int i = 5; i < 12; i++) imm2 |= (offset & (1 << i));
    imm2 >>= 5;
    
    unsigned int machine_code = ins_opCode_map[parsed_ins[0]] | (imm1 << 7) | (func3 << 12) | (src << 15) | (data << 20) | (imm2 << 25);
    return machine_code;
}

unsigned int op3(vector<string>& parsed_ins) {
    int dest = stoi(parsed_ins[1].substr(1));
    int src = stoi(parsed_ins[3].substr(1));
    int imm = stoi(parsed_ins[2]);
    
    auto [_, func3] = func_codes_map[parsed_ins[0]];
    unsigned int machine_code =  3 | (dest << 7) | (func3 << 12) | (src << 15) | (imm << 20);
    
    return machine_code;
}

unsigned int op111(vector<string>& parsed_ins) {
    int dest = stoi(parsed_ins[1].substr(1));
    // int src = stoi(parsed_ins[3].substr(1));
    int imm = stoi(parsed_ins[2]);
    
    auto [_, func3] = func_codes_map[parsed_ins[0]];
    unsigned int machine_code =  111 | (dest << 7) | (imm << 12);
    
    return machine_code;
}

unsigned int op103(vector<string>& parsed_ins) {
    int dest = stoi(parsed_ins[1].substr(1));
    int base = stoi(parsed_ins[2].substr(1));
    int imm = stoi(parsed_ins[3]);
    
    auto [_, func3] = func_codes_map[parsed_ins[0]];
    unsigned int machine_code =  103 | (dest << 7) | (func3 << 12) | (base << 15) | (imm << 20);
    
    return machine_code;
}


void initOpCodes() {
    ins_opCode_map.insert({"ADD", 51});
    ins_opCode_map.insert({"SLT", 51});
    ins_opCode_map.insert({"SLTU", 51});
    ins_opCode_map.insert({"AND", 51});
    ins_opCode_map.insert({"OR", 51});
    ins_opCode_map.insert({"XOR", 51});
    ins_opCode_map.insert({"SLL", 51});
    ins_opCode_map.insert({"SRL", 51});
    ins_opCode_map.insert({"SUB", 51});
    ins_opCode_map.insert({"SRA", 51});
    ins_opCode_map.insert({"MUL", 51});
    ins_opCode_map.insert({"DIV", 51});
    
    ins_opCode_map.insert({"ADDI", 19});
    ins_opCode_map.insert({"SLTI", 19});
    ins_opCode_map.insert({"SLTIU", 19});
    ins_opCode_map.insert({"ANDI", 19});
    ins_opCode_map.insert({"ORI", 19});
    ins_opCode_map.insert({"XORI", 19});
    ins_opCode_map.insert({"SLLI", 19});
    ins_opCode_map.insert({"SRLI", 19});
    ins_opCode_map.insert({"SRAI", 19});
    ins_opCode_map.insert({"MULI", 19});
    ins_opCode_map.insert({"DIVI", 19});
    
    ins_opCode_map.insert({"BEQ", 99});
    ins_opCode_map.insert({"BNE", 99});
    ins_opCode_map.insert({"BLT", 99});
    ins_opCode_map.insert({"BLTU", 99});
    ins_opCode_map.insert({"BGE", 99});
    ins_opCode_map.insert({"BGEU", 99});
    ins_opCode_map.insert({"BGT", 99});
    
    ins_opCode_map.insert({"SW", 35});
    ins_opCode_map.insert({"SHW", 35});
    ins_opCode_map.insert({"SD", 35});
    ins_opCode_map.insert({"SB", 35});
    
    
    ins_opCode_map.insert({"LW", 3});
    ins_opCode_map.insert({"LD", 3});
    ins_opCode_map.insert({"LH", 3});
    ins_opCode_map.insert({"LB", 3});
    ins_opCode_map.insert({"LWU", 3});
    ins_opCode_map.insert({"LHU", 3});
    ins_opCode_map.insert({"LBU", 3});
    
    ins_opCode_map.insert({"JAL", 111});
    
    ins_opCode_map.insert({"JALR", 103});
}

void initFuncCodes() {
    func_codes_map.insert({"ADD", {0, 0}});
    func_codes_map.insert({"SLT", {0, 1}});
    func_codes_map.insert({"SLTU", {0, 2}});
    func_codes_map.insert({"AND", {0, 3}});
    func_codes_map.insert({"OR", {0, 4}});
    func_codes_map.insert({"XOR", {0, 5}});
    func_codes_map.insert({"SLL", {0, 6}});
    func_codes_map.insert({"SRL", {0, 7}});
    func_codes_map.insert({"SUB", {32, 0}});
    func_codes_map.insert({"SRA", {32, 1}});
    func_codes_map.insert({"MUL", {31, 0}});
    func_codes_map.insert({"DIV", {31, 1}});
    
    func_codes_map.insert({"ADDI", {1, 0}});
    func_codes_map.insert({"SLTI", {1, 1}});
    func_codes_map.insert({"SLTIU", {1, 2}});
    func_codes_map.insert({"ANDI", {1, 3}});
    func_codes_map.insert({"ORI", {1, 4}});
    func_codes_map.insert({"XORI", {1, 5}});
    func_codes_map.insert({"MULI", {1, 6}});
    func_codes_map.insert({"DIVI", {1, 7}});
    func_codes_map.insert({"SLLI", {0, 0}});
    func_codes_map.insert({"SRLI", {0, 1}});
    func_codes_map.insert({"SRAI", {32, 0}});
    
    
    func_codes_map.insert({"BEQ", {0, 0}});
    func_codes_map.insert({"BNE", {0, 1}});
    func_codes_map.insert({"BLT", {0, 2}});
    func_codes_map.insert({"BLTU", {0, 3}});
    func_codes_map.insert({"BGE", {0, 4}});
    func_codes_map.insert({"BGEU", {0, 5}});
    func_codes_map.insert({"BGT", {0, 4}});
    
    func_codes_map.insert({"SW", {0, 0}});
    func_codes_map.insert({"SD", {0, 1}});
    func_codes_map.insert({"SHW", {0, 2}});
    func_codes_map.insert({"SB", {0, 3}});
    
    func_codes_map.insert({"LW", {0,0}});
    func_codes_map.insert({"LD", {0,1}});
    func_codes_map.insert({"LH", {0,2}});
    func_codes_map.insert({"LB", {0,3}});
    func_codes_map.insert({"LWU", {0,4}});
    func_codes_map.insert({"LHU", {0,5}});
    func_codes_map.insert({"LBU", {0,6}});
    
    func_codes_map.insert({"JAL", {0,0}});
    
    func_codes_map.insert({"JALR", {0,0}});
}

void initOpTypes() {
    opTypes[51] = op51;
    opTypes[19] = op19;
    opTypes[99] = op99;
    opTypes[35] = op35;
    opTypes[3] = op3;
    opTypes[111] = op111;
    opTypes[103] = op103;
} 

// int main() {
//     initOpCodes();
//     initFuncCodes();
//     initOpTypes();
    
    
//     ifstream in("input_max_N.txt");
//     ofstream out("output.txt");
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
//         cout << D2B(opTypes[opCode](instruction)) << '\n';
//     }
    
//     in.close();
//     out.close();
    
//     return 0;
// }