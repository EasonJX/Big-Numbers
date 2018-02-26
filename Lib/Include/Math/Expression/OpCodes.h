#pragma once

#include <Math/Int128.h>

// General purpose registers
// 8 bit registers
#define AL 0
#define CL 1
#define DL 2
#define BL 3
#define AH 4
#define CH 5
#define DH 6
#define BH 7

// 16 bit registers
#define AX 0
#define CX 1
#define DX 2
#define BX 3
#define SP 4
#define BP 5
#define SI 6
#define DI 7

// 32 bit registers
#define EAX 0
#define ECX 1
#define EDX 2
#define EBX 3
#define ESP 4
#define EBP 5
#define ESI 6
#define EDI 7

#ifdef IS64BIT
// 64 bit registers
#define RAX  0
#define RCX  1
#define RDX  2
#define RBX  3
#define RSP  4
#define RBP  5
#define RSI  6
#define RDI  7
#define R8   8
#define R9   9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define R15 15

#define XMM0   0
#define XMM1   1
#define XMM2   2
#define XMM3   3
#define XMM4   4
#define XMM5   5
#define XMM6   6
#define XMM7   7
#define XMM8   8
#define XMM9   9
#define XMM10 10
#define XMM11 11
#define XMM12 12
#define XMM13 13
#define XMM14 14
#define XMM15 15
#endif // IS64BIT

// 16 bit segment registers
#define ES 0
#define CS 1
#define SS 2
#define DS 3
#define FS 4
#define GS 5

#define _SWAP2(op)     ((((op)&0xff)<< 8) | (((op)>> 8)&0xff))
#define _SWAP3(op)     ((_SWAP2(op) << 8) | (((op)>>16)&0xff))
#define _SWAP4(op)     ((_SWAP2(op) <<16) | (_SWAP2((op)>>16)))
#define _SWAP5(op)     ((_SWAP4(op) << 8) | (((op)>>32)&0xff))
#define _SWAP6(op)     ((_SWAP4(op) <<16) | (_SWAP2((op)>>32)))

#define I64(n)        ((UINT64)(n))

#ifdef IS32BIT
#define MAX_REFERENCE_REGISTER 7
#else
#define MAX_REFERENCE_REGISTER 15
#endif
#define MAX_INSTRUCTIONSIZE 15

class IntelOpcode {
private:
  static inline UINT64 swapBytes(UINT64 bytes, int sz) {
    switch(sz) {
    case 1 : return bytes;
    case 2 : return _SWAP2(bytes);
    case 3 : return _SWAP3(bytes);
    case 4 : return _SWAP4(bytes);
    case 5 : return _SWAP5(bytes);
    case 6 : return _SWAP6(bytes);
    default: throwInvalidArgumentException(__TFUNCTION__, _T("sz=%d"), sz);
             return bytes;
    }
  }

protected:
  union {
    _uint128 m_bytes;
    BYTE     m_byte[16];
  };
  UINT             m_size         : 4; // = [0..15].
  UINT             m_memAddrMode  : 1;
  UINT             m_regSrcMode   : 1;
#ifdef IS64BIT
  UINT             m_hasRexMode   : 1;
  UINT             m_hasRexByte   : 1;
  UINT             m_rexByteIndex : 2;
#endif
public:
  inline IntelOpcode(BYTE size, UINT64 bytes, bool memAddrMode, bool regSrcMode)
    : m_bytes(swapBytes(bytes,size))
    , m_size(size)
    , m_memAddrMode(memAddrMode)
    , m_regSrcMode(regSrcMode)
#ifdef IS64BIT
    , m_hasRexMode(1)
    , m_hasRexByte(0)
    , m_rexByteIndex(0)
#endif
  {
  }
  inline IntelOpcode &prefix(BYTE value) {
    assert(m_size < MAX_INSTRUCTIONSIZE);
    m_bytes <<= 8;
    m_byte[0] = value;
    m_size++;
    return *this;
  }
  inline IntelOpcode &insertByte(BYTE index, BYTE value) {
    if(index==0) return prefix(value);
    assert((m_size < MAX_INSTRUCTIONSIZE) && (index<m_size));
    const _uint128 mask = (1ui64<<(index<<3))-1;
    m_bytes = ((m_bytes << 8) & ~mask) | (m_bytes&mask);
    m_byte[index] = value;
    m_size++;
    return *this;
  }

#ifdef IS32BIT
#define SETREXBITONHIGHREG(r,bit)
#define SETREXBITSONHIGHREG2(reg,addReg)
#else // IS64BIT
  inline IntelOpcode &setRexBits(BYTE bits) {
    assert(((bits&0xf0)==0) && hasRexMode());
    if(m_hasRexByte) {
      m_byte[m_rexByteIndex] |= bits; // just add new bits to rex-byte
      return *this;
    }
    m_hasRexByte = true;
    return insertByte(m_rexByteIndex, 0x40|bits);
  }
#define SETREXBITONHIGHREG(r,bit) { if((r) >= R8) setRexBits(bit); }
#define SETREXBITSONHIGHREG2(reg,addReg) {                  \
  const BYTE _rexBits = (((reg)>>3)&1) | (((addReg)>>2)&2); \
  if(_rexBits) setRexBits(_rexBits);                        \
}
#endif // IS64BIT

  inline IntelOpcode &wordOp() {
#ifdef IS64BIT
    m_rexByteIndex++;
#endif
    return prefix(0x66);
  }
  // in bytes
  inline UINT size() const {
    return m_size;
  }
  const BYTE *getBytes() const {
    return m_byte;
  }
  // are MEM_ADDR_* macroes allowed for this opcode
  inline bool hasMemAddrMode() const {
    return m_memAddrMode ? true : false;
  }
  // is REG_SRC-macro allowed for this opcode
  inline bool hasRegSrcMode() const {
    return m_regSrcMode ? true : false;
  }
  inline bool hasRexMode() const {
#ifdef IS32BIT
    return false;
#else
    return m_hasRexMode ? true : false;
#endif
  }
  inline bool hasRexByte() const {
#ifdef IS32BIT
    return false;
#else
    return m_hasRexByte ? true : false;
#endif
  }
};

class IntelInstruction : public IntelOpcode {
private:
  // Always |= the last byte in instruction
  inline IntelInstruction &or(BYTE b) {
    m_byte[m_size - 1] |= b;
    return *this;
  }
  // Append count bytes to sequence of m_bytes
  inline IntelInstruction &add(INT64 bytesToAdd, BYTE count) {
    assert(m_size+count < MAX_INSTRUCTIONSIZE);
    if(count == 1) {
      m_byte[m_size++] = (BYTE)bytesToAdd;
    } else {
      for(BYTE *dst = m_byte+m_size, *src = (BYTE*)&bytesToAdd, *end = src+count; src<end;) {
        *(dst++) = *(src++);
      }
      m_size += count;
    }
    return *this;
  }
public:
  inline IntelInstruction(BYTE size, UINT64 bytes) : IntelOpcode(size, bytes, false, false) {
  }
  inline IntelInstruction(const IntelOpcode &op) : IntelOpcode(op) {
  }

  // ptr[reg]. (reg&7) != ESP,EBP
  inline IntelInstruction &memAddrPtr(BYTE reg) {
    assert(m_memAddrMode);
    assert((reg <= MAX_REFERENCE_REGISTER) && ((reg&7)!=ESP) && ((reg&7)!=EBP));
    SETREXBITONHIGHREG(reg,1);
    return or(reg&7);
  }

  // ptr[reg+offset], (reg&7) != ESP, offset=[-128;127]
  inline IntelInstruction &memAddrPtr1(BYTE reg, BYTE offset) {
    assert(m_memAddrMode);
    assert((reg <= MAX_REFERENCE_REGISTER) && ((reg&7)!=ESP));
    SETREXBITONHIGHREG(reg,1);
    return or(0x40 | (reg&7)).add(offset, 1);
  }

  // ptr[reg+offset], (reg&7) != ESP, offset=[INT_MIN;INT_MAX]
  inline IntelInstruction &memAddrPtr4(BYTE reg, int offset) {
    assert(m_memAddrMode);
    assert((reg <= MAX_REFERENCE_REGISTER) && ((reg&7)!=ESP));
    SETREXBITONHIGHREG(reg,1);
    return or(0x80 | (reg&7)).add(offset, 4);
  }

   // ptr[(reg<<p2)+offset], (reg&7) != ESP, p2<=3, offset=[INT_MIN;INT_MAX]
  inline IntelInstruction &memAddrMp2Ptr4(BYTE reg, BYTE p2, int offset) {
    assert(m_memAddrMode);
    assert((reg <= MAX_REFERENCE_REGISTER) && ((reg&7)!=ESP) && (p2<=3));
    SETREXBITONHIGHREG(reg,2);
    return or(0x04).add(0x05 | (p2 << 6) | ((reg&7) << 3), 1).add(offset, 4);
  }

  // ptr[reg+(addReg<<p2)], (reg&7) != EBP, (addReg&7) != ESP, p2<=3
  inline IntelInstruction &memAddrMp2Reg(BYTE reg, BYTE p2, BYTE addReg) {
    assert(m_memAddrMode);
    assert((reg <= MAX_REFERENCE_REGISTER) && (addReg <= MAX_REFERENCE_REGISTER) && ((reg&7)!=EBP) && ((addReg&7)!=ESP) && (p2<=3));
    SETREXBITSONHIGHREG2(reg,addReg);
    return or(0x04).add((p2 << 6) | ((addReg&7) << 3) | (reg&7), 1);
  }

  // ptr[reg+(addReg<<p2)+offset], (addReg&7) != ESP, p2<=3, offset=[-128;127]
  inline IntelInstruction &memAddrMp2Reg1(BYTE reg, BYTE p2, BYTE addReg, BYTE offset) {
    assert(m_memAddrMode);
    assert((reg <= MAX_REFERENCE_REGISTER) && (addReg <= MAX_REFERENCE_REGISTER) && ((addReg&7)!=ESP) && (p2<=3));
    SETREXBITSONHIGHREG2(reg,addReg);
    return or(0x44).add((p2 << 6) | ((addReg&7) << 3) | (reg&7), 1).add(offset, 1);
  }

  // ptr[reg+(addReg<<p2)+offset], (addReg&7) != ESP, p2<=3, offset=[INT_MIN;INT_MAX]
  inline IntelInstruction &memAddrMp2Reg4(BYTE reg, BYTE p2, BYTE addReg, int offset) {
    assert(m_memAddrMode);
    assert((reg <= MAX_REFERENCE_REGISTER) && (addReg <= MAX_REFERENCE_REGISTER) && ((addReg&7)!=ESP) && (p2<=3));
    SETREXBITSONHIGHREG2(reg,addReg);
    return or(0x84).add((p2 << 6) | ((addReg&7) << 3) | (reg&7), 1).add(offset, 4);
  }

  inline IntelInstruction &memAddrEsp() {
    assert(m_memAddrMode);
    return or(0x04).add(0x24, 1);
  }

  inline IntelInstruction &memAddrEsp1(BYTE offset) {
    assert(m_memAddrMode);
    return or(0x44).add(0x24, 1).add(offset, 1);
  }

  inline IntelInstruction &memAddrEsp4(int offset) {
    assert(m_memAddrMode);
    return or(0x84).add(0x24, 1).add(offset, 4);
  }

