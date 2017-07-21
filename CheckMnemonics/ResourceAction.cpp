/* DO NOT EDIT THIS FILE - it is machine generated */
#line 114 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
#include "stdafx.h"
#include "ResourceParser.h"

#pragma warning(disable:4312)


#line 21 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"

int ResourceParser::reduceAction(unsigned int prod) {

  switch(prod) {
  case 0: /* start -> resourceFile */
#line 122 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_tree.setRoot(getStackTop(0)); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 2: /* resourceDefinitionList -> resourceDefinitionList resourceDefinition */
#line 128 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = (getStackTop(1)&&getStackTop(0))
                                                                                                   ? newNode(getPos(2), COMMA, getStackTop(1), getStackTop(0), NULL)
                                                                                                   : getStackTop(1)? getStackTop(1): getStackTop(0);
                                                                                            }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 4: /* resourceDefinition -> bitmapDefinition */
#line 135 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 5: /* resourceDefinition -> iconDefinition */
#line 136 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 6: /* resourceDefinition -> customTypeDefinition */
#line 137 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 9: /* resourceDefinition -> guideLinesDefinition */
#line 140 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 10: /* resourceDefinition -> toolbarDefnition */
#line 141 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 12: /* resourceDefinition -> dialogInitDefinition */
#line 143 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 14: /* resourceDefinition -> rcdataDefinition */
#line 145 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 16: /* resourceDefinition -> textIncludeDirective */
#line 147 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 18: /* resourceDefinition -> versionDefinition */
#line 149 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 19: /* resourceDefinition -> typelibDefinition */
#line 150 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 20: /* resourceDefinition -> afxDialogLayout */
#line 151 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 24: /* dialogDefinition -> resourceId DIALOG resourceAttributeList rectangleSpec dialogSpecList BEGIN opt_dialogControlSpecList END */
#line 166 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), dialogDefinition, getStackTop(7), getStackTop(1), getStackTop(3), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 25: /* extendedDialogDefinition -> resourceId DIALOGEX rectangleSpec extendedDialogSpecList BEGIN opt_dialogControlSpecList END */
#line 171 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), dialogDefinition, getStackTop(6), getStackTop(1), getStackTop(3), NULL);  }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 26: /* dialogSpecList -> dialogSpecList dialogSpec */
#line 174 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = (getStackTop(1)&&getStackTop(0))
                                                                                                   ? newNode(getPos(2), COMMA, getStackTop(1), getStackTop(0), NULL)
                                                                                                   : getStackTop(1)? getStackTop(1): getStackTop(0);
                                                                                            }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 28: /* dialogSpec -> STYLE dialogStyleExpr */
#line 181 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 29: /* dialogSpec -> CAPTION string */
#line 182 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 30: /* dialogSpec -> _MENU identifier */
#line 183 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), _MENU, getStackTop(0), NULL);            }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 31: /* dialogSpec -> _FONT fontSpec */
#line 184 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 32: /* extendedDialogSpecList -> extendedDialogSpecList extendedDialogSpec */
#line 188 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = (getStackTop(1)&&getStackTop(0))
                                                                                                   ? newNode(getPos(2), COMMA, getStackTop(1), getStackTop(0), NULL)
                                                                                                   : getStackTop(1)? getStackTop(1): getStackTop(0);
                                                                                            }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 34: /* extendedDialogSpec -> STYLE dialogStyleExpr */
#line 195 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 35: /* extendedDialogSpec -> EXSTYLE extendedDialogStyleExpr */
#line 196 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 36: /* extendedDialogSpec -> CAPTION string */
#line 197 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 37: /* extendedDialogSpec -> _MENU identifier */
#line 198 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), _MENU, getStackTop(0), NULL);            }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 38: /* extendedDialogSpec -> _FONT extendedFontSpec */
#line 199 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 49: /* opt_dialogControlSpecList -> epsilon */
#line 224 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(0), dialogControlSpecList, NULL    ); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 50: /* opt_dialogControlSpecList -> dialogControlSpecList */
#line 225 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), dialogControlSpecList, getStackTop(0), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 52: /* dialogControlSpecList -> dialogControlSpecList dialogControlSpec */
#line 229 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = (getStackTop(1)&&getStackTop(0))
                                                                                                 ? newNode(getPos(2), COMMA, getStackTop(1), getStackTop(0), NULL)
                                                                                                 : getStackTop(1)? getStackTop(1): getStackTop(0);
                                                                                            }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 55: /* dialogControlSpec -> editSpec */
