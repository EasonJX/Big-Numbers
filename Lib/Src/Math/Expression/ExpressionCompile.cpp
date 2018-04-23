#include "pch.h"
#include "ExpressionCompile.h"

DEFINECLASSNAME(MachineCode);

MachineCode::MachineCode(const CompactRealArray &valueTable) : m_valueTable(valueTable) {
  setValueCount(m_valueTable.size());
  m_entryPoint = NULL;
#ifdef TRACE_CALLS
  m_callsGenerated = false;
#endif
#ifdef IS64BIT
#ifndef LONGDOUBLE
  m_referenceFunction = (BYTE*)(BuiltInFunction1)exp;
#else // LONGDOUBLE
  m_referenceFunction = (BYTE*)(BuiltInFunctionRef1)exp;
#endif // LONGDOUBLE
#endif // IS64BIT
}

MachineCode::~MachineCode() {
  clear();
}

void MachineCode::clear() {
  __super::clear();
  clearJumpTable();
#ifdef IS32BIT
  clearFunctionCalls();
#endif
  setValueCount(0);
}

void MachineCode::finalize() {
  fixupJumps();
#ifdef IS32BIT
  linkFunctionCalls();
#endif

  m_entryPoint = (ExpressionEntryPoint)getData();
  clearJumpTable();
#ifdef IS32BIT
  clearFunctionCalls();
#endif
  flushInstructionCache();
}

int MachineCode::addBytes(const void *bytes, int count) {
  const int ret = (int)size();
  append((BYTE*)bytes,count);
  return ret;
}

int MachineCode::emit(const InstructionBase &ins) {
  const int pos = (int)size();
  append(ins.getBytes(), ins.size());
  return pos;
}

void MachineCode::setValueCount(size_t valueCount) {
  if(valueCount == 0) {
    m_esiOffset = 0;
    m_esi       = NULL;
  } else {
    const int maxOffset = 127-127%sizeof(Real);
    m_esiOffset = (char)min(maxOffset, (valueCount / 2) * sizeof(Real));
    m_esi = (BYTE*)m_valueTable.getBuffer() + m_esiOffset;
  }
}

void MachineCode::emitFLoad(const ExpressionNode *n) {
  if(n->isOne()) {
    emit(FLD1);
  } else if (n->isPi()) {
    emit(FLDPI);
  } else if(n->isZero()) {
    emit(FLDZ);
  } else {
    EMITTABLEOP(FLD_REAL, n->getValueIndex());
  }
}

#ifdef IS64BIT
bool MachineCode::emitFLoad(const ExpressionNode *n, const ExpressionDestination &dst) {
  bool returnValue = true;
  switch(dst.getType()) {
  case RESULT_IN_ADDRRDI   :
  case RESULT_ON_STACK     :
  case RESULT_IN_VALUETABLE:
    returnValue = false; // false  indicates that value needs to be moved from FPU to desired destination
    // NB continue case
  case RESULT_IN_FPU       :
    emitFLoad(n);
    break;
#ifndef LONGDOUBLE
  case RESULT_IN_XMM     :
    emitValToXMM(dst.getXMMReg(), n->getValueIndex());
    return true;
#endif // LONGDOUBLE
  }
  return returnValue;
}
#endif // IS64BIT

#ifdef IS32BIT
void MachineCode::emitAddESP(int n) {
  if(n == 0) return;
  emit(ADD(ESP,n));
}

void MachineCode::emitSubESP(int n) {
  if(n == 0) return;
  emit(SUB(ESP,n));
}

#else // IS64BIT

void MachineCode::emitAddRSP(int n) {
  emitAddR64(RSP,n);
}

void MachineCode::emitSubRSP(int n) {
  emitSubR64(RSP,n);
}

void MachineCode::emitAddR64(const GPRegister &r64, int value) {
  if(value == 0) return;
  emit(ADD(r64,value));
}

void MachineCode::emitSubR64(const GPRegister &r64, int value) {
  if(value == 0) return;
  emit(SUB(r64,value));
}

BYTE MachineCode::pushTmp() {
  const BYTE offset = m_stackTop;
  m_stackTop += sizeof(Real);
  return offset;
}

BYTE MachineCode::popTmp()  {
  m_stackTop -= sizeof(Real);
  return m_stackTop;
}

#endif // IS64BIT

InstructionBase JumpFixup::makeInstruction() const {
  int lastSize = m_instructionSize;
  for(;;) {
    const InstructionBase ins = m_op(m_jmpTo - m_instructionPos - lastSize);
    if(ins.size() == lastSize) {
      return ins;
    }
    lastSize = ins.size();
  }
}

int MachineCode::emitJmp(const OpcodeBase &op) {
  const int result = (int)m_jumpFixups.size();
  JumpFixup jf(op, (int)size());
  emit(op(0));
  jf.m_instructionSize = (BYTE)((int)size() - jf.m_instructionPos);
  m_jumpFixups.add(jf);
  return result;
}

void MachineCode::fixupJumps(const CompactIntArray &jumps, int jmpTo) {
  for(size_t i = 0; i < jumps.size(); i++) {
    fixupJump(jumps[i],jmpTo);
  }
}

void MachineCode::fixupJumps() {
  bool stable;
  do {
    stable = true;
    for(size_t i = 0; i < m_jumpFixups.size(); i++) {
      JumpFixup &jf = m_jumpFixups[i];
      if(jf.m_isShortJump) {
        const int v = jf.m_jmpTo - jf.m_instructionPos - jf.m_instructionSize;
        if(!isByte(v)) {
          changeShortJumpToNearJump(jf);
          stable = false;
        }
      }
    }
  } while(!stable);

  for(size_t i = 0; i < m_jumpFixups.size(); i++) {
    JumpFixup &jf = m_jumpFixups[i];
    const int ipRel = jf.m_jmpTo - jf.m_instructionPos - jf.m_instructionSize;
    if(jf.m_isShortJump) {
      assert(isByte(ipRel));
    }
    const InstructionBase ins = jf.m_op(ipRel);
    setBytes(jf.m_instructionPos,ins.getBytes(),ins.size());
  }
}

