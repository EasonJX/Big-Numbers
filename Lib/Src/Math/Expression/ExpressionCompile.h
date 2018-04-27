#pragma once
#include "CodeGeneration.h"

#ifndef LONGDOUBLE
#define RealPtr QWORDPtr
#else  // LONGDOUBLE
#define RealPtr TBYTEPtr
#endif // LONGDOUBLE

#ifdef IS32BIT
#define TABLEREF_REG ESI
#define STACK_REG    ESP
#else  // IS64BIT
#define TABLEREF_REG RSI
#define STACK_REG    RSP
#endif // IS64BIT

#ifdef IS64BIT
extern "C" {
  void callRealResultExpression(ExpressionEntryPoint ep, const void *rsiValue, Real &result);
  int  callIntResultExpression( ExpressionEntryPoint ep, const void *rsiValue);
};
#endif // IS64BIT

class MachineCode : public ExecutableByteArray {
private:
  DECLARECLASSNAME;
  ExpressionEntryPoint              m_entryPoint;
  void                             *m_esi;
  CompactArray<JumpFixup>           m_jumpFixups;
  Array<FunctionCallInfo>           m_callTable;
  bool                              m_callsGenerated;
  // Reference to first element in ParserTree::m_valueTable
  const CompactRealArray           &m_valueTable;
  // Offset in bytes, of esi/rsi from m_valueTable[0], when code is executing. 0 <= m_esiOffset < 128
  char                              m_esiOffset;
  FILE                             *m_listFile;
  int                               m_lastCodeSize;
  StringArray                       m_valueStr;     // for comments in listfile
  String                            m_insStr;
  const TCHAR                      *m_listComment;

#ifdef IS64BIT
  BYTE                              m_stackTop;
  int                               m_loadRBXInsPos;
  BYTE                              m_loadRBXInsSize;
  int                               m_codeSize;
  typedef CompactKeyType<BuiltInFunction> FunctionKey;
  CompactHashMap<FunctionKey, int>  m_fpMap; // value is index into m_uniqueFunctionCall, not offset
  Array<FunctionCall>               m_uniqueFunctionCall;
  int getFunctionRefIndex(const FunctionCall &fc);
#endif // IS64BIT

  void changeShortJumpToNearJump(JumpFixup &jf);
  void finalJumpFixup();
  inline int getESIOffset(size_t valueIndex) const {
    if(valueIndex >= getValueCount()) {
      throwInvalidArgumentException(__TFUNCTION__, _T("valueIndex=%zu. #values=%zu"), valueIndex, getValueCount());
    }
    return (int)valueIndex * sizeof(Real) - m_esiOffset;
  }
  inline size_t esiOffsetToIndex(int offset) const {
    return (offset + m_esiOffset) / sizeof(Real);
  }

  void clearFunctionCalls() {
    m_callTable.clear();
#ifdef IS64BIT
    m_fpMap.clear();
    m_uniqueFunctionCall.clear();
#endif
  }

#ifdef IS32BIT
  void linkFunctionCall(const FunctionCallInfo &call);
#else // IS64BIT
  void emit(int offset, const FunctionCall &fc);
#endif // IS64BIT
  void linkFunctionCalls();
  void adjustFunctionCalls(int pos, int bytesAdded);
  int emitIns(const InstructionBase &ins);
  void initValueStr(const ExpressionVariableArray &variables);
  void clearValueStr();
  // return NULL if not comment found
  const TCHAR *findListComment(const InstructionOperand &op) const;
  void listIns(const TCHAR *format,...);
  void listFixupTable() const;
  void listCallTable() const;
  MachineCode(const MachineCode &src);             // not implemented
  MachineCode &operator=(const MachineCode &src);  // not implemented
public:
  MachineCode(ParserTree &m_tree, FILE *listFile = NULL);
  ~MachineCode();
  void clear();
  int  addBytes(const void *bytes, int count);
  int  emit(const Opcode0Arg &opCode);
  int  emit(const OpcodeBase &opCode, const InstructionOperand &op);
  int  emit(const OpcodeBase &opCode, const InstructionOperand &op1, const InstructionOperand &op2);
  int  emit(const StringPrefix &prefix, const StringInstruction &strins);
#ifdef IS32BIT
  void emitCall(const FunctionCall &fc);
#else // IS64BIT
private:
  void emitCall(const FunctionCall &fc);
public:
  void emitCall(const FunctionCall &fc, const ExpressionDestination &dst);
#endif // IS64BIT
  inline void emitFSTP(const MemoryRef &mem) {
    emit(FSTP, RealPtr(mem));
  }
  inline void emitFLD(const MemoryRef &mem) {
    emit(FLD, RealPtr(mem));
  }
  void emitFLD(const ExpressionNode *n);
  inline MemoryRef getTableRef(int index) const {
    return TABLEREF_REG + getESIOffset(index);
  }
  inline MemoryRef getStackRef(int offset) const {
    return STACK_REG + offset;
  }
#ifndef LONGDOUBLE
  inline void emitFPUOpVal(const OpcodeBase &op, const ExpressionNode *n) {
    emit(op, RealPtr(getTableRef(n->getValueIndex())));
  }
#endif

#ifdef IS64BIT
  bool emitFLoad(const ExpressionNode *n, const ExpressionDestination &dst);
#ifndef LONGDOUBLE
  inline void emitXMMToMem(const XMMRegister &reg, const MemoryRef &mem) {
    emit(MOVSD1,MMWORDPtr(mem), reg);
  }
  inline void emitMemToXMM(const XMMRegister &reg, const MemoryRef &mem) {
    emit(MOVSD1,reg,MMWORDPtr(mem));
  }
#endif // !LONGDOUBLE
#endif // IS64BIT

