#pragma once

#include <FastSemaphore.h>

class PropertyDialog;
class CPropertyDialogThread : public CWinThread {
private:
  PropertyDialog         *m_dlg;
  bool                    m_setVisibleBusy : 1;
  bool                    m_inModalLoop    : 1;
  bool                    m_killed         : 1;
  mutable FastSemaphore   m_lock;
  void noDialogException(const TCHAR *method) const;
  // no lock-protection
  inline bool isDialogVisible1() const {
    return m_dlg && m_inModalLoop && m_dlg->isVisible();
  }

  DECLARE_DYNCREATE(CPropertyDialogThread)
protected:
  CPropertyDialogThread();
  virtual ~CPropertyDialogThread();

  DECLARE_MESSAGE_MAP()
public:
  bool setCurrentDialogProperty(const void *v, size_t size);
  const void *getCurrentDialogProperty(size_t size) const;
  void reposition();
  void setDialogVisible(bool visible);
  bool isDialogVisible() const;
  void kill();
  PropertyContainer *getPropertyContainer();
  int getPropertyId() const;
  String getPropertyTypeName() const;
  inline int getPropertyIdOffset(int propertyId) const {
    return propertyId - getPropertyId();
  }
  virtual BOOL InitInstance();
  virtual int ExitInstance();
  static CPropertyDialogThread *startThread(PropertyDialog *dlg);
};
