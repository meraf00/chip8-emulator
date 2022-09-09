#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

// Registers
byte V[16];         // general purpose registers V[0] to V[14] (8-bits)
                    // flag-register V[15] (8-bits)
byte DT, ST;        // delay timer register, sound timer register (8-bits)
byte SP;            // stack pointer register (8-bits) - not accessible for programs running on the emulator
word PC;            // program counter register (16-bits) - not accessible for programs running on the emulator
word I;             // index-register (16-bits)

byte RAM[RAM_SIZE];
word Stack[STACK_SIZE];

byte CHIP_Fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};


// Sets all pixels of Display to BLACK
void clearScreen()
{
    for (int i=0; i < WIDTH*HEIGHT; i++)
    {
        Display[i] = 0;
    } 
}

// pushes address to CHIP8 Stack
// @param address - address to store in stack usually value of PC of caller routine
void push(word address)
{
    if (SP < STACK_SIZE)
    {           
        Stack[SP] = address;
        SP++;
    }
}

// pops and returns address from CHIP8 Stack
// returns -1 if Stack is empty
word pop()
{   
    word address;
    if (SP > 0)
    {
        address = Stack[SP - 1];
        SP--;
    }

    return address;
}

// Fetches 2-byte instruction from RAM at memory address pointed by PC and PC + 1 (PC = program counter)
// Increments PC by 2
// used internally
word fetchInstruction()
{
    word instruction;
    
    instruction = RAM[PC] << 8;
    instruction = instruction | RAM[PC + 1];
    
    PC += 2;

    return instruction;
}

// Executes instruction
// @param instruction - 2 byte long instruction to parse and execute
void executeInstruction(word instruction)
{   
    
    if (instruction == CLS)
    {                   
        clearScreen();
    }
    else if (instruction == RET)
    {   
        word address;
        address = pop();
        PC = (address != -1) ? address : PC;
    }    
    else if (instruction == NOP)
    {

    }
    else 
    {
        // the following code uses flags to extract different nibbles from instructions
        // example. 0xF000 - flag to extract most significant nibble
        
        word data;
        byte x, y;        

        switch (instruction & 0xF000)
        {
            // Get address and set PC to address
            // on the next fetchInstruction() call program continues from address(PC)
            case JP:
                PC = instruction & 0x0FFF;
                break;
            
            // Store PC to Stack and Jump to called routine address
            case CALL:                
                push(PC);            
                PC = instruction & 0x0FFF;
                break;
            
            case SE:      
                x = (instruction & 0x0F00) >> 8;                    
                data = instruction & 0x00FF;
                if (V[x] == data)
                    PC += 2;                
                break;         
            
            case SNE:
                x = (instruction & 0x0F00) >> 8;                        
                data = instruction & 0x00FF;
                if (V[x] != data)
                    PC += 2;                
                break;
            
            case SER:  
                x = (instruction & 0x0F00) >> 8;
                y = (instruction & 0x00F0) >> 4;                
                if (V[x] == V[y])
                    PC += 2;                
                break;

            case LD:
                x = (instruction & 0x0F00) >> 8;
                data = instruction & 0xFF;
                V[x] = data;
                break;

            case ADD:                
                x = (instruction & 0x0F00) >> 8;
                data = instruction & 0x00FF;
                V[x] += data;
                break;
            
            case LDR:
                x = (instruction & 0x0F00) >> 8;
                y = (instruction & 0x00F0) >> 4;

                switch (instruction & 0xF00F)
                {
                    case LDR:                        
                        V[x] = V[y];
                        break;

                    case OR:
                        V[x] |= V[y];
                        break;

                    case AND:
                        V[x] &= V[y];
                        break;

                    case XOR:
                        V[x] ^= V[y];
                        break;

                    case ADDR:
                    {   
                        int sum = (int) V[x] + (int) V[y];

                        if (sum > 0xFF)
                        {
                            V[15] = 1;
                        }
                        else
                        {
                            V[15] = 0;
                        }

                        V[x] += V[y];
                        break;
                    }

                    case SUB:
                        V[15] = V[x] > V[y] ? 1 : 0;
                        V[x] = V[x] - V[y];                        
                        break;

                    case SHR:
                        V[15] = V[x] & 0x1;
                        V[x] = V[x] >> 1;
                        break;
                    
                    case SUBN:
                        V[15] = V[y] > V[x] ? 1 : 0;
                        V[x] = V[y] - V[x];                        
                        break;
                    
                    case SHL:
                        V[15] = V[x] >> 7;
                        V[x] = V[x] << 1;
                        break;
                    
                    default:
                        printf("Unknown instruction: %x\n", instruction);
                        break;
                }
            
            case SNER:
                x = (instruction & 0x0F00) >> 8;
                y = (instruction & 0x00F0) >> 4;                
                if (V[x] != V[y])
                    PC += 2;                                
                break;
            
            case LDI:                 
                data = instruction & 0x0FFF;
                I = data;
                break; 
            
            case RND:
                x = (instruction & 0x0F00) >> 8;
                data = instruction & 0x00FF;

                V[x] = ((byte) rand()) & data;
                break;


            // Draw sprite starting at coord (V[x], V[y])
            case DRW:                
                x = (instruction & 0x0F00) >> 8;
                y = (instruction & 0x00F0) >> 4;
                
                byte n = (instruction & 0x000F);

                byte row;

                V[15] = 0;  // set collision flag to 0
                
                drawFlag = 1;

                for (int iy = 0; iy < n; iy++)
                {   
                    // row contains 8 bits each bits represent a pixel
                    row = RAM[I + iy];          

                    for (int ix = 0; ix < 8; ix++)
                    {                            
                        // loop through row bits and copy to display 
                        // row & (0x80 >> i) expands to row & 0b1000 0000 , row & 0b0100 0000, row & 0b0010 0000, row & 0b0001 0000, row & 0b0000 1000, row & 0b0000 0100, row & 0b0000 0010, row & 0b0000 0001
                        if (row & (0x80 >> ix))
                        {                               
                            // collision detected                            
                            if ( Display[ (V[x] + ix + (V[y] + iy) * WIDTH) ])
                            {                                                                                       
                                V[15] = 1;                                
                            }
                            
                            // no collision
                            Display[ (V[x] + ix + (V[y] + iy) * WIDTH) ] ^= 1;
                            
                                                        
                        }    
                    }
                }

                break; 

            case ECMDS:
                x = (instruction & 0x0F00) >> 8;

                switch (instruction & 0xF0FF)
                {   
                    // no waiting just check if key in V[x] is currently pressed and IF SO skip next instruction
                    case SKP:                        
                        if (Keyboard[ V[x] ])
                        {
                            PC += 2;
                        }
                        break;
                    
                    // no waiting just check if key in V[x] is currently pressed and if it is NOT skip next instruction
                    case SKPN:
                        if ( ! Keyboard[ V[x] ])
                        {
                            PC += 2;
                        }
                        break;
                    
                    default:
                        printf("Unknown instruction: %x\n", instruction);
                        break;
                }
                break;

            case FCMDS:

                x = (instruction & 0x0F00) >> 8;

                switch (instruction & 0xF0FF)
                {   
                    case LDDT:
                        V[x] = DT;
                        break;

                    // Wait for key press
                    case LDK:
                    {
                        byte keypressed = 0;

                        for (int i=0; i<16; i++)
                        {   
                            if (Keyboard[i] != 0)
                            {
                                V[x] = i;
                                keypressed = 1;
                            }
                        }

                        // Keep PC unchanged untill key is pressed
                        if (!keypressed)
                        {
                            //PC -= 2;                            
                        }
                        break;
                    }

                    case SETDT:
                        DT = V[x];
                        break;

                    case SETST:
                        ST = V[x];
                        break;
                    
                    case ADDI:
                        I += V[x];
                        break;

                    case LDCH:
                        // V[x] - is char in range 0 to 15                        
                        I = V[x] * 5;
                        break;

                    case BCD:                        
                        RAM[I] = V[x] / 100;
                        RAM[I + 1] = (V[x] % 100) / 10;
                        RAM[I + 2] = V[x] % 10;
                        break;

                    case PUSHR:                                                
                        for (int i = 0; i <= x; i++)
                        {
                            RAM[I + i] = V[i];
                        }
                        break;

                    case POPR:
                        for (int i = 0; i <= x; i++)
                        {
                            V[i] = RAM[I + i];
                        }
                        break;

                    default:
                        printf("Unknown instruction: %x\n", instruction);
                        break;

                }
                break;
                

            default:
                printf("Unknown instruction: %x\n", instruction);
                break;

        }
    }
}

