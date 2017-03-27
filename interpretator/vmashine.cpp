#include "vmashine.h"

VMashine::VMashine()
: pc(regs[6]), sp(regs[5]), esp(regsFloat[5])
{
}

bool VMashine::load(const std::string &filename, int address)
{
    std::ifstream program(filename.c_str(), std::ios::binary);
    if(!program.is_open())
    {
        std::cerr << "Can not open file " << filename << std::endl;
        return false;
    }

    program.seekg(0,std::ios::end);
    size_t length = program.tellg();

    program.seekg(0,std::ios::beg);
    program.read((char*)(memory+address), length);
    program.close();

    pc = address;
    sp = length+1;

    return true;
}

void VMashine::execute()
{
    exit = false;
    while(!exit)
    {
        switch(memory[pc])
        {
        case OP_PASS:
            break;

        case OP_INT:
        {
            short intCode = *((short*)(memory+pc+1));
            interrupt(intCode);
            pc+=2;
        } break;

        case OP_MOV_1:
        {
            short num = *((short*)(memory+pc+1));
            size_t reg = *(memory+pc+3);
            regs[reg] = num;
            pc+=3;
        } break;

        case OP_MOV_2:
        {
            short num = *((short*)(memory+pc+1));
            short addr = *((short*)(memory+pc+3));
            *((short*)(memory+addr)) = num;
            pc+=4;
        } break;

        case OP_MOV_3:
        {
            short num = *((short*)(memory+pc+1));
            size_t reg = *(memory+pc+3);
            short addr = regs[reg];
            *((short*)(memory+addr)) = num;
            pc+=5;
        } break;

        case OP_MOV_7:
        {
            size_t reg1 = *(memory+pc+1);
            size_t reg2 = *(memory+pc+2);
            regs[reg2] = regs[reg1];
            pc+=2;
        } break;

        case OP_MOV_8:
        {
            size_t reg = *(memory+pc+1);
            short addr = *((short*)(memory+pc+2));
            *((short*)(memory+addr)) = regs[reg];
            pc+=3;
        } break;

        case OP_MOV_9:
        {
            size_t reg1 = *(memory+pc+1);
            size_t reg2 = *(memory+pc+2);
            short addr = regs[reg2];
            *((short*)(memory+addr)) = regs[reg1];
            pc+=2;
        } break;

        case OP_MOV_10:
        {
            size_t reg1 = *(memory+pc+1);
            size_t reg2 = *(memory+pc+2);
            regsFloat[reg2] = regsFloat[reg1];
            pc+=2;
        } break;

        case OP_MOV_11:
        {
            size_t reg = *(memory+pc+1);
            short addr = *((short*)(memory+pc+2));
            *((float*)(memory+addr)) = regsFloat[reg];
            pc+=3;
        } break;

        case OP_MOV_12:
        {
            size_t reg1 = *(memory+pc+1);
            size_t reg2 = *(memory+pc+2);
            short addr = regs[reg2];
            *((float*)(memory+addr)) = regsFloat[reg1];
            pc+=2;
        } break;

        case OP_MOV_13:
        {
            short addr = *((short*)(memory+pc+1));
            size_t reg = *(memory+pc+3);
            regs[reg] = *((short*)(memory+addr));
            pc+=3;
        } break;

        case OP_ADD_1:
        {
            short num = *((short*)(memory+pc+1));
            size_t reg = *(memory+pc+3);
            regs[reg] += num;
            pc+=3;
        } break;

        case OP_ADD_2:
        {
            size_t reg1 = *(memory+pc+1);
            size_t reg2 = *(memory+pc+2);
            regs[reg2] += regs[reg1];
            pc+=2;
        } break;

        case OP_SUB_1:
        {
            short num = *((short*)(memory+pc+1));
            size_t reg = *(memory+pc+3);
            regs[reg] -= num;
            pc+=3;
        } break;

        case OP_SUB_2:
        {
            size_t reg1 = *(memory+pc+1);
            size_t reg2 = *(memory+pc+2);
            regs[reg2] -= regs[reg1];
            pc+=2;
        } break;

        case OP_MUL_1:
        {
            short num = *((short*)(memory+pc+1));
            regs[0] *= num;
            pc+=2;
        } break;

        case OP_MUL_2:
        {
            size_t reg = *(memory+pc+1);
            regs[0] *= regs[reg];
            pc+=1;
        } break;

        case OP_DIV_1:
        {
            short num = *((short*)(memory+pc+1));
            regs[4] = regs[0] % num;
            regs[0] /= num;
            pc+=2;
        } break;

        case OP_DIV_2:
        {
            size_t reg = *(memory+pc+1);
            regs[4] = regs[0] % regs[reg];
            regs[0] /= regs[reg];
            pc+=1;
        } break;

        default:
        {
            std::cerr << "\nUnknown command! " << (int)memory[pc] << "\n";
            return;
        }
        }
        
        pc++;
    }

}

