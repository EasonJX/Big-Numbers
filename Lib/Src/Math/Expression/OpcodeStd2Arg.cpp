#include "pch.h"
#include <Math/Expression/NewOpCode.h>
#include "RexByte.h"

class InstructionStd2Arg : public InstructionBuilder {
public:
  InstructionStd2Arg(const OpcodeBase &opcode) : InstructionBuilder(opcode)
  {
  }
  // Assume s�me size of dst and src
  InstructionStd2Arg &set2GPReg(  const GPRegister    &dst, const GPRegister    &src);
  InstructionStd2Arg &setGPRegMem(const GPRegister    &dst, const MemoryOperand &src);
  InstructionStd2Arg &setGPRegImm(const GPRegister    &dst, int   immv              );
  InstructionStd2Arg &setMemGPReg(const MemoryOperand &dst, const GPRegister    &src);
  InstructionStd2Arg &setMemImm(  const MemoryOperand &dst, int   immv              );
};

InstructionStd2Arg &InstructionStd2Arg::set2GPReg(const GPRegister &dst, const GPRegister &src) {
  const BYTE    srcIndex = src.getIndex();
  const BYTE    dstIndex = dst.getIndex();
  const RegSize size     = src.getSize();
  switch(size) {
  case REGSIZE_BYTE :
    or(2).add(0xC0 | ((dstIndex&7)<<3) | (srcIndex&7));
    break;
  case REGSIZE_WORD : wordIns();
    // continue case
  default           :
    or(3).add(0xC0 | ((dstIndex&7)<<3) | (srcIndex&7));
    break;
  }
  SETREXBITS(QWORDTOREX(size)|HIGHINDEXTOREX(dstIndex,2)|HIGHINDEXTOREX(srcIndex,0))
  return *this;
}

InstructionStd2Arg &InstructionStd2Arg::setGPRegMem(const GPRegister    &dst, const MemoryOperand &src) {
  or(2);
  return setMemGPReg(src, dst);
}

InstructionStd2Arg &InstructionStd2Arg::setGPRegImm(const GPRegister &reg, int immv) {
  DEFINEMETHODNAME;
  const BYTE    regIndex = reg.getIndex();
  const RegSize regSize  = reg.getSize();
  switch(regSize) {
  case REGSIZE_BYTE :
    if(!isByte(immv)) {
      throwInvalidArgumentException(method,_T("Immediate value %08x doesn't fit in %s"),immv, reg.getName().cstr());
    }
    if(regIndex != 0) {
      prefix(0x80).or(0xC0 | (regIndex&7)).add((char)immv);
    } else {
      or(0x04).add((char)immv);
    }
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) {
      throwInvalidArgumentException(method,_T("Immediate value %08x doesn't fit in %s"),immv, reg.getName().cstr());
    }
    if(regIndex != 0) {
      prefix(isByte(immv)?0x83:0x81).or(0xC0 | (regIndex&7))
     .add(immv,isByte(immv)?1:2);
    } else {
      or(0x05).add(immv,2);
    }
    wordIns();
    break;
  default           :
    if(regIndex != 0) {
      prefix(isByte(immv)?0x83:0x81).or(0xC0 | (regIndex&7))
     .add(immv,isByte(immv)?1:4);
    } else {
      or(0x05).add(immv,4);
    }
    break;
  }
  SETREXBITS(QWORDTOREX(regSize)|HIGHINDEXTOREX(regIndex,0));
  return *this;
}

InstructionStd2Arg &InstructionStd2Arg::setMemGPReg(const MemoryOperand &dst, const GPRegister &src) {
  const BYTE    regIndex = src.getIndex();
  const RegSize regSize  = src.getSize();
  switch(regSize) {
  case REGSIZE_BYTE :
    addMemoryReference(dst);
    or(getArgIndex(), (regIndex&7)<<3);
    break;
  case REGSIZE_WORD :
    wordIns();
    // continue case
  default           :
    or(1).addMemoryReference(dst);
    or(getArgIndex(), (regIndex&7)<<3);
    break;
  }
  SETREXBITS(QWORDTOREX(regSize)|HIGHINDEXTOREX(regIndex,2));
  return *this;
}