// Should be called when chip8 is booted 
// Resets memory, display, registers and stack to 0 
// Sets PC to LOAD_ADDRESS
void CHIP_Initalize()
{   
    // reset memory
    for (int i=0; i < RAM_SIZE; i++)
    {
        RAM[i] = 0;
    }

    // reset stack
    for (int i=0; i < STACK_SIZE; i++)
    {
        Stack[i] = 0;
    }

    // reset registers
    for (int i=0; i < 16; i++)
    {
        V[i] = 0;
    }

    DT = ST = SP = I = 0;
    PC = LOAD_ADDRESS;

    clearScreen();

    // load fontset to memory
    for (int i=0; i < 80; i++)
    {
        RAM[i] = CHIP_Fontset[i];
    }
}

// Loads program to CHIP8 RAM from file
int CHIP_LoadProgram(char *fname)
{
    FILE *fp;
    int size;

    fp = fopen(fname, "rb");

    if (fp == NULL)
    {                   
        return -1;
    }

    fseek(fp, 0L, SEEK_END);
    size = (ftell(fp) < RAM_SIZE - LOAD_ADDRESS) ? ftell(fp) : RAM_SIZE - LOAD_ADDRESS;
    fseek(fp, 0L, SEEK_SET);

    for (int i = 0; i < size; i++)
    {           
        RAM[PC + i] = getc(fp);
    }

    fclose(fp);

    return size;

}

void CHIP_EmulateCycle()
{
    executeInstruction(fetchInstruction()); 
    if (DT > 0)
        DT--;
    
    if (ST > 0)
    {
        ST--;
        soundFlag = 1;
    }        
    
    //print_chip_content();    
}

// used for debugging purpose
// prints content of RAM(if content is not 0), registers and stack, display
void print_chip_content()
{
    for(int i=0; i<RAM_SIZE; i++)
    {   
        if (RAM[i] != 0)
            printf("%d %d\n", i, RAM[i]);
    }

    printf("\n\nStack\n");
    for(int i=0; i<STACK_SIZE; i++)
    {
        printf("%d ", Stack[i]);
    }

    printf("\n\nDisplay\n");
    for(int i=0; i<WIDTH*HEIGHT; i++)
    {
        printf("%d ", Display[i]);
    }

    printf("\n\n");
    for (int i=0; i<16; i++)
    {
        printf("V%X %x\n", i, V[i]);
    }
    printf("\nDT %d\nST %d\n\nPC %d\nSP %d\n\nI %d\n", DT, ST, PC, SP, I);


}