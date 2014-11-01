/*#include <iostream>
#include "cpusim_lib.cc"

int main() {
    std::cout << "Hello World!" << std::endl;
}*/


#include <iostream>
#include "cpusim_lib.cc"

//PRE: -
//POST: Reads the values from input and stores it in consecutive order in memory indexes starting at 0.
//It stops when one input is 0.

void filling_memory()
{
    for (unsigned int index=0 ; index <=255 ; ++index){
        unsigned int value;
        std::cin >> std::hex >> value;
        memory_write(index, value);
        if (value==0)
            break;
    }
}

void decode_instruction(unsigned int startvalue, unsigned int& opcode, unsigned int& op1, unsigned int& op2, unsigned int&op3);

//PRE: Memory should be filled
//POST: outputs the decoded information in the memory
void decode_all_instructions()
{
    for (unsigned int index=0; index <=255 ; ++index){
        unsigned int startvalue;
        startvalue=memory_read(index);
        unsigned int opcode, op1, op2 ,op3;
        decode_instruction(startvalue, opcode, op1, op2, op3);
        cpusim_outdecode(opcode, op1, op2, op3);
        if (startvalue==0)
            break;
    }
}

//PRE: 32-bit instruction in unsigned int and hex format. Locations for references opcode, op1, op2, op3.
//POST: Splits up 32-bit instruction into opcode, op1 , op2, op3 and changes their value accordingly.
void decode_instruction(unsigned int startvalue, unsigned int& opcode, unsigned int& op1, unsigned int& op2, unsigned int&op3)
{
    unsigned int value;
    opcode=startvalue/0x1000000;
    value=startvalue%0x1000000;
    op1=value/0x10000;
    value%=0x10000;
    op2=value/0x100;
    value%=0x100;
    op3=value;
}

//PRE: opcode
//POST: assigns to each (defined) value of opcode a specific output (which is its function)
void disassemble_opcode(unsigned int opcode)
{
    if (opcode==0x01)
        cpu::aout << "add ";
    else if (opcode==0x02)
        cpu::aout << "sub ";
    else if (opcode==0x03)
        cpu::aout << "mul ";
    else if (opcode==0x04)
        cpu::aout << "div ";
    else if (opcode==0x05)
        cpu::aout << "mod ";
    else if (opcode==0x06)
        cpu::aout << "set ";
    else if (opcode==0x10)
        cpu::aout << "jmp ";
    else if (opcode==0x11)
        cpu::aout << "jge ";
    else if (opcode==0x12)
        cpu::aout << "jle ";
    else if (opcode==0x13)
        cpu::aout << "jgr ";
    else if (opcode==0x14)
        cpu::aout << "jls ";
    else if (opcode==0x15)
        cpu::aout << "jeq ";
    else if (opcode==0x16)
        cpu::aout << "jne ";
    else if (opcode==0x21)
        cpu::aout << "out ";
    else if (opcode==0x22)
        cpu::aout << "in ";
    else if (opcode==0x30)
        cpu::aout << "hlt ";
    else
        cpu::aout << "illegal ";

}

//PRE: filled "memory"
//POST: transforms the instructions from hexadecimal numbers to "<name> <operand list> (new line)" and outputs that
void disassemble_all ()
{
    for (unsigned int index=0; index <=255; ++index){
        unsigned int value;
        value=memory_read(index);
        if(value==0)
            break;
        unsigned int opcode, op1, op2, op3;
        decode_instruction(value, opcode, op1, op2, op3);
        disassemble_opcode (opcode);
        if (opcode==0x10 || opcode==0x21 || opcode==0x22)
            cpu::aout << std::dec << op1 << std::endl;
        else if (opcode==0x30);
        else
            cpu::aout << std::dec << op1 << ", " << op2 << ", " << op3 << std::endl;
        }
}

void execute (unsigned int& index, unsigned int opcode, unsigned int op1, unsigned int op2, unsigned int op3);

void fetch_decode_execute ()
{
    for (unsigned int index=0; index <= 255; ++index){
        unsigned int value;
        value=memory_read(index); //fetch
        unsigned int opcode, op1, op2, op3;
        decode_instruction(value, opcode, op1, op2, op3); //decode
        execute(index, opcode, op1, op2, op3);

    }
}

void execute (unsigned int& index, unsigned int opcode, unsigned int op1, unsigned int op2, unsigned int op3)
{
    int res;
    if (opcode==0x01){//add
        res=memory_read(op1)+memory_read(op2);
        memory_write(op3, res);
    }
    else if (opcode==0x02){//subtract
        res=memory_read(op1)-memory_read(op2);
        memory_write(op3, res);
    }
    else if (opcode==0x03){//multiply
        res=memory_read(op1)*memory_read(op2);
        memory_write(op3, res);
    }
    else if (opcode==0x04){//divide
        res=memory_read(op1)/memory_read(op2);
        memory_write(op3, res);
    }
    else if (opcode==0x05){//modulo
        res=memory_read(op1)%memory_read(op2);
        memory_write(op3, res);
    }
    else if (opcode==0x06){//set
        res=op2+op3*256;
        memory_write(op1, res);
    }
    else if (opcode==0x10){//jump to
        index=op1-1;
    }
    else if (opcode==0x11){//jump if greater equal
        if(memory_read(op2)>=memory_read(op3))
            index=op1-1;
    }
    else if (opcode==0x12){//jump if less equal
        if(memory_read(op2)<=memory_read(op3))
            index=op1-1;
    }
    else if (opcode==0x13){//jump if greater
        if(memory_read(op2)>memory_read(op3))
            index=op1-1;
    }
    else if (opcode==0x14){//jump if less
        if(memory_read(op2)<memory_read(op3))
            index=op1-1;
    }
    else if (opcode==0x15){//jump if equal
        if (memory_read(op2)==memory_read(op3))
            index=op1-1;
    }
    else if (opcode==0x16){//jump if not equal
        if(memory_read(op2)!=memory_read(op3))
            index=op1-1;
    }
    else if (opcode==0x21){//output
        res=memory_read(op1);
        cpusim_out(res);
    }
    else if (opcode==0x22){//input
        std::cin >> std::dec >> res;
        memory_write(op1, res);
    }
    else if (opcode==0x30)//end
        return;
    else                  //error (end)
        return;
}

int main()
{
    filling_memory();
    decode_all_instructions();
    disassemble_all();
    fetch_decode_execute();
    return 0;

}