InstructionStd2Arg &InstructionStd2Arg::setMemImm(const MemoryOperand &dst, int immv) {
  DEFINEMETHODNAME;
  const OperandSize size = dst.getSize();
  switch(size) {
  case REGSIZE_BYTE :
    if(!isByte(immv)) {
      throwInvalidArgumentException(method,_T("Immediate value %08x doesn't fit in %s"),immv, dst.toString().cstr());
    }
    prefix(0x80).setMemoryReference(dst).add((char)immv);
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) {
      throwInvalidArgumentException(method,_T("Immediate value %08x doesn't fit in %s"),immv, dst.toString().cstr());
    }
    prefix(isByte(immv)?0x83:0x81).setMemoryReference(dst).add(immv,isByte(immv)?1:2);
    wordIns();
    break;
  default           :
    prefix(isByte(immv)?0x83:0x81).setMemoryReference(dst).add(immv,isByte(immv)?1:4);
    break;
  }
  SETREXBITS(QWORDTOREX(size));
  return *this;
}

static bool sizeContainsSrcSize(OperandSize dstSize, OperandSize srcSize) {
  switch(dstSize) {
  case REGSIZE_BYTE :
    return  srcSize == REGSIZE_BYTE;
  case REGSIZE_WORD :
    return (srcSize == REGSIZE_BYTE )
        || (srcSize == REGSIZE_WORD );
  case REGSIZE_DWORD :
    return (srcSize == REGSIZE_BYTE )
        || (srcSize == REGSIZE_WORD )
        || (srcSize == REGSIZE_DWORD);
  case REGSIZE_QWORD :
    return (srcSize == REGSIZE_BYTE )
        || (srcSize == REGSIZE_WORD )
        || (srcSize == REGSIZE_DWORD)
        || (srcSize == REGSIZE_QWORD);
  default            :
    return false;
  }
}

bool OpcodeStd2Arg::isValidOperandCombination(const Register &reg, const InstructionOperand &op2) const {
  if(!isRegisterAllowed()) return false;
  if(!isRegisterTypeAllowed(reg.getType()) || !isOperandSizeAllowed(reg.getSize())) {
    return false;
  }
  switch(op2.getType()) {
  case REGISTER       :
    { const Register &regSrc = op2.getRegister();
      if(!isRegisterTypeAllowed(regSrc.getType())) return false;
      return reg.getSize() == regSrc.getSize();
    }
  case MEMREFERENCE   : // reg <- mem
    if(!isMemoryReferenceAllowed()) return false;
    return reg.getSize() == op2.getSize();
  case IMMEDIATEVALUE :
    if(!isImmediateValueAllowed()) return false;
    return sizeContainsSrcSize(reg.getSize(), op2.getSize());
  }
  return false;
}

bool OpcodeStd2Arg::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2) const {
  switch(op1.getType()) {
  case REGISTER       :
    return isValidOperandCombination(op1.getRegister(), op2);
  case MEMREFERENCE   :
    if(!isMemoryReferenceAllowed()) return false;
    switch(op2.getType()) {
    case REGISTER       : // mem <- reg
      { const Register &regSrc = op2.getRegister();
        if(!isRegisterTypeAllowed(regSrc.getType())) return false;
        return op1.getSize() == regSrc.getSize();
      }
      break;
    case IMMEDIATEVALUE : // mem <- imm
      if(!isImmediateValueAllowed()) return false;
      return sizeContainsSrcSize(op1.getSize(), op2.getSize());
    }
    break;
  }
  return false;
}

InstructionBase OpcodeStd2Arg::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  InstructionStd2Arg result(*this);
  switch(op1.getType()) {
  case REGISTER       :
    validateRegisterAllowed(op1.getRegister());
    switch(op2.getType()) {
    case REGISTER       :
      validateRegisterAllowed(op2.getRegister());
      validateSameSize(op1,op2);
      return result.set2GPReg((GPRegister&)op1.getRegister(),(GPRegister&)op2.getRegister());
    case MEMREFERENCE   : // reg <- mem
      validateMemoryReferenceAllowed();
      validateSameSize(op1,op2);
      return result.setGPRegMem((GPRegister&)op1.getRegister(), (MemoryOperand&)op2);
    case IMMEDIATEVALUE :
      validateImmediateValueAllowed();
      return result.setGPRegImm((GPRegister&)op1.getRegister(), op2.getImmInt32());
    }
    break;
  case MEMREFERENCE   :
    validateMemoryReferenceAllowed();
    switch(op2.getType()) {
    case REGISTER       : // mem <- reg
      validateRegisterAllowed(op2.getRegister());
      validateSameSize(op1,op2);
      return result.setMemGPReg((MemoryOperand&)op1, (GPRegister&)op2.getRegister());
    case IMMEDIATEVALUE : // mem <- imm
      validateImmediateValueAllowed();
      return result.setMemImm((MemoryOperand&)op1, op2.getImmInt32());
    }
  }
  throwInvalidArgumentException(__TFUNCTION__,_T("Invalid combination of operands:%s,%s")
                               ,op1.toString().cstr()
                               ,op2.toString().cstr()
                               );
  return result;
}