  inline IntelInstruction &memImmDword() { // add 4 bytes address
    assert(m_memAddrMode);
    return or(0x05);
  }

  inline IntelInstruction &regSrc(BYTE reg) {
    assert(m_regSrcMode);
    assert(reg <= MAX_REFERENCE_REGISTER);
    SETREXBITONHIGHREG(reg,1);
    return or(0xc0 | (reg&7));
  }
};

#define WORDOP(  op32) ((op32).wordOp())

// reg!=ESP,EBP.                                                          Ex:fild word ptr[reg]
#define MEM_ADDR_PTR(       op,reg                 ) IntelInstruction(op).memAddrPtr(    reg                 )
// reg!=ESP             offset=1 byte signed  [-128;127].                 Ex:fild word ptr[reg + offset]
#define MEM_ADDR_PTR1(      op,reg          ,offset) IntelInstruction(op).memAddrPtr1(   reg,          offset)
// reg!=ESP             offset=4 bytes signed [INT_MIN;INT_MAX].          Ex:fild word ptr[reg + offset]
#define MEM_ADDR_PTR4(      op,reg          ,offset) IntelInstruction(op).memAddrPtr4(   reg,          offset)
// reg!=ESP             offset=4 bytes signed [INT_MIN;INT_MAX] p2=[0;3]. Ex:fild word ptr[(reg<<p2) + offset]
#define MEM_ADDR_MP2PTR4(   op,reg       ,p2,offset) IntelInstruction(op).memAddrMp2Ptr4(reg,p2,       offset)
// reg!=EBP addReg!=ESP                                         p2=[0;3]. Ex:fild word ptr[reg + (addReg<<p2)]
#define MEM_ADDR_PTRMP2REG( op,reg,addReg,p2       ) IntelInstruction(op).memAddrMp2Reg( reg,p2,addReg       )
// addReg!=ESP          offset=1 byte signed  [-128;127]        p2=[0;3]. Ex:fild word ptr[reg + (addReg<<p2) + offset]
#define MEM_ADDR_PTRMP2REG1(op,reg,addReg,p2,offset) IntelInstruction(op).memAddrMp2Reg1(reg,p2,addReg,offset)
// addReg!=ESP          offset=4 bytes signed [INT_MIN;INT_MAX] p2=[0;3]. Ex:fild word ptr[reg + (addReg<<p2) + offset]
#define MEM_ADDR_PTRMP2REG4(op,reg,addReg,p2,offset) IntelInstruction(op).memAddrMp2Reg4(reg,p2,addReg,offset)
//                                                                        Ex:fild word ptr[esp]
#define MEM_ADDR_ESP(       op                     ) IntelInstruction(op).memAddrEsp()
//                      offset=1 byte signed  [-128;127].                 Ex:fild word ptr[esp + offset]
#define MEM_ADDR_ESP1(      op              ,offset) IntelInstruction(op).memAddrEsp1(                 offset)
//                      offset=4 bytes signed [INT_MIN;INT_MAX].          Ex:fild word ptr[esp + offset]
#define MEM_ADDR_ESP4(      op              ,offset) IntelInstruction(op).memAddrEsp4(                 offset)
//  + 4 byte address. In x64 mode PC-relative offset
#define MEM_ADDR_DS(        op                     ) IntelInstruction(op).memImmDword()
//                                                                        Ex:add dst, reg
#define REG_SRC(            op,reg                 ) IntelInstruction(op).regSrc(        reg                )

#define MEM_ADDR_PTRREG(    op,reg,addReg         ) MEM_ADDR_PTRMP2REG( op,reg,addReg,0      )               // reg!=EBP addReg!=ESP                              ex:fild word ptr[esp + ecx]
#define MEM_ADDR_PTRREG1(   op,reg,addReg   ,offs1) MEM_ADDR_PTRMP2REG1(op,reg,addReg,0,offs1)               //          addReg!=ESP         offs1=1 byte  signed ex:fild word ptr[ebp + ecx + 127]
#define MEM_ADDR_PTRREG4(   op,reg,addReg   ,offs4) MEM_ADDR_PTRMP2REG4(op,reg,addReg,0,offs4)               //          addReg!=ESP         offs1=4 bytes signed ex:fild word ptr[esp + eax + 0x12345678]

// Instructions defined with these macroes, cannot be combined with the various addressing-nmodes
#define B1INS(op)      IntelInstruction(1, op)
#define B2INS(op)      IntelInstruction(2, op)
#define B3INS(op)      IntelInstruction(3, op)
#define B4INS(op)      IntelInstruction(4, op)

// Instruction defined with these macroes, must be combined with macroes MEM_ADDR_* (and evt. REG_SRC)
#define B1OP(op)       IntelOpcode(1, op, true , true)
#define B2OP(op)       IntelOpcode(2, op, true , true)
#define B3OP(op)       IntelOpcode(3, op, true , true)
#define B4OP(op)       IntelOpcode(4, op, true , true)
#define B2OPNOREG(op)  IntelOpcode(2, op, true ,false)
#define B4OPNOREG(op)  IntelOpcode(4, op, true ,false)

#define FPUINS(op)       B2INS(    op)
#define FPUINSA(op)      B2OPNOREG(op)

#ifdef IS64BIT
// 0 arguments instructions (push,pop...)
// For r8-15, add rexbyte, and set bit 0
#define REX0(op32,r64) (((r64)<R8)?op32(r64):op32((r64)&7).setRexBits(1))
// Use for IMM operations, and other instructions where only 1 general purpose register is used.
// Set rex bit 3 (and bit 0 for r8-15)
#define REX1(op32,r64) (op32((r64)&7).setRexBits(8|(((r64)>>3)&1)))
// Use for all instructions, where 2 general purpose registers are involved.
// Set rex bit 3 (and bit 2 for r8-15)
#define REX2(op32,r64) (op32((r64)&7).setRexBits(8|(((r64)>>1)&4)))
// Set rex bit 3
#define REX3(op32)     (op32).setRexBits(8)
#endif // IS64BIT

// Use less/greater opcode for signed comparison. below/above for unsigned.

// 1 byte PC relative offset
#define JOSHORT                                B1INS(0x70)                              // Jump short if overflow                                 (OF==1 )
#define JNOSHORT                               B1INS(0x71)                              // Jump short if not overflow                             (OF==0 )
#define JBSHORT                                B1INS(0x72)                              // Jump short if below                (unsigned)          (CF==1 )
#define JAESHORT                               B1INS(0x73)                              // Jump short if above or equal       (unsigned)          (CF==0 )
#define JESHORT                                B1INS(0x74)                              // Jump short if equal                (signed/unsigned)   (ZF==1 )
#define JNESHORT                               B1INS(0x75)                              // Jump short if not equal            (signed/unsigned)   (ZF==0 )
#define JBESHORT                               B1INS(0x76)                              // Jump short if below or equal       (unsigned)          (CF==1 || ZF==1)
#define JASHORT                                B1INS(0x77)                              // Jump short if above                (unsigned)          (CF==0 && ZF==0)
#define JSSHORT                                B1INS(0x78)                              // Jump short if sign                                     (SF==1 )
#define JNSSHORT                               B1INS(0x79)                              // Jump short if not sign                                 (SF==0 )
#define JPESHORT                               B1INS(0x7A)                              // Jump short if parity even                              (PF==1 )
#define JPOSHORT                               B1INS(0x7B)                              // Jump short if parity odd                               (PF==0 )
#define JLSHORT                                B1INS(0x7C)                              // Jump short if less                 (signed  )          (SF!=OF)
#define JGESHORT                               B1INS(0x7D)                              // Jump short if greater or equal     (signed  )          (SF==OF)
#define JLESHORT                               B1INS(0x7E)                              // Jump short if less or equal        (signed  )          (ZF==1 || SF!=OF)
#define JGSHORT                                B1INS(0x7F)                              // Jump short if greater              (signed  )          (ZF==0 && SF==OF)

#define JNAESHORT                              JBSHORT                                  // Jump short if not above or equal   (unsigned)
#define JCSHORT                                JBSHORT                                  // Jump short if carry                (unsigned)
#define JNCSHORT                               JAESHORT                                 // Jump short if not carry            (unsigned)
#define JNBSHORT                               JAESHORT                                 // Jump short if not below            (unsigned)
#define JZSHORT                                JESHORT                                  // Jump short if zero                 (signed/unsigned)
#define JNZSHORT                               JNESHORT                                 // Jump short if not zero             (signed/unsigned)
#define JNASHORT                               JBESHORT                                 // Jump short if not above            (unsigned)
#define JNBESHORT                              JASHORT                                  // Jump short if not below or equal   (unsigned)
#define JNGESHORT                              JLSHORT                                  // Jump short if not greater or equal (signed  )
#define JNLSHORT                               JGESHORT                                 // Jump short if not less             (signed  )
#define JNGSHORT                               JLESHORT                                 // Jump short if not greater          (signed  )
#define JNLESHORT                              JGSHORT                                  // Jump short if not less or equal    (signed  )

// 4 byte PC relative offset
#define JONEAR                                 B2INS(0x0F80)                            // Jump near  if overflow
#define JNONEAR                                B2INS(0x0F81)                            // Jump near  if not overflow
#define JBNEAR                                 B2INS(0x0F82)                            // Jump near  if below                 (unsigned)
#define JAENEAR                                B2INS(0x0F83)                            // Jump near  if above or equal        (unsigned)
#define JENEAR                                 B2INS(0x0F84)                            // Jump near  if equal                 (signed/unsigned)
#define JNENEAR                                B2INS(0x0F85)                            // Jump near  if not equal             (signed/unsigned)
#define JBENEAR                                B2INS(0x0F86)                            // Jump near  if below or equal        (unsigned)
#define JANEAR                                 B2INS(0x0F87)                            // Jump near  if above                 (unsigned)
#define JSNEAR                                 B2INS(0x0F88)                            // Jump near  if sign
#define JNSNEAR                                B2INS(0x0F89)                            // Jump near  if not sign
#define JPENEAR                                B2INS(0x0F8A)                            // Jump near  if parity even
#define JPONEAR                                B2INS(0x0F8B)                            // Jump near  if parity odd
#define JLNEAR                                 B2INS(0x0F8C)                            // Jump near  if less                  (signed  )
#define JGENEAR                                B2INS(0x0F8D)                            // Jump near  if greater or equal      (signed  )
#define JLENEAR                                B2INS(0x0F8E)                            // Jump near  if less or equal         (signed  )
#define JGNEAR                                 B2INS(0x0F8F)                            // Jump near  if greater               (signed  )

#define JNAENEAR                               JBNEAR                                   // Jump near  if not above or equal    (unsigned)
#define JCNEAR                                 JBNEAR                                   // Jump near  if carry                 (unsigned)
#define JNCNEAR                                JAENEAR                                  // Jump near  if not carry             (unsigned)
#define JNBNEAR                                JAENEAR                                  // Jump near  if not below             (unsigned)
#define JZNEAR                                 JENEAR                                   // Jump near  if 0                     (signed/unsigned)
#define JNZNEAR                                JNENEAR                                  // Jump near  if not zero              (signed/unsigned)
#define JNANEAR                                JBENEAR                                  // Jump near  if not above             (unsigned)
#define JNBENEAR                               JANEAR                                   // Jump near  if not below or equal    (unsigned)
#define JNGENEAR                               JLNEAR                                   // Jump near  if not greater or equal  (signed  )
#define JNLNEAR                                JGENEAR                                  // Jump near  if not less              (signed  )
#define JNGNEAR                                JLENEAR                                  // Jump near  if not greater           (signed  )
#define JNLENEAR                               JGNEAR                                   // Jump near  if not less or equal     (signed  )

