#include "stdafx.h"
#include "TimtalkDlg.h"
#include "LixDlg.h"
#include <Random.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
    CAboutDlg();
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CTimtalkDlg::CTimtalkDlg(CWnd *pParent) : CDialog(CTimtalkDlg::IDD, pParent) {
    m_sentence = EMPTYSTRING;
    m_hIcon    = theApp.LoadIcon(IDR_MAINFRAME);
}

void CTimtalkDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITTEXT, m_sentence);
}

BEGIN_MESSAGE_MAP(CTimtalkDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTONSENTENCE, OnButtonsentence)
    ON_COMMAND(ID_FILE_QUIT, OnFileQuit)
    ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
    ON_COMMAND(ID_EDIT_LIX, OnEditLix)
END_MESSAGE_MAP()


BOOL CTimtalkDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  randomize();
  m_lix       = 75;
  m_lixfilter = FALSE;
  GetDlgItem(IDC_BUTTONSENTENCE)->SetFocus();

  return FALSE;
}

void CTimtalkDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CTimtalkDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
  }
}

HCURSOR CTimtalkDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CTimtalkDlg::OnCancel() {
  __super::OnCancel();
}

class Text {
public:
  const char *m_text;
  bool        m_used;
  Text(const char *text) : m_text(text) {
    m_used = false;
  }
};

static Text modalVerbs[] = {
  "b�r"
 ,"kan"
 ,"m�"
 ,"skal"
 ,"vil"
};

static Text verbs1[] = {
  "er"
 ,"har"
};

static Text verbs2[] = {
  "adressere"
 ,"ber�re"
 ,"beslutte"
 ,"betyde"
 ,"delagtigg�re"
 ,"etablere"
 ,"fokusere"
 ,"forberede"
 ,"forenkle"
 ,"fors�ge"
 ,"fusionere"
 ,"h�ndtere"
 ,"iv�rks�tte"
 ,"konkretisere"
 ,"kr�ve"
 ,"nedl�gge"
 ,"n�"
 ,"oparbejde"
 ,"opbygge"
 ,"placere"
 ,"realisere"
 ,"reducere"
 ,"samarbejde"
 ,"sikre"
 ,"simplificere"
 ,"slutte"
 ,"styrke"
 ,"tilbyde"
 ,"�ndre"
};

static Text subjects[] = {
  "Cap Gemini"
 ,"den finansielle sektor"
 ,"den offentlige sektor"
 ,"Finn Peder Hove"
 ,"Henrik S�by"
 ,"I"
 ,"Ilse Kristiansen"
 ,"Jan H�jlund Nielsen"
 ,"jeg"
 ,"Jens Markussen"
 ,"konkurrenterne"
 ,"Lars Klausen"
 ,"Lene Schmidt"
 ,"Lone Skjoldborg"
 ,"Michael Bloch"
 ,"Morten Rye M�ller"
 ,"Niels Ove Kyvsgaard"
 ,"Ole Bach"
 ,"Per Lykkegaard Hansen"
 ,"Sten Kr�ger Poulsen"
 ,"TMN"
 ,"vi"
};

static Text objects[] = {
  "ADC support-teamet"
 ,"danske virksomheder"
 ,"de resterende opgaver"
 ,"de �vrige nordiske lande"
 ,"det overordnede m�l"
 ,"det rette fundament"
 ,"det rette kompetence-mix"
 ,"det sv�re projekt- og konsulentmarked"
 ,"dette marked"
 ,"disse tiltag"
 ,"en bedre struktur"
 ,"en forh�jet oms�tning"
 ,"en h�rd konkurrence"
 ,"en h�jere timepris"
 ,"en m�ls�tning"
 ,"en omverden"
 ,"en r�kke strukturelle og produktivitetsm�ssige forhold"
 ,"en sovepude"
 ,"enhver forandringsopgave"
 ,"et enormt potentiale"
 ,"et godt netv�rk"
 ,"et nyt salgsteam"
 ,"et par vigtige tiltag"
 ,"et slagkraftigt, mindre kerneteam"
 ,"et st�rkt salgs-setup"
 ,"et st�rre overskud"
 ,"forretningsprocesserne hos kunden"
 ,"Fredagsflash"
 ,"gode arkitekturydelser"
 ,"kollegerne"
 ,"kreative deals"
 ,"mange forretningsm�ssige muligheder"
 ,"revitaliseringsprocessen"
 ,"SAS-enheden"
 ,"Tims Hj�rne p� intranettet"
 ,"to nye enheder i TS"
 ,"vor indsats"
 ,"vores 2013 strategiproces"
 ,"vores gode �konomi"
 ,"vores handlingsplan"
 ,"vores medarbejderforhold og �konomi"
 ,"vores meget st�rke position"
 ,"vores nuv�rende PRM director"
 ,"vores opstilling i CS"
 ,"vores opstilling"
 ,"vores produktionsomkostninger"
 ,"vores produktivitet"
 ,"vores produktportef�lje"
 ,"vores support omkostninger"
};

