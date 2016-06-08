#include "stdafx.h"
#include <MyUtil.h>
#include <Math.h>
#include <float.h>
#include <Stack.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/ColorSpace.h>
#include <MFCUtil/PixRect.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

#pragma comment(lib,"d3d9.lib")

class InitDirectX {
public:
  InitDirectX();
  ~InitDirectX();
};

InitDirectX::InitDirectX() {
  PixRectDevice::initialize();
}

InitDirectX::~InitDirectX() {
  PixRectDevice::uninitialize();
}

static InitDirectX initDirectX;

LPDIRECT3D         PixRectDevice::s_direct3d = NULL;
DEFINECLASSNAME(PixRectDevice);

void PixRectDevice::initialize() { // static
  try {
    s_direct3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (s_direct3d == NULL) {
      throwException(_T("%s::%s. Failed to create Direct3D object"), s_className, _T(__FUNCTION__));
    }
//    CHECK3DRESULT(directDraw->SetCooperativeLevel(NULL, DDSCL_NORMAL));
  } catch(Exception e) {
    MessageBox(NULL, e.what(), _T("Error"), MB_OK|MB_ICONSTOP);
    abort();
  } catch(...) {
    MessageBox(NULL, _T("Unknown exception"), _T("Error"), MB_OK|MB_ICONSTOP);
    abort();
  }
}

void PixRectDevice::uninitialize() { // static
  CHECK3DRESULT(s_direct3d->Release());
  s_direct3d = NULL;
}

PixRectDevice::PixRectDevice() {
  m_device = NULL;
}

PixRectDevice::~PixRectDevice() {
  detach();
}

void PixRectDevice::attach(HWND hwnd, bool windowed, const CSize *size) {
  detach();

  CSize sz;
  if(!windowed) {
    sz = getScreenSize(false);
  } else {
    if(size == NULL) {
      sz = getClientRect(hwnd).Size();
    }
    else {
      sz = *size;
    }
  }

  HDC screenDC = getScreenDC();
  m_appScaleX = (float)(GetDeviceCaps(screenDC, LOGPIXELSX) / 96.0);
  m_appScaleY = (float)(GetDeviceCaps(screenDC, LOGPIXELSX) / 96.0);
  DeleteDC(screenDC);

  D3DPRESENT_PARAMETERS param;
  ZeroMemory(&param, sizeof(param));

  param.Windowed               = windowed ? TRUE : FALSE;
  param.MultiSampleType        = D3DMULTISAMPLE_NONE;
  param.SwapEffect             = D3DSWAPEFFECT_DISCARD;
  param.EnableAutoDepthStencil = FALSE;
  param.BackBufferFormat       = D3DFMT_X8R8G8B8;
  param.hDeviceWindow          = hwnd;
  param.Flags                  = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
  param.BackBufferCount        = 1;
  param.BackBufferWidth        = sz.cx;
  param.BackBufferHeight       = sz.cy;
  param.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

  CHECK3DRESULT(s_direct3d->CreateDevice(
                         D3DADAPTER_DEFAULT
                        ,D3DDEVTYPE_HAL
                        ,hwnd
                        ,D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING
                        ,&param
                        ,&m_device));

  D3DDISPLAYMODE displayMode;
  CHECK3DRESULT(m_device->GetDisplayMode(0, &displayMode));
  m_defaultPixelFormat = displayMode.Format;
}

void PixRectDevice::detach() {
  if (m_device != NULL) {
    m_device->Release();
    m_device = NULL;
  }
}

LPDIRECT3DTEXTURE PixRectDevice::createTexture(const CSize &size, D3DFORMAT format, D3DPOOL pool) {
  LPDIRECT3DTEXTURE texture;

  // Create an alpha texture
  if (format == D3DFMT_FORCE_DWORD) {
    format = getDefaultPixelFormat();
  }
  CHECK3DRESULT(m_device->CreateTexture(size.cx, size.cy, 1, D3DUSAGE_DYNAMIC, format, pool, &texture, NULL));
  int levels = texture->GetLevelCount();
  D3DSURFACE_DESC desc;
  CHECK3DRESULT(texture->GetLevelDesc(0, &desc));
  return texture;
}

LPDIRECT3DSURFACE PixRectDevice::createRenderTarget(const CSize &size, D3DFORMAT format, bool lockable) {
  LPDIRECT3DSURFACE surface;

  LPDIRECT3DSURFACE oldRenderTarget;
  CHECK3DRESULT(m_device->GetRenderTarget(0, &oldRenderTarget));
  D3DSURFACE_DESC desc;
  CHECK3DRESULT(oldRenderTarget->GetDesc(&desc));
  oldRenderTarget->Release();
  if(format == D3DFMT_FORCE_DWORD) {
    format = desc.Format;
  }
  CHECK3DRESULT(m_device->CreateRenderTarget(size.cx, size.cy, format, desc.MultiSampleType, desc.MultiSampleQuality, lockable, &surface, NULL));
  return surface;
}

LPDIRECT3DSURFACE PixRectDevice::createOffscreenPlainSurface(const CSize &size, D3DFORMAT format, D3DPOOL pool) {
  if (format == D3DFMT_FORCE_DWORD) {
    format = getDefaultPixelFormat();
  }
  LPDIRECT3DSURFACE surface;
  CHECK3DRESULT(m_device->CreateOffscreenPlainSurface(size.cx, size.cy, format, pool, &surface, NULL));
  return surface;
}

Array<D3DDISPLAYMODE> PixRectDevice::getDisplayModes(UINT adapter) { // static
  Array<D3DDISPLAYMODE> result;
  D3DDISPLAYMODE adapterMode;
  CHECK3DRESULT(s_direct3d->GetAdapterDisplayMode(adapter, &adapterMode));
  const UINT modeCount = s_direct3d->GetAdapterModeCount(adapter, adapterMode.Format);
  for (UINT mode = 0; mode < modeCount; mode++) {
    D3DDISPLAYMODE dp;
    CHECK3DRESULT(s_direct3d->EnumAdapterModes(adapter, adapterMode.Format, mode, &dp));
    result.add(dp);
  };
  return result;
}

D3DCAPS9 PixRectDevice::getDeviceCaps() {
  D3DCAPS9 caps;
  CHECK3DRESULT(m_device->GetDeviceCaps(&caps));
  return caps;
}

/*
DDCAPS PixRect::getEmulatorCaps() { // static
  DDCAPS result;
  result.dwSize = sizeof(DDCAPS);
  CHECK3DRESULT(directDraw->GetCaps(NULL, &result));
  return result;
}
*/

void PixRectDevice::render(const PixRect *pr) {
  if(!m_device) {
    return;  // Haven't been initialized yet!
  }

  CHECK3DRESULT(m_device->BeginScene());

  LPDIRECT3DSURFACE renderTarget;
  CHECK3DRESULT(m_device->GetRenderTarget(0, &renderTarget));
//  CHECK3DRESULT(m_device->SetRenderTarget(0, m_renderTarget));

//  unsigned long clear_color = 0xffffffff;
//  CHECK3DRESULT(m_device->Clear(0, NULL, D3DCLEAR_TARGET, clear_color, 1.0f, 0));

//  CHECK3DRESULT(m_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE));
//  CHECK3DRESULT(m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
//  CHECK3DRESULT(m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1));
//  CHECK3DRESULT(m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE));
  CHECK3DRESULT(m_device->SetRenderState(D3DRS_LIGHTING, FALSE));

//  const CSize winSize = getSize(m_boardSurface); //getGraphicsSize();
//  render2d(winSize);

  CHECK3DRESULT(m_device->UpdateSurface(pr->m_surface, NULL, renderTarget, NULL));
  CHECK3DRESULT(m_device->EndScene());
//  CHECK3DRESULT(m_device->StretchRect(m_renderTarget, NULL, oldRenderTarget, NULL, D3DTEXF_NONE));
//  CHECK3DRESULT(m_device->SetRenderTarget(0, oldRenderTarget));
  renderTarget->Release();
  CHECK3DRESULT(m_device->Present(NULL, NULL, NULL, NULL));
}