  // if offset==0, emit(MOV dst,src); else emit(LEA dst,RealPtr(src+offset));
  void emitLoadAddr(const IndexRegister &dst, const MemoryRef &ref);
  // Return index in m_jumpFixups of new jump-instruction
  int emitJmp(const OpcodeBase &op, CodeLabel label);
  void fixupJump(int index, int jmpTo);
  void fixupJumps(const JumpList &list, bool b);
  void clearJumpTable() {
    m_jumpFixups.clear();
  }
  void setValueCount(size_t valueCount);
  inline size_t getValueCount() const {
    return m_valueTable.size();
  }
  inline void emitAddReg(const GPRegister &reg, int  value) {
    if(value == 0) return;
    emit(ADD,reg,value);
  }
  inline void emitSubReg(const GPRegister &reg, int value) {
    if(value == 0) return;
    emit(SUB,reg,value);
  }
  inline void emitAddStack(int n) {
    emitAddReg(STACK_REG,n);
  }
  inline void emitSubStack(int n) {
    emitSubReg(STACK_REG,n);
  }
#ifdef IS64BIT
  inline void resetStack(BYTE startOffset) {
    m_stackTop = startOffset;
  }
  inline BYTE pushTmp() {
    const BYTE offset = m_stackTop;
    m_stackTop += sizeof(Real);
    return offset;
  }
  inline BYTE popTmp() {
    m_stackTop -= sizeof(Real);
    return m_stackTop;
  }
  void emitLoadRBX();
  void fixupLoadRBX();
#endif // IS64BIT

  inline bool isCallsGenerated() const {
    return m_callsGenerated;
  }
  void finalize();
  void list(const TCHAR *format,...) const;
  inline void listLabel(CodeLabel label) {
    if(hasListFile()) list(_T("%s:\n"), labelToString(label).cstr());
  }
  inline bool hasListFile() const {
    return m_listFile != NULL;
  }
  inline Real evaluateReal() const {
    Real result;
#ifdef IS32BIT
    const ExpressionEntryPoint ep    = m_entryPoint;
    const void                *daddr = m_esi;
    __asm {
      push esi
      mov  esi, daddr
      call ep
      pop  esi
      fstp result;
    }
#else // IS64BIT
    callRealResultExpression(m_entryPoint, m_esi, result);
#endif // IS64BIT
    return result;
  }

  inline bool evaluateBool() const {
#ifdef IS32BIT
    const ExpressionEntryPoint ep    = m_entryPoint;
    const void                *daddr = m_esi;
    int result;
    __asm {
      push esi
      mov  esi, daddr
      call ep
      pop  esi
      mov result, eax
    }
    return result ? true : false;
#else  // IS64BIT
    return callIntResultExpression(m_entryPoint, m_esi) ? true : false;
#endif // IS64BIT
  }
};

class CodeGenerator {
private:
  ParserTree                     &m_tree;
  const TrigonometricMode         m_trigonometricMode;
  MachineCode                    *m_code;
  int                             m_nextLbl;
#ifdef IS64BIT
  bool                            m_hasCalls;
#endif // IS64BIT