void MachineCode::changeShortJumpToNearJump(JumpFixup &jf) {
  assert(jf.m_isShortJump);

  const int             pos        = jf.m_instructionPos;
  const int             oldInsSize = jf.m_instructionSize;
  const InstructionBase newIns     = jf.makeInstruction();
  const int             newInsSize = newIns.size();
  const int             bytesAdded = newInsSize - oldInsSize;
  if(bytesAdded > 0) {
    insertZeroes(pos, bytesAdded);
    for(size_t i = 0; i < m_jumpFixups.size(); i++) {
      JumpFixup &jf1 = m_jumpFixups[i];
      if(jf1.m_instructionPos > pos) jf1.m_instructionPos += bytesAdded;
      if(jf1.m_jmpTo          > pos) jf1.m_jmpTo          += bytesAdded;
    }
    jf.m_isShortJump     = false;
    jf.m_instructionSize = newInsSize;
#ifdef IS32BIT
    adjustFunctionCalls(pos, bytesAdded);
#endif
  }
}

#ifdef IS32BIT
void MachineCode::adjustFunctionCalls(int pos, int bytesAdded) {
  for(size_t i = 0; i < m_callArray.size(); i++) {
    FunctionCall &fc = m_callArray[i];
    if(fc.m_pos > pos) {
      fc.m_pos += bytesAdded;
    }
  }
}

InstructionBase FunctionCall::makeInstruction(const MachineCode *code) const {
  int lastSize = m_instructionSize;
  const BYTE    *insAddr  = code->getData() + m_pos;
  for(;;) {
    const intptr_t iprel    = (BYTE*)m_func - insAddr - lastSize;
    const InstructionBase ins = CALL(iprel);
    if(ins.size() == lastSize) {
      return ins;
    }
    lastSize = ins.size();
  }
}

void MachineCode::linkFunctionCalls() {
  for(size_t i = 0; i < m_callArray.size(); i++) {
    linkFunctionCall(m_callArray[i]);
  }
}

void MachineCode::linkFunctionCall(const FunctionCall &fc) {
  const InstructionBase ins = fc.makeInstruction(this);
  assert(ins.size() == fc.m_instructionSize);
  setBytes(fc.m_pos, ins.getBytes(), ins.size());
}

void MachineCode::emitCall(BuiltInFunction f) {
#ifdef TRACE_CALLS
  m_callsGenerated = true;
#endif
  const int pos = emit(CALL((intptr_t)f));
  m_callArray.add(FunctionCall(pos, (BYTE)(size()-pos), f));
}

void MachineCode::emitCall(BuiltInFunction f, const ExpressionDestination &dummy) {
  emitCall(f);
#ifdef LONGDOUBLE
  emit(MOV(EAX,DWORDPtr(ESP)));
  emit(FLD_REAL(EAX));
#endif // LONGDOUBLE
}

#else // IS64BIT

void MachineCode::emitCall(BuiltInFunction f) {
#ifdef TRACE_CALLS
  m_callsGenerated = true;
#endif
  emit(LEA(RAX,QWORDPtr(RBP+(int)((BYTE*)f-m_referenceFunction))));
  emit(CALL(RAX));
//  emit(MOV(RAX,(INT64)f));
//  emit(CALL(RAX));
}

#ifndef LONGDOUBLE
void MachineCode::emitCall(BuiltInFunction f, const ExpressionDestination &dst) {
  emitCall(f);
  switch(dst.getType()) {
  case RESULT_IN_FPU       :
    emitXMMToStack(XMM0,0);                                // XMM0 -> FPU-top
    EMITSTACKOP(FLD_REAL,0);
    break;
  case RESULT_IN_XMM       :
    if(dst.getXMMReg() == XMM1) {
      emit(MOVAPS(XMM1, XMM0));                            // XMM0 -> XMM1
    } // else do nothing
    break;
  case RESULT_IN_ADDRRDI   :
    emit(MOVSD1(MMWORDPtr(RDI),XMM0));                     // XMM0 -> *RDI
    break;
  case RESULT_ON_STACK:
    emitXMMToStack(XMM0, dst.getStackOffset());            // XMM0 -> RSP[dst.stackOffset]
    break;
  case RESULT_IN_VALUETABLE:
    emitXMMToVal(XMM0, dst.getTableIndex());               // XMM0 -> QWORD PTR[RSI+tableOffset]
    break;
  }
}

#else // LONGDOUBLE

void MachineCode::emitCall(BuiltInFunction f, const ExpressionDestination &dst) {
  switch(dst.getType()) {
  case RESULT_IN_FPU       :
    emit(MOV(RCX, RSI));                                   // RCX = RSI + getESIOffset(0);
    emitAddR64(RCX, getESIOffset(0));                      // Will generate code after call, that pushed *rsi in FPU
    break;
  case RESULT_IN_ADDRRDI   :
    emit(MOV(RCX, RDI));                                   // RCX = RDI
    break;
  case RESULT_ON_STACK:
    { emit(MOV(RCX, RSP));                                 // RCX = RSP + dst.stackOffset
      emitAddR64(RCX, dst.getStackOffset());
    }
    break;
  case RESULT_IN_VALUETABLE:
    { emit(MOV(RCX, RSI));                                 // RCX = RSI + getESIOffset(dst.tableIndex))
      emitAddR64(RCX, getESIOffset(dst.getTableIndex()));
      break;
    }
  }

  emitCall(f);

  switch(dst.getType()) {
  case RESULT_IN_FPU       :
    emit(FLD_REAL(RAX)); // push *rax into FPU
    break;
  }
}

