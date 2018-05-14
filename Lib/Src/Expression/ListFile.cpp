#include "pch.h"
#include "ListFile.h"

namespace Expr {

// --------------------------------- ListFile functions ----------------------------

ListFile::ListFile(FILE *f, const ValueAddressCalculation &addressTable, const StringArray &commentArray, const IndexRegister &tableRefRegister) 
  : m_f(f)
  , m_addressTable(addressTable)
  , m_nameCommentArray(commentArray)
  , m_tableRefRegister(tableRefRegister)
{
}

ListFile::~ListFile() {
  flush();
  clear();
}

void ListFile::addLine(ListLine *l) {
  TRACE_NEW(l);
  if(hasFPUComment()) {
    l->setFPUComment(m_FPUComment);
    m_FPUComment = EMPTYSTRING;
  }
  m_lineArray.add(l);
  if(isTracing()) {
    _ftprintf(m_f, _T("%s\n"), l->toString().cstr()); fflush(m_f);
  }
}

void ListFile::adjustPositions(UINT pos, UINT bytesAdded) {
  const size_t n = m_lineArray.size();
  for(size_t i = 0; i < n; i++) {
    ListLine *l = m_lineArray[i];
    if(l->m_pos >= pos) l->m_pos += bytesAdded;
  }
}

void ListFile::clear() {
  const size_t n = m_lineArray.size();
  for(size_t i = 0; i < n; i++) {
    SAFEDELETE(m_lineArray[i]);
  }
  m_lineArray.clear();
}

static int listLineCmp(ListLine * const &l1, ListLine * const &l2) {
  int c = (int)l1->m_pos - (int)l2->m_pos;
  if(c) return c;
  return ordinal(l2->m_isLabel) - ordinal(l1->m_isLabel);
}

void ListFile::flush() {
  if(!isOpen()) return;
  if(isTracing()) {
    _ftprintf(m_f,_T("----------------------------------------------------\n"));
  }
  m_lineArray.sort(listLineCmp);
  const size_t n = m_lineArray.size();
  for(size_t i = 0; i < n; i++) {
    const ListLine *l = m_lineArray[i];
    _ftprintf(m_f, _T("%s\n"), l->toString().cstr());
  }
}

void ListFile::vprintf(const TCHAR *format, va_list argptr) const {
  _vftprintf(m_f, format, argptr);
  fflush(m_f);
}

const TCHAR *ListFile::findArgComment(const InstructionOperand &arg) const {
  if(arg.isMemoryRef()) {
    const MemoryRef &mr = arg.getMemoryReference();
    if(mr.hasBase() && (mr.getBase() == &m_tableRefRegister)) {
      const UINT index = m_addressTable.esiOffsetToIndex(mr.getOffset());
      if(index < m_nameCommentArray.size()) {
        return m_nameCommentArray[index].cstr();
      }
    }
  } else if(arg.isImmediateValue() && (arg.getSize() == REGSIZE_DWORD)) {
    const size_t index = m_addressTable.realRefToIndex((Real*)(size_t)arg.getImmUint32());
    if(index < m_nameCommentArray.size()) {
      return m_nameCommentArray[index].cstr();
    }
  }
  return NULL;
}

ListLine *ListFile::findLineByPos(UINT pos) {
  const size_t n = m_lineArray.size();
  for(size_t i = 0; i < n; i++) {
    ListLine *line = m_lineArray[i];
    if(line->m_pos == pos) {
      return line;
    }
  }
  return NULL;
}

// --------------------------------------------- ListLines ----------------------------


String ListLine::formatIns(const InstructionBase &ins) { // static
  return format(_T("%-*s"),LF_INSLEN, ins.toString().cstr());
}

String ListLine::formatOp(const OpcodeBase &opcode) { // static
  return format(_T("%-*s")
               ,LF_MNELEN, opcode.getMnemonic().cstr()
               );
}

String ListLine::formatOp(const OpcodeBase &opcode, const InstructionOperand *arg) { // static
  return format(_T("%-*s %s")
               ,LF_MNELEN, opcode.getMnemonic().cstr()
               ,arg->toString().cstr()
               );
}

String ListLine::formatOp(const OpcodeBase &opcode, const InstructionOperand *arg1, const InstructionOperand *arg2) { // static
  return format(_T("%-*s %s, %s")
               ,LF_MNELEN, opcode.getMnemonic().cstr()
               ,arg1->toString().cstr()
               ,arg2->toString().cstr()
               );
}

String ListLine::formatOp(const StringPrefix &prefix, const StringInstruction &strins) { // static
  return format(_T("%-*s %s")
               ,LF_MNELEN, prefix.getMnemonic().cstr()
               ,strins.getMnemonic().cstr()
               );
}

String ListLine::formatOpAndComment(const String &opstr, const TCHAR *comment) { // static
  if(!comment) comment = EMPTYSTRING;
  return format(_T("%-*s; %-*s"),LF_OPSLEN, opstr.cstr(), LF_COMLEN, comment);
}



String ListLine::toString() const {
  return format(_T("%*s%-*d: ")
               ,LF_MARGIN, _T("")
               ,LF_POSLEN, m_pos);
}

ListLine0Arg::ListLine0Arg(UINT pos, const Opcode0Arg &opcode)
  : ListLine(pos)
  , m_opcode0(opcode)
{
}

String ListLine0Arg::toString() const {
  return __super::toString()
       + formatIns(m_opcode0)
       + formatOpAndComment(formatOp(m_opcode0))
       + getFPUComment();
}


ListLine1Arg::ListLine1Arg(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg, const TCHAR *nameComment)
  : ListLine(pos)
  , m_opcode(opcode)
  , m_arg(arg.clone())
  , m_nameComment(nameComment)
{
  TRACE_NEW(m_arg);
}
ListLine1Arg::~ListLine1Arg() {
  SAFEDELETE(m_arg);
}
String ListLine1Arg::toString() const {
  return __super::toString()
       + formatIns(m_opcode(*m_arg))
       + formatOpAndComment(formatOp(m_opcode,m_arg), m_nameComment)
       + getFPUComment();
}


ListLine2Arg::ListLine2Arg(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2, const TCHAR *nameComment)
  : ListLine(pos)
  , m_opcode(opcode)
  , m_arg1(arg1.clone())
  , m_arg2(arg2.clone())
  , m_nameComment(nameComment)
{
  TRACE_NEW(m_arg1);
  TRACE_NEW(m_arg2);
}

ListLine2Arg::~ListLine2Arg() {
  SAFEDELETE(m_arg1);
  SAFEDELETE(m_arg2);
}
String ListLine2Arg::toString() const {
  return __super::toString()
       + formatIns(m_opcode(*m_arg1,*m_arg2))
       + formatOpAndComment(formatOp(m_opcode,m_arg1,m_arg2), m_nameComment)
       + getFPUComment();
}


ListLineStringOp::ListLineStringOp(UINT pos, const StringPrefix &prefix, const StringInstruction &strins)
  : ListLine(pos)
  , m_prefix(prefix), m_strins(strins)
{
}
String ListLineStringOp::toString() const {
  return __super::toString()
       + formatIns(m_prefix(m_strins))
       + formatOp(m_prefix, m_strins);
}


ListLineJump::ListLineJump(UINT pos, const OpcodeBase &opcode, int iprel, CodeLabel label)
  : ListLine(pos)
  , m_opcode(opcode)
  , m_label(label)
  , m_iprel(iprel)
{
}
String ListLineJump::toString() const {
  return __super::toString()
       + formatIns(m_opcode(m_iprel))
       + formatOpAndComment(format(_T("%s %s")
                                  ,formatOp(m_opcode).cstr()
                                  ,formatHexValue(m_iprel,false).cstr()
                                  )
                           ,labelToString(m_label).cstr()
                           );
}



ListLineCall::ListLineCall(UINT pos, const OpcodeCall &opcode, const InstructionOperand &arg, const FunctionCall &fc)
  : ListLine(pos)
  , m_opcode(opcode)
  , m_arg(arg.clone())
  , m_fc(fc)
{
  TRACE_NEW(m_arg);
}
ListLineCall::~ListLineCall() {
  SAFEDELETE(m_arg);
}
String ListLineCall::toString() const {
  return __super::toString()
       + formatIns(m_opcode(*m_arg))
       + formatOpAndComment(formatOp(m_opcode,m_arg)
                           ,m_fc.m_signature.cstr()
                           );
}


ListLineLabel::ListLineLabel(UINT pos, CodeLabel label)
  : ListLine(pos, true)
  , m_label(label)
{
}
String ListLineLabel::toString() const {
  return format(_T("%s:"), labelToString(m_label).cstr());
}


ListLineFuncAddr::ListLineFuncAddr(UINT pos, UINT rbxOffset, const FunctionCall &fc)
  : ListLine(pos)
  , m_rbxOffset(rbxOffset)
  , m_fc(fc)
{
}
String ListLineFuncAddr::toString() const {
  return __super::toString()
       + format(_T("[%-#0*x] %s (%s)")
               ,LF_POSLEN, m_rbxOffset
               ,formatHexValue((UINT64)m_fc.m_fp).cstr()
               ,m_fc.m_signature.cstr());
}

}; // namespace Expr