void PixRectDevice::set2DTransform(const CSize &size) {
  D3DMATRIX projection;
  memset(&projection, 0, sizeof(D3DMATRIX));

  projection._11 = 2.0f * m_appScaleX / (float)size.cx;
  projection._41 = -m_appScaleX;
  projection._22 = -2.0f * m_appScaleY / (float)size.cy;
  projection._42 = m_appScaleY;

  projection._33 = 1;
  projection._43 = 0;

  projection._34 = 0;
  projection._44 = 1;
/*
  CHECK3DRESULT(m_device->SetTransform(D3DTS_PROJECTION, &projection));
  const CSize sz = getSize(m_someTexture);
  const D3DXVECTOR2 rotationCenter(37,37);
  D3DXMATRIX matWorld;
  CHECKD3DRESULT(m_device->SetTransform( D3DTS_WORLD, D3DXMatrixAffineTransformation2D(&matWorld, 1, &rotationCenter, (float)GRAD2RAD(m_rotation), NULL)));
*/
}

void PixRect::reOpenDirectX() { // static 
//  uninitialize();
//  initialize();
}

#define ASM_OPTIMIZED

#define RGB565_GETRED(rgb)        (((rgb) >> 11) & 0x1f)
#define RGB565_GETGREEN(rgb)      (((rgb) >> 5)  & 0x3f)
#define RGB565_GETBLUE(rgb)       ( (rgb)        & 0x1f)

#define RGB555_GETRED(rgb)        (((rgb) >> 10) & 0x1f)
#define RGB555_GETGREEN(rgb)      (((rgb) >> 5)  & 0x1f)
#define RGB555_GETBLUE(rgb)       ( (rgb)        & 0x1f)

#define RGB565_MAKE(r, g, b)      ((unsigned short) (((r) << 11) | ((g) << 5) | (b)))
#define RGB555_MAKE(r, g, b)      ((unsigned short) (((r) << 10) | ((g) << 5) | (b)))

static WORD D3DColorToShortColor(D3DCOLOR color) {
#ifndef ASM_OPTIMIZED
  int r = RGB_GETRED(color)   >> 3;
  int g = RGB_GETGREEN(color) >> 2;
  int b = RGB_GETBLUE(color)  >> 3;
  return RGB565_MAKE(r,g,b);
#else
  WORD result;
  __asm {
    mov	eax, color
    mov ebx, eax
    mov ecx, eax
    shr	eax, 8
    and	eax, 0xf800
    shr ebx, 5
    and ebx, 0x07e0
    or  eax, ebx
    shr ecx, 3
    and ecx, 0x001f
    or  eax, ecx
    mov result,ax
  }
  return result;
#endif
}

static D3DCOLOR shortColorToD3DColor(WORD color) {
#ifndef ASM_OPTIMIZED
  int r = RGB565_GETRED(color)   << 3;
  int g = RGB565_GETGREEN(color) << 2;
  int b = RGB565_GETBLUE(color)  << 3;

  return RGB_MAKE(r,g,b);
#else
  D3DCOLOR result;
  __asm {
    mov	ax, color
    and	eax, 0xffff
    mov ebx, eax
    mov ecx, eax
    and eax, 0xf800
    shl eax, 8
    and ebx, 0x07e0
    shl	ebx, 5
    or  eax, ebx
    and ecx, 0x001f
    shl ecx, 3
    or  eax, ecx
    mov result,eax
  }
  return result;
#endif
}

static D3DCOLOR byteColorToD3DColor(BYTE color) {
  return 0;
}

static BYTE D3DColorToByteColor(D3DCOLOR color) {
  return 0;
}

double colorDistance(D3DCOLOR c1, D3DCOLOR c2) { // value between [0..1]
  const int r = GetRValue(c1) - GetRValue( c2);
  const int g = GetGValue(c1) - GetGValue(c2);
  const int b = GetBValue(c1) - GetBValue(c2);
  return sqrt((double)r*r + g*g + b*b)/441.67295593;
}

class SimpleColorComparator : public ColorComparator {
public:
  bool equals(const D3DCOLOR &c1, const D3DCOLOR &c2);
};

#define RGBA_TORGB(d3c) ((d3c) & 0x00ffffff)

bool SimpleColorComparator::equals(const D3DCOLOR &c1, const D3DCOLOR &c2) {
  return RGBA_TORGB(c1) == RGBA_TORGB(c2);
}

class ApproximateColorComparator : public ColorComparator {
  double m_tolerance;
public:
  ApproximateColorComparator(double tolerance);
  bool equals(const D3DCOLOR &c1, const D3DCOLOR &c2);
};

ApproximateColorComparator::ApproximateColorComparator(double tolerance) {
  m_tolerance = tolerance;
}

bool ApproximateColorComparator::equals(const D3DCOLOR &c1, const D3DCOLOR &c2) {
  return colorDistance(c1,c2) < m_tolerance;
}

/*static*/ D3DLOCKED_RECT PixelAccessor::lockRect(LPDIRECT3DSURFACE surface, DWORD flags) {
  D3DLOCKED_RECT lr;
  CHECK3DRESULT(surface->LockRect(&lr, NULL, flags));
  return lr;
}

/*static*/ void PixelAccessor::unlockRect(LPDIRECT3DSURFACE surface) {
  CHECK3DRESULT(surface->UnlockRect());
}

PixelAccessor::PixelAccessor(PixRect *pixRect, DWORD flags) : m_pixRect(pixRect) {
  m_desc = pixRect->m_desc;
  m_lockedRect = lockRect(m_pixRect->m_surface, flags);
}

PixelAccessor::~PixelAccessor() {
  unlockRect(m_pixRect->m_surface);
}

void PixelAccessor::fill(const CPoint &p, D3DCOLOR color) {
  fill(p, color, SimpleColorComparator());
}

void PixelAccessor::fill(const CPoint &p, D3DCOLOR color, ColorComparator &cmp) {
  if(!m_pixRect->contains(p)) {
    return;
  }
  D3DCOLOR oldColor = getPixel(p);
  if(cmp.equals(color, oldColor)) {
    return;
  }
  const int width  = m_pixRect->getWidth();
  const int height = m_pixRect->getHeight();
  Stack<CPoint> stack;
  stack.push(p);
  while(!stack.isEmpty()) {
    CPoint np = stack.pop();
    int x;
    bool n1Stacked = false;
    bool n2Stacked = false;
    int y1 = np.y-1;
    int y2 = np.y+1;
    for(x = np.x; x >= 0 && cmp.equals(getPixel(x,np.y),oldColor); x--) {
      setPixel(x,np.y,color);
      if(y1 >= 0) {
        if(!n1Stacked) {
          if(cmp.equals(getPixel(x,y1),oldColor)) {
            stack.push(CPoint(x,y1));
            n1Stacked = true;
          }
        } else if(!cmp.equals(getPixel(x,y1),oldColor)) {
          n1Stacked = false;
        }
      }
      if(y2 < height) {
        if(!n2Stacked) {
          if(cmp.equals(getPixel(x,y2),oldColor)) {
            stack.push(CPoint(x,y2));
            n2Stacked = true;
          }
        } else if(!cmp.equals(getPixel(x,y2),oldColor)) {
          n2Stacked = false;
        }
      }
    }
    n1Stacked = false;
    n2Stacked = false;
    for(x = np.x+1; x < width && cmp.equals(getPixel(x,np.y),oldColor); x++) {
      setPixel(x,np.y,color);
      if(y1 >= 0) {
        if(!n1Stacked) {
          if(cmp.equals(getPixel(x,y1),oldColor)) {
            stack.push(CPoint(x,y1));
            n1Stacked = true;
          }
        } else if(!cmp.equals(getPixel(x,y1),oldColor)) {
          n1Stacked = false;
        }
      }
      if(y2 < height) {
        if(!n2Stacked) {
          if(cmp.equals(getPixel(x,y2),oldColor)) {
            stack.push(CPoint(x,y2));
            n2Stacked = true;
          }
        } else if(!cmp.equals(getPixel(x,y2),oldColor)) {
          n2Stacked = false;
        }
      }
    }
  }
}

#ifdef __NEVER__
class BytePixelAccessor : public PixelAccessor {
private:
  BYTE *m_pixels;
  unsigned int m_pixelsPerLine;
public:
  BytePixelAccessor(PixRect *pixRect) : PixelAccessor(pixRect) {
    m_pixels = (BYTE*)m_ddsd.lpSurface;
    m_pixelsPerLine = m_ddsd.lPitch / sizeof(m_pixels[0]);
  }
  void setPixel(unsigned int x, unsigned int y, D3DCOLOR color);
  D3DCOLOR getPixel(unsigned int x, unsigned int y);
  void setPixel(const CPoint &p, D3DCOLOR color);
  D3DCOLOR getPixel(const CPoint &p);
};