  inline TrigonometricMode getTrigonometricMode() const {
    return m_trigonometricMode;
  }

  inline MemoryRef getTableRef(const ExpressionNode *n) {
    return m_code->getTableRef(n->getValueIndex());
  }

  // Code generation (compile to machinecode)
  void genMachineCode();
  void genProlog();
  void genEpilog();
  void genStatementList(       const ExpressionNode *n);
  void genReturnBoolExpression(const ExpressionNode *n);
  void genExpression(          const ExpressionNode *n DCL_DSTPARAM);
  void genCall1Arg(            const ExpressionNode *arg                             , BuiltInFunction1    f, const String &name DCL_DSTPARAM);
  void genCall1Arg(            const ExpressionNode *arg                             , BuiltInFunctionRef1 f, const String &name DCL_DSTPARAM);
  void genCall2Arg(            const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunction2    f, const String &name DCL_DSTPARAM);
  void genCall2Arg(            const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunctionRef2 f, const String &name DCL_DSTPARAM);

#ifdef IS64BIT
#ifdef LONGDOUBLE
#define ALLARGS_BYREF
#endif
#endif

  void genCall(const ExpressionNode *n, BuiltInFunctionRef1 f, const String &name DCL_DSTPARAM) {
    genCall1Arg(n->left(), f, name DST_PARAM);
  }
  void genCall(const ExpressionNode *n, BuiltInFunctionRef2 f, const String &name DCL_DSTPARAM) {
    genCall2Arg(n->left(), n->right(), f, name DST_PARAM);
  }
#ifndef ALLARGS_BYREF
  void genCall(const ExpressionNode *n, BuiltInFunction1 f, const String &name DCL_DSTPARAM) {
    genCall1Arg(n->left(), f, name DST_PARAM);
  }
  void genCall(const ExpressionNode *n, BuiltInFunction2 f, const String &name DCL_DSTPARAM) {
    genCall2Arg(n->left(), n->right(), f, name DST_PARAM);
  }
#else // ALLARGS_BYREF
  void genCall(const ExpressionNode *n, BuiltInFunction1 f, const String &name DCL_DSTPARAM) {
    genCall(n, (BuiltInFunctionRef1)f, name DST_PARAM);
  }
  void genCall(const ExpressionNode *n, BuiltInFunction2 f, const String &name DCL_DSTPARAM) {
    genCall(n, (BuiltInFunctionRef2)f, name DST_PARAM);
  }
#endif // ALLARGS_BYREF

  void     genPolynomial(       const ExpressionNode *n DCL_DSTPARAM);
  void     genIf(               const ExpressionNode *n DCL_DSTPARAM);
  void     genPowMultSequence(  UINT y);
  void     genCall(             const FunctionCall  &fc DCL_DSTPARAM);

#ifdef IS32BIT
  int      genPush(             const ExpressionNode *n);
  int      genPushRef(          const ExpressionNode *n, int index);
  int      genPushReal(         const Real           &x);
  int      genPushValue(        const ExpressionNode *n); // assume n is name/number
  int      genPushReturnAddr();
  int      genPushInt(int n);
  int      genPush(             const void           *p, UINT size); // return size
  int      genPushRef(          const void           *p);
#else // IS64BIT
  void     genSetParameter(     const ExpressionNode *n, int index);
  void     genSetRefParameter(  const ExpressionNode *n, int index);
  BYTE     genSetRefParameter(  const ExpressionNode *n, int index, bool &savedOnStack);
#endif // IS64BIT

  void     genAssignment(       const ExpressionNode *n);
  void     genIndexedExpression(const ExpressionNode *n);
  void     genBoolExpression(   const ExpressionNode *n, JumpList &jumpList, bool trueAtEnd);
  inline CodeLabel nextLabel() {
    return m_nextLbl++;
  }
  inline CodeLabelPair getLabelPair() {
    return CodeLabelPair(nextLabel(), nextLabel());
  }
  void throwInvalidTrigonometricMode();

public:
  // Allocates a pointer to MachineCode with new, and generate instructions from parserTree to
  // be executed. The allocated pointer can be retrieved by getCode() and should be released with
  // delete after usage. No exceptions should be thrown, as syntax-checks has been done, when
  // building the parsertree/symbol-table, but in case of an internal error, m_code will
  // automaticly be deleted.
  CodeGenerator(ParserTree *tree, TrigonometricMode trigonometricMode, FILE *listFile = NULL);
  const MachineCode *getCode() const {
    return m_code;
  }
};
