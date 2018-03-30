#include "pch.h"
#include "InstructionBuilder.h"

const RegSizeSet InstructionBuilder::s_sizeBitSet(REGSIZE_WORD ,REGSIZE_DWORD ,REGSIZE_QWORD, REGSIZE_END);

InstructionBuilder::InstructionBuilder(const OpcodeBase &opcode)
  : InstructionBase(opcode               )
  , m_flags(        opcode.getFlags()    )
  , m_extension(    opcode.getExtension())
  , m_opcodeSize(   opcode.size()        )
  , m_opCount(      opcode.getOpCount()  )
{
  init();
  if(m_extension) {
    addExtension();
  }
}

InstructionBuilder::InstructionBuilder(const InstructionBase &ins, UINT flags)
  : InstructionBase(ins       )
  , m_flags(        flags     )
  , m_extension(    0         )
  , m_opcodeSize(   ins.size())
  , m_opCount(      0         )
{
  init();
}

InstructionBuilder &InstructionBuilder::insert(BYTE index, BYTE b) {
  assert((m_size < MAX_INSTRUCTIONSIZE) && (index<m_size));
  BYTE *bp = m_bytes + index;
  for(BYTE *dst = m_bytes + m_size++, *src = dst-1; dst > bp;) {
    *(dst--) = *(src--);
  }
  *bp = b;
  if(index <= m_opcodePos) m_opcodePos++;
  return *this;
}

#ifdef IS64BIT
InstructionBuilder &InstructionBuilder::setRexBits(BYTE bits) {
  assert((bits&0xf0)==0);
  if(m_hasRexByte) {
    m_bytes[m_rexByteIndex] |= bits; // just add new bits to rex-byte
    return *this;
  }
  m_hasRexByte = true;
  return insert(m_rexByteIndex, 0x40|bits);
}
#endif // IS64BIT

InstructionBuilder &InstructionBuilder::add(const BYTE *src, BYTE count) {
  assert(m_size+count <= MAX_INSTRUCTIONSIZE);
  if(count == 1) {
    m_bytes[m_size++] = *src;
  } else {
    for(BYTE *dst = m_bytes+m_size, *end = dst+count; dst<end;) {
      *(dst++) = *(src++);
    }
    m_size += count;
  }
  return *this;
}

InstructionBuilder &InstructionBuilder::setOperandSize(OperandSize size, bool isRegister) {
  switch(size) {
  case REGSIZE_WORD :
    setSizeBit().wordIns(isRegister);
    break;
  case REGSIZE_DWORD:
    setSizeBit();
    break;
  case REGSIZE_QWORD:
    setSizeBit();
    SETREXBITS(QWORDTOREX(REGSIZE_QWORD));
  }
  return *this;
}

InstructionBuilder &InstructionBuilder::setModeBits(BYTE bits) {
  if(m_hasModeByte) {
    if(bits) {
      or(getModeByteIndex(), bits);
    }
  } else {
    m_hasModeByte = true;
    add(bits);
  }
  return *this;
}

InstructionBuilder &InstructionBuilder::prefixImm(BYTE b, OperandSize size, bool isRegister, bool immIsByte) {
  if(needSizeBit(size)) b |= 1;
  if((size != REGSIZE_BYTE) && immIsByte) b |= 2;
  prefix(b);
  m_hasModeByte = true;
  m_opcodePos--;
  switch(size) {
  case REGSIZE_WORD:
    wordIns(isRegister);
    break;
  case REGSIZE_QWORD:
    SETREXBITS(QWORDTOREX(REGSIZE_QWORD));
    break;
  }
  return *this;
}

// ptr[(inx<<shift)+offset], (inx&7) != 4, shift<=3, offset=[INT_MIN;INT_MAX]
InstructionBuilder &InstructionBuilder::addrShiftInx(const IndexRegister &inx, BYTE shift, int offset) {
  const BYTE inxIndex = inx.getIndex();
  assert(((inxIndex&7)!=4) && (shift<=3));
  SETREXBITONHIGHINX(inxIndex,1);
  return setModeBits(MR_SIB(0)).add(SIB_BYTE(5,inxIndex,shift)).add((BYTE*)&offset, 4);
}

// ------------------------------------------------------------------------------------------

InstructionBuilder &InstructionBuilder::addrBase(const IndexRegister &base, int offset) {
  const BYTE baseIndex = base.getIndex();
  switch(baseIndex&7) {
  case 4 :
    if(offset == 0) {
      setModeBits(MR_SIB(DP_0BYTE)).add(0x24);                        // ptr[esp]
    } else if(isByte(offset)) {
      setModeBits(MR_SIB(DP_1BYTE)).add(0x24).add((char)offset);      // ptr[esp+1 byte offset] 
    } else {
      setModeBits(MR_SIB(DP_4BYTE)).add(0x24).add((BYTE*)&offset, 4); // ptr[esp+4 byte offset]
    }
    break;
  default:
    if((offset == 0) && ((baseIndex&7)!=5)) {
      setModeBits(baseIndex&7);                                       // ptr[base],               (base&7) != {4,5}
    } else if(isByte(offset)) {
      setModeBits(MR_DP1BYTE(baseIndex)).add((char)offset);           // ptr[base+1 byte offset], (base&7) != 4
    } else {
      setModeBits(MR_DP4BYTE(baseIndex)).add((BYTE*)&offset, 4);      // ptr[base+4 byte offset], (base&7) != 4
    }
  }
  SETREXBITONHIGHINX(baseIndex,0);
  return *this;
}