void BytePixelAccessor::setPixel(unsigned int x, unsigned int y, D3DCOLOR color) {
#ifdef _DEBUG
  if(x >= m_ddsd.dwWidth || y >= m_ddsd.dwHeight) {
    throwException(_T("BytePixelAccessor::setPixel(%u,%u) outside pixrect. Size=(%d,%d)"),x,y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  m_pixels[m_pixelsPerLine * y + x] = D3DColorToByteColor(color);
}

D3DCOLOR BytePixelAccessor::getPixel(unsigned int x, unsigned int y) {
#ifdef _DEBUG
  if(x >= m_ddsd.dwWidth || y >= m_ddsd.dwHeight) {
    throwException(_T("BytePixelAccessor::getPixel(%u,%u) outside pixrect. Size=(%d,%d)"),x,y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  return byteColorToD3DColor(m_pixels[m_pixelsPerLine * y + x]);
}

void BytePixelAccessor::setPixel(const CPoint &p, D3DCOLOR color) {
#ifdef _DEBUG
  if((unsigned int)p.x >= m_ddsd.dwWidth || (unsigned int)p.y >= m_ddsd.dwHeight) {
    throwException(_T("BytePixelAccessor::setPixel(%d,%d) outside pixrect. Size=(%d,%d)"),p.x,p.y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  m_pixels[m_pixelsPerLine * p.y + p.x] = D3DColorToByteColor(color);
}

D3DCOLOR BytePixelAccessor::getPixel(const CPoint &p) {
#ifdef _DEBUG
  if(((unsigned int)p.x >= m_ddsd.dwWidth) || ((unsigned int)p.y >= m_ddsd.dwHeight)) {
    throwException(_T("BytePixelAccessor::getPixel(%d,%d) outside pixrect. Size=(%d,%d)"),p.x,p.y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  return byteColorToD3DColor(m_pixels[m_pixelsPerLine * p.y + p.x]);
}

class WordPixelAccessor : public PixelAccessor {
private:
  WORD *m_pixels;
  unsigned int m_pixelsPerLine;
public:
  WordPixelAccessor(PixRect *pixRect) : PixelAccessor(pixRect) {
    m_pixels = (WORD*)m_ddsd.lpSurface;
    m_pixelsPerLine = m_ddsd.lPitch / sizeof(m_pixels[0]);
  }
  void setPixel(unsigned int x, unsigned int y, D3DCOLOR color);
  D3DCOLOR getPixel(unsigned int x, unsigned int y);
  void setPixel(const CPoint &p, D3DCOLOR color);
  D3DCOLOR getPixel(const CPoint &p);
};

void WordPixelAccessor::setPixel(unsigned int x, unsigned int y, D3DCOLOR color) {
#ifdef _DEBUG
  if(x >= m_ddsd.dwWidth || y >= m_ddsd.dwHeight) {
    throwException(_T("WordPixelAccessor::setPixel(%u,%u) outside pixrect. Size=(%d,%d)"),x,y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  m_pixels[m_pixelsPerLine * y + x] = D3DColorToShortColor(color);
}

D3DCOLOR WordPixelAccessor::getPixel(unsigned int x, unsigned int y) {
#ifdef _DEBUG
  if(x >= m_ddsd.dwWidth || y >= m_ddsd.dwHeight) {
    throwException(_T("WordPixelAccessor::getPixel(%u,%u) outside pixrect. Size=(%d,%d)"),x,y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  return shortColorToD3DColor(m_pixels[m_pixelsPerLine * y + x]);
}

void WordPixelAccessor::setPixel(const CPoint &p, D3DCOLOR color) {
#ifdef _DEBUG
  if(((unsigned int)p.x >= m_ddsd.dwWidth) || ((unsigned int)p.y >= m_ddsd.dwHeight)) {
    throwException(_T("WordPixelAccessor::setPixel(%d,%d) outside pixrect. Size=(%d,%d)"),p.x,p.y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  m_pixels[m_pixelsPerLine * p.y + p.x] = D3DColorToShortColor(color);
}

D3DCOLOR WordPixelAccessor::getPixel(const CPoint &p) {
#ifdef _DEBUG
  if(((unsigned int)p.x >= m_ddsd.dwWidth) || ((unsigned int)p.y >= m_ddsd.dwHeight)) {
    throwException(_T("WordPixelAccessor::getPixel(%d,%d) outside pixrect. Size=(%d,%d)"),p.x,p.y,m_ddsd.dwWidth,m_ddsd.dwHeight);
  }
#endif
  return shortColorToD3DColor(m_pixels[m_pixelsPerLine * p.y + p.x]);
}
#endif

class DWordPixelAccessor : public PixelAccessor {
private:
  DECLARECLASSNAME;
  DWORD       *m_pixels;
  unsigned int m_pixelsPerLine;
public:
  DWordPixelAccessor(PixRect *pixRect, DWORD flags) : PixelAccessor(pixRect, flags) {
    m_pixels        = (DWORD*)m_lockedRect.pBits;
    m_pixelsPerLine = m_lockedRect.Pitch / sizeof(m_pixels[0]);
  }
  void     setPixel(unsigned int x, unsigned int y, D3DCOLOR color);
  D3DCOLOR getPixel(unsigned int x, unsigned int y);
  void     setPixel(const CPoint &p, D3DCOLOR color);
  D3DCOLOR getPixel(const CPoint &p);
};

DEFINECLASSNAME(DWordPixelAccessor);

void DWordPixelAccessor::setPixel(unsigned int x, unsigned int y, D3DCOLOR color) {
#ifdef _DEBUG
  checkPoint(s_className, _T(__FUNCTION__), x, y);
#endif
  m_pixels[m_pixelsPerLine * y + x] = color;
}

D3DCOLOR DWordPixelAccessor::getPixel(unsigned int x, unsigned int y) {
#ifdef _DEBUG
  checkPoint(s_className, _T(__FUNCTION__), x, y);
#endif
  return m_pixels[m_pixelsPerLine * y + x];
}

void DWordPixelAccessor::setPixel(const CPoint &p, D3DCOLOR color) {
#ifdef _DEBUG
  checkPoint(s_className, _T(__FUNCTION__), p.x, p.y);
#endif
  m_pixels[m_pixelsPerLine * p.y + p.x] = color;
}

D3DCOLOR DWordPixelAccessor::getPixel(const CPoint &p) {
#ifdef _DEBUG
  checkPoint(s_className, _T(__FUNCTION__), p.x, p.y);
#endif
  return m_pixels[m_pixelsPerLine * p.y + p.x];
}

PixelAccessor *PixelAccessor::createPixelAccessor(PixRect *pixRect, DWORD flags) {
  switch(pixRect->getPixelFormat()) {
    //  case 8 : return new BytePixelAccessor(pixRect);
    //  case 16: return new WordPixelAccessor(pixRect);
  case D3DFMT_A8R8G8B8:
  case D3DFMT_X8R8G8B8:
    return new DWordPixelAccessor(pixRect, flags);
  default: throwException(_T("Unknown pixel format:%d. Must be 8, 16 or 32"),pixRect->getPixelFormat());
           return NULL;
  }
}

PixRectOperator::PixRectOperator(PixRect *pr) {
  init();
  setPixRect(pr);
}

void PixRectOperator::init() {
  m_pixRect       = NULL;
  m_pixelAccessor = NULL;
}

void PixRectOperator::setPixRect(PixRect *pixRect) {
  m_pixRect = pixRect;
  if(m_pixelAccessor != NULL) {
    delete m_pixelAccessor;
    m_pixelAccessor = NULL;
  }
  
  if(m_pixRect != NULL) {
    m_pixelAccessor = PixelAccessor::createPixelAccessor(m_pixRect);
  }
}

PixRectOperator::~PixRectOperator() {
  setPixRect(NULL);
}

void PixRectFilter::init() {
  m_result              = NULL;
  m_resultPixelAccessor = NULL;
}

void PixRectFilter::setPixRect(PixRect *pixRect) {
  PixRectOperator::setPixRect(pixRect);
  m_result              = m_pixRect;
  m_resultPixelAccessor = m_pixelAccessor;
}

CRect PixRectFilter::getRect() const {
  return (m_pixRect == NULL) ? CRect(0,0,0,0) : CRect(0,0,m_pixRect->getWidth(),m_pixRect->getHeight());
}

DEFINECLASSNAME(PixRect);

PixRect::PixRect(PixRectDevice &device) : m_device(device) {
  m_surface = NULL;
}

PixRect::PixRect(PixRectDevice &device, unsigned int width, unsigned int height, D3DFORMAT pixelFormat) : m_device(device) {
  createSurface(width,height, pixelFormat);
  fillRect(0,0,width,height, WHITE);
}

PixRect::PixRect(PixRectDevice &device, const CSize &size, D3DFORMAT pixelFormat) : m_device(device) {
  createSurface(size.cx,size.cy, pixelFormat);
  fillRect(0,0,size.cx,size.cy,WHITE);
}

PixRect *PixRect::clone(bool cloneImage) const {
  PixRect *copy = new PixRect(m_device, getWidth(),getHeight(),getPixelFormat());
  if(cloneImage) {
    m_device.getD3Device()->UpdateSurface(m_surface, NULL, copy->m_surface, NULL);
  }
  return copy;
}

LPDIRECT3DSURFACE PixRect::cloneSurface() const {
  LPDIRECT3DSURFACE result = m_device.createOffscreenPlainSurface(getSize(), getPixelFormat(), getPool());
  CHECK3DRESULT(m_device.getD3Device()->UpdateSurface(m_surface, NULL, result, NULL));
  return result;
}

void PixRect::showPixRect(PixRect *pr) { // static
  HDC screenDC = getScreenDC();
  BitBlt(screenDC,0,0,pr->getWidth(),pr->getHeight(),NULL,0,0,WHITENESS);
  PixRect::bitBlt(screenDC, 0,0,pr->getWidth(),pr->getHeight(), SRCCOPY, pr, 0,0);
  DeleteDC(screenDC);
}

void PixRect::checkHasAlphaChannel() const { // throw Exception if no alpha-channel
}

void PixRect::setSize(const CSize &size) {
  const CSize oldSize = getSize();
  CSize areaToCopy;
  if(size == oldSize) {
    return;
  }
  areaToCopy.cx = min(oldSize.cx, size.cx);
  areaToCopy.cy = min(oldSize.cy, size.cy);
  PixRect copy(m_device, areaToCopy, getPixelFormat());
  copy.rop(0, 0, areaToCopy.cx, areaToCopy.cy, SRCCOPY, this, 0,0);
  destroySurface();
  createSurface(size.cx, size.cy, copy.getPixelFormat());
  fillRect(getRect(), WHITE);
  rop(0,0, areaToCopy.cx, areaToCopy.cy, SRCCOPY, &copy, 0, 0);
}

CSize PixRect::getSizeInMillimeters(HDC hdc) const {
  if(hdc != NULL) {
    return pixelsToMillimeters(hdc,getSize());
  } else {
    HDC screenDC = getScreenDC();
    const CSize result = pixelsToMillimeters(screenDC,getSize());
    DeleteDC(screenDC);
    return result;
  }
}

PixRect::PixRect(PixRectDevice &device, HBITMAP src, D3DFORMAT pixelFormat) : m_device(device) {
  init(src, pixelFormat);
}

void PixRect::init(HBITMAP src, D3DFORMAT pixelFormat) {
  String errMsg;
  BITMAP bm;
  GetObject(src,sizeof(bm),&bm);
  createSurface(bm.bmWidth,bm.bmHeight, pixelFormat);

  HDC prDC = getDC();
  HDC bmDC = CreateCompatibleDC(prDC);
  HGDIOBJ oldGDI = SelectObject(bmDC, src);
  BOOL ok = BitBlt(prDC,0,0,getWidth(),getHeight(), bmDC,0,0,SRCCOPY);
  if(!ok) {
    errMsg = getLastErrorText();
  }
    
  SelectObject(bmDC, oldGDI);

  DeleteDC(bmDC);
  releaseDC(prDC);
  if(!ok) {
    throwException(_T("%s::init failed:%s"), s_className, errMsg.cstr());
  }
}

PixRect::~PixRect() {
  destroySurface();
}

void PixRect::createSurface(UINT width, UINT height, D3DFORMAT format, D3DPOOL pool) {
  m_surface = m_device.createOffscreenPlainSurface(CSize(width,height), format, pool);
  CHECK3DRESULT(m_surface->GetDesc(&m_desc));
}

void PixRect::destroySurface() {
  CHECK3DRESULT(m_surface->Release());
}

PixRect &PixRect::operator=(HBITMAP src) {
  String errMsg;
  BITMAP bm;
  GetObject(src,sizeof(bm),&bm);
  const D3DFORMAT pixelFormat = getPixelFormat();

  destroySurface();
  createSurface(bm.bmWidth,bm.bmHeight, pixelFormat);

  HDC prDC = getDC();
  HDC bmDC = CreateCompatibleDC(prDC);
  HGDIOBJ gdi1 = SelectObject(bmDC,src);
  BOOL ok = BitBlt(prDC,0,0,getWidth(),getHeight(),bmDC,0,0,SRCCOPY);
  if(!ok) {
    errMsg = getLastErrorText();
  }
  SelectObject(bmDC,gdi1);
  DeleteDC(bmDC);
  releaseDC(prDC);
  if(!ok) {
    throwException(_T("%s::operator=(HBITMAP) failed:%s"), s_className, errMsg.cstr());
  }
  return *this;
}

D3DFORMAT PixRect::getPixelFormat(const BITMAP &bm) { // static
  switch(bm.bmBitsPixel) {
  case 32:
    return D3DFMT_X8B8G8R8;
  default:
    throwException(_T("%s::getPixelFormat(BITMAP):BITMAP.bmBitsPixel=%d. Must be 32"), s_className, bm.bmBitsPixel);
    return D3DFMT_X8B8G8R8;
  }
}

PixRect::operator HBITMAP() const {
  String errMsg;
  HBITMAP result = CreateBitmap( getWidth(), getHeight(), 1, 32, NULL);

  HDC     prDC = getDC();
  HDC     bmDC = CreateCompatibleDC(prDC);
  HGDIOBJ gdi1 = SelectObject(bmDC,result);
  BOOL    ok   = BitBlt(bmDC,0,0,getWidth(),getHeight(),prDC,0,0,SRCCOPY);
  if(!ok) {
    errMsg = getLastErrorText();
  }
  DeleteDC(bmDC);
  releaseDC(prDC);
  if(!ok) {
    throwException(_T("%s::operator HBITMAP failed:%s"), s_className, errMsg.cstr());
  }

  return result;
}

void PixRect::fromBitmap(CBitmap &src) {
  String errMsg;
  const CSize bmSize = ::getBitmapSize(src);
  const CSize prSize = getSize();

  if(bmSize != prSize) {
    const D3DFORMAT pixelFormat = getPixelFormat();
    destroySurface();
    createSurface( bmSize.cx, bmSize.cy, pixelFormat);
  }

  HDC     prDC = getDC();
  HDC     bmDC = CreateCompatibleDC(prDC);
  HGDIOBJ gdi1 = SelectObject(bmDC,(HBITMAP)src);
  BOOL    ok   = BitBlt(prDC,0,0,bmSize.cx,bmSize.cy,bmDC,0,0,SRCCOPY);
  if(!ok) {
    errMsg = getLastErrorText();
  }
  SelectObject(bmDC, gdi1);
  DeleteDC(bmDC);
  releaseDC(prDC);
  if(!ok) {
    throwException(_T("%s::fromBitmap failed:%s"), s_className, errMsg.cstr());
  }
}

void PixRect::toBitmap(CBitmap &dst) const {
  String errMsg;
  if(dst.m_hObject == NULL) {
    dst.CreateBitmap(getWidth(),getHeight(),1,32,NULL);
  } else {
    const CSize bmSize = getBitmapSize(dst);
    const CSize prSize = getSize();

    if(bmSize != prSize) {
      dst.DeleteObject();
      dst.CreateBitmap(prSize.cx, prSize.cy, 1, 32, NULL);
    }
  }
  HDC     prDC = getDC();
  HDC     bmDC = CreateCompatibleDC(prDC);
  HGDIOBJ gdi1 = SelectObject(bmDC,(HBITMAP)dst);
  BOOL    ok   = BitBlt(bmDC,0,0,getWidth(),getHeight(),prDC,0,0,SRCCOPY);
  if(!ok) {
    errMsg = getLastErrorText();
  }
  SelectObject(bmDC, gdi1);

  DeleteDC(bmDC);
  releaseDC(prDC);
  if(!ok) {
    throwException(_T("%s::toBitmap failed:%s"), s_className, errMsg.cstr());
  }
}

bool PixRect::contains(const Point2D &p) const {
  return p.x >= 0 && p.x < m_desc.Width && p.y >= 0 && p.y < m_desc.Height;
}

bool PixRect::containsExtended(const Point2D &p) const {
  return p.x > -1 && p.x < m_desc.Width && p.y > -1 && p.y < m_desc.Height;
}

bool operator==(const D3DSURFACE_DESC &d1, const D3DSURFACE_DESC &d2) {
  return d1.Width  == d2.Width
      && d1.Height == d2.Height
      && d1.Format == d2.Format;
}

bool operator!=(const D3DSURFACE_DESC &d1, const D3DSURFACE_DESC &d2) {
  return !(d1 == d2);
}

bool operator==(const PixRect &p1, const PixRect &p2) {
  if(&p1 == &p2) {
    return true;
  }
  if(p1.m_desc != p2.m_desc) {
    return false;
  }
  PixelAccessor *pa1 = PixelAccessor::createPixelAccessor(&(PixRect&)p1);
  PixelAccessor *pa2 = PixelAccessor::createPixelAccessor(&(PixRect&)p2);
  const int width  = p1.getWidth();
  const int height = p1.getHeight();
  bool equals = true;
  CPoint pt;
  for(pt.x = 0; pt.x < width; pt.x++) {
    for(pt.y = 0; pt.y < height; pt.y++) {
      if(pa1->getPixel(pt) != pa2->getPixel(pt)) {
        equals = false;
        goto done;
      }
    }
  }
done:
  delete pa1;
  delete pa2;
  return equals;
}

bool operator!=(const PixRect &p1, const PixRect &p2) {
  return !(p1 == p2);
}

void PixRect::setPixel(const CPoint &p, D3DCOLOR color) {
  setPixel(p.x,p.y,color);
}

D3DCOLOR PixRect::getPixel(const CPoint &p) const {
  return getPixel(p.x,p.y);
}

void PixRect::setPixel(unsigned int x, unsigned int y, D3DCOLOR color) {
  if(!contains(x,y)) {
    return;
  }
  PixelAccessor *pixelAccessor = getPixelAccessor();
  pixelAccessor->setPixel(x,y,color);
  delete pixelAccessor;
}

D3DCOLOR PixRect::getPixel(unsigned int x, unsigned int y) const {
  if(!contains(x,y)) {
    return 0;
  }
  PixelAccessor *pixelAccessor = PixelAccessor::createPixelAccessor((PixRect*)this);
  D3DCOLOR result = pixelAccessor->getPixel(x,y);
  delete pixelAccessor;
  return result;
}

void PixRect::rop( const CRect  &dr, unsigned long op, const PixRect *src, const CPoint &sp) {
  rop(dr.left,dr.top,dr.Width(),dr.Height(), op, src,sp.x,sp.y);
}

void PixRect::rop( const CPoint &dp, const CSize &size, unsigned long op, const PixRect *src, const CPoint &sp) {
  rop(dp.x,dp.y,size.cx,size.cy, op, src,sp.x,sp.y);
}

void PixRect::rop(int x, int y, int w, int h, unsigned long op, const PixRect *src, int sx, int sy) {
  String errorMsg;

  HDC srcDC = src ? src->getDC() : NULL;
  HDC dstDC = getDC();

  BOOL ok = BitBlt(dstDC,x,y,w,h,srcDC,sx,sy,op);
  if(!ok) {
    errorMsg = getLastErrorText();
  }
  if(src != NULL) {
    src->releaseDC(srcDC);
  }
  releaseDC(dstDC);
  if(!ok) {
    throwException(_T("PixRect::rop failed:%s"), errorMsg.cstr());
  }
}

void PixRect::rop(const CRect &dr, unsigned long op, const PixRect *src, const CRect &sr) {
  AfxMessageBox(_T("PixRect::rop no implemented yet"));
//  CHECK3DRESULT(m_surface->Blt(&dstRect, src->m_surface, &srcRect, DDBLT_ROP | DDBLT_WAIT, &bltFunc));
}

void PixRect::bitBlt(HDC dst, const CPoint &p, const CSize &size, unsigned long op, const PixRect *src, const CPoint &sp) { // static
  bitBlt(dst, p.x,p.y,size.cx,size.cy, op, src, sp.x,sp.y);
}

void PixRect::bitBlt(PixRect *dst, const CPoint &p, const CSize &size, unsigned long op, HDC src, const CPoint &sp) { // static
  bitBlt(dst, p.x,p.y,size.cx,size.cy, op, src, sp.x,sp.y);
}

void PixRect::bitBlt(HDC dst, int x, int y, int w, int h, unsigned long op, const PixRect *src, int sx, int sy) { // static
  String errorMsg;

  HDC srcDC = src ? src->getDC() : NULL;
  BOOL ok = BitBlt(dst,x,y,w,h,srcDC,sx,sy,op);
  if(!ok) {
    errorMsg = getLastErrorText();
  }
  if(src != NULL) {
    src->releaseDC(srcDC);
  }
  if(!ok) {
    throwException(_T("%s::bitBlt failed:%s"), s_className, errorMsg.cstr());
  }
}

void PixRect::bitBlt(PixRect *dst, int x, int y, int w, int h, unsigned long op, HDC src, int sx, int sy) { // static
  String errorMsg;

  HDC dstDC = dst->getDC();

  BOOL ok = BitBlt(dstDC,x,y,w,h,src,sx,sy,op);
  if(!ok) {
    errorMsg = getLastErrorText();
  }
  dst->releaseDC(dstDC);
  if(!ok) {
    throwException(_T("%s::bitBlt failed:%s"), s_className, errorMsg.cstr());
  }
}

void PixRect::stretchBlt(HDC dst, int x, int y, int w, int h, unsigned long op, const PixRect *src, int sx, int sy, int sw, int sh) { // static
  String errorMsg;

  HDC  srcDC = src ? src->getDC() : NULL;
  BOOL ok    = StretchBlt(dst,x,y,w,h,srcDC,sx,sy,sw,sh,op);
  if(!ok) {
    errorMsg = getLastErrorText();
  }
  if(src) {
    src->releaseDC(srcDC);
  }
  if(!ok) {
    throwException(_T("%s::stretchBlt failed:%s"), s_className, errorMsg.cstr());
  }
}

void PixRect::stretchBlt(PixRect *dst, int x, int y, int w, int h, unsigned long op, const HDC src, int sx, int sy, int sw, int sh) { // static
  String errorMsg;

  HDC  dstDC = dst->getDC();
  BOOL ok    = StretchBlt(dstDC,x,y,w,h,src,sx,sy,sw,sh,op);
  if(!ok) {
    errorMsg = getLastErrorText();
  }
  dst->releaseDC(dstDC);
  if(!ok) {
    throwException(_T("%s::stretchBlt failed:%s"), s_className, errorMsg.cstr());
  }
}

void PixRect::stretchBlt(HDC dst, const CPoint &p, const CSize &size, unsigned long op, const PixRect *src, const CPoint &sp, const CSize &ssize) { // static
  stretchBlt(dst, p.x,p.y,size.cx,size.cy, op, src, sp.x,sp.y,ssize.cx,ssize.cy);
}

void PixRect::stretchBlt(PixRect *dst, const CPoint &p, const CSize &size, unsigned long op, const HDC src, const CPoint &sp, const CSize &ssize) { // static 
  stretchBlt(dst, p.x,p.y,size.cx,size.cy, op, src, sp.x,sp.y,ssize.cx,ssize.cy);
}

void PixRect::stretchBlt(HDC dst, const CRect &dstRect, unsigned long op, const PixRect *src, const CRect &sr) { // static
  const CSize dsz = dstRect.Size();
  const CSize ssz = sr.Size();
  stretchBlt(dst, dstRect.left,dstRect.top,dsz.cx,dsz.cy, op, src, sr.left,sr.top,ssz.cx,ssz.cy);
}

void PixRect::stretchBlt(PixRect *dst, const CRect &dstRect, unsigned long op, const HDC src, const CRect &sr) { // static
  const CSize dsz = dstRect.Size();
  const CSize ssz = sr.Size();
  stretchBlt(dst, dstRect.left,dstRect.top,dsz.cx,dsz.cy, op, src, sr.left,sr.top,ssz.cx,ssz.cy);
}

void PixRect::mask(const CRect  &dr, unsigned long op, const PixRect *src, const CPoint &sp, const PixRect *prMask) {
  mask(dr.left,dr.top,dr.Width(),dr.Height(), op, src,sp.x,sp.y, prMask);
}

void PixRect::mask(const CPoint &dp, const CSize &size, unsigned long op, const PixRect *src, const CPoint &sp, const PixRect *prMask) {
  mask(dp.x,dp.y,size.cx,size.cy, op, src, sp.x,sp.y, prMask);
}

// Performs rasteropreation op on this using src, for the pixels which are non-zero in prMask
void PixRect::mask(int x, int y, int w, int h, unsigned long op, const PixRect *src, int sx, int sy, const PixRect *prMask) {
  String errorMsg;

  HDC srcDC  = src ? src->getDC() : NULL;
  HDC dstDC  = getDC();
  HDC maskDC = prMask->getDC();

  HBITMAP maskBitmap = CreateBitmap(prMask->getWidth(),prMask->getHeight(),1,1,NULL);
  HDC bmDC = CreateCompatibleDC(maskDC);
  SelectObject(bmDC,maskBitmap);

  BOOL ok = BitBlt(bmDC,0,0,prMask->getWidth(),prMask->getHeight(),maskDC,0,0,SRCCOPY);
  if(!ok) {
    errorMsg = getLastErrorText();
  }

//  BitBlt(dstDC,x,y,w,h,maskDC,0,0,SRCCOPY);

  if(ok) {
    BitBlt(dstDC,x,y,w,h,NULL,0,0,DSTINVERT); // this is only necessary, because windows has no DSTCOPY-rasteroperation to use for the 
                                              // background-operation (used for 0-bits in maskBitmap), so we have to invert dst twice. #!&$?
    ok = MaskBlt(dstDC,x,y,w,h,srcDC,sx,sy,maskBitmap,0,0,MAKEROP4(op,DSTINVERT));
    if(!ok) {
      errorMsg = getLastErrorText();
    }
  }

  if(src != NULL) {
    src->releaseDC(srcDC);
  }
  releaseDC(dstDC);
  prMask->releaseDC(maskDC);
  DeleteDC(bmDC);
  DeleteObject(maskBitmap);

  if(!ok) {
    throwException(_T("PixRect::mask failed:%s"), errorMsg.cstr());
  }
}
/*
void PixRect::fastCopy(const CRect &rect, const PixRect *src) {
  DDBLTFX ddbfx;
  ZeroMemory(&ddbfx, sizeof(DDBLTFX));
  ddbfx.dwSize = sizeof(DDBLTFX);
  ddbfx.dwROP  = SRCCOPY;

  if(getArea(rect) != 0) {
    CHECK3DRESULT(m_surface->Blt(&(CRect&)rect, src->m_surface, NULL, DDBLT_ROP | DDBLT_WAIT, &ddbfx));
  }
}

PixRectClipper::PixRectClipper(HWND hwnd) {
  CHECK3DRESULT(PixRect::directDraw->CreateClipper(0, &m_clipper, NULL));
  CHECK3DRESULT(m_clipper->SetHWnd(0, hwnd));
}

PixRectClipper::~PixRectClipper() {
  CHECK3DRESULT(m_clipper->Release());
}

void PixRect::setClipper(PixRectClipper *clipper) {
  if(clipper != NULL) {
    CHECK3DRESULT(m_surface->SetClipper(clipper->m_clipper));
  } else {
    CHECK3DRESULT(m_surface->SetClipper(NULL));
  }
}

void PixRect::copy(VIDEOHDR &videoHeader) {
  CHECK3DRESULT(m_surface->Lock(NULL, &m_ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL));
  memcpy(m_ddsd.lpSurface, videoHeader.lpData, videoHeader.dwBytesUsed); 
  CHECK3DRESULT(m_surface->Unlock(NULL));
}
*/

void PixRect::fill(const CPoint &p, D3DCOLOR color, ColorComparator &cmp) {
  PixelAccessor *pa = getPixelAccessor();
  pa->fill(p, color, cmp);
  delete pa;
}

void PixRect::fill(const CPoint &p, D3DCOLOR color) {
  fill(p,color,SimpleColorComparator());
}

void PixRect::fillTransparent(const CPoint &p, unsigned char alpha) { // alpha = 0 => opaque, 255 = transparent
  if(!contains(p)) {
    return;
  }

  const int width  = getWidth();
  const int height = getHeight();
  PixRect *copy = clone(true);
  D3DCOLOR tmpColor = ~getPixel(p);
  copy->fill(p,tmpColor);
  copy->rop( 0,0,width,height,SRCINVERT,this,0,0);

  PixelAccessor *pa  = getPixelAccessor();
  PixelAccessor *cpa = copy->getPixelAccessor();

  CPoint sp;
  const D3DCOLOR alphaMask = D3DCOLOR_ARGB(alpha, 0, 0, 0);
  for(sp.x = 0; sp.x < width; sp.x++) {
    for(sp.y = 0; sp.y < height; sp.y++) {
      if(cpa->getPixel(p)) {
        D3DCOLOR c = pa->getPixel(sp) & 0x00ffffff;
        pa->setPixel(sp, c | alphaMask);
      }
    }
  }
  delete pa;
  delete cpa;
  delete copy;
}

void PixRect::approximateFill(const CPoint &p, D3DCOLOR color, double tolerance) {
  if(tolerance == 0) {
    tolerance = colorDistance(getPixel(p), color);
  }

  fill(p,color,ApproximateColorComparator(tolerance));
}

void PixRect::line(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert) {
  line(CPoint(x1,y1), CPoint(x2,y2), color, invert);
}

void PixRect::line(const CPoint &p1, const CPoint &p2, D3DCOLOR color, bool invert) {
  if(invert) {
    applyToLine(p1,p2,InvertColor(this));
  } else {
    applyToLine(p1,p2,SetColor(color, this));
  }
}

void PixRect::rectangle(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert) {
  rectangle(CRect(x1,y1,x2,y2),color,invert);
}

void PixRect::rectangle(const CPoint &p0, const CSize &size, D3DCOLOR color, bool invert) {
  rectangle(CRect(p0,size),color,invert);
}

void PixRect::rectangle(const CRect &rect, D3DCOLOR color, bool invert) {
  if(invert) {
    applyToRectangle(rect, InvertColor(this));
  } else {
    applyToRectangle(rect, SetColor(color, this));
  }
}

void PixRect::drawDragRect(CRect *newRect, const CSize &newSize, CRect *lastRect, const CSize &lastSize) {
  HDC hdc = getDC();
  CDC *dc = CDC::FromHandle(hdc);
  CRect tmp;
  if(lastRect) {
    tmp = makePositiveRect(*lastRect);
  }
  dc->DrawDragRect(&makePositiveRect(newRect), newSize,(lastRect!=NULL) ? tmp : NULL, lastSize);
  releaseDC(hdc);
}

void PixRect::polygon(const MyPolygon &polygon, D3DCOLOR color, bool invert, bool closingEdge) {
  if(invert) {
    polygon.applyToEdge(InvertColor(this), closingEdge);
  } else {
    polygon.applyToEdge(SetColor(color, this), closingEdge);
  }
}

void PixRect::ellipse(const CRect &rect, D3DCOLOR color, bool invert) {
  if(invert) {
    applyToEllipse(rect,InvertColor(this));
  } else {
    applyToEllipse(rect,SetColor(color, this));
  }
}

void PixRect::fillRect(const CRect &rect, D3DCOLOR color, bool invert) {
  if(rect.Width() == 0 || rect.Height() == 0) {
    return;
  }
  CRect dstRect = makePositiveRect(rect);
  HDC hdc = getDC();
  if (invert) {
    BitBlt(hdc, dstRect.left, dstRect.top, dstRect.Width(), dstRect.Height(), NULL, 0, 0, DSTINVERT);
  }
  else {
    FillRect(hdc, &dstRect, CreateSolidBrush(D3DCOLOR2COLORREF(color)));
  }
}

void PixRect::fillRect(const CPoint &p0, const CSize &size, D3DCOLOR color, bool invert) {
  fillRect(CRect(p0,size),color,invert);
}

void PixRect::fillRect(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert) {
  fillRect(CRect(x1,y1,x2-x1,y2-y1),color,invert);
}

void PixRect::replicate(int x, int y, int w, int h, const PixRect *src) {
  if(x < 0) {
    w += x;
    x = 0;
  }
  if(y < 0) {
    h += y;
    y = 0;
  }
  if(x >= getWidth() || y >= getHeight()) {
    return;
  }
  w = min(w,getWidth()-x);
  h = min(h,getHeight()-y);
  if(w == 0 || h == 0) {
    return;
  }
  for(int r = 0; r < h; r += src->getHeight()) {
    for(int c = 0; c < w; c += src->getWidth()) {
      rop(x+c,y+r,src->getWidth(),src->getHeight(),SRCCOPY, src,0,0);
    }
  }
}

PixRect &PixRect::apply(PixRectOperator &op) {
  op.setPixRect(this);
  applyToFullRectangle(CRect(CPoint(0,0), getSize()), op);
  op.setPixRect(NULL);
  return *this;
}

PixRect &PixRect::apply(PixRectFilter &filter) {
  filter.setPixRect(this);
  applyToFullRectangle(filter.getRect(),filter);
  if(filter.m_result != this) {
    rop(0,0,getWidth(),getHeight(),SRCCOPY,filter.m_result,0,0);
  }
  filter.setPixRect(NULL);
  return *this;
}

void applyToLine(const CPoint &p1, const CPoint &p2, PointOperator &op) {
  applyToLine(p1.x,p1.y,p2.x,p2.y,op);
}

void applyToLine(int x1, int y1, int x2, int y2, PointOperator &op) {
  if(x1 == x2) {
    if(y1 > y2) {
      swap(y1,y2);
    }
    for(CPoint p = CPoint(x1,y1); p.y <= y2; p.y++) {
      op.apply(p);
    }
  } else if(y1 == y2) {
    if(x1 > x2) {
      swap(x1,x2);
    }
    for(CPoint p = CPoint(x1,y1); p.x <= x2; p.x++) {
      op.apply(p);
    }
  } else {
    const int vx = x2 - x1;
    const int vy = y2 - y1;
    const int dx = sign(vx);
    const int dy = sign(vy);
    for(CPoint p = CPoint(x1,y1), p1 = CPoint(p.x-x1,p.y-y1);;) {
      op.apply(p);
      if(p.x == x2 && p.y == y2) {
        break;
      }
      if(abs((p1.x+dx)*vy - p1.y*vx) < abs(p1.x*vy - (p1.y+dy)*vx)) {
        p.x  += dx;
        p1.x += dx;
      } else {
        p.y  += dy;
        p1.y += dy;
      }
    }
  }
}

void applyToRectangle(const CRect &rect, PointOperator &op) {
  const CRect r = makePositiveRect(rect);
  if(r.left == r.right) {
    if(r.top == r.bottom) {
      op.apply(r.TopLeft());                                // rect contains only 1 pixel
    } else {
      applyToLine(r.left , r.top, r.left  , r.bottom , op); // rect is a vertical line
    }
  } else if(r.top == r.bottom) {
    applyToLine(r.left , r.top   , r.right, r.top    , op); // rect is a horizontal line
  } else {                                                  // rect is a "real" rectangle
    applyToLine(r.left , r.top   , r.right-1, r.top     , op); 
    applyToLine(r.right, r.top   , r.right  , r.bottom-1, op);
    applyToLine(r.right, r.bottom, r.left+1 , r.bottom  , op);
    applyToLine(r.left , r.bottom, r.left   , r.top+1   , op);
  }
}

void applyToFullRectangle(const CRect &rect, PointOperator &op) {
  const CRect r = makePositiveRect(rect);
  CPoint p;
  for(p.y = r.top; p.y < r.bottom; p.y++) {
    for(p.x = r.left; p.x < r.right; p.x++) {
      op.apply(p);
    }
  }
}

static void applyToEllipsePart(const CPoint &start, const CPoint &end, const CPoint &center, PointOperator &op) {
  const int    minx = min(start.x,end.x);
  const int    miny = min(start.y,end.y);
  const int    maxx = max(start.x,end.x);
  const int    maxy = max(start.y,end.y);
  const int    dx   = sign(end.x - start.x);
  const int    dy   = sign(end.y - start.y);
  const int    rx   = max(abs(start.x-center.x),abs(end.x-center.x));
  const int    ry   = max(abs(start.y-center.y),abs(end.y-center.y));
  const double rx2  = rx*rx;
  const double ry2  = ry*ry;
  const double rxy2 = rx2*ry2;

  if(rx == 0 || ry == 0) {
    return;
  }

  CPoint p  = start;
  CPoint pc = p - center;
  while(minx <= p.x && p.x <= maxx && miny <= p.y && p.y <= maxy) {
    if(p == end) {
      break;
    }
    op.apply(p);
    const double d1 = fabs(ry2*sqr(pc.x+dx) + rx2*sqr(pc.y   ) - rxy2);
    const double d2 = fabs(ry2*sqr(pc.x   ) + rx2*sqr(pc.y+dy) - rxy2);
    if(d1 < d2) {
      p.x  += dx;
      pc.x += dx;
    } else {
      p.y  += dy;
      pc.y += dy;
    }
  }
}

void applyToEllipse(const CRect &rect, PointOperator &op) {
  const CRect r = makePositiveRect(rect);
  const CPoint center = r.CenterPoint();
  applyToEllipsePart(CPoint(center.x,r.top   ),CPoint(r.right,center.y ),center, op);
  applyToEllipsePart(CPoint(r.right,center.y ),CPoint(center.x,r.bottom),center, op);
  applyToEllipsePart(CPoint(center.x,r.bottom),CPoint(r.left,center.y  ),center, op);
  applyToEllipsePart(CPoint(r.left,center.y  ),CPoint(center.x,r.top   ),center, op);
}

void applyToBezier(const Point2D &start, const Point2D &cp1, const Point2D &cp2, const Point2D &end, CurveOperator &op, bool applyStart) {
  Point2D p = start;
  if(applyStart) {
    op.apply(start);
  }
  for(float t = 0.1f; t < 1.01; t += 0.1f) {
    float tt = t*t;
    float s  = 1.0f - t;
    float ss = s*s;
    Point2D np = start*(ss*s) + cp1*(3*ss*t) + cp2*(3*s*tt) + end*(t*tt);
    op.apply(np);
    p = np;
  }
}

void SetColor::apply(const CPoint &p) {
  if(m_pixRect->contains(p)) {
    m_pixelAccessor->setPixel(p, m_color);
  }
}

void SetAlpha::apply(const CPoint &p) {
  if(m_pixRect->contains(p)) {
    const D3DCOLOR color  = RGBA_TORGB(m_pixelAccessor->getPixel(p)) | m_alphaMask;
    m_pixelAccessor->setPixel(p, color);
  }
}

void SubstituteColor::apply(const CPoint &p) {
  if(m_pixelAccessor->getPixel(p) == m_from) {
    m_pixelAccessor->setPixel(p,m_to);
  }
}

void InvertColor::apply(const CPoint &p) {
  if(m_pixRect->contains(p)) {
    m_pixelAccessor->setPixel(p,~m_pixelAccessor->getPixel(p));
  }
}

void CurveOperator ::apply(const Point2D &p) {
  if(m_firstTime) {
    m_currentPoint = p;
    m_firstTime = false;
  } else {
    line(m_currentPoint,p);
    m_currentPoint = p;
  }
}

class PolygonFiller : public PixRectOperator {
private:
  MyPolygon &m_polygon;
  void checkAndFill(const CPoint &p);
public:
  Array<CPoint> m_pointsOutside;
  PolygonFiller(PixRect *pixRect, MyPolygon &polygon);
  void apply(const CPoint &p);
  void restoreBlackOutSideRegion();
};

PolygonFiller::PolygonFiller(PixRect *pixRect, MyPolygon &polygon) 
: m_polygon(polygon) {
  setPixRect(pixRect);
}

// Fills BLACK inside polygon, To speed up edge-scanner we fill RED outside, and collect all points used initially to fill 
// these regions. They are afterwards removed (filled whith BLACK) from mask, so mask finally contains WHITE inside (and edge),
// and BLACK outside.
void PolygonFiller::checkAndFill(const CPoint &p) {
  if(!m_pixRect->contains(p)) {
    return;
  }
  if(m_pixelAccessor->getPixel(p) == 0)
    switch(m_polygon.contains(p)) {
    case 1:
      m_pixelAccessor->fill(p,WHITE);
      break;
    case -1:
      m_pixelAccessor->fill(p,D3DCOLOR_XRGB(255,0,0));
      m_pointsOutside.add(p);
      break;
    case 0:
      break;
  }
}

void PolygonFiller::restoreBlackOutSideRegion() {
  for(int i = 0; i < m_pointsOutside.size(); i++) {
    m_pixelAccessor->fill(m_pointsOutside[i],BLACK);
  }
}

void PolygonFiller::apply(const CPoint &p) {
  checkAndFill(CPoint(p.x-1,p.y));
  checkAndFill(CPoint(p.x+1,p.y));
  checkAndFill(CPoint(p.x,p.y-1));
  checkAndFill(CPoint(p.x,p.y+1));
}

void PixRect::fillPolygon(const MyPolygon &polygon, D3DCOLOR color, bool invert) {
  CRect rect = polygon.getBoundsRect();
  MyPolygon poly = polygon;
  poly.move(-rect.TopLeft());
  PixRect *psrc  = new PixRect(m_device, rect.Size(), getPixelFormat());
  PixRect *pmask = new PixRect(m_device, rect.Size(), getPixelFormat());
  psrc->fillRect(0,0,rect.Width(),rect.Height(),color);
  pmask->fillRect(0,0,rect.Width(),rect.Height(),0);   // set mask to black
  pmask->polygon(poly,WHITE);                          // draw white frame around polygon on mask

  PolygonFiller *polygonFiller = new PolygonFiller(pmask, poly);
  poly.applyToEdge(*polygonFiller);
  polygonFiller->restoreBlackOutSideRegion();
  delete polygonFiller;

//  rop(rect.left,rect.top,rect.Width(),rect.Height(),DSTINVERT,NULL,0,0);
//  mask(rect.left,rect.top,rect.Width(),rect.Height(), MAKEROP4(SRCCOPY,DSTINVERT), psrc, 0,0, pmask);
  mask(rect.left,rect.top,rect.Width(),rect.Height(), SRCCOPY, psrc, 0,0, pmask);
  delete pmask;
  delete psrc;
}

void PixRect::fillEllipse(const CRect &rect, D3DCOLOR color, bool invert) {
  CRect r = rect;
  if(rect.Width() == 0 || rect.Height() == 0) {
    return;
  }
  r -= rect.TopLeft();
  PixRect *psrc  = new PixRect(m_device, r.Size(), getPixelFormat());
  PixRect *pmask = new PixRect(m_device, r.Size(), getPixelFormat());
  psrc->fillRect(0,0,r.Width(),r.Height(),color);
  pmask->fillRect(0,0,r.Width(),r.Height(),BLACK);  // set mask to black
  pmask->ellipse(r,WHITE);                          // draw white ellipse on mask
  pmask->fill(r.CenterPoint(),WHITE);

//  rop(rect.left,rect.top,rect.Width(),rect.Height(),DSTINVERT,NULL,0,0);
//  mask(rect.left,rect.top,rect.Width(),rect.Height(), MAKEROP4(SRCCOPY,DSTINVERT), psrc, 0,0, pmask);
  mask(rect.left,rect.top,rect.Width(),rect.Height(), SRCCOPY, psrc, 0,0, pmask);

  delete pmask;
  delete psrc;
}

PixRect *PixRect::mirror(const PixRect *src, bool vertical) {
  PixRect *result = src->clone();
  const int width  = result->getWidth();
  const int height = result->getHeight();
  PixelAccessor *srcPA = PixelAccessor::createPixelAccessor((PixRect*)src);
  PixelAccessor *dstPA = result->getPixelAccessor();
  if(vertical) {
    for(int x1 = 0, x2=width-1; x1 < width; x1++, x2--) {
      for(int y = 0; y < height; y++) {
        dstPA->setPixel(x1,y,srcPA->getPixel(x2,y));
      }
    }
  } else {
    for(int y1 = 0, y2=height-1; y1 < height; y1++, y2--) {
      for(int x = 0; x < width; x++) {
        dstPA->setPixel(x,y1,srcPA->getPixel(x,y2));
      }
    }
  }
  delete srcPA;
  delete dstPA;
  return result;
}

PixRectTextMaker::PixRectTextMaker(PixRect *pixRect, const Point2D &textPos, D3DCOLOR color, bool invert) {
  m_pixRect = pixRect;
  m_textPos = textPos;
  m_color   = color;
  m_invert  = invert;
}

void PixRectTextMaker::line(const Point2D &from, const Point2D &to) {
//  m_pixRect->line(m_glyphPos%from,m_glyphPos%to,m_color,m_invert);
}

void PixRectTextMaker::beginGlyph(const Point2D &offset) {
  m_glyphPos = m_textPos + offset;
}

void PixRect::bezier(const Point2D &start, const Point2D &cp1, const Point2D &cp2, const Point2D &end, D3DCOLOR color, bool invert) {
  applyToBezier(start,cp1,cp2,end,PixRectTextMaker(this,Point2D(0,0), color,invert));
}

void MyPolygon::move(const CPoint &dp) {
  for(int i = 0; i < size(); i++) {
    (*this)[i] += dp;
  }
}

CRect MyPolygon::getBoundsRect() const {
  const Array<CPoint> &a = *this;
  if(a.size() == 0) {
    return CRect(0,0,0,0);
  }
  CRect result(a[0],a[0]);
  for(int i = 1; i < a.size(); i++) {
    const CPoint &p = a[i];
    if(p.x < result.left  ) result.left   = p.x;
    if(p.x > result.right ) result.right  = p.x;
    if(p.y < result.top   ) result.top    = p.y;
    if(p.y > result.bottom) result.bottom = p.y;
  }
  return CRect(result.left,result.top,result.right+1,result.bottom+1);
}

class Vector : public CPoint {
public:
  Vector(const CPoint &from, const CPoint &to);
  double length() const;
};

Vector::Vector(const CPoint &from, const CPoint &to) {
  x = to.x - from.x;
  y = to.y - from.y;
}

double operator*(const Vector &v1, const Vector &v2) {
  return v1.x*v2.x + v1.y*v2.y;
}

double Vector::length() const {
  return sqrt(sqr(x) + sqr(y));
}

static int det(const Vector &v1, const Vector &v2) {
  return v1.x*v2.y - v1.y*v2.x;
}

static double angle(const Vector &v1, const Vector &v2) {
  if(v1 == v2) {
    return 0;
  }
  double a = acos(v1*v2 / v1.length() / v2.length());
  return a * sign(det(v1,v2));
}

bool MyPolygon::add(const CPoint &p) {
  if(size() > 0 && p == last()) {
    return false;
  }
  return Array<CPoint>::add(p);
}

int MyPolygon::contains(const CPoint &p) const { // 1=inside, -1=outside, 0=edge
  unsigned int n = size();
  if(n < 3) {
    return -1;
  }
  Vector v(p,(*this)[0]);
  if(v.x == 0 && v.y == 0) {
    return 0; // edge
  }
  double d = 0;
  for(unsigned int i = 1; i <= n; i++) {
    Vector vnext(p,(*this)[i%n]);
    if(vnext.x == 0 && vnext.y == 0) {
      return 0; // edge
    }
    d += angle(v,vnext);
    v = vnext;
  }
  return fabs(d) > 1 ? 1 : -1; // d always +/- 2PI or 0.
}

class PolygonEdgeOperator : public PointOperator {
  PointOperator &m_op;
  const CPoint  &m_endPoint;
public:
  PolygonEdgeOperator(PointOperator &op, const CPoint &endPoint);
  void apply(const CPoint &p);
};

PolygonEdgeOperator::PolygonEdgeOperator(PointOperator &op, const CPoint &endPoint) : m_op(op), m_endPoint(endPoint) {
}

void PolygonEdgeOperator::apply(const CPoint &p) {
  if(p != m_endPoint) {
    m_op.apply(p);
  }
}

void MyPolygon::applyToEdge(PointOperator &op, bool closingEdge) const {
  int n = size();
  for(int i = 1; i < n; i++) {
    const CPoint &p1 = (*this)[i-1];
    const CPoint &p2 = (*this)[i];
    applyToLine(p1,p2,PolygonEdgeOperator(op,p2));
  }
  if(closingEdge && (n > 2)) {
    const CPoint &p1 = last();
    const CPoint &p2 = (*this)[0];
    applyToLine(p1,p2,PolygonEdgeOperator(op,p2));
  }
}
