#ifndef CHIP8_H
#define CHIP8_H

typedef unsigned char   byte;
typedef unsigned short  word;

// Opcodes - 16 bit long
#define NOP     0x0000     // SYS      addr            0nnn            - No operation
#define CLS     0x00E0     // CLS                      00E0            - clear the display
#define RET     0x00EE     // RET                      00EE            - return from subroutine
#define JP      0x1000     // JP       addr            1nnn            - same as SYS
#define CALL    0x2000     // CALL     addr            2nnn            - call subroutine at address
#define SE      0x3000     // SE       Vx, byte        3xkk            - skip next instruction if Vx = byte
#define SNE     0x4000     // SNE      Vx, byte        4xkk            - skip next instruction if Vx != byte
#define SER     0x5000     // SER      Vx, Vy          5xy0            - skip next instruction if Vx = Vy
#define LD      0x6000     // LD       Vx, byte        6xkk            - write kk to register Vx
#define ADD     0x7000     // ADD      Vx, byte        7xkk            - Vx += byte
#define LDR     0x8000     // LD       Vx, Vy          8xy0            - Vx = Vy
#define OR      0x8001     // OR       Vx, Vy          8xy1            - Vx = Vx | Vy
#define AND     0x8002     // AND      Vx, Vy          8xy2            - Vx = Vx & Vy
#define XOR     0x8003     // XOR      Vx, Vy          8xy3            - Vx = Vx ^ Vy
#define ADDR    0x8004     // ADD      Vx, Vy          8xy4            - Vx += Vy, VF = Carry
#define SUB     0x8005     // SUB      Vx, Vy          8xy5            - Vx -= Vy, VF = not borrow (1 if Vx > Vy)
#define SHR     0x8006     // SHR      Vx              8x06            - Vx = Vx >> 1, VF = least significant field
#define SUBN    0x8007     // SUBN     Vx, Vy          8xy7            - Vx = Vy - Vx, VF = not borrow (1 if Vy > Vx)
#define SHL     0x800E     // SHL      Vx              8xyE            - Vx = Vx << 1, VF = most significant field
#define SNER    0x9000     // SNE      Vx, Vy          9xy0            - skip next instruction if Vx != Vy
#define LDI     0xA000     // LDI      I, addr         Annn            - I = nnn
#define RND     0xC000     // RND      Vx, byte        Cxkk            - Vx = (random byte) & kk
#define DRW     0xD000     // DRW      Vx, Vy, n       Dxyn            - display sprite and set VF = collision
#define ECMDS   0xE000     //                                          - commands that start with 0xE---
#define SKP     0xE09E     // SKP      Vx              Ex9E            - skip next instruction if key with value of Vx is pressed
#define SKPN    0xE0A1     // SKNP     Vx              ExA1            - skip next instruction if key with value of Vx is not pressed
#define FCMDS   0xF000     //                                          - commands that start with 0xF---
#define LDDT    0xF007     // LD       Vx, DT          Fx07            - Vx = DT
#define LDK     0xF00A     // LD       Vx, Keyboard    Fx0A            - wait for keypress and store it in Vx
#define SETDT   0xF015     // LD       DT, Vx          Fx15            - DT = Vx
#define SETST   0xF018     // LD       ST, Vx          Fx18            - ST = Vx
#define ADDI    0xF01E     // ADD      I, Vx           Fx1E            - I += Vx
#define LDCH    0xF029     // LD       F, Vx           Fx29            - I = location of sprite(from character fontset) for value of Vx
#define BCD     0xF033     // LD       B, Vx           Fx33            - store Binary-coded decimal of Vx in memory location I, I+1, I+2
#define PUSHR   0xF055     // LD       [I], Vx         Fx55            - copy (V0, V1 ... to Vx) into memory starting at address I
#define POPR    0xF065     // LD       Vx, [I]         Fx65            - copy value stored at memory location starting at address I into (V0, V1 ... to Vx)

// Programs are loaded at this memory address 
#define LOAD_ADDRESS    0x200

// Memory
#define RAM_SIZE        4096    // byte
#define STACK_SIZE      32      // word

// Display
// Each pixle will be represented by 1 byte (0 Black, 1 White)
#define WIDTH           64
#define HEIGHT          32

byte drawFlag;
byte Display[WIDTH * HEIGHT];

byte soundFlag;

// Keyboard
byte Keyboard[16];

void CHIP_Initalize();

int CHIP_LoadProgram(char *fname);

void CHIP_EmulateCycle();

void print_chip_content();

#endif