// Set Byte on Condition
#define SETO                                   B3OP(0x0F9000)                           // Set byte   if overflow
#define SETNO                                  B3OP(0x0F9100)                           // Set byte   if not overflow
#define SETB                                   B3OP(0x0F9200)                           // Set byte   if below                 (unsigned)
#define SETAE                                  B3OP(0x0F9300)                           // Set byte   if above or equal        (unsigned)
#define SETE                                   B3OP(0x0F9400)                           // Set byte   if equal                 (signed/unsigned)
#define SETNE                                  B3OP(0x0F9500)                           // Set byte   if not equal             (signed/unsigned)
#define SETBE                                  B3OP(0x0F9600)                           // Set byte   if below or equal        (unsigned)
#define SETA                                   B3OP(0x0F9700)                           // Set byte   if above                 (unsigned)
#define SETS                                   B3OP(0x0F9800)                           // Set byte   if sign
#define SETNS                                  B3OP(0x0F9900)                           // Set byte   if not sign
#define SETPE                                  B3OP(0x0F9A00)                           // Set byte   if parity even
#define SETPO                                  B3OP(0x0F9B00)                           // Set byte   if parity odd
#define SETL                                   B3OP(0x0F9C00)                           // Set byte   if less                  (signed  )
#define SETGE                                  B3OP(0x0F9D00)                           // Set byte   if greater or equal      (signed  )
#define SETLE                                  B3OP(0x0F9E00)                           // Set byte   if less or equal         (signed  )
#define SETG                                   B3OP(0x0F9F00)                           // Set byte   if greater               (signed  )

#define SETNAE                                 SETB                                     // Set byte   if not above or equal    (unsigned)
#define SETC                                   SETB                                     // Set byte   if carry                 (unsigned)
#define SETNC                                  SETAE                                    // Set byte   if not carry             (unsigned)
#define SETNB                                  SETAE                                    // Set byte   if not below             (unsigned)
#define SETZ                                   SETE                                     // Set byte   if 0                     (signed/unsigned)
#define SETNZ                                  SETNE                                    // Set byte   if not zero              (signed/unsigned)
#define SETNA                                  SETBE                                    // Set byte   if not above             (unsigned)
#define SETNBE                                 SETA                                     // Set byte   if not below or equal    (unsigned)
#define SETNGE                                 SETL                                     // Set byte   if not greater or equal  (signed  )
#define SETNL                                  SETGE                                    // Set byte   if not less              (signed  )
#define SETNG                                  SETLE                                    // Set byte   if not greater           (signed  )
#define SETNLE                                 SETG                                     // Set byte   if not less or equal     (signed  )

#define JMPSHORT                               B1INS(0xEB)                              // Unconditional short jump. 1 byte PC relative offset
#define JMPNEAR                                B1INS(0xE9)                              // Unconditional near jump. 4 byte PC relative offset
#ifdef IS64BIT
#define JMP_QWORDPTR                           B2INS(0xFF25)                            // Unconditional jump. 4 byte PC relative offset
#endif
#ifdef IS32BIT
#define JCXZSHORT                              B2INS(0x67E3)                            // Jump short if CX  register is 0. 1 byte PC relative offset
#define JECXSHORT                              B1INS(0xE3)                              // Jump short if ECX register is 0  1 byte PC relative offset
#else
#define JEXZSHORT                              B2INS(0x67E3)                            // Jump short if ECX register is 0. 1 byte PC relative offset
#define JRCXSHORT                              B1INS(0xE3)                              // Jump short if RCX register is 0  1 byte PC relative offset
#endif // IS64BIT

#define CALL                                   B1INS(0xE8)                              // Call near, 4 byte PC relative, displacement
#define CALLABSOLUTE                           B2OP(0xFFD0)                             // Call far, absolute address given by operand
#define RET                                    B1INS(0xC3)                              // Near return to calling procedure

#define CMC                                    B1INS(0xF5)                              // Complement carry flag
#define CLC                                    B1INS(0xF8)                              // Clear carry flag     CF = 0	
#define STC                                    B1INS(0xF9)                              // Set   carry flag     CF = 1
#define CLI                                    B1INS(0xFA)                              // Clear interrupt flag IF = 0
#define STI                                    B1INS(0xFB)                              // Set   interrupt flag IF = 1
#define CLD                                    B1INS(0xFC)                              // Clear direction flag DF = 0
#define STD                                    B1INS(0xFD)                              // Set   direction flag DF = 1

#define PUSHF                                  B1INS(0x9C)                              // Push FLAGS onto stack *--SP = FLAGS;	
#define POPF                                   B1INS(0x9D)                              // Pop  FLAGS register from stack FLAGS = *SP++
#define SAHF                                   B1INS(0x9E)                              // Store AH into FLAGS
#define LAHF                                   B1INS(0x9F)                              // Load FLAGS into AH register
#define PUSHFD                                 PUSHF                                    // Push EFLAGS register onto stack
#define POPFD                                  POPF                                     // Pop data into EFLAGS register
#define PUSHAD                                 B1INS(0x60)                              // Push all double-word (32-bit) registers onto stack
#define POPAD                                  B1INS(0x61)                              // Pop  all double-word (32-bit) registers from stack
#define PUSHA                                  WORDOP(PUSHAD)                           // Push all word        (16-bit) registers onto stack
#define POPA                                   WORDOP(POPAD )                           // Pop  all word        (16-bit) registers from stack

#define TEST_AL_IMM_BYTE                       B1INS(0xA8)                              // 1 byte operand
#define TEST_EAX_IMM_DWORD                     B1INS(0xA9)                              // 4 byte operand
#define TEST_IMM_BYTE                          B2OP(0xF600)                             // Build src with MEM_ADDR-*,REG_SRC-macroes. 1 byte operand
#define TEST_IMM_DWORD                         B2OP(0xF700)                             // Build src with MEM_ADDR-*,REG_SRC-macroes. 4 byte operand
#define TEST_R8_BYTE(        r8 )              B2OP(0x8400    | ((r8 )<<3))             // Build op2 with MEM_ADDR-*,REG_SRC-macroes
#define TEST_R32_DWORD(      r32)              B2OP(0x8500    | ((r32)<<3))             // Build op2 with MEM_ADDR-*,REG_SRC-macroes

#define TEST_AX_IMM_WORD                       WORDOP(TEST_EAX_IMM_DWORD)               // 2 byte operand
#define TEST_IMM_WORD                          WORDOP(TEST_IMM_DWORD)                   // Build src with MEM_ADDR-*,REG_SRC-macroes. 2 byte operand
#define TEST_R16_WORD(       r16)              WORDOP(TEST_R32_DWORD(r16))              // Build op2 with MEM_ADDR-*,REG_SRC-macroes


#define NOOP                                   B1INS(0x90)

#define XCHG_EAX_R32(        r32)              B1INS(0x90     |  (r32))                 // r32=eax-edi
#define XCHG_R8_BYTE(        r8 )              B2OP(0x8600    | ((r8 )<<3))             // Build op2 with MEM_ADDR-*,REG_SRC-macroes
#define XCHG_R32_DWORD(      r32)              B2OP(0x8700    | ((r32)<<3))             // Build op2 with MEM_ADDR-*,REG_SRC-macroes

#define XCHG_AX_R16(         r16)              WORDOP(XCHG_EAX_R32(r16))                // r16=ax-di
#define XCHG_R16_WORD(       r16)              WORDOP(XCHG_R32_DWORD(r16))              // Build op2 with MEM_ADDR-*,REG_SRC-macroes

#define MOV_FROM_SEGREG_WORD(seg)              B2OP(0x8C00    | ((seg)<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define LEA_R32_DWORD(       dst)              B2OPNOREG(0x8D00 | ((dst)<<3))           // Build src with MEM_ADDR-macroes
#define MOV_TO_SEGREG_WORD(  seg)              B2OP(0x8E00    | ((seg)<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define POP_DWORD                              B2OP(0x8F00)                             // Build dst with MEM_ADDR-*,REG_SRC-macroes

#define MOV_BYTE_R8(         r8 )              B2OP(0x8800    | ((r8 )<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define MOV_DWORD_R32(       r32)              B2OP(0x8900    | ((r32)<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define MOV_R8_BYTE(         r8 )              B2OP(0x8A00    | ((r8 )<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define MOV_R32_DWORD(       r32)              B2OP(0x8B00    | ((r32)<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes

#define MOV_R8_IMM_BYTE(     r8)               B1INS(0xB0     |  (r8 ))                 // 1 byte operand
#define MOV_R32_IMM_DWORD(   r32)              B1INS(0xB8     |  (r32))                 // 4 byte operand

#define MOV_TO_AL_IMM_ADDR_BYTE                B1INS(0xA0  )                            // 4/8 byte address. move byte  pointed to by 2. operand to AL
#define MOV_TO_EAX_IMM_ADDR_DWORD              B1INS(0xA1  )                            // 4/8 byte address. move dword pointed to by 2. operand to EAX
#define MOV_FROM_AL_IMM_ADDR_BYTE              B1INS(0xA2  )                            // 4/8 byte address. move AL  to byte  pointed to by 2. operand
#define MOV_FROM_EAX_IMM_ADDR_DWORD            B1INS(0xA3  )                            // 4/8 byte address. move EAX to dword pointed to by 2. operand

#define MOV_WORD_R16(        r16)              WORDOP(MOV_DWORD_R32(    r16))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define MOV_R16_WORD(        r16)              WORDOP(MOV_R32_DWORD(    r16))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define MOV_R16_IMM_WORD(    r16)              WORDOP(MOV_R32_IMM_DWORD(r16))           // 2 byte operand
#define MOV_TO_AX_IMM_ADDR_WORD                WORDOP(MOV_TO_EAX_IMM_ADDR_DWORD)        // 4/8 byte address. move word  pointed to by 2. operand to AX
#define MOV_FROM_AX_IMM_ADDR_WORD              WORDOP(MOV_FROM_EAX_IMM_ADDR_DWORD)      // 4/8 byte address. move AX  to word  pointed to by 2. operand

#define ADD_BYTE_R8(         r8 )              B2OP( 0x0000     | ((r8 )<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define ADD_DWORD_R32(       r32)              B2OP( 0x0100     | ((r32)<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define ADD_R8_BYTE(         r8 )              B2OP( 0x0200     | ((r8 )<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define ADD_R32_DWORD(       r32)              B2OP( 0x0300     | ((r32)<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define ADD_AL_IMM_BYTE                        B1INS(0x04)                                // 1 byte operand
#define ADD_EAX_IMM_DWORD                      B1INS(0x05)                                // 4 byte operand
#define ADD_R8_IMM_BYTE(     r8 )              B2INS(0x80C0     | (r8 ))                  // 1 byte operand
#define ADD_R32_IMM_DWORD(   r32)              B2INS(0x81C0     | (r32))                  // 4 byte operand
#define ADD_R32_IMM_BYTE(    r32)              B2INS(0x83C0     | (r32))                  // 1 byte operand