#line 238 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 56: /* dialogControlSpec -> comboBoxSpec */
#line 239 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 57: /* dialogControlSpec -> listBoxSpec */
#line 240 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 58: /* dialogControlSpec -> iconSpec */
#line 241 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 59: /* dialogControlSpec -> scrollBarSpec */
#line 242 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 61: /* pushButtonSpec -> buttonType string COMMA identifier controlAttributeList */
#line 246 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), getStackTop(4)->getSymbol(), getStackTop(1), getStackTop(3), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 62: /* buttonType -> PUSHBUTTON */
#line 249 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), PUSHBUTTON   , NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 63: /* buttonType -> DEFPUSHBUTTON */
#line 250 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), DEFPUSHBUTTON, NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 64: /* buttonType -> GROUPBOX */
#line 251 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), GROUPBOX     , NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 65: /* lrcTextSpec -> lrcText string COMMA identifier controlAttributeList */
#line 254 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), LTEXT, getStackTop(1), getStackTop(3), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 74: /* controlSpec -> CONTROL controlId COMMA identifier COMMA string controlAttributeList */
#line 277 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), CONTROL, getStackTop(3), getStackTop(5), getStackTop(1), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 360: /* menuDefinition -> resourceId menuType resourceAttributeList menuBody */
#line 636 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), menuDefinition, getStackTop(3), getStackTop(0), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 363: /* menuBody -> BEGIN menuSpecList END */
#line 643 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = getStackTop(1); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 364: /* menuSpecList -> menuSpecList menuSpec */
#line 646 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = (getStackTop(1)&&getStackTop(0))
                                                                                                     ? newNode(getPos(2), COMMA, getStackTop(1), getStackTop(0), NULL)
                                                                                                     : getStackTop(1)? getStackTop(1): getStackTop(0);
                                                                                                }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 368: /* popupMenu -> POPUP string menuItemModifierList menuBody */
#line 657 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), POPUP, getStackTop(2), getStackTop(0), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 369: /* menuItem -> _MENUITEM string COMMA identifierOrNumber menuItemModifierList */
#line 660 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), _MENUITEM, getStackTop(3), getStackTop(1), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 370: /* menuItem -> _MENUITEM SEPARATOR */
#line 661 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = NULL; }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 408: /* acceleratorsDefinition -> resourceId ACCELERATORS resourceAttributeList BEGIN acceleratorList END */
#line 715 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), acceleratorsDefinition, getStackTop(5), getStackTop(1), NULL ); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 409: /* acceleratorList -> acceleratorList accelerator */
#line 718 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), COMMA, getStackTop(1), getStackTop(0), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 411: /* accelerator -> constant COMMA identifier opt_acceleratorSpecList */
#line 722 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), accelerator, getStackTop(1), getStackTop(3), getStackTop(0), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 413: /* opt_acceleratorSpecList -> COMMA acceleratorSpecList */
#line 726 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = getStackTop(0); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 415: /* acceleratorSpecList -> acceleratorSpecList COMMA acceleratorModifier */
#line 730 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(2), COMMA, getStackTop(2), getStackTop(0), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 416: /* acceleratorModifier -> VIRTKEY */
#line 733 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), VIRTKEY , NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 417: /* acceleratorModifier -> ASCII */
#line 734 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), ASCII   , NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 418: /* acceleratorModifier -> SHIFT */
#line 735 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), SHIFT   , NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 419: /* acceleratorModifier -> CONTROL */
#line 736 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), CONTROL , NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 420: /* acceleratorModifier -> ALT */
#line 737 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), ALT     , NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 421: /* acceleratorModifier -> NOINVERT */
#line 738 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), NOINVERT, NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 426: /* stringTableDefinition -> STRINGTABLE resourceAttributeList BEGIN stringSpecList END */
#line 749 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), stringTableDefinition, getStackTop(1), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 427: /* stringSpecList -> stringSpecList stringSpec */
#line 753 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), COMMA, getStackTop(1), getStackTop(0), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 429: /* stringSpec -> identifier string */
#line 757 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), stringSpec, getStackTop(1), getStackTop(0), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 433: /* languageDirective -> LANGUAGE constant COMMA constant */
#line 767 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode(getPos(1), languageDirective, getStackTop(2), getStackTop(0), NULL); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 457: /* name -> identifier */
#line 817 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode( getPos(1), STRING, getStackTop(0)->getName().cstr());                                 }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 458: /* name -> name DOT identifier */
#line 818 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode( getPos(1), STRING, String(getStackTop(2)->getString() + _T(".") + getStackTop(0)->getName()).cstr()); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 472: /* number -> NUMBER */
#line 848 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode( getPos(1), NUMBER, ttoi(getText()));       }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 473: /* identifier -> IDENTIFIER */
#line 851 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode( getPos(1), IDENTIFIER, getText());         }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 474: /* string -> STRING */
#line 854 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
    { m_dollardollar = newNode( getPos(1), STRING, stripQuotes(getText()).cstr()); }
#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  }
  return 0;
}

#line 860 "C:\\mytools2015\\CheckMnemonics\\Resource.y"
SyntaxNode *ResourceParser::newNode(const SourcePosition &pos, int symbol, ...) {
  va_list argptr;
  va_start(argptr,symbol);
  SyntaxNode *p = m_tree.vFetchNode(pos,symbol,argptr);
  va_end(argptr);
  return p;
}

void ResourceParser::verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) {
  m_tree.vAddError(&pos,format,argptr);
}

int ResourceParser::ttoi(const TCHAR *s) { // static
  int result;
  if(_tcsncicmp(s,_T("0x"),2) == 0) {
    _stscanf(s+2,_T("%x"), &result);
  } else {
    _stscanf(s, _T("%d"), &result);
  }
  return result;
}

String ResourceParser::stripQuotes(const String &s) { // static
  const intptr_t len = s.length();
  if((len >= 2) && (s[0]=='"') && (s[len-1] == '"')) {
    return substr(s, 1, len-2);
  } else {
    return s;
  }
}

#line 31 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