#endif // LONGDOUBLE

#endif // IS64BIT

void MachineCode::dump(const String &fname, const String &title) const {
  FILE *f = FOPEN(fname,_T("a"));
  _ftprintf(f, _T("%s\n"), title.cstr());
  hexdump(getData(), (int)size(), f);
  _ftprintf(f,_T("----------------------------------------------\n"));
  fclose(f);
}

// --------------------------------- CodeGenerator -----------------------------------

CodeGenerator::CodeGenerator(ParserTree *tree, TrigonometricMode trigonometricMode)
  : m_tree(*tree)
  , m_trigonometricMode(trigonometricMode)
{
  if(tree->getTreeForm() != TREEFORM_STANDARD) {
    throwException(_T("Treeform must be STANDARD to generate machinecode. Form=%s"), m_tree.getTreeFormName().cstr());
  }
  m_code = new MachineCode(m_tree.getValueTable());
  genMachineCode();
}

void CodeGenerator::genMachineCode() {
  genProlog();
  genStatementList(m_tree.getRoot());
  m_code->finalize();
#ifdef TRACE_CALLS
  debugLog(_T("hasCalls:%5s Calls generated:%5s"), boolToStr(m_hasCalls), boolToStr(m_code->m_callsGenerated));
#endif
}

void CodeGenerator::genProlog() {
#ifdef IS64BIT
#define LOCALSTACKSPACE   72
#define RESERVESTACKSPACE 40
  m_hasCalls = m_tree.getRoot()->containsFunctionCall();
  m_code->resetStack(RESERVESTACKSPACE);
  if(m_hasCalls) {
    m_code->emit(PUSH(RBP));
    m_code->emit(MOV(RBP,(INT64)m_code->getReferenceFunction()));
    m_code->emitSubRSP(LOCALSTACKSPACE + RESERVESTACKSPACE); // to get 16-byte aligned RSP
  }
#endif
}

void CodeGenerator::genEpilog() {
#ifdef IS64BIT
  if(m_hasCalls) {
    m_code->emitAddRSP(LOCALSTACKSPACE + RESERVESTACKSPACE);
    m_code->emit(POP(RBP));
  }
#endif
  m_code->emit(RET);
}

void CodeGenerator::genStatementList(const ExpressionNode *n) {
  switch(n->getSymbol()) {
  case SEMI      :
    genStatementList(n->left());
    genStatementList(n->right());
    break;
  case ASSIGN    :
    genAssignment(n);
    break;
  case RETURNREAL:
    genExpression(n->left(), DST_ADDRRDI);
    genEpilog();
    break;
  case RETURNBOOL:
    genReturnBoolExpression(n);
    break;
  default    :
    ParserTree::throwUnknownSymbolException(__TFUNCTION__, n);
    break;
  }
}

#ifdef IS32BIT
void CodeGenerator::genAssignment(const ExpressionNode *n) {
  genExpression(n->right(), DST_FPU);
  m_code->emitFPopVal(n->left());
}
#else // IS64BIT
void CodeGenerator::genAssignment(const ExpressionNode *n) {
  genExpression(n->right(), DST_INVALUETABLE(n->left()->getValueIndex()));
}
#endif IS32BIT

void CodeGenerator::genReturnBoolExpression(const ExpressionNode *n) {
  const JumpList jumps     = genBoolExpression(n->left());
  const int      trueLabel = m_code->emit(MOV(EAX,1));
  genEpilog();

  const int falseLabel = m_code->emit(XOR(EAX,EAX));
  genEpilog();

  m_code->fixupJumps(jumps.trueJumps ,trueLabel );
  m_code->fixupJumps(jumps.falseJumps,falseLabel);
}

// Generate multiplication-sequence to calculate st(0) = st(0)^y
// using st(0), and if neccessary st(1), by logarithmic powering
// Assume st(0) = x contains value to raise to the power y
void CodeGenerator::genPowMultSequence(UINT y) {
  UINT p2 = 1;
  for(UINT t = y; t && ((t&1)==0); t >>= 1) {
    p2 <<= 1;
    m_code->emit(FMUL(ST0,ST0));  // ST(0) *= ST(0)
  }
  // ST(0) = x^p2
  if(p2 < y) {
    m_code->emit(FLD(ST0)); // ST(0) = x^p2; ST(1) = x^p2
    UINT reg = 0, count = 0;
    // Calculate the sequence of multiplications of st(1), st(0) needed
    // to make st(0) = x^(y-p2) and st(1) = x^p2
    for(y -= p2; y > p2; count++) {
      reg <<= 1;
      if(y&p2) { y-=p2; reg |= 1; } else y >>= 1;
    }
    for(;count--; reg >>= 1) {
      m_code->emit(FMUL(ST0,ST(reg&1)));
    }
    m_code->emit(FMUL); // finally st(1) *= st(0), pop st(0)
  }
}

#define GENEXPRESSION(n) genExpression(n  ,dst)
#define GENCALL(n,f)     genCall(      n,f,dst); return
#define GENCALLARG(n,f)  genCall1Arg(  n,f,dst); return
#define GENPOLY(n)       genPolynomial(n  ,dst); return
#define GENIF(n)         genIf(n,dst);           return

void CodeGenerator::throwInvalidTrigonometricMode() {
  throwInvalidArgumentException(_T("genExpression"), _T("Invalid trigonometricMode:%d"), m_trigonometricMode);
}