InstructionBuilder &InstructionBuilder::addrBaseShiftInx(const IndexRegister &base, const IndexRegister &inx, BYTE shift, int offset) {
  DEFINEMETHODNAME;
  const BYTE baseIndex = base.getIndex(), inxIndex = inx.getIndex();
  if((inxIndex&7)==4) {
    throwInvalidArgumentException(method, _T("Invalid index register:%s"), inx.getName().cstr());
  }
  if(shift > 3) {
    throwInvalidArgumentException(method, _T("shift=%d. Valid range=[0;3]"),shift);
  }
  if((offset == 0) && ((baseIndex&7) != 5)) {
    setModeBits(MR_SIB(DP_0BYTE)).add(SIB_BYTE(baseIndex,inxIndex,shift));
  } else if(isByte(offset)) {
    setModeBits(MR_SIB(DP_1BYTE)).add(SIB_BYTE(baseIndex,inxIndex,shift)).add((char)offset);
  } else {
    setModeBits(MR_SIB(DP_4BYTE)).add(SIB_BYTE(baseIndex,inxIndex,shift)).add((BYTE*)&offset, 4);
  }
  SETREXBITSONHIGHINX2(baseIndex,inxIndex);
  return *this;
}

InstructionBuilder &InstructionBuilder::prefixSegReg(const SegmentRegister &reg) {
  static const BYTE segRegPrefix[] = { 0x26,0x2e,0x36,0x3e,0x64,0x65 }; // es,cs,ss,ds,fs,gs
  assert(reg.getIndex() < ARRAYSIZE(segRegPrefix));
  return prefix(segRegPrefix[reg.getIndex()]);
}

InstructionBuilder &InstructionBuilder::setRegisterOperand(const GPRegister &reg) {
  const BYTE    index = reg.getIndex();
  const RegSize size  = reg.getSize();
  setOperandSize(size,true).setModeBits(MR_REG(index));
  SETREXBITS(HIGHINDEXTOREX(index,0))
  return *this;
}

InstructionBuilder &InstructionBuilder::setMemoryOperand(const MemoryOperand &mem) {
  const MemoryRef &mr = mem.getMemoryReference();
  if(mem.hasSegmentRegister()) {
    prefixSegReg(*mem.getSegmentRegister());
  }
  if(mr.isDisplaceOnly()) {
    if(m_flags & IMMMADDR_ALLOWED) {
      addImmAddr(mr.getAddr());
    } else {
      addrDisplaceOnly(mr.getOffset());
    }
  } else if(mr.hasInx()) {
    if(mr.hasBase()) {
      addrBaseShiftInx(*mr.getBase(), *mr.getInx(), mr.getShift(), mr.getOffset());
    } else if(mr.hasShift()) {
      addrShiftInx(*mr.getInx(), mr.getShift(), mr.getOffset());
    } else { // no base, no shift
      addrBase(*mr.getInx(), mr.getOffset());
    }
  } else {
    assert(!mr.hasShift());
    addrBase(*mr.getBase(), mr.getOffset());
  }
  return setOperandSize(mem.getSize(),false);
}

InstructionBuilder &InstructionBuilder::setMemoryRegOperands(const MemoryOperand &mem, const Register &reg) {
  setMemoryOperand(mem);
  if(mem.getSize() == REGSIZE_VOID) {
    setOperandSize(reg.getSize(),true);
  }
  if(hasModeByte()) {
    const BYTE regIndex = reg.getIndex();
    setModeBits((regIndex&7)<<3);
    SETREXBITS(HIGHINDEXTOREX(regIndex,2));
  } else {
    assert(reg.getIndex() == 0);
  }
  return *this;
}

InstructionBuilder &InstructionBuilder::setRegRegOperands(const Register &reg1, const Register &reg2) {
  const BYTE    reg1Index = reg1.getIndex();
  const BYTE    reg2Index = reg2.getIndex();
  if(reg2.getType() != REGTYPE_GPR) {
    setOperandSize(reg2.getSize(),true).setModeBits(MR_REGREG(reg2Index,reg1Index));
    SETREXBITS(HIGHINDEXTOREX(reg2Index,2) | HIGHINDEXTOREX(reg1Index,0))
  } else {
    setDirectionBit().setOperandSize(reg2.getSize(),true).setModeBits(MR_REGREG(reg1Index,reg2Index)); // set direction bit
    SETREXBITS(HIGHINDEXTOREX(reg1Index,2) | HIGHINDEXTOREX(reg2Index,0))
  }
  return *this;
}