static Text sentenceStart[] = {
  "Der arbejdes derfor p� h�jtryk alle steder for, at"
 ,"Derfor vil I selvf�lgelig kunne regne med, at"
 ,"Derudover er det ideen, at"
 ,"Det er derfor besluttet, at"
 ,"Det overordnede m�l med disse forskellige tiltag er selvf�lgelig, at"
 ,"Dette b�r kunne sikre, at"
 ,"Dette er samtidig foruds�tningen for, at"
 ,"Dette g�res naturligvis ud fra en betragtning om, at"
 ,"Dette initiativ skal fortrinsvis fokusere p�, at"
 ,"Dette kan sikre, at"
 ,"Dette m� ikke betyde, at"
 ,"Dette vil betyde, at"
 ,"F.eks. har jeg selv sammen med Anders Sj�kvist ansvaret for, at"
 ,"Form�let med SAS er, at"
 ,"I Danmark v�lger vi i f�rste omgang, at"
 ,"I den nordiske ledelse har vi derfor besluttet at"
 ,"I denne sammenh�ng b�r det ogs� n�vnes, at"
 ,"Ideen med PTM teamet er, at"
 ,"Internationalt er der ogs� et �nske om, at"
 ,"Jeg kan derfor ikke garantere, at"
 ,"Jeg vender i l�bet af de n�ste par uger tilbage med forslag til, at"
 ,"Jeg vender snarest tilbage med forslag til, at"
 ,"Jeg vil derfor fors�ge, at"
 ,"Samtidigt kan vi tydeligt se, at"
 ,"Som et led i ovenn�vnte proces har vi naturligvis i lighed med de �vrige nordiske lande besluttet, at"
 ,"Tag endelig fat p� jeres disciplinchef eller teamchef for, at"
 ,"Vi kan derfor forvente, at"
 ,"Vi m� derfor regne med, at"
};

static const String getUnusedRandomString(Text *t, int size) {
  for(int i = 0; i < 10; i++) { // to prevent loop, in case all are used
    Text &r = t[randInt()%size];
    if(!r.m_used) {
      r.m_used = true;
      return r.m_text;
    }
  }
  return t[randInt()%size].m_text;
}

static void setAllUnused(Text *t, int size) {
  for(int i = 0; i < size; i++) {
    t[i].m_used = false;
  }
}

#define RANDOMTEXT(  a) getUnusedRandomString(a, ARRAYSIZE(a)).cstr()
#define SETALLUNUSED(a) setAllUnused(         a, ARRAYSIZE(a))

#define randomModalVerb()      RANDOMTEXT(modalVerbs   )
#define randomVerb1()          RANDOMTEXT(verbs1       )
#define randomVerb2()          RANDOMTEXT(verbs2       )
#define randomSubject()        RANDOMTEXT(subjects     )
#define randomObject()         RANDOMTEXT(objects      )
#define randomSentenceStart()  RANDOMTEXT(sentenceStart)

static void setAllUnused() {
  SETALLUNUSED(subjects     );
  SETALLUNUSED(modalVerbs   );
  SETALLUNUSED(verbs1       );
  SETALLUNUSED(verbs2       );
  SETALLUNUSED(objects      );
  SETALLUNUSED(sentenceStart);
}

static String randomVerb(bool notFirst) {
  int  r      =  randInt() % 10;
  bool useNot = (randInt() % 10) > 8;
  if(r > 7) {
    if(useNot) {
      return notFirst ? format(_T("ikke %s %s"), randomModalVerb(), randomVerb2()) : format(_T("%s ikke %s"), randomModalVerb(), randomVerb2());
    } else {
      return format(_T("%s %s"), randomModalVerb(), randomVerb2());
    }
  } else if(r > 4) {
    if(useNot) {
      return notFirst ? format(_T("ikke %s"), randomVerb1()) : format(_T("%s ikke"), randomVerb1());
    } else {
      return format(_T("%s"), randomVerb1());
    }
  } else {
    if(useNot) {
      return notFirst ? format(_T("ikke %sr"), randomVerb2()) : format(_T("%sr ikke"), randomVerb2());
    } else {
      return format(_T("%sr"), randomVerb2());
    }
  }
}

static const String somDer() {
  return (randInt() % 2 == 1) ? _T("som") : _T("der");
}

static String randomBiSentence() {
  return format(_T("%s %s %s"), somDer().cstr(), randomVerb(true).cstr(), randomObject());
}

static String randomSentence1(bool notFirst) {
  const String verb = randomVerb(notFirst);
  const int    r    = randInt() % 10;
  if(r > 5) {
    return format(_T("%s %s %s"), randomSubject(), verb.cstr(), randomObject());
  } else if(r > 2) {
    return format(_T("%s, %s, %s %s"), randomSubject(), randomBiSentence().cstr(), verb.cstr(), randomObject());
  } else {
    return format(_T("%s %s %s, %s"),  randomSubject(), verb.cstr(), randomObject(), randomBiSentence().cstr());
  }
}

static String randomSentence() {
  setAllUnused();
  String s1 = randomSentence1(false);
  if(islower(s1[0])) {
    s1[0] = toupper(s1[0]);
  }
  const String s2 = randomSentence1(true);
  if((randInt() % 10) > 4) {
    return format(_T("%s. %s %s og at %s."), s1.cstr(), randomSentenceStart(), s2.cstr(), randomSentence1(true).cstr());
  } else {
    return format(_T("%s. %s %s."), s1.cstr(), randomSentenceStart(), s2.cstr());
  }
}

void CTimtalkDlg::OnButtonsentence() {
  String s;
  if(m_lixfilter) {
    do {
      s = randomSentence();
    } while(abs((long)(s.length() - m_lix)) > 50);
  } else {
    s = randomSentence();
  }

  m_sentence = s.cstr();
  UpdateData(false);
}

void CTimtalkDlg::OnFileQuit() {
  EndDialog(IDOK);
}

void CTimtalkDlg::OnHelpAbout() {
  CAboutDlg().DoModal();
}

void CTimtalkDlg::OnEditLix() {
  CLixDlg dlg(m_lix,m_lixfilter);
  if(dlg.DoModal() == IDOK) {
    m_lix       = dlg.m_lix;
    m_lixfilter = dlg.m_lixfilter;
  }
}