#define GENTRIGOCALL(n,f)                         \
  switch(getTrigonometricMode()) {                \
  case RADIANS: GENCALL(n, f          );          \
  case DEGREES: GENCALL(n, f##Degrees );          \
  case GRADS  : GENCALL(n, f##Grads   );          \
  default     : throwInvalidTrigonometricMode();  \
  }

void CodeGenerator::genExpression(const ExpressionNode *n, const ExpressionDestination &dst) {
  switch(n->getSymbol()) {
  case NAME  :
  case NUMBER:
#ifdef IS32BIT
    m_code->emitFLoad(n);
#else // IS64BIT
    if(m_code->emitFLoad(n, dst)) {
      return;
    }
#endif // IS64BIT
    break;
  case SEMI:
    genStatementList(n);
    break;

  case PLUS  :
#ifdef LONGDOUBLE
    genExpression(n->left() , DST_FPU);
    genExpression(n->right(), DST_FPU);
    m_code->emit(FADD);
    break;
#else // !LONGDOUBLE
    if(n->left()->isNameOrNumber()) {
      genExpression(n->right(), DST_FPU);
      m_code->emitFAddVal(n->left());
    } else if(n->right()->isNameOrNumber()) {
      genExpression(n->left(), DST_FPU);
      m_code->emitFAddVal(n->right());
    } else {
      genExpression(n->left() , DST_FPU);
      genExpression(n->right(), DST_FPU);
      m_code->emit(FADD);
    }
    break;
#endif // LONGDOUBLE

  case MINUS :
    if(n->isUnaryMinus()) {
      genExpression(n->left(), DST_FPU);
      m_code->emit(FCHS);
      break;
    }
#ifdef LONGDOUBLE
    genExpression(n->left() , DST_FPU);
    genExpression(n->right(), DST_FPU);
    m_code->emit(FSUB);
    break;
#else // !LONGDOUBLE
    if(n->right()->isNameOrNumber()) {
      genExpression(n->left(), DST_FPU);
      m_code->emitFSubVal(n->right());
    } else if(n->left()->isNameOrNumber()) {
      genExpression(n->right(), DST_FPU);
      m_code->emitFSubRVal(n->left());
    } else {
      genExpression(n->left() , DST_FPU);
      genExpression(n->right(), DST_FPU);
      m_code->emit(FSUB);
    }
    break;
#endif // LONGDOUBLE

  case PROD  :
#ifdef LONGDOUBLE
    genExpression(n->left() , DST_FPU);
    genExpression(n->right(), DST_FPU);
    m_code->emit(FMUL);
    break;
#else // !LONGDOUBLE
    if(n->left()->isNameOrNumber()) {
      genExpression(n->right(), DST_FPU);
      m_code->emitFMulVal(n->left());
    } else if(n->right()->isNameOrNumber()) {
      genExpression(n->left(), DST_FPU);
      m_code->emitFMulVal(n->right());
    } else {
      genExpression(n->left() , DST_FPU);
      genExpression(n->right(), DST_FPU);
      m_code->emit(FMUL);
    }
    break;
#endif // LONGDOUBLE

  case QUOT :
#ifdef LONGDOUBLE
    genExpression(n->left() , DST_FPU);
    genExpression(n->right(), DST_FPU);
    m_code->emit(FDIV);
    break;
#else // !LONGDOUBLE
    if(n->right()->isNameOrNumber()) {
      genExpression(n->left(), DST_FPU);
      m_code->emitFDivVal(n->right());
    } else if(n->left()->isNameOrNumber()) {
      genExpression(n->right(), DST_FPU);
      m_code->emitFDivRVal(n->left());
    } else {
      genExpression(n->left() , DST_FPU);
      genExpression(n->right(), DST_FPU);
      m_code->emit(FDIV);
    }
    break;
#endif // LONGDOUBLE

  case MOD           :    GENCALL(     n, fmod                  );
  case POW           :
    if(n->left()->isEulersConstant()) {
      GENCALLARG(  n->right(), exp                   );
    } else if(n->left()->isTwo()) {
      GENCALLARG(  n->right(), exp2                  );
    } else if (n->left()->isTen()) {
      GENCALLARG(  n->right(), exp10                 );
    }
    if(n->right()->isConstant()) {
      const Real p = m_tree.evaluateRealExpr(n->right());
      if((fabs(p) <= 64) && (p - floor(p) == 0)) {
        const int y = getInt(p);
        if(y == 0) {
          genExpression(m_tree.getOne(), dst);
          return;
        } else if(y == 1) {
          genExpression(n->left(), dst);
          return;
        } else {
          genExpression(n->left(), DST_FPU);
          genPowMultSequence(abs(y));
          if(y < 0) {                    // make st0 = 1/st0
            m_code->emit(FLD1);          // st0=1 , st1=x^|y|
            m_code->emit(FDIVRP(ST1));   // st1=st0/st1; pop st0 => st0=x^y
          }
        }
        break;
      }
    }
    GENCALL(     n, mypow                 );
  case SQR           :
    genExpression(n->left(), DST_FPU);
    m_code->emit(FMUL(ST0,ST0));           // st0=x^2
    break;
  case SQRT          :
    genExpression(n->left(), DST_FPU);
    m_code->emit(FSQRT);                   // st0=sqrt(st0)
    break;
  case ABS           :
    genExpression(n->left(), DST_FPU);
    m_code->emit(FABS);                    // st0=|st0|
    break;
  case ATAN          :                     // atan(left)
    if(getTrigonometricMode() == RADIANS) {
      genExpression(n->left(), DST_FPU);
      m_code->emit(FLD1);                  // st0=1, st1=left
      m_code->emit(FPATAN);                // st1=atan(st1/st0); pop st0
      break;
    }
    GENTRIGOCALL(n, atan                  );
  case ATAN2         :                     // atan2(left,right)
    if(getTrigonometricMode() == RADIANS) {
      genExpression(n->left() , DST_FPU);  // st0=left
      genExpression(n->right(), DST_FPU);  // st0=right, st1=left
      m_code->emit(FPATAN);                // st1=atan(st1/st0); pop st0
      break;
    }
    GENTRIGOCALL(n, atan2                 );

  case ROOT          :    GENCALL(     n, root                  );
  case SIN           :    GENTRIGOCALL(n, sin                   );
  case COS           :    GENTRIGOCALL(n, cos                   );
  case TAN           :    GENTRIGOCALL(n, tan                   );
  case COT           :    GENTRIGOCALL(n, cot                   );
  case CSC           :    GENTRIGOCALL(n, csc                   );
  case SEC           :    GENTRIGOCALL(n, sec                   );
  case ASIN          :    GENTRIGOCALL(n, asin                  );
  case ACOS          :    GENTRIGOCALL(n, acos                  );
  case ACOT          :    GENTRIGOCALL(n, acot                  );
  case ACSC          :    GENTRIGOCALL(n, acsc                  );
  case ASEC          :    GENTRIGOCALL(n, asec                  );
  case COSH          :    GENCALL(     n, cosh                  );
  case SINH          :    GENCALL(     n, sinh                  );
  case TANH          :    GENCALL(     n, tanh                  );
  case ACOSH         :    GENCALL(     n, acosh                 );
  case ASINH         :    GENCALL(     n, asinh                 );
  case ATANH         :    GENCALL(     n, atanh                 );
  case LN            :    GENCALL(     n, log                   );
  case LOG10         :    GENCALL(     n, log10                 );
  case LOG2          :    GENCALL(     n, log2                  );
  case EXP           :    GENCALL(     n, exp                   );
  case EXP10         :    GENCALL(     n, exp10                 );
  case EXP2          :    GENCALL(     n, exp2                  );
  case FLOOR         :    GENCALL(     n, floor                 );
  case CEIL          :    GENCALL(     n, ceil                  );
  case HYPOT         :    GENCALL(     n, hypot                 );
  case LINCGAMMA     :    GENCALL(     n, lowerIncGamma         );
  case CHI2DENS      :    GENCALL(     n, chiSquaredDensity     );
  case CHI2DIST      :    GENCALL(     n, chiSquaredDistribution);
  case BINOMIAL      :    GENCALL(     n, binomial              );
  case GAMMA         :    GENCALL(     n, gamma                 );
  case GAUSS         :    GENCALL(     n, gauss                 );
  case FAC           :    GENCALL(     n, fac                   );
  case NORM          :    GENCALL(     n, norm                  );
  case PROBIT        :    GENCALL(     n, probitFunction        );
  case ERF           :    GENCALL(     n, errorFunction         );
  case INVERF        :    GENCALL(     n, inverseErrorFunction  );
  case SIGN          :    GENCALL(     n, dsign                 );
  case MAX           :    GENCALL(     n, dmax                  );
  case MIN           :    GENCALL(     n, dmin                  );
  case RAND          :    GENCALL(     n, randReal              );
  case NORMRAND      :    GENCALL(     n, randomGaussian        );
  case POLY          :    GENPOLY(     n);
  case INDEXEDSUM    :
  case INDEXEDPRODUCT:
    genIndexedExpression(n);
    break;
  case IIF           :    GENIF(n);
  default            :
    ParserTree::throwUnknownSymbolException(__TFUNCTION__, n);
    break;
  }
#ifdef IS64BIT
// At this point, the result is at the top in FPU-stack. Move result to dst
  switch(dst.getType()) {
  case RESULT_IN_FPU       : // do nothing
    break;
  case RESULT_IN_ADDRRDI   :
    m_code->emit(FSTP_REAL(RDI));                                          // FPU -> *RDI
    break;
  case RESULT_ON_STACK     :
    m_code->EMITSTACKOP(FSTP_REAL, dst.getStackOffset());                  // FPU -> RSP[offset]
    break;
  case RESULT_IN_VALUETABLE:
    m_code->emitFPopVal(dst.getTableIndex());                              // FPU -> m_valuetable[tableIndex]
    break;
#ifndef LONGDOUBLE
  case RESULT_IN_XMM       :
    m_code->EMITSTACKOP(FSTP_REAL,0);                                      // FPU  -> *RSP
    m_code->emitStackToXMM(dst.getXMMReg(),0);                             // *RSP -> XMM0 or XMM1
    break;
#endif // LONGDOUBLE
  }
#endif // IS64BIT
}

void CodeGenerator::genIndexedExpression(const ExpressionNode *n) {
  const bool            summation       = n->getSymbol() == INDEXEDSUM;
  const ExpressionNode *startAssignment = n->child(0);
  const ExpressionNode *loopVar         = startAssignment->left();
  const ExpressionNode *endExpr         = n->child(1);
  const ExpressionNode *expr            = n->child(2);

  genExpression(endExpr, DST_FPU);                      // Evaluate end value for loopVar. and keep it in FPU-register
  m_code->emit(summation ? FLDZ : FLD1);                // Initialize accumulator
  genExpression(startAssignment->right(), DST_FPU);     // Evaluate start value for loopVar
  const int loopStart = (int)m_code->size();
  m_code->emit(FCOMI(ST2));                             // Invariant:loopVar in st(0), endExpr in st(2)
  const int jmpEnd   = m_code->emitJmp(JA);             // Jump loopEnd if st(0) > st(2)
  m_code->emitFPopVal(loopVar);                         // Pop st(0) to loopVar
  genExpression(expr, DST_FPU);                         // Accumulator in st(0) (starting at 0 for INDEXEDSUM, 1 for INDEXEDPRODUCT)
  m_code->emit(summation ? FADD : FMUL);                // Update accumulator with st(0)
  m_code->emitFLoad(loopVar);
  m_code->emit(FLD1);
  m_code->emit(FADD);                                   // Increment loopVar
  const int jmpStart = m_code->emitJmp(JMP);       // Jump loopStart
  const int loopEnd  = (int)m_code->size();
  m_code->emit(FSTP(ST0));                              // Pop loopVar
  m_code->emit(FXCH(ST1));                              // Result in st(0), end value in st(1). swap these and pop st(0)
  m_code->emit(FSTP(ST0));                              // Pop end value
  m_code->fixupJump(jmpStart,loopStart);
  m_code->fixupJump(jmpEnd  ,loopEnd  );
}

void CodeGenerator::genIf(const ExpressionNode *n, const ExpressionDestination &dst) {
  const JumpList jumps = genBoolExpression(n->child(0));
  m_code->fixupJumps(jumps.trueJumps,(int)m_code->size());
  GENEXPRESSION(n->child(1)); // true-expression
  const int trueResultJump  = m_code->emitJmp(JMP);
  m_code->fixupJumps(jumps.falseJumps,(int)m_code->size());
  GENEXPRESSION(n->child(2)); // false-expression
  m_code->fixupJump(trueResultJump,(int)m_code->size());
}

static ExpressionInputSymbol reverseComparator(ExpressionInputSymbol symbol) {
  switch(symbol) {
  case EQ :
  case NE : return symbol;
  case LE : return GE;
  case LT : return GT;
  case GE : return LE;
  case GT : return LT;
  }
  throwInvalidArgumentException(__TFUNCTION__,_T("symbol=%d"), symbol);
  return EQ;
}

JumpList CodeGenerator::genBoolExpression(const ExpressionNode *n) {
//  dumpSyntaxTree(n);
  JumpList result;
  switch(n->getSymbol()) {
  case SYMNOT:
    { const JumpList jumps    = genBoolExpression(n->child(0));
      const int      trueJump = m_code->emitJmp(JMP);
      result.falseJumps.addAll(jumps.trueJumps);
      result.falseJumps.add(trueJump);
      result.trueJumps.addAll(jumps.falseJumps);
    }
    break;
  case SYMAND:
    { const JumpList jump1 = genBoolExpression(n->left());
      const JumpList jump2 = genBoolExpression(n->right());
      m_code->fixupJumps(jump1.trueJumps,(int)m_code->size());
      result.falseJumps.addAll(jump1.falseJumps);
      result.falseJumps.addAll(jump2.falseJumps);
      result.trueJumps.addAll(jump2.trueJumps);
    }
    break;
  case SYMOR   :
    { const JumpList jump1 = genBoolExpression(n->left());
      const int trueJump   = m_code->emitJmp(JMP);
      m_code->fixupJumps(jump1.falseJumps,(int)m_code->size());
      const JumpList jump2 = genBoolExpression(n->right());
      result.falseJumps.addAll(jump2.falseJumps);
      result.trueJumps.addAll(jump1.trueJumps);
      result.trueJumps.addAll(jump2.trueJumps);
      result.trueJumps.add(trueJump);
    }
    break;
  case EQ   :
  case NE   :
  case LE   :
  case LT   :
  case GE   :
  case GT   :
    { ExpressionInputSymbol symbol = n->getSymbol();
#ifdef LONGDOUBLE
      genExpression(n->right(), DST_FPU);
      genExpression(n->left() , DST_FPU);
      m_code->emit(FCOMPP);
#else // !LONGDOUBLE
      if(n->left()->isNameOrNumber()) {
        genExpression(n->right(), DST_FPU);
        m_code->emitFCompVal(n->left());
        symbol = reverseComparator(symbol);
      } else if(n->right()->isNameOrNumber()) {
        genExpression(n->left(), DST_FPU);
        m_code->emitFCompVal(n->right());
      } else {
        genExpression(n->right(), DST_FPU);
        genExpression(n->left(), DST_FPU);
        m_code->emit(FCOMPP);
      }
#endif // LONGDOUBLE
      m_code->emit(FNSTSWAX);
      m_code->emit(SAHF);

      switch(symbol) {
      case EQ:
        result.falseJumps.add(m_code->emitJmp(JNE));
        break;
      case NE:
        result.falseJumps.add(m_code->emitJmp(JE));
        break;
      case LE:
        result.falseJumps.add(m_code->emitJmp(JA));
        break;
      case LT:
        result.falseJumps.add(m_code->emitJmp(JAE));
        break;
      case GE:
        result.falseJumps.add(m_code->emitJmp(JB));
        break;
      case GT:
        result.falseJumps.add(m_code->emitJmp(JBE));
        break;
      }
    }
    break;

  default:
    ParserTree::throwUnknownSymbolException(__TFUNCTION__, n);
  }
  return result;
}


// n is number of coeffients which is degree - 1.
// coef[0] if coefficient for x^(n-1), coef[n-1] is constant term of polynomial
static Real evaluatePolynomial(Real x, int n, const Real *coef) {
  const Real *last   = coef + n;
  Real        result = *coef;
  while(++coef < last) {
    result = result * x + *coef;
  }
  return result;
}


#ifdef IS32BIT

void CodeGenerator::genCall1Arg(const ExpressionNode *arg, BuiltInFunction1 f, const ExpressionDestination &dummy) {
  int bytesPushed = 0;
  bytesPushed += genPush(arg);
  bytesPushed += genPushReturnAddr();
  m_code->emitCall((BuiltInFunction)f, dummy);
  m_code->emitAddESP(bytesPushed);
}

void CodeGenerator::genCall2Arg(const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunction2 f, const ExpressionDestination &dummy) {
  int bytesPushed = 0;
  bytesPushed += genPush(arg2);
  bytesPushed += genPush(arg1);
  bytesPushed += genPushReturnAddr();
  m_code->emitCall((BuiltInFunction)f, dummy);
  m_code->emitAddESP(bytesPushed);
}

void CodeGenerator::genCall1Arg(const ExpressionNode *arg, BuiltInFunctionRef1 f, const ExpressionDestination &dummy) {
  int bytesPushed = 0;
  bytesPushed += genPushRef(arg,0);
  bytesPushed += genPushReturnAddr();
  m_code->emitCall((BuiltInFunction)f, dummy);
  m_code->emitAddESP(bytesPushed);
}

void CodeGenerator::genCall2Arg(const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunctionRef2 f, const ExpressionDestination &dummy) {
  int bytesPushed = 0;
  bytesPushed += genPushRef(arg2,0);
  bytesPushed += genPushRef(arg1,1);
  bytesPushed += genPushReturnAddr();
  m_code->emitCall((BuiltInFunction)f, dummy);
  m_code->emitAddESP(bytesPushed);
}

void CodeGenerator::genPolynomial(const ExpressionNode *n, const ExpressionDestination &dummy) {
  const ExpressionNodeArray &coefArray       = n->getCoefficientArray();
  const int                  firstCoefIndex  = n->getFirstCoefIndex();
  for(int i = 0; i < (int)coefArray.size(); i++) {
    const ExpressionNode *coef = coefArray[i];
    if(coef->isConstant()) {
      m_tree.getValueRef(firstCoefIndex + i) = m_tree.evaluateRealExpr(coef);
    } else {
      genExpression(coef, dummy);
      m_code->emitFPopVal(firstCoefIndex + i);
    }
  }

  int bytesPushed = 0;
  bytesPushed += genPushRef(&m_tree.getValueRef(firstCoefIndex));
  bytesPushed += genPushInt((int)coefArray.size());
  bytesPushed += genPush(n->getArgument());
  bytesPushed += genPushReturnAddr();
  m_code->emitCall((BuiltInFunction)::evaluatePolynomial, dummy);
  m_code->emitAddESP(bytesPushed);
}

static int getAlignedSize(int size) {
  const int rest = size % 4;
  return rest ? (size + (4-rest)) : size;
}

int CodeGenerator::genPush(const ExpressionNode *n) {
  if(n->isNameOrNumber()) {
    return genPushReal(n->getValueRef());
  } else {
    genExpression(n, DST_FPU);
    int bytesPushed = getAlignedSize(sizeof(Real));
    m_code->emitSubESP(bytesPushed);
    m_code->emit(FSTP_REAL(ESP));
    return bytesPushed;
  }
}

int CodeGenerator::genPushRef(const ExpressionNode *n, int index) {
  if(n->isNameOrNumber()) {
    return genPushRef(&n->getValueRef());
  } else {
    genExpression(n, DST_FPU);
    m_code->emitFPopVal(index);
    return genPushRef(&m_tree.getValueRef(index));
  }
}

int CodeGenerator::genPushReal(const Real &x) {
  return genPush(&x,sizeof(Real));
}

// return size rounded up to nearest multiply of 4
int CodeGenerator::genPush(const void *p, UINT size) {
  switch(size) {
  case 2:
    m_code->emit(PUSH(WORDPtr((intptr_t)p)));
    return 4;
  case 4:
    m_code->emit(PUSH(DWORDPtr((intptr_t)p)));
    return 4;
  case 6:
    genPush(((BYTE*)p)+4,4);
    genPush(p,4);
    return 8;
  case 8:
    genPush(((BYTE*)p)+4,4);
    genPush(p,4);
    return 8;
  case 10:
    genPush(((BYTE*)p)+8,4);
    genPush(p,8);
    return 12;
  default:
    size = getAlignedSize(size);
    const UINT count = size / 4;
    m_code->emitSubESP(size   );
    m_code->emit(MOV(ECX,count));
    m_code->emit(MOV(ESI,(intptr_t)p));
    m_code->emit(MOV(EDI,ESP  ));
    m_code->emit(REP(MOVSD    ));
    return size;
  }
}

int CodeGenerator::genPushRef(const void *p) {
  m_code->emit(PUSH((intptr_t)p));
  return sizeof(void*);
}

int CodeGenerator::genPushInt(int n) {
  m_code->emit(PUSH(n));
  return sizeof(int);
}

int CodeGenerator::genPushReturnAddr() {
#ifdef LONGDOUBLE
  return genPushRef(&m_tree.getValueRef(0));
#else
  return 0;
#endif
}

#else // IS64BIT

static const IndexRegister int64ParamRegister[] = {
#ifndef LONGDOUBLE
  RCX ,RDX ,R8 ,R9
#else
  RDX, R8, R9
#endif
};

#ifndef LONGDOUBLE

void CodeGenerator::genCall1Arg(const ExpressionNode *arg, BuiltInFunction1 f, const ExpressionDestination &dst) {
  genSetParameter(arg, 0, false);
  emitCall((BuiltInFunction)f, dst);
}

void CodeGenerator::genCall2Arg(const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunction2 f, const ExpressionDestination &dst) {
  const bool arg1HasCalls = arg1->containsFunctionCall();
  const bool arg2HasCalls = arg2->containsFunctionCall();
  if (!arg2HasCalls) {
    genSetParameter(arg1, 0, false);
    genSetParameter(arg2, 1, false);
  } else if (!arg1HasCalls) {
    genSetParameter(arg2, 1, false);
    genSetParameter(arg1, 0, false);
  } else { // both parmeters are expressions using function calls
    const BYTE offset = genSetParameter(arg1, 0, true);
    genSetParameter(arg2, 1, false);
    m_code->emitValToXMM(XMM0, offset);
    m_code->popTmp();
  }
  emitCall((BuiltInFunction)f, dst);
}

void CodeGenerator::genCall1Arg(const ExpressionNode *arg, BuiltInFunctionRef1 f, const ExpressionDestination &dst) {
  genSetRefParameter(arg, 0);
  emitCall((BuiltInFunction)f, dst);
}

void CodeGenerator::genCall2Arg(const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunctionRef2 f, const ExpressionDestination &dst) {
  const bool arg2HasCalls = arg2->containsFunctionCall();

  bool stacked1, stacked2;
  BYTE offset1, offset2;

  if(!arg2HasCalls) {
    offset1 = genSetRefParameter(arg1, 0, stacked1);
    offset2 = genSetRefParameter(arg2, 1, stacked2);
  } else {
    offset2 = genSetRefParameter(arg2, 1, stacked2);
    offset1 = genSetRefParameter(arg1, 0, stacked1);
  }
  if(stacked1) {
    m_code->emit(MOV(RCX, RSP    ));
    m_code->emit(ADD(RCX, offset1));
    m_code->popTmp();
  }
  if(stacked2) {
    m_code->emit(MOV(RDX, RSP    ));
    m_code->emit(ADD(RDX, offset2));
    m_code->popTmp();
  }
  emitCall((BuiltInFunction)f, dst);
}

#else // LONGDOUBLE

void CodeGenerator::genCall1Arg(const ExpressionNode *arg, BuiltInFunctionRef1 f, const ExpressionDestination &dst) {
  genSetRefParameter(arg, 0);
  emitCall((BuiltInFunction)f, dst);
}

void CodeGenerator::genCall2Arg(const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunctionRef2 f, const ExpressionDestination &dst) {
  const bool arg2HasCalls = arg2->containsFunctionCall();

  bool stacked1, stacked2;
  BYTE offset1, offset2;

  if(!arg2HasCalls) {
    offset1 = genSetRefParameter(arg1, 0, stacked1);
    offset2 = genSetRefParameter(arg2, 1, stacked2);
  } else {
    offset2 = genSetRefParameter(arg2, 1, stacked2);
    offset1 = genSetRefParameter(arg1, 0, stacked1);
  }
  if(stacked1) {
    m_code->emit(MOV(RDX, RSP));
    if(offset1) {
      m_code->emit(ADD(RDX, offset1));
      m_code->popTmp();
    }
  }
  if(stacked2) {
    m_code->emit(MOV(R8, RSP));
    if(offset2) {
      m_code->emit(ADD(R8,offset2));
      m_code->popTmp();
    }
  }
  emitCall((BuiltInFunction)f, dst);
}

#endif // LONGDOUBLE

void CodeGenerator::genPolynomial(const ExpressionNode *n, const ExpressionDestination &dst) {
  const ExpressionNodeArray &coefArray       = n->getCoefficientArray();
  const int                  firstCoefIndex  = n->getFirstCoefIndex();
  for(int i = 0; i < (int)coefArray.size(); i++) {
    const ExpressionNode *coef = coefArray[i];
    if(coef->isConstant()) {
      m_tree.getValueRef(firstCoefIndex + i) = m_tree.evaluateRealExpr(coef);
    } else {
      genExpression(coef, DST_INVALUETABLE(firstCoefIndex + i));
    }
  }
#ifndef LONGDOUBLE
  genSetParameter(n->getArgument(), 0, false);
#else
  genSetRefParameter(n->getArgument(), 0);
#endif // LONGDOUBLE

  const IndexRegister &param2    = int64ParamRegister[1];
  const size_t         coefCount = coefArray.size();
  m_code->emit(MOV(param2,coefCount));

  const IndexRegister &param3    = int64ParamRegister[2];
  const Real          *coef0     = &m_tree.getValueRef(firstCoefIndex);
  m_code->emit(MOV(param3,(intptr_t)coef0));

  emitCall((BuiltInFunction)::evaluatePolynomial, dst);
}

void CodeGenerator::emitCall(BuiltInFunction f, const ExpressionDestination &dst) {
  assert(m_hasCalls);
  m_code->emitCall(f,dst);
}

void CodeGenerator::genSetRefParameter(const ExpressionNode *n, int index) {
  bool stacked;
  const BYTE offset = genSetRefParameter(n, index, stacked);
  if(stacked) {
    const GPRegister &r64 = int64ParamRegister[index];
    m_code->emit(MOV(r64, RSP));
    m_code->emitAddR64(r64, offset);
    m_code->popTmp();
  }
}

BYTE CodeGenerator::genSetRefParameter(const ExpressionNode *n, int index, bool &savedOnStack) {
  const GPRegister &dstRegister = int64ParamRegister[index];
  if(n->isNameOrNumber()) {
    m_code->emit(MOV(dstRegister, RSI));
    m_code->emitAddR64(dstRegister, m_code->getESIOffset(n->getValueIndex()));
    savedOnStack = false;
    return 0;
  } else {
    const BYTE offset = m_code->pushTmp();
    genExpression(n, DST_ONSTACK(offset));
    savedOnStack = true;
    return offset;
  }
}

#ifndef LONGDOUBLE
BYTE CodeGenerator::genSetParameter(const ExpressionNode *n, int index, bool saveOnStack) {
  const XMMRegister &dstRegister = (index == 0) ? XMM0 : XMM1;
  if(n->isNameOrNumber()) {
    assert(saveOnStack == false);
    m_code->emitValToXMM(dstRegister,n->getValueIndex());
  } else if(saveOnStack) {
    const BYTE offset = m_code->pushTmp();
    genExpression(n, DST_ONSTACK(offset));
    return offset;
  } else {
    genExpression(n, DST_XMM(dstRegister));
  }
  return 0;
}

#endif // LONGDOUBLE

#endif // IS64BIT
