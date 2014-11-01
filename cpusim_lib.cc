// IMPORTANT: DO NOT MODIFY THIS FILE. ALL PROGRAMS THAT DO NOT WORK
// WITH AN UNMODIFIED VERSION OF THIS FILE DO NOT RECEIVE ANY POINTS

#include <iostream>
#include <sstream>
#include <vector>


// useful constants
const int OC = 0x1000000;
const int O1 = 0x10000;
const int O2 = 0x100;

// output streams, used for verification
namespace cpu{
std::stringstream out;
std::stringstream aout;
std::stringstream dout;
}

// memory subsystem
unsigned int memory[256];

// memory access functions, to be used by initial setup in exercise part (a)
// and during simulation in exercise part (d)

// write data to virtual memory
// pre: 0 <= index < 256
void memory_write(const int index, const unsigned int value) {
    memory[index] = value;
}
// read data from virtual memory
// pre: 0 <= index < 256
unsigned int memory_read(const int index) {
    return memory[index];
}

// output function, to be called by simulator, exercise part (d)
void cpusim_out(const unsigned int value) {
    cpu::out << value << " ";
    std::cout << "out> " << value << std::endl;
}

// output function for decoding in exercise part (b).
void cpusim_outdecode(unsigned int opcode, unsigned int op1, unsigned int op2, unsigned int op3)
{
    std::cout << "dec> opcode= " << opcode << ", ops= " << op1 << ", " << op2 << ", " <<op3 << "\n";
    cpu::dout << std::hex << opcode * OC + op1 *O1 + op2 * O2 + op3 << " ";
}

// definition of test programs
struct Program {
    std::string name; // program name
    std::string code; // program code, also used for verification of disassembly and decoding
    std::string output; // expected output
};

Program Output =
{
    /*
     0: set A, 42    // variable: a
     1: out 10       // output a
     2: hlt          // stop
    */
    "output",
    "060a2a00 210A0000 30000000 0",
    "42"
};

Program Adder =
{
    /*
     0: set A, 2     // variable: a
     1: set B, 3     // variable: b
     2: add A, B, A  // a = a + b
     3: out 10       // output a
     4: hlt          // stop
    */
    "adder",
    "060a0200 060b0300 010a0b0a 210a0000 30000000 0",
    "5"
};

Program Resistor =
{
    /*
     0: set 1A, 10      // variable: r1 = 10
     1: set 1B, 100     // variable: r2 = 100
     2: set 1C, 50      // variable: r3 = 50
     3: set 1D, 190     // variable: r4 = 190
     4: set 1E, 2       // constant: 2
     5: add 1A, 1B, 1F  // r12 = r1 + r2
     6: add 1C, 1D, 20  // r34 = r3 + r4
     7: add 1F, 20, 21  // r1234 = r12 + r34
     8: mul 1F, 20, 22  // r = r12 * r34
     9: div 21, 1E, 23  // temp = r1234 / 2
    10: add 22, 23, 22  // r = r + temp
    11: div 22, 21, 22  // r = r / r1234
    12: out 22          // output r
    13: hlt             // stop
    */
    "resistor",
    "061a0a00 061b6400 061c3200 061dbe00 061e0200 011a1b1f 011c1d20 011f2021 031f2022 04211e23 01222322 04222122 21220000 30000000 0",
    "75"
};

Program Loop =
{
    /*
     0: set A, 10      // variable: i
     1: set B, 0       // constant: 0
     2: set C, 1       // constant: 1
     3: jeq 7, 10, 11  // if i == 0 goto 7
     4: out 10         // output i
     5: sub 10, 12     // i = i - 1
     6: jmp 3          // goto loop
     7: hlt
    */
    "loop",
    "060a0a00 060b0000 060c0100 15070a0b 210a0000 020a0c0a 10030000 30000000 0",
    "10 9 8 7 6 5 4 3 2 1"
};

Program Ggt =
{
    /*
     0: set b, 20     // variable: L
     1: set c, 12     // variable: R
     2: set d, 0      // constant: 0
     3: jeq 9, b, d   // if L == 0 goto 9
     4: jgr 7, c, b   // if R > L goto 7
     5: sub b, c, b   // L = L - R
     6: jmp 3         // goto 3
     7: sub c, b, c   // R = R - L
     8: jmp 3         // goto 3
     9: out 0xc       // output L
    10: hlt           // stop
    */
    "ggt",
    "60b1400 60c0c00 60d0000 15090b0d 13070c0b 20b0c0b 10030000 20c0b0c 10030000 210c0000 30000000 0",
    "4"
};

Program GgtIn =
{
    /*
     0: in b          // variable: L
     1: in c          // variable: R
     2: set d, 0      // constant: 0
     3: jeq 9, b, d   // if L == 0 goto 9
     4: jgr 7, c, b   // if R > L goto 7
     5: sub b, c, b   // L = L - R
     6: jmp 3         // goto 3
     7: sub c, b, c   // R = R - L
     8: jmp 3         // goto 3
     9: out 0xc       // output L
    10: hlt           // stop
    */
    "ggt",
    "220b0000 220c0000 60d0000 15090b0d 13070c0b 20b0c0b 10030000 20c0b0c 10030000 210c0000 30000000 0",
    "%"
};


class Verify
{
    std::vector<Program> programs;

    bool memory_matches_program(const Program p)
    {
        std::stringstream s;
        s << p.code;
        unsigned int number;
        unsigned int index=0;
        while (!(s >> std::hex >> number).fail())
        {
            if (number != memory_read(index))
                return false;
            ++index;
        }
        return true;
    }