void VMashine::interrupt(short code)
{
    switch(code)
    {
    case INT_HALT:
        exit = true;
        break;

    case INT_OUT:
        std::cout << regs[0] << std::endl;
        break;

    case INT_OUT_FLOAT:
        std::cout << regsFloat[0] << std::endl;
        break;

    case INT_IN:
        std::cin >> regs[0];
        break;

    case INT_IN_FLOAT:
        std::cin >> regsFloat[0];
        break;

    default:
        std::cerr << "\nUnknown interrupt!\n";
        return;
    }
}

/*
OP_MOV_1	1 mov цел_число цел_регистр
OP_MOV_2	2 mov цел_число память
OP_MOV_3	3 mov цел_число память_из_регистра

OP_MOV_4	4 mov дроб_число дроб_регистр
OP_MOV_5	5 mov дроб_число память
OP_MOV_6	6 mov дроб_число память_из_регистра

OP_MOV_7	7 mov цел_регистр цел_регистр
OP_MOV_8	8 mov цел_регистр память
OP_MOV_9	9 mov цел_регистр память_из_регистра

OP_MOV_10	10 mov дроб_регистр дроб_регистр
OP_MOV_11	11 mov дроб_регистр память
OP_MOV_12	12 mov дроб_регистр память_из_регистра

OP_MOV_13	13 mov память цел_регистр
OP_MOV_14	14 mov память дроб_регистр
OP_MOV_15	15 mov память_из_регистра цел_регистр
OP_MOV_16	16 mov память_из_регистра дроб_регистр

OP_MOV_17	17 mov цел_регистр дроб_регистр
OP_MOV_18	18 mov дроб_регистр цел_регистр

OP_PASS		19 pass (пустая команда)

OP_INT		20 int (прерывание, аргумент - цел_число)

OP_ADD_1	22 add цел_число цел_регистр
OP_ADD_2	23 add цел_регистр цел_регистр
OP_ADD_3	24 add дроб_число дроб_регистр
OP_ADD_4	25 add дроб_регистр дроб_регистр

OP_SUB_1	26 sub цел_число цел_регистр
OP_SUB_2	27 sub цел_регистр цел_регистр
OP_SUB_3	28 sub дроб_число дроб_регистр
OP_SUB_4	29 sub дроб_регистр дроб_регистр

OP_MUL_1	30 mul цел_число
OP_MUL_2	31 mul цел_регистр
OP_MUL_3	32 mul дроб_число
OP_MUL_4	33 mul дроб_регистр

OP_DIV_1	34 div цел_число
OP_DIV_2	35 div цел_регистр
OP_DIV_3	36 div дроб_число
OP_DIV_4	37 div дроб_регистр

OP_JMP_1	40 jmp число
OP_JMP_2	41 jmp цел_число
OP_JE_1		42 je число
OP_JE_2		43 je цел_число
OP_JG_1		44 jg число
OP_JG_2		45 jg цел_число
OP_JL_1		46 jl число
OP_JL_2		47 jl цел_число
OP_JNE_1	48 jne число
OP_JNE_2	49 jne цел_число

OP_CMP_1	50 cmp цел_число цел_регистр
OP_CMP_2	51 cmp цел_регистр цел_регистр
OP_CMP_3	52 cmp дроб_число дроб_регистр
OP_CMP_4	53 cmp дроб_регистр дроб_регистр

OP_PUSH_1	60 push цел_число
OP_PUSH_2	61 push цел_регистр
OP_PUSH_3	62 push вещ_число
OP_PUSH_4	63 push вещ_регистр

OP_POP_1	65 pop цел_регистр
OP_POP_2	66 pop вещ_регистр

OP_CALL_1	70 call память
OP_CALL_2	71 call память_из_регистра
OP_RET		75 ret
		
OP_AND_1	76 and цел_число цел_регистр
OP_AND_2	77 and цел_регистр цел_регистр
OP_OR_1		78 or  цел_число цел_регистр
OP_OR_2		79 or  цел_регистр цел_регистр
OP_XOR_1	80 xor цел_число цел_регистр
OP_XOR_2	81 xor цел_регистр цел_регистр
OP_NOT		82 not цел_регистр
*/