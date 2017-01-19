#include "stdafx.h"
#include <HashMap.h>
#include "DrawTool.h"

class BitmapKey {
public:
  int m_id, m_theta; // theta in degrees
  BitmapKey() : m_id(0), m_theta(0) {
  };
  BitmapKey(int id, int theta) : m_id(id), m_theta(theta) {
  };
  bool operator==(const BitmapKey &key) const {
    return (m_id == key.m_id) && (m_theta == key.m_theta);
  }
  unsigned long hashCode() const {
    return 360 * m_id + m_theta;
  }
};

class BitmapCache : public CompactHashMap<BitmapKey,CBitmap*> {
private:
  HWND              m_hwnd;
  LPDIRECT3DDEVICE9 m_device;

  D3DPRESENT_PARAMETERS getPresentParameters() const;
  void initDevice();
  void releaseDevice();
public:
  LPDIRECT3DDEVICE9 getDevice() {
    return m_device;
  }
  BitmapCache(HWND hwnd);
  ~BitmapCache();
};

static BitmapCache *bitmapCache = NULL;

void createBitmapCache(HWND hwnd) {
  if(bitmapCache != NULL) {
    destroyBitmapCache();
  }
  bitmapCache = new BitmapCache(hwnd);
}

void destroyBitmapCache() {
  delete bitmapCache;
  bitmapCache = NULL;
}

static int MarkerBitmaps[] = {
  IDB_BLACKSQUAREBITMAP
 ,IDB_LEFTRIGHTARROWBITMAP
 ,IDB_ROTATECORNERBITMAP
 ,IDB_CIRCLEARROWBITMAP
};

BitmapCache::BitmapCache(HWND hwnd) : m_hwnd(hwnd) {
  m_device = NULL;
  initDevice();

  for(int i = 0; i < ARRAYSIZE(MarkerBitmaps); i++) {
    int id = MarkerBitmaps[i];
    CBitmap *b = new CBitmap();
    if(b->LoadBitmap(id) == 0) {
      delete b;
      throwException(_T("LoadBitmap(%d) failed"), id);
    }
    put(BitmapKey(id, 0), b);
  }
}

BitmapCache::~BitmapCache() {
  releaseDevice();

  for(Iterator<Entry<BitmapKey, CBitmap*> > it = getEntryIterator(); it.hasNext();) {
    Entry<BitmapKey, CBitmap*> &e = it.next();
    CBitmap *b = e.getValue();
    b->DeleteObject();
    delete b;
  }
}

D3DPRESENT_PARAMETERS BitmapCache::getPresentParameters() const {

  D3DPRESENT_PARAMETERS param;
  ZeroMemory(&param, sizeof(param));

  const CSize sz = getClientRect(m_hwnd).Size();

  param.Windowed               = TRUE;
  param.MultiSampleType        = D3DMULTISAMPLE_NONE;
  param.SwapEffect             = D3DSWAPEFFECT_DISCARD;
  param.EnableAutoDepthStencil = FALSE;
  param.BackBufferFormat       = D3DFMT_X8R8G8B8;
  param.hDeviceWindow          = NULL;
  param.Flags                  = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
  param.BackBufferCount        = 1;
  param.BackBufferWidth        = sz.cx;
  param.BackBufferHeight       = sz.cy;
  param.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
  return param;
}

void BitmapCache::initDevice() {
  D3DPRESENT_PARAMETERS present = getPresentParameters();
  m_device = D3DeviceFactory::createDevice(m_hwnd, &present);
}

void BitmapCache::releaseDevice() {
  if(m_device != NULL) {
    m_device->Release();
    m_device = NULL;
  }
}


static CBitmap *cloneBitmap(CBitmap *bm) {
  const BITMAP info = getBitmapInfo(*bm);

  HDC srcDC = CreateCompatibleDC(NULL);
  HDC dstDC = CreateCompatibleDC(NULL);

  CBitmap *result = new CBitmap;
  result->CreateBitmap(info.bmWidth, info.bmHeight, info.bmPlanes, info.bmBitsPixel, NULL);

  SelectObject(srcDC, *bm);
  SelectObject(dstDC, *result);

  BitBlt(dstDC, 0,0,info.bmWidth,info.bmHeight, srcDC, 0,0, SRCCOPY);

  DeleteDC(dstDC);
  DeleteDC(srcDC);
  return result;
}

static CBitmap *rotate(LPDIRECT3DDEVICE9 device, CBitmap *b0, double degree) {
  HBITMAP bm = rotateBitmap(device, *b0, degree);
  CBitmap *result = createFromHandle(bm);
  DeleteObject(bm);
  return result;
}

CBitmap *getBitmap(int id, int degree) {
  BitmapKey key(id, degree);
  CBitmap **b = bitmapCache->get(key);
  if(b == NULL) {
    BitmapKey k0(id,0);
    CBitmap **b0 = bitmapCache->get(k0);
    if(b0 == NULL) {
      throwException(_T("No bitmap with id=%d"),id);
    }
    CBitmap *br = rotate(bitmapCache->getDevice(), *b0, degree);
    bitmapCache->put(key,br);
    b = bitmapCache->get(key);
  }
  return *b;
}

LPDIRECT3DDEVICE9 getBitmapCacheDevice() {
  return bitmapCache ? bitmapCache->getDevice() : NULL;
}