#define ADD_WORD_R16(        r16)              WORDOP(ADD_DWORD_R32(    r16))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define ADD_R16_WORD(        r16)              WORDOP(ADD_R32_DWORD(    r16))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define ADD_AX_IMM_WORD                        WORDOP(ADD_EAX_IMM_DWORD)                  // 2 byte operand
#define ADD_R16_IMM_WORD(    r16)              WORDOP(ADD_R32_IMM_DWORD(r16))             // 2 byte operand
#define ADD_R16_IMM_BYTE(    r16)              WORDOP(ADD_R32_IMM_BYTE( r16))             // 1 byte operand

#define ADC_BYTE_R8(         r8 )              B2OP( 0x1000     | ((r8 )<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define ADC_DWORD_R32(       r32)              B2OP( 0x1100     | ((r32)<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define ADC_R8_BYTE(         r8 )              B2OP( 0x1200     | ((r8 )<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define ADC_R32_DWORD(       r32)              B2OP( 0x1300     | ((r32)<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define ADC_AL_IMM_BYTE                        B1INS(0x14)                                // 1 byte operand
#define ADC_EAX_IMM_DWORD                      B1INS(0x15)                                // 4 byte operand
#define ADC_R8_IMM_BYTE(     r8 )              B2INS(0x80D0     | (r8 ))                  // 1 byte operand
#define ADC_R32_IMM_DWORD(   r32)              B2INS(0x81D0     | (r32))                  // 4 byte operand
#define ADC_R32_IMM_BYTE(    r32)              B2INS(0x83D0     | (r32))                  // 1 byte operand

#define ADC_WORD_R16(        r16)              WORDOP(ADC_DWORD_R32(    r16))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define ADC_R16_WORD(        r16)              WORDOP(ADC_R32_DWORD(    r16))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define ADC_AX_IMM_WORD                        WORDOP(ADC_EAX_IMM_DWORD)                  // 2 byte operand
#define ADC_R16_IMM_WORD(    r16)              WORDOP(ADC_R32_IMM_DWORD(r16))             // 2 byte operand
#define ADC_R16_IMM_BYTE(    r16)              WORDOP(ADC_R32_IMM_BYTE( r16))             // 1 byte operand

#define OR_BYTE_R8(          r8 )              B2OP( 0x0800     | ((r8 )<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define OR_DWORD_R32(        r32)              B2OP( 0x0900     | ((r32)<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define OR_R8_BYTE(          r8 )              B2OP( 0x0A00     | ((r8 )<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define OR_R32_DWORD(        r32)              B2OP( 0x0B00     | ((r32)<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define OR_AL_IMM_BYTE                         B1INS(0x0C)                                // 1 byte operand
#define OR_EAX_IMM_DWORD                       B1INS(0x0D)                                // 4 byte operand
#define OR_R8_IMM_BYTE(      r8 )              B2INS(0x80C8     |  (r8 ))                 // 1 byte operand
#define OR_R32_IMM_DWORD(    r32)              B2INS(0x81C8     |  (r32))                 // 4 byte operand
#define OR_R32_IMM_BYTE(     r32)              B2INS(0x83C8     |  (r32))                 // 1 byte operand

#define OR_WORD_R16(         r16)              WORDOP(OR_DWORD_R32(     r16))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define OR_R16_WORD(         r16)              WORDOP(OR_R32_DWORD(     r16))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define OR_AX_IMM_WORD                         WORDOP(OR_EAX_IMM_DWORD)                   // 2 byte operand
#define OR_R16_IMM_WORD(     r16)              WORDOP(OR_R32_IMM_DWORD( r16))             // 2 byte operand
#define OR_R16_IMM_BYTE(     r16)              WORDOP(OR_R32_IMM_BYTE(  r16))             // 1 byte operand

#define AND_BYTE_R8(         r8 )              B2OP( 0x2000     | ((r8 )<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define AND_DWORD_R32(       r32)              B2OP( 0x2100     | ((r32)<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define AND_R8_BYTE(         r8 )              B2OP( 0x2200     | ((r8 )<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define AND_R32_DWORD(       r32)              B2OP( 0x2300     | ((r32)<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define AND_AL_IMM_BYTE                        B1INS(0x24)                                // 1 byte operand
#define AND_EAX_IMM_DWORD                      B1INS(0x25)                                // 4 byte operand
#define AND_R8_IMM_BYTE(     r8 )              B2INS(0x80E0     |  (r8 ))                 // 1 byte operand
#define AND_R32_IMM_DWORD(   r32)              B2INS(0x81E0     |  (r32))                 // 4 byte operand
#define AND_R32_IMM_BYTE(    r32)              B2INS(0x83E0     |  (r32))                 // 1 byte operand

#define AND_WORD_R16(        r16)              WORDOP(AND_DWORD_R32(    r16))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define AND_R16_WORD(        r16)              WORDOP(AND_R32_DWORD(    r16))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define AND_AX_IMM_WORD                        WORDOP(AND_EAX_IMM_DWORD)                  // 2 byte operand
#define AND_R16_IMM_WORD(    r16)              WORDOP(AND_R32_IMM_DWORD(r16))             // 2 byte operand
#define AND_R16_IMM_BYTE(    r16)              WORDOP(AND_R32_IMM_BYTE( r16))             // 1 byte operand

#define SUB_BYTE_R8(         r8 )              B2OP( 0x2800     | ((r8 )<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define SUB_DWORD_R32(       r32)              B2OP( 0x2900     | ((r32)<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define SUB_R8_BYTE(         r8 )              B2OP( 0x2A00     | ((r8 )<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define SUB_R32_DWORD(       r32)              B2OP( 0x2B00     | ((r32)<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define SUB_AL_IMM_BYTE                        B1INS(0x2C)                                // 1 byte operand
#define SUB_EAX_IMM_DWORD                      B1INS(0x2D)                                // 4 byte operand
#define SUB_R8_IMM_BYTE(     r8 )              B2INS(0x80E8     |  (r8 ))                 // 1 byte operand
#define SUB_R32_IMM_DWORD(   r32)              B2INS(0x81E8     |  (r32))                 // 4 byte operand
#define SUB_R32_IMM_BYTE(    r32)              B2INS(0x83E8     |  (r32))                 // 1 byte operand

#define SUB_WORD_R16(        r16)              WORDOP(SUB_DWORD_R32(    r16))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define SUB_R16_WORD(        r16)              WORDOP(SUB_R32_DWORD(    r16))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define SUB_AX_IMM_WORD                        WORDOP(SUB_EAX_IMM_DWORD)                  // 2 byte operand
#define SUB_R16_IMM_WORD(    r16)              WORDOP(SUB_R32_IMM_DWORD(r16))             // 2 byte operand
#define SUB_R16_IMM_BYTE(    r16)              WORDOP(SUB_R32_IMM_BYTE( r16))             // 1 byte operand

#define SBB_BYTE_R8(         r8 )              B2OP( 0x1800     | ((r8 )<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define SBB_DWORD_R32(       r32)              B2OP( 0x1900     | ((r32)<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define SBB_R8_BYTE(         r8 )              B2OP( 0x1A00     | ((r8 )<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define SBB_R32_DWORD(       r32)              B2OP( 0x1B00     | ((r32)<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define SBB_AL_IMM_BYTE                        B1INS(0x1C)                                // 1 byte operand
#define SBB_EAX_IMM_DWORD                      B1INS(0x1D)                                // 4 byte operand
#define SBB_R8_IMM_BYTE(     r8 )              B2INS(0x80D8     |  (r8 ))                 // 1 byte operand
#define SBB_R32_IMM_DWORD(   r32)              B2INS(0x81D8     |  (r32))                 // 4 byte operand
#define SBB_R32_IMM_BYTE(    r32)              B2INS(0x83D8     |  (r32))                 // 1 byte operand

#define SBB_WORD_R16(        r16)              WORDOP(SBB_DWORD_R32(    r16))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define SBB_R16_WORD(        r16)              WORDOP(SBB_R32_DWORD(    r16))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define SBB_AX_IMM_WORD                        WORDOP(SBB_EAX_IMM_DWORD)                  // 2 byte operand
#define SBB_R16_IMM_WORD(    r16)              WORDOP(SBB_R32_IMM_DWORD(r16))             // 2 byte operand
#define SBB_R16_IMM_BYTE(    r16)              WORDOP(SBB_R32_IMM_BYTE( r16))             // 1 byte operand

#define XOR_BYTE_R8(         r8 )              B2OP( 0x3000     | ((r8 )<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define XOR_DWORD_R32(       r32)              B2OP( 0x3100     | ((r32)<<3))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define XOR_R8_BYTE(         r8 )              B2OP( 0x3200     | ((r8 )<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define XOR_R32_DWORD(       r32)              B2OP( 0x3300     | ((r32)<<3))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define XOR_AL_IMM_BYTE                        B1INS(0x34)                                // 1 byte operand
#define XOR_EAX_IMM_DWORD                      B1INS(0x35)                                // 4 byte operand
#define XOR_R8_IMM_BYTE(     r8 )              B2INS(0x80F0     |  (r8 ))                 // 1 byte operand
#define XOR_R32_IMM_DWORD(   r32)              B2INS(0x81F0     |  (r32))                 // 4 byte operand
#define XOR_R32_IMM_BYTE(    r32)              B2INS(0x83F0     |  (r32))                 // 1 byte operand

#define XOR_WORD_R16(        r16)              WORDOP(XOR_DWORD_R32(    r16))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define XOR_R16_WORD(        r16)              WORDOP(XOR_R32_DWORD(    r16))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define XOR_AX_IMM_WORD                        WORDOP(XOR_EAX_IMM_DWORD)                  // 2 byte operand
#define XOR_R16_IMM_WORD(    r16)              WORDOP(XOR_R32_IMM_DWORD(r16))             // 2 byte operand
#define XOR_R16_IMM_BYTE(    r16)              WORDOP(XOR_R32_IMM_BYTE( r16))             // 1 byte operand

#define CMP_BYTE_R8(         r8 )              B2OP( 0x3800     | ((r8 )<<3))             // Build op1 with MEM_ADDR-*,REG_SRC-macroes
#define CMP_DWORD_R32(       r32)              B2OP( 0x3900     | ((r32)<<3))             // Build op1 with MEM_ADDR-*,REG_SRC-macroes
#define CMP_R8_BYTE(         r8 )              B2OP( 0x3A00     | ((r8 )<<3))             // Build op2 with MEM_ADDR-*,REG_SRC-macroes
#define CMP_R32_DWORD(       r32)              B2OP( 0x3B00     | ((r32)<<3))             // Build op2 with MEM_ADDR-*,REG_SRC-macroes
#define CMP_AL_IMM_BYTE                        B1INS(0x3C)                                // 1 byte operand
#define CMP_EAX_IMM_DWORD                      B1INS(0x3D)                                // 4 byte operand
#define CMP_R8_IMM_BYTE(     r8 )              B2INS(0x80F8     |  (r8 ))                 // 1 byte operand
#define CMP_R32_IMM_DWORD(   r32)              B2INS(0x81F8     |  (r32))                 // 4 byte operand
#define CMP_R32_IMM_BYTE(    r32)              B2INS(0x83F8     |  (r32))                 // 1 byte operand