    void pass_msg(bool b, char part)
    {
        std::cout << "part (" << part << ")";
        if (b)
            std::cout << " passed.\n";
        else
            std::cout << " not (yet) passed.\n";
    }

    void check_decode(const Program p)
    {
        std::stringstream s;
        s << p.code;
        std::string obss = cpu::dout.str();
        unsigned int observed;
        unsigned int expected;
        bool b1;
        bool b2;
        do
        {
            b1 = !(s >> std::hex >> expected).fail() && (expected != 0);
            b2 = !(cpu::dout >> std::hex >> observed).fail() && (observed != 0);
        }
        while (b1 && b2 && (expected == observed));
        pass_msg(!b1 && !b2,'b');
    }

    bool assemble(std::stringstream & s, unsigned int & value)
    {
        std::string str;
        unsigned int ops;
        value = 0;
        if ((s >> str).fail())
            return false;
        if      (str == "set") {value = 0x06; ops = 3;}
        else if (str == "out") {value = 0x21; ops = 1;}
        else if (str == "in")  {value = 0x22; ops = 1;}
        else if (str == "hlt") {value = 0x30; ops = 0;}
        else if (str == "add") {value = 0x01; ops = 3;}
        else if (str == "sub") {value = 0x02; ops = 3;}
        else if (str == "mul") {value = 0x03; ops = 3;}
        else if (str == "div") {value = 0x04; ops = 3;}
        else if (str == "mod") {value = 0x05; ops = 3;}
        else if (str == "jmp") {value = 0x10; ops = 1;}
        else if (str == "jge") {value = 0x11; ops = 3;}
        else if (str == "jle") {value = 0x12; ops = 3;}
        else if (str == "jgr") {value = 0x13; ops = 3;}
        else if (str == "jls") {value = 0x14; ops = 3;}
        else if (str == "jeq") {value = 0x15; ops = 3;}
        else if (str == "jne") {value = 0x16; ops = 3;}
        else return false;

        value *= 0x1000000;
        unsigned int fact = 0x10000;
        while (ops > 0)
        {
            unsigned int op;
            char ch;
            if ((s >> op).fail())
                return false;
            value += op * fact;
            fact /= 0x100;
            --ops;
            if ((ops > 0) && ((s >> ch).fail() || (ch != ',')))
                return false;
        }
        return true;
    }

    void trytoassemble()
    {
        std::string obss = cpu::aout.str();
        std::stringstream d;
        d << obss;
        unsigned int value;

        bool first = true;
        while (!d.eof())
        {

            std::string line;
            std::getline(d, line);
            if (!line.empty())
            {
                if (first)
                {
                    std::cout << "your disassembly         [and my assembly of it]:\n\n";
                    first = false;
                }

                std::cout << "asm> " << line;
                std::stringstream scan;
                scan << line;
                unsigned int pos = line.length();
                while (pos < 20) // tab
                {
                    std::cout << " ";
                    pos++;
                }
                if(assemble (scan, value))
                    std::cout << "[" << std::hex << value << "]";
                else
                    std::cout << "[" << "could not assemble]";
                std::cout << "\n";
            }
        }
        if (!first)
            std::cout << "\n";

    }

    void check_disasm(const Program p)
    {
        std::stringstream s;
        s << p.code;
        std::string obss = cpu::aout.str();
        std::stringstream d;
        d << obss;

        bool b1;
        bool b2;
        unsigned int observed;
        unsigned int expected;
        do
        {
            b1 = assemble(d, observed) && (observed != 0);
            b2 = !(s >> std::hex >> expected).fail() && (expected != 0);
        }
        while (b1 && b2 && (expected == observed));
        pass_msg(!b1 && !b2,'c');
    }

    void check_output(const Program p) {
        std::stringstream s;
        s << p.output;
        std::string obss = cpu::out.str();
        int observed;
        int expected;
        bool b1;
        bool b2;
        do
        {
            b1 = !(s >> observed).fail();
            b2 = !(cpu::out >> expected).fail();
        }
        while (b1 && b2 && (expected == observed));
        pass_msg(!b1 && !b2,'d');
    }

public:
    Verify()
    {
        for (int i=0; i<256; ++i)
        {
            memory[i] = 0xFFFFFFFF;
        }
        programs.push_back(Output);
        programs.push_back(Adder);
        programs.push_back(Resistor);
        programs.push_back(Loop);
        programs.push_back(Ggt);

        // std::cout << "verify initialized \n";
    }



    void Error(const std::string error)
    {
        std::cerr << "cpusim autograder error " << error << "\n";
    }

    ~Verify()
    {
        std::cout
        << "---------------------------------------------------------------- \n"
        << "the following output has been generated by cpusim autograder     \n"
        << "---------------------------------------------------------------- \n";
        trytoassemble();
        for (std::vector<Program>::iterator it = programs.begin(); it < programs.end(); it++)
        {

            if (memory_matches_program(*it)){
                std::cout << "Detected program " << (*it).name << "\n";
                std::cout << "Part (a) passed." << "\n";
                check_decode(*it);
                check_disasm(*it);
                check_output(*it);
            }
        }
        std::cout
        << "---------------------------------------------------------------- \n";



    }
}
verify;
// IMPORTANT: DO NOT MODIFY THIS FILE. ALL PROGRAMS THAT DO NOT WORK
// WITH AN UNMODIFIED VERSION OF THIS FILE DO NOT RECEIVE ANY POINTS