#define CMP_WORD_R16(        r16)              WORDOP(CMP_DWORD_R32(    r16))             // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define CMP_R16_WORD(        r16)              WORDOP(CMP_R32_DWORD(    r16))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define CMP_AX_IMM_WORD                        WORDOP(CMP_EAX_IMM_DWORD)                  // 2 byte operand
#define CMP_R16_IMM_WORD(    r16)              WORDOP(CMP_R32_IMM_DWORD(r16))             // 2 byte operand
#define CMP_R16_IMM_BYTE(    r16)              WORDOP(CMP_R32_IMM_BYTE( r16))             // 1 byte operand

#define MUL_BYTE                               B2OP(0xF620)                               // Unsigned multiply (ax      = al  * src  )
#define MUL_DWORD                              B2OP(0xF720)                               //                   (edx:eax = eax * src  )
#define MUL_WORD                               WORDOP(MUL_DWORD)                          //                   (dx:ax   = ax  * src  )

#define IMUL_BYTE                              B2OP(0xF628)                               // Signed multiply   (ax      = al  * src  )
#define IMUL_DWORD                             B2OP(0xF728)                               //                   (edx:eax = eax * src  )
#define IMUL_WORD                              WORDOP(IMUL_DWORD)                         //                   (dx:ax   = ax  * src  )

// Additional forms of IMUL
#define IMUL2_R32_DWORD(      r32)             B3OP(0x0FAF00    | ((r32)<<3))             // 2 arguments       (r32 *= src           )
#define IMUL2_R16_WORD(       r16)             WORDOP(IMUL2_R32_DWORD(r16  ))             //                   (r16 *= src           )

#define IMUL3_DWORD_IMM_DWORD(r32)             B2OP(0x6900      | ((r32)<<3))             // 3 args, r32,src,4 byte operand (r32 = src * imm.dword)
#define IMUL3_DWORD_IMM_BYTE( r32)             B2OP(0x6B00      | ((r32)<<3))             // 3 args. r32.src.1 byte operand (r32 = src * imm.byte )

#define IMUL3_WORD_IMM_WORD(  r16)             WORDOP(IMUL3_DWORD_IMM_DWORD(r16))         // 2 byte operand    (r16 = src * imm word )
#define IMUL3_WORD_IMM_BYTE(  r16)             WORDOP(IMUL3_DWORD_IMM_BYTE( r16))         // 1 byte operand    (r16 = src * imm byte )

#define DIV_BYTE                               B2OP(0xF630          )                     // Unsigned divide ax      /= src, Result:al = quot. ah = rem
#define DIV_DWORD                              B2OP(0xF730          )                     //                 edx:eax /= src. Result:eax= quot. edx= rem
#define DIV_WORD                               WORDOP(DIV_DWORD     )                     //                 dk:ax   /= src. Result:ax = quot. dx = rem

#define IDIV_BYTE                              B2OP(0xF638          )                     // Signed divide   ax      /= src, ah  must contain sign extension of al . Result:al = quot. ah = rem
#define IDIV_DWORD                             B2OP(0xF738          )                     //                 edx:eax /= src. edx must contain sign extension of eax. Result:eax= quot. edx= rem
#define IDIV_WORD                              WORDOP(IDIV_DWORD    )                     //                 dk:ax   /= src. dx  must contain sign extension of ax . Result:ax = quot. dx = rem

#define CWDE                                   B1INS( 0x98 )                              // Convert word to dword   Copy sign (bit 15) of AX  into higher 16 bits of EAX
#define CBW                                    WORDOP(CWDE )                              // Convert byte to word    Copy sign (bit 7)  of AL  into every bit of AH
#define CDQ                                    B1INS( 0x99 )                              // Convert dword to qword  Copy sign (bit 31) of EAX into every bit of EDX
#define CWD                                    WORDOP(CDQ  )                              // Convert word to dword   Copy sign (bit 15) of AX  into every bit of DX

// Rotate and shifting

#define ROL_BYTE                               B2OP(   0xD200                )            // Rotate left by cl
#define ROL_DWORD                              B2OP(   0xD300                )
#define ROL_WORD                               WORDOP( ROL_DWORD             )
#define ROL_BYTE_IMM_BYTE                      B2OP(   0xC000                )            // 1 byte operand as shift amount
#define ROL_DWORD_IMM_BYTE                     B2OP(   0xC100                )            // 1 byte operand as shift amount
#define ROL_WORD_IMM_BYTE                      WORDOP( ROL_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount

#define ROL_R8(          r8 )                  REG_SRC(ROL_BYTE          ,r8 )
#define ROL_R32(         r32)                  REG_SRC(ROL_DWORD         ,r32)
#define ROL_R16(         r16)                  REG_SRC(ROL_WORD          ,r16)
#define ROL_R8_IMM_BYTE( r8 )                  REG_SRC(ROL_BYTE_IMM_BYTE ,r8 )            // 1 byte operand as shift amount
#define ROL_R32_IMM_BYTE(r32)                  REG_SRC(ROL_DWORD_IMM_BYTE,r32)            // 1 byte operand as shift amount
#define ROL_R16_IMM_BYTE(r16)                  REG_SRC(ROL_WORD_IMM_BYTE ,r16)            // 1 byte operand as shift amount

#define ROR_BYTE                               B2OP(   0xD208                )            // Rotate right by cl
#define ROR_DWORD                              B2OP(   0xD308                )
#define ROR_WORD                               WORDOP( ROR_DWORD             )
#define ROR_BYTE_IMM_BYTE                      B2OP(   0xC008                )            // 1 byte operand as shift amount
#define ROR_DWORD_IMM_BYTE                     B2OP(   0xC108                )            // 1 byte operand as shift amount
#define ROR_WORD_IMM_BYTE                      WORDOP( ROR_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount

#define ROR_R8(          r8 )                  REG_SRC(ROR_BYTE          ,r8 )
#define ROR_R32(         r32)                  REG_SRC(ROR_DWORD         ,r32)
#define ROR_R16(         r16)                  REG_SRC(ROR_WORD          ,r16)
#define ROR_R8_IMM_BYTE( r8 )                  REG_SRC(ROR_BYTE_IMM_BYTE ,r8 )            // 1 byte operand as shift amount
#define ROR_R32_IMM_BYTE(r32)                  REG_SRC(ROR_DWORD_IMM_BYTE,r32)            // 1 byte operand as shift amount
#define ROR_R16_IMM_BYTE(r16)                  REG_SRC(ROR_WORD_IMM_BYTE ,r16)            // 1 byte operand as shift amount

#define RCL_BYTE                               B2OP(   0xD210                )            // Rotate left by cl       (with carry)
#define RCL_DWORD                              B2OP(   0xD310                )
#define RCL_WORD                               WORDOP( RCL_DWORD             )
#define RCL_BYTE_IMM_BYTE                      B2OP(   0xC010                )            // 1 byte operand as shift amount
#define RCL_DWORD_IMM_BYTE                     B2OP(   0xC110                )            // 1 byte operand as shift amount
#define RCL_WORD_IMM_BYTE                      WORDOP( RCL_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount

#define RCL_R8(          r8 )                  REG_SRC(RCL_BYTE          ,r8 )
#define RCL_R32(         r32)                  REG_SRC(RCL_DWORD         ,r32)
#define RCL_R16(         r16)                  REG_SRC(RCL_WORD          ,r16)
#define RCL_R8_IMM_BYTE( r8 )                  REG_SRC(RCL_BYTE_IMM_BYTE ,r8 )            // 1 byte operand as shift amount
#define RCL_R32_IMM_BYTE(r32)                  REG_SRC(RCL_DWORD_IMM_BYTE,r32)            // 1 byte operand as shift amount
#define RCL_R16_IMM_BYTE(r16)                  REG_SRC(RCL_WORD_IMM_BYTE ,r16)            // 1 byte operand as shift amount

#define RCR_BYTE                               B2OP(   0xD218                )            // Rotate right by cl (with carry)
#define RCR_DWORD                              B2OP(   0xD318                )
#define RCR_WORD                               WORDOP( RCR_DWORD             )
#define RCR_BYTE_IMM_BYTE                      B2OP(   0xC018                )            // 1 byte operand as shift amount
#define RCR_DWORD_IMM_BYTE                     B2OP(   0xC118                )            // 1 byte operand as shift amount
#define RCR_WORD_IMM_BYTE                      WORDOP( RCR_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount

#define RCR_R8(          r8 )                  REG_SRC(RCR_BYTE          ,r8 )
#define RCR_R32(         r32)                  REG_SRC(RCR_DWORD         ,r32)
#define RCR_R16(         r16)                  REG_SRC(RCR_WORD          ,r16)
#define RCR_R8_IMM_BYTE( r8 )                  REG_SRC(RCR_BYTE_IMM_BYTE ,r8 )            // 1 byte operand as shift amount
#define RCR_R32_IMM_BYTE(r32)                  REG_SRC(RCR_DWORD_IMM_BYTE,r32)            // 1 byte operand as shift amount
#define RCR_R16_IMM_BYTE(r16)                  REG_SRC(RCR_WORD_IMM_BYTE ,r16)            // 1 byte operand as shift amount

#define SHL_BYTE                               B2OP(   0xD220                )            // Shift left by cl                      (unsigned shift left )
#define SHL_DWORD                              B2OP(   0xD320                )
#define SHL_WORD                               WORDOP( SHL_DWORD             )
#define SHL_BYTE_IMM_BYTE                      B2OP(   0xC020                )            // 1 byte operand as shift amount
#define SHL_DWORD_IMM_BYTE                     B2OP(   0xC120                )            // 1 byte operand as shift amount
#define SHL_WORD_IMM_BYTE                      WORDOP( SHL_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount

#define SHL_R8(          r8 )                  REG_SRC(SHL_BYTE          ,r8 )
#define SHL_R32(         r32)                  REG_SRC(SHL_DWORD         ,r32)
#define SHL_R16(         r16)                  REG_SRC(SHL_WORD          ,r16)
#define SHL_R8_IMM_BYTE( r8 )                  REG_SRC(SHL_BYTE_IMM_BYTE ,r8 )            // 1 byte operand as shift amount
#define SHL_R32_IMM_BYTE(r32)                  REG_SRC(SHL_DWORD_IMM_BYTE,r32)            // 1 byte operand as shift amount
#define SHL_R16_IMM_BYTE(r16)                  REG_SRC(SHL_WORD_IMM_BYTE ,r16)            // 1 byte operand as shift amount

#define SHR_BYTE                               B2OP(   0xD228                )            // Shift right by cl                     (unsigned shift right)
#define SHR_DWORD                              B2OP(   0xD328                )
#define SHR_WORD                               WORDOP( SHR_DWORD             )
#define SHR_BYTE_IMM_BYTE                      B2OP(   0xC028                )            // 1 byte operand as shift amount
#define SHR_DWORD_IMM_BYTE                     B2OP(   0xC128                )            // 1 byte operand as shift amount
#define SHR_WORD_IMM_BYTE                      WORDOP( SHR_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount

#define SHR_R8(          r8 )                  REG_SRC(SHR_BYTE          ,r8 )
#define SHR_R32(         r32)                  REG_SRC(SHR_DWORD         ,r32)
#define SHR_R16(         r16)                  REG_SRC(SHR_WORD          ,r16)
#define SHR_R8_IMM_BYTE( r8 )                  REG_SRC(SHR_BYTE_IMM_BYTE ,r8 )            // 1 byte operand as shift amount
#define SHR_R32_IMM_BYTE(r32)                  REG_SRC(SHR_DWORD_IMM_BYTE,r32)            // 1 byte operand as shift amount
#define SHR_R16_IMM_BYTE(r16)                  REG_SRC(SHR_WORD_IMM_BYTE ,r16)            // 1 byte operand as shift amount

#define SAL_BYTE                               SHL_BYTE                                   // Shift Arithmetically left by cl       (signed shift left ) 
#define SAL_DWORD                              SHL_DWORD
#define SAL_WORD                               SHL_WORD
#define SAL_BYTE_IMM_BYTE                      SHL_BYTE_IMM_BYTE                          // 1 byte operand as shift amount
#define SAL_DWORD_IMM_BYTE                     SHL_DWORD_IMM_BYTE                         // 1 byte operand as shift amount
#define SAL_WORD_IMM_BYTE                      SHL_WORD_IMM_BYTE                          // 1 byte operand as shift amount

#define SAL_R8(          r8 )                  SHL_R8(                    r8 )
#define SAL_R32(         r32)                  SHL_R32(                   r32)
#define SAL_R16(         r16)                  SHL_R16(                   r16)
#define SAL_R8_IMM_BYTE( r8 )                  SHL_R8_IMM_BYTE(           r8 )            // 1 byte operand as shift amount
#define SAL_R32_IMM_BYTE(r32)                  SHL_R32_IMM_BYTE(          r32)            // 1 byte operand as shift amount
#define SAL_R16_IMM_BYTE(r16)                  SHL_R16_IMM_BYTE(          r16)            // 1 byte operand as shift amount

#define SAR_BYTE                               B2OP(   0xD238                )            // Shift Arithmetically right by cl       (signed shift right) 
#define SAR_DWORD                              B2OP(   0xD338                )
#define SAR_WORD                               WORDOP( SAR_DWORD             )
#define SAR_BYTE_IMM_BYTE                      B2OP(   0xC038                )            // 1 byte operand as shift amount
#define SAR_DWORD_IMM_BYTE                     B2OP(   0xC138                )            // 1 byte operand as shift amount
#define SAR_WORD_IMM_BYTE                      WORDOP( SAR_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount

#define SAR_R8(          r8 )                  REG_SRC(SAR_BYTE          ,r8 )
#define SAR_R32(         r32)                  REG_SRC(SAR_DWORD         ,r32)
#define SAR_R16(         r16)                  REG_SRC(SAR_WORD          ,r16)
#define SAR_R8_IMM_BYTE( r8 )                  REG_SRC(SAR_BYTE_IMM_BYTE ,r8 )            // 1 byte operand as shift amount
#define SAR_R32_IMM_BYTE(r32)                  REG_SRC(SAR_DWORD_IMM_BYTE,r32)            // 1 byte operand as shift amount
#define SAR_R16_IMM_BYTE(r16)                  REG_SRC(SAR_WORD_IMM_BYTE ,r16)            // 1 byte operand as shift amount

#define SHLD_DWORD(         r32)               B3OP(0x0FA500 | ((r32)<<3))                // Shift left by cl, filling opened bitpositions, by most significant bits of r32
#define SHLD_DWORD_IMM_BYTE(r32)               B3OP(0x0FA400 | ((r32)<<3))                // 1 byte operand as shift amount
#define SHLD_WORD(          r16)               WORDOP(SHLD_DWORD(r16))
#define SHLD_WORD_IMM_BYTE( r16)               WORDOP(SHLD_DWORD_IMM_BYTE(r16))           // 1 byte operand as shift amount

#define SHRD_DWORD(         r32)               B3OP(0x0FAD00 | ((r32)<<3))                // Shift right by cl, filling opened bitpositions, by least significant bits of r32
#define SHRD_DWORD_IMM_BYTE(r32)               B3OP(0x0FAC00 | ((r32)<<3))                // 1 byte operand as shift amount
#define SHRD_WORD(          r16)               WORDOP(SHRD_DWORD(r16))
#define SHRD_WORD_IMM_BYTE( r16)               WORDOP(SHRD_DWORD_IMM_BYTE(r16))           // 1 byte operand as shift amount

#define BSF_DWORD(          r32)               B3OP(0x0FBC00 | ((r32)<<3))                // Bitscan forward
#define BSR_DWORD(          r32)               B3OP(0x0FBD00 | ((r32)<<3))                // Bitscan reversed
#define BSF_WORD(           r16)               WORDOP(BSF_DWORD(r16))
#define BSR_WORD(           r16)               WORDOP(BSR_DWORD(r16))

#define NOT_BYTE                               B2OP(0xF610          )
#define NOT_DWORD                              B2OP(0xF710          )
#define NOT_WORD                               WORDOP(NOT_DWORD     )

#define NOT_R8( r8 )                           REG_SRC(NOT_BYTE, r8 )
#define NOT_R32(r32)                           REG_SRC(NOT_DWORD,r32)
#define NOT_R16(r16)                           REG_SRC(NOT_WORD, r16)

#define NEG_BYTE                               B2OP(0xF618          )
#define NEG_DWORD                              B2OP(0xF718          )
#define NEG_WORD                               WORDOP(NEG_DWORD     )

#define NEG_R8( r8 )                           REG_SRC(NEG_BYTE, r8 )
#define NEG_R16(r16)                           REG_SRC(NEG_WORD, r16)
#define NEG_R32(r32)                           REG_SRC(NEG_DWORD,r32)

#define INS_BYTE                               B1INS(0x6C)
#define INS_DWORD                              B1INS(0x6D)
#define INS_WORD                               WORDOP(INS_DWORD)
#define OUTS_BYTE                              B1INS(0x6E)
#define OUTS_DWORD                             B1INS(0x6F)
#define OUTS_WORD                              WORDOP(OUTS_DWORD)
#define MOVS_BYTE                              B1INS(0xA4)
#define MOVS_DWORD                             B1INS(0xA5)
#define MOVS_WORD                              WORDOP(MOVS_DWORD)
#define CMPS_BYTE                              B1INS(0xA6)
#define CMPS_DWORD                             B1INS(0xA7)
#define CMPS_WORD                              WORDOP(CMPS_DWORD)
#define STOS_BYTE                              B1INS(0xAA)
#define STOS_DWORD                             B1INS(0xAB)
#define STOS_WORD                              WORDOP(STOS_DWORD)
#define LODS_BYTE                              B1INS(0xAC)
#define LODS_DWORD                             B1INS(0xAD)
#define LODS_WORD                              WORDOP(LODS_DWORD)
#define SCAS_BYTE                              B1INS(0xAE)
#define SCAS_DWORD                             B1INS(0xAF)
#define SCAS_WORD                              WORDOP(SCAS_DWORD)

#define REP                                    B1INS(0xF3) // Apply to INS, OUTS, MOVS, LODS, and STOS
#define REPE                                   B1INS(0xF3) // Apply to CMPS and SCAS instructions
#define REPNE                                  B2INS(0xF2) // Apply to CMPS and SCAS instructions

#define INC_BYTE                               B2OP(0xFE00          )
#define INC_DWORD                              B2OP(0xFF00          )
#define INC_R8( r8 )                           REG_SRC(INC_BYTE, r8 )

#define DEC_BYTE                               B2OP(0xFE08          )
#define DEC_DWORD                              B2OP(0xFF08          )
#define DEC_R8( r8 )                           REG_SRC(DEC_BYTE, r8 )

#ifdef IS32BIT

#define INC_R32(r32)                           B1INS(0x40    | (r32))
#define DEC_R32(r32)                           B1INS(0x48    | (r32))

#else // IS64BIT

#define INC_R32(r32)                           REG_SRC( INC_DWORD, r32)
#define INC_QWORD                              REX3(INC_DWORD         )
#define INC_R64(r64)                           REX1(    INC_R32  , r64)

#define DEC_R32(r32)                           REG_SRC( DEC_DWORD, r32)
#define DEC_QWORD                              REX3(DEC_DWORD         )
#define DEC_R64(r64)                           REX1(    DEC_R32  , r64)

#endif // IS64BIT

#define INC_WORD                               WORDOP(INC_DWORD)
#define INC_R16(r16)                           WORDOP(INC_R32(  r16))

#define DEC_WORD                               WORDOP(DEC_DWORD)
#define DEC_R16(r16)                           WORDOP(DEC_R32(  r16))


#ifdef IS64BIT

#define PUSHFQ                                 PUSHFD                                     // Push RFLAGS register
#define POPFQ                                  POPFD                                      // Pop  RFLAGS register

#define MOV_R64_QWORD(       r64)              REX2(MOV_R32_DWORD    ,r64)                // Build src with MEM_ADDR-*,REG_SRC-macroes
#define MOV_QWORD_R64(       r64)              REX2(MOV_DWORD_R32    ,r64)                // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define MOV_R64_IMM_QWORD(   r64)              REX1(MOV_R32_IMM_DWORD,r64)                // 8 byte operand
#define MOV_TO_RAX_IMM_ADDR_QWORD              REX3(MOV_TO_EAX_IMM_ADDR_DWORD)            // 8 byte address
#define MOV_FROM_RAX_IMM_ADDR_QWORD            REX3(MOV_FROM_EAX_IMM_ADDR_DWORD)          // 8 byte address

#define ADD_QWORD_R64(       r64)              REX2(ADD_DWORD_R32    ,r64)
#define ADD_R64_QWORD(       r64)              REX2(ADD_R32_DWORD    ,r64)
#define ADD_R64_IMM_DWORD(   r64)              REX1(ADD_R32_IMM_DWORD,r64)                // 4 byte operand
#define ADD_R64_IMM_BYTE(    r64)              REX1(ADD_R32_IMM_BYTE ,r64)

#define ADC_QWORD_R64(       r64)              REX2(ADC_DWORD_R32    ,r64)
#define ADC_R64_QWORD(       r64)              REX2(ADC_R32_DWORD    ,r64)
#define ADC_R64_IMM_DWORD(   r64)              REX1(ADC_R32_IMM_DWORD,r64)                // 4 byte operand
#define ADC_R64_IMM_BYTE(    r64)              REX1(ADC_R32_IMM_BYTE ,r64)

#define OR_QWORD_R64(        r64)              REX2(OR_DWORD_R32     ,r64)
#define OR_R64_QWORD(        r64)              REX2(OR_R32_DWORD     ,r64)
#define OR_R64_IMM_DWORD(    r64)              REX1(OR_R32_IMM_DWORD ,r64)                // 4 byte operand
#define OR_R64_IMM_BYTE(     r64)              REX1(OR_R32_IMM_BYTE  ,r64)

#define AND_QWORD_R64(       r64)              REX2(AND_DWORD_R32    ,r64)
#define AND_R64_QWORD(       r64)              REX2(AND_R32_DWORD    ,r64)
#define AND_R64_IMM_DWORD(   r64)              REX1(AND_R32_IMM_DWORD,r64)                // 4 byte operand
#define AND_R64_IMM_BYTE(    r64)              REX1(AND_R32_IMM_BYTE ,r64)

#define SUB_QWORD_R64(       r64)              REX2(SUB_DWORD_R32    ,r64)
#define SUB_R64_QWORD(       r64)              REX2(SUB_R32_DWORD    ,r64)
#define SUB_R64_IMM_DWORD(   r64)              REX1(SUB_R32_IMM_DWORD,r64)                // 4 byte operand
#define SUB_R64_IMM_BYTE(    r64)              REX1(SUB_R32_IMM_BYTE ,r64)

#define SBB_QWORD_R64(       r64)              REX2(SBB_DWORD_R32    ,r64)
#define SBB_R64_QWORD(       r64)              REX2(SBB_R32_DWORD    ,r64)
#define SBB_R64_IMM_DWORD(   r64)              REX1(SBB_R32_IMM_DWORD,r64)                // 4 byte operand
#define SBB_R64_IMM_BYTE(    r64)              REX1(SBB_R32_IMM_BYTE ,r64)

#define XOR_QWORD_R64(       r64)              REX2(XOR_DWORD_R32    ,r64)
#define XOR_R64_QWORD(       r64)              REX2(XOR_R32_DWORD    ,r64)
#define XOR_R64_IMM_DWORD(   r64)              REX1(XOR_R32_IMM_DWORD,r64)                // 4 byte operand
#define XOR_R64_IMM_BYTE(    r64)              REX1(XOR_R32_IMM_BYTE ,r64)

#define CMP_QWORD_R64(       r64)              REX2(CMP_DWORD_R32    ,r64)
#define CMP_R64_QWORD(       r64)              REX2(CMP_R32_DWORD    ,r64)
#define CMP_R64_IMM_DWORD(   r64)              REX1(CMP_R32_IMM_DWORD,r64)                // 4 byte operand

#define MUL_QWORD                              REX3(MUL_DWORD)                            // (rdx:rax = rax * src  )
#define IMUL_QWORD                             REX3(IMUL_DWORD)                           // (rdx:rax = rax * src  )

#define IMUL2_R64_DWORD(      r64)             REX2(IMUL2_R32_DWORD      ,r64)            // 2 arguments       (r64 *= src           )

#define IMUL3_QWORD_IMM_DWORD(r64)             REX1(IMUL3_DWORD_IMM_DWORD,r64)            // 3 args, r64,src,4 byte operand (r64 = src * imm.dword)
#define IMUL3_QWORD_IMM_BYTE( r64)             REX1(IMUL3_DWORD_IMM_BYTE ,r64)            // 3 args. r64.src.1 byte operand (r64 = src * imm.byte )

#define DIV_QWORD                              REX3(DIV_DWORD)                            //                 rdx:rax /= src. Result:rax= quot. rdx= rem
#define IDIV_QWORD                             REX3(IDIV_DWORD)                           //                 rdx:rax /= src. rdx must contain sign extension of rax. Result:rax= quot. rdx= rem

#define CDQE                                   REX3(CWDE)                                 // Sign extend EAX into RAX
#define CQO                                    REX3(CDQ )                                 // Sign extend RAX into RDX:RAX

#define ROL_QWORD                              REX3(   ROL_DWORD             )
#define ROL_QWORD_IMM_BYTE                     REX3(   ROL_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount
#define ROL_R64(            r64)               REG_SRC(ROL_QWORD         ,r64)
#define ROL_R64_IMM_BYTE(   r64)               REG_SRC(ROL_QWORD_IMM_BYTE,r64)            // 1 byte operand as shift amount

#define ROR_QWORD                              REX3(   ROR_DWORD             )
#define ROR_QWORD_IMM_BYTE                     REX3(   ROR_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount
#define ROR_R64(            r64)               REG_SRC(ROR_QWORD         ,r64)
#define ROR_R64_IMM_BYTE(   r64)               REG_SRC(ROR_QWORD_IMM_BYTE,r64)            // 1 byte operand as shift amount

#define RCL_QWORD                              REX3(   RCL_DWORD             )
#define RCL_QWORD_IMM_BYTE                     REX3(   RCL_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount
#define RCL_R64(            r64)               REG_SRC(RCL_QWORD         ,r64)
#define RCL_R64_IMM_BYTE(   r64)               REG_SRC(RCL_QWORD_IMM_BYTE,r64)            // 1 byte operand as shift amount

#define RCR_QWORD                              REX3(   RCR_DWORD             )
#define RCR_QWORD_IMM_BYTE                     REX3(   RCR_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount
#define RCR_R64(            r64)               REG_SRC(RCR_QWORD         ,r64)
#define RCR_R64_IMM_BYTE(   r64)               REG_SRC(RCR_QWORD_IMM_BYTE,r64)            // 1 byte operand as shift amount

#define SHL_QWORD                              REX3(   SHL_DWORD             )
#define SHL_QWORD_IMM_BYTE                     REX3(   SHL_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount
#define SHL_R64(            r64)               REG_SRC(SHL_QWORD         ,r64)
#define SHL_R64_IMM_BYTE(   r64)               REG_SRC(SHL_QWORD_IMM_BYTE,r64)            // 1 byte operand as shift amount

#define SHR_QWORD                              REX3(   SHR_DWORD             )
#define SHR_QWORD_IMM_BYTE                     REX3(   SHR_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount
#define SHR_R64(            r64)               REG_SRC(SHR_QWORD         ,r64)
#define SHR_R64_IMM_BYTE(   r64)               REG_SRC(SHR_QWORD_IMM_BYTE,r64)            // 1 byte operand as shift amount

#define SAL_QWORD                              SHL_QWORD
#define SAL_QWORD_IMM_BYTE                     SHL_QWORD_IMM_BYTE                         // 1 byte operand as shift amount
#define SAL_R64(            r64)               SHL_R64(                   r64)
#define SAL_R64_IMM_BYTE(   r64)               SHL_R64_IMM_BYTE(          r64)            // 1 byte operand as shift amount

#define SAR_QWORD                              REX3(   SAR_DWORD             )
#define SAR_QWORD_IMM_BYTE                     REX3(   SAR_DWORD_IMM_BYTE    )            // 1 byte operand as shift amount
#define SAR_R64(            r64)               REG_SRC(SAR_QWORD         ,r64)
#define SAR_R64_IMM_BYTE(   r64)               REG_SRC(SAR_QWORD_IMM_BYTE,r64)            // 1 byte operand as shift amount

#define SHLD_QWORD(         r64)               REX2(SHLD_DWORD           ,r64)
#define SHLD_QWORD_IMM_BYTE(r64)               REX2(SHLD_DWORD_IMM_BYTE  ,r64)            // 1 byte operand as shift amount

#define SHRD_QWORD(         r64)               REX2(SHRD_DWORD           ,r64)
#define SHRD_QWORD_IMM_BYTE(r64)               REX2(SHRD_DWORD_IMM_BYTE  ,r64)            // 1 byte operand as shift amount

#define BSF_QWORD(          r64)               REX2(BSF_DWORD            ,r64)
#define BSR_QWORD(          r64)               REX2(BSR_DWORD            ,r64)

#define NOT_QWORD                              REX3(   NOT_DWORD             )
#define NOT_R64(r64)                           REG_SRC(NOT_QWORD         ,r64)

#define NEG_QWORD                              REX3(   NEG_DWORD             )
#define NEG_R64(r64)                           REG_SRC(NEG_QWORD         ,r64)

#define CMPS_QWORD                             REX3(   CMPS_DWORD            )
#define STOS_QWORD                             REX3(   STOS_DWORD            )
#define LODS_QWORD                             REX3(   LODS_DWORD            )
#define SCAS_QWORD                             REX3(   SCAS_DWORD            )

#define MOVSD_XMM_MMWORD(xmm)                  B4OPNOREG(0xF20F1000 | ((xmm) << 3))       // Build src with MEM_ADDR-*
#define MOVSD_MMWORD_XMM(xmm)                  B4OPNOREG(0xF20F1100 | ((xmm) << 3))       // Build dst with MEM_ADDR-*

#define MOVEAPS(xmmDst, xmmSrc)                B3INS(0x0F28C0  | ((xmmDst)<<3) | (xmmSrc))

#define ADDSD(xmm)                             B4OP(0xF20F5800 | ((xmm) << 3))            // Build src with MEM_ADDR-*,REG_SRC-macroes
#define MULSD(xmm)                             B4OP(0xF20F5900 | ((xmm) << 3))
#define SUBSD(xmm)                             B4OP(0xF20F5C00 | ((xmm) << 3))
#define DIVSD(xmm)                             B4OP(0xF20F5E00 | ((xmm) << 3))

#endif // IS64BIT

// PUSH/POP _R8 not available

#ifdef IS32BIT

#define PUSH_R32(r32)                          B1INS(0x50       | (r32))                  // No operand
#define POP_R32( r32)                          B1INS(0x58       | (r32))                  // No operand

#define PUSH_R16(r16)                          WORDOP(PUSH_R32(r16))                      // No operand
#define POP_R16( r16)                          WORDOP(POP_R32( r16))                      // No operand


#define PUSH_IMM_BYTE                          B1INS(0x6A    )                            // 1 byte value
#define PUSH_IMM_DWORD                         B1INS(0x68    )                            // 4 byte value
#define PUSH_IMM_WORD                          WORDOP(PUSH_IMM_DWORD)                     // 2 byte value

#else // IS64BIT

#define _PUSH_R32(r32)                         B1INS(0x50       | (r32))                  // No operand
#define _POP_R32( r32)                         B1INS(0x58       | (r32))                  // No operand

#define PUSH_R64(r64)                          REX0(_PUSH_R32, r64)                       // No operand
#define POP_R64( r64)                          REX0(_POP_R32 , r64)                       // No operand

#endif // IS64BIT

// FPU instructions

#define FWAIT                                  B1INS(0x9B)                                // Wait while FPU is busy

#define FADD_0i(  i)                           FPUINS(0xD8C0     | (i))                   // st(0) += st(i)
#define FADD_i0(  i)                           FPUINS(0xDCC0     | (i))                   // st(i) += st(0)
#define FADDP_i0( i)                           FPUINS(0xDEC0     | (i))                   // st(i) += st(0); pop st(0)
#define FADD                                   FADDP_i0(1)                                // st(1) += st(0); pop st(0)

#define FMUL_0i(  i)                           FPUINS(0xD8C8     | (i))                   // st(0) *= st(i)
#define FMUL_i0(  i)                           FPUINS(0xDCC8     | (i))                   // st(i) *= st(0)
#define FMULP_i0( i)                           FPUINS(0xDEC8     | (i))                   // st(i) *= st(0); pop st(0)
#define FMUL                                   FMULP_i0(1)                                // st(1) *= st(0); pop st(0)

#define FSUB_0i(  i)                           FPUINS(0xD8E0     | (i))                   // st(0) -= st(i)
#define FSUBR_0i( i)                           FPUINS(0xD8E8     | (i))                   // st(0) =  st(i) - st(0)
#define FSUBR_i0( i)                           FPUINS(0xDCE0     | (i))                   // st(i) =  st(0) - st(i)
#define FSUB_i0(  i)                           FPUINS(0xDCE8     | (i))                   // st(i) -= st(0)
#define FSUBRP_i0(i)                           FPUINS(0xDEE0     | (i))                   // st(i) =  st(0) - st(i); pop st(0)
#define FSUBP_i0( i)                           FPUINS(0xDEE8     | (i))                   // st(i) -= st(0); pop st(0)
#define FSUB                                   FSUBP_i0(1)                                // st(1) -= st(0); pop st(0)

#define FDIV_0i(  i)                           FPUINS(0xD8F0     | (i))                   // st(0) /= st(i)
#define FDIVR_0i( i)                           FPUINS(0xD8F8     | (i))                   // st(0) =  st(i) / st(0)
#define FDIVR_i0( i)                           FPUINS(0xDCF0     | (i))                   // st(i) =  st(0) / st(i)
#define FDIV_i0(  i)                           FPUINS(0xDCF8     | (i))                   // st(i) /= st(0)
#define FDIVRP_i0(i)                           FPUINS(0xDEF0     | (i))                   // st(i) =  st(0) / st(i); pop st(0)
#define FDIVP_i0( i)                           FPUINS(0xDEF8     | (i))                   // st(i) /= st(0); pop st(0)
#define FDIV                                   FDIVP_i0(1)                                // st(1) /= st(0); pop st(0)

#define FCOM(     i)                           FPUINS(0xD8D0     | (i))                   // Compare st(0) to st(i)
#define FCOMP(    i)                           FPUINS(0xD8D8     | (i))                   // Compare st(0) to st(i), pop st(0)
#define FCOMI(    i)                           FPUINS(0xDBF0     | (i))                   // Compare st(0) to st(i) and set CPU-flags
#define FUCOM(    i)                           FPUINS(0xDDE0     | (i))                   // Unordered compare st(0) to st(i)
#define FUCOMI(   i)                           FPUINS(0xDBE8     | (i))                   // Unordered compare st(0) to st(i) and set CPU-flags
#define FUCOMP(   i)                           FPUINS(0xDDE8     | (i))                   // Unordered compare st(0) to st(i); pop st(0)
#define FCOMIP(   i)                           FPUINS(0xDFF0     | (i))                   // Compare st(0) to st(i) and set CPU-flags; pop st(0)
#define FUCOMIP(  i)                           FPUINS(0xDFE8     | (i))                   // Unordered compare st(0) to st(i) and set CPU-flags; pop st(0)
#define FCOMPP                                 FPUINS(0xDED9)                             // Compare st(0) to st(1); pop both
#define FUCOMPP                                FPUINS(0xDAE9)                             // Unordered compare st(0) to st(1); pop both
#define FFREE(    i)                           FPUINS(0xDDC0     | (i))                   // Free a data register
#define FST(      i)                           FPUINS(0xDDD0     | (i))                   // Store st(0) into st(i)
#define FSTP(     i)                           FPUINS(0xDDD8     | (i))                   // Store st(0) into st(i); pop st(0)

#define FLD(      i)                           FPUINS(0xD9C0     | (i))                   // Push st(i) into st(0)
#define FXCH(     i)                           FPUINS(0xD9C8     | (i))                   // Swap st(0) and st(i)
#define FNOP                                   FPUINS(0xD9D0)                             // No operation
#define FCHS                                   FPUINS(0xD9E0)                             // st(0) = -st(0)
#define FABS                                   FPUINS(0xD9E1)                             // st(0) = abs(st(0))
#define FTST                                   FPUINS(0xD9E4)                             // Compare st(0) to 0.0
#define FXAM                                   FPUINS(0xD9E5)                             // Examine the content of st(0)
#define FLD1                                   FPUINS(0xD9E8)                             // push 1.0
#define FLDL2T                                 FPUINS(0xD9E9)                             // push log2(10)
#define FLDL2E                                 FPUINS(0xD9EA)                             // push log2(e)
#define FLDPI                                  FPUINS(0xD9EB)                             // push pi
#define FLDLG2                                 FPUINS(0xD9EC)                             // push log10(2)
#define FLDLN2                                 FPUINS(0xD9ED)                             // push ln(2)
#define FLDZ                                   FPUINS(0xD9EE)                             // push 0.0
#define F2XM1                                  FPUINS(0xD9F0)                             // st(0) = 2^st(0)-1, assume -1 <= st(0) <= 1
#define FYL2X                                  FPUINS(0xD9F1)                             // st(1) = log2(st(0))*st(1); pop st(0)
#define FPTAN                                  FPUINS(0xD9F2)                             // st(0) = tan(st(0)); push 1.0
#define FPATAN                                 FPUINS(0xD9F3)                             // st(1) = atan(st(1)/st(0)); pop st(0)
#define FXTRACT                                FPUINS(0xD9F4)                             // st(0) = unbiased exponent in floating point format of st(0). then push signinificant wiht exponent 0
#define FPREM1                                 FPUINS(0xD9F5)                             // As FPREM. Magnitude of the remainder <= ST(1) / 2
#define FDECSTP                                FPUINS(0xD9F6)                             // Decrement stack pointer. st0->st1, st7->st0, ..., st1->st2
#define FINCSTP                                FPUINS(0xD9F7)                             // Increment stack pointer. st0->st7, st1->st0, ..., st7->st6
#define FPREM                                  FPUINS(0xD9F8)                             // Partial remainder. st(0) %= st(1). Exponent of st(0) reduced with at most 63
#define FYL2XP1                                FPUINS(0xD9F9)                             // st(1) = log2(st(0)+1)*st(1); pop st(0)
#define FSQRT                                  FPUINS(0xD9FA)                             // st(0) = sqrt(st(0))
#define FSINCOS                                FPUINS(0xD9FB)                             // Sine and cosine of the angle value in ST(0), st(0)=sin; push(cos)
#define FRNDINT                                FPUINS(0xD9FC)                             // st(0) = nearest integral value according to the rounding mode
#define FSCALE                                 FPUINS(0xD9FD)                             // st(0) *= 2^int(st(1))
#define FSIN                                   FPUINS(0xD9FE)                             // st(0) = sin(ST(0))
#define FCOS                                   FPUINS(0xD9FF)                             // st(0) = cos(ST(0))

// Move st(i) to st(0) if specified CPU condition is true
#define FCMOVB( i)                             FPUINS(0xDAC0 | (i))                       // Move if below (CF=1)
#define FCMOVEQ(i)                             FPUINS(0xDAC8 | (i))                       // Move if equal (ZF=1)
#define FCMOVBE(i)                             FPUINS(0xDAD0 | (i))                       // Move if below or equal (CF=1 or ZF=1)
#define FCMOVU( i)                             FPUINS(0xDAD8 | (i))                       // Move if unordered (PF=1)
#define FCMOVAE(i)                             FPUINS(0xDBC0 | (i))                       // Move if above or equal (CF=0)
#define FCMOVNE(i)                             FPUINS(0xDBC8 | (i))                       // Move if not equal (ZF=0)
#define FCMOVA( i)                             FPUINS(0xDBD0 | (i))                       // Move if above (CF=0 and ZF=0)
#define FCMOVNU(i)                             FPUINS(0xDBD8 | (i))                       // Move if not unordered (PF=0)

#define FNSTSW_AX                              FPUINS(0xDFE0)                             // Store status word into CPU register AX
// These opcodes should all be used with MEM_ADDR_* to get the various addressing-modes

#define FLDCW_WORD                             FPUINSA(0xD928)                            // load control word
#define FNSTCW_WORD                            FPUINSA(0xD938)                            // store control word
#define FNSTSW_WORD                            FPUINSA(0xDD38)                            // store status word

// Real4 (float)
#define FLD_DWORD                              FPUINSA(0xD900)
#define FST_DWORD                              FPUINSA(0xD910)
#define FSTP_DWORD                             FPUINSA(0xD918)

// Real8 (double)
#define FLD_QWORD                              FPUINSA(0xDD00)
#define FST_QWORD                              FPUINSA(0xDD10)
#define FSTP_QWORD                             FPUINSA(0xDD18)

// Real10 (Double80)
#define FLD_TBYTE                              FPUINSA(0xDB28)
#define FSTP_TBYTE                             FPUINSA(0xDB38)

// 16-bit integer (signed short)
#define FILD_WORD                              FPUINSA(0xDF00)
#define FIST_WORD                              FPUINSA(0xDF10)
#define FISTP_WORD                             FPUINSA(0xDF18)
#define FISTTP_WORD                            FPUINSA(0xDF40)

// 32-bit integer (signed int)
#define FILD_DWORD                             FPUINSA(0xDB00)
#define FIST_DWORD                             FPUINSA(0xDB10)
#define FISTP_DWORD                            FPUINSA(0xDB18)
#define FISTTP_DWORD                           FPUINSA(0xDB40)

// 64-bit integer (signed __int64)
#define FILD_QWORD                             FPUINSA(0xDF28)
#define FISTP_QWORD                            FPUINSA(0xDF38)
#define FISTTP_QWORD                           FPUINSA(0xDD40)

// Real4 (float)
#define FADD_DWORD                             FPUINSA(0xD800)
#define FMUL_DWORD                             FPUINSA(0xD808)
#define FCOM_DWORD                             FPUINSA(0xD810)
#define FCOMP_DWORD                            FPUINSA(0xD818)
#define FSUB_DWORD                             FPUINSA(0xD820)
#define FSUBR_DWORD                            FPUINSA(0xD828)
#define FDIV_DWORD                             FPUINSA(0xD830)
#define FDIVR_DWORD                            FPUINSA(0xD838)

// Real8 (double)
#define FADD_QWORD                             FPUINSA(0xDC00)
#define FMUL_QWORD                             FPUINSA(0xDC08)
#define FCOM_QWORD                             FPUINSA(0xDC10)
#define FCOMP_QWORD                            FPUINSA(0xDC18)
#define FSUB_QWORD                             FPUINSA(0xDC20)
#define FSUBR_QWORD                            FPUINSA(0xDC28)
#define FDIV_QWORD                             FPUINSA(0xDC30)
#define FDIVR_QWORD                            FPUINSA(0xDC38)

// 16-bit integer (short)
#define FIADD_WORD                             FPUINSA(0xDE00)
#define FIMUL_WORD                             FPUINSA(0xDE08)
#define FICOM_WORD                             FPUINSA(0xDE10)
#define FICOMP_WORD                            FPUINSA(0xDE18)
#define FISUB_WORD                             FPUINSA(0xDE20)
#define FISUBR_WORD                            FPUINSA(0xDE28)
#define FIDIV_WORD                             FPUINSA(0xDE30)
#define FIDIVR_WORD                            FPUINSA(0xDE38)

// 32-bit integer (int)
#define FIADD_DWORD                            FPUINSA(0xDA00)
#define FIMUL_DWORD                            FPUINSA(0xDA08)
#define FICOM_DWORD                            FPUINSA(0xDA10)
#define FICOMP_DWORD                           FPUINSA(0xDA18)
#define FISUB_DWORD                            FPUINSA(0xDA20)
#define FISUBR_DWORD                           FPUINSA(0xDA28)
#define FIDIV_DWORD                            FPUINSA(0xDA30)
#define FIDIVR_DWORD                           FPUINSA(0xDA38)

#define FBLD                                   FPUINSA(0xDF20)                            // LoaD BCD data from memory
#define FBSTP                                  FPUINSA(0xDF30)                            // STore BCD data to memory
