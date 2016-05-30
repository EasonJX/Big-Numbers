#include "stdafx.h"
#include <MyUtil.h>
#include <FileContent.h>
#include <MFCUtil/WinTools.h>

static HICON createImageHandle(HINSTANCE hInstance, bool isIcon, int colorResId, int maskResId, int xHotSpot=0, int yHotSpot=0) {
  HBITMAP colorBitmap = 0, maskBitmap = 0;
  HICON   icon = 0;
  try {
    colorBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(colorResId));
    if(colorBitmap == NULL) {
      throwLastErrorOnSysCallException(_T("LoadBitmap"));
    }
    HBITMAP maskBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(maskResId));
    if(maskBitmap == NULL) {
      throwLastErrorOnSysCallException(_T("LoadBitmap"));
    }

    ICONINFO iconInfo;
    iconInfo.fIcon    = isIcon;
    iconInfo.xHotspot = xHotSpot;
    iconInfo.yHotspot = yHotSpot;
    iconInfo.hbmColor = colorBitmap;
    iconInfo.hbmMask  = maskBitmap;
    HICON icon = CreateIconIndirect(&iconInfo);

    if(icon == NULL) {
      throwLastErrorOnSysCallException(_T("CreateIconIndirect"));
    }
    DeleteObject(colorBitmap);
    DeleteObject(maskBitmap);
    return icon;
  } catch(...) {
    if(colorBitmap) {
      DeleteObject(colorBitmap);
    }
    if(maskBitmap) {
      DeleteObject(maskBitmap);
    }
    if(icon) {
      DestroyIcon(icon);
    }
    throw;
  }
}

HCURSOR createCursor(HINSTANCE hInstance, int colorResId, int maskResId, int xHotSpot, int yHotSpot) {
  return (HCURSOR)createImageHandle(hInstance, false, colorResId, maskResId, xHotSpot, yHotSpot);
}

HICON createIcon(HINSTANCE hInstance, int colorResId, int maskResId) {
  return createImageHandle(hInstance, true, colorResId, maskResId);
}

HICON createIcon(const ByteArray &bytes) {
  HICON icon = CreateIconFromResource((BYTE*)bytes.getData(), bytes.size(), TRUE, 0x00030000);
  if(icon == NULL) {
    throwLastErrorOnSysCallException(_T("CreateIconFromResource"));
  }
  return icon;
}

HICON loadIcon(FILE *f) {
  return createIcon(FileContent(f));
}

HICON loadIcon(const String &fileName) {
  return createIcon(FileContent(fileName));
}

/*
Use DrawIconEx instead
void paintIcon(HICON icon, HDC dc, CPoint &p, COLORREF bgColor) {
  ICONINFO    iconInfo  = getIconInfo(icon);;
  HDC         srcDC     = CreateCompatibleDC(NULL);
  HGDIOBJ     oldSrcGDI = SelectObject(srcDC, iconInfo.hbmColor);
  HBRUSH      brush     = CreateSolidBrush(bgColor);
  HGDIOBJ     oldDstGDI = SelectObject(dc, brush);
  const CSize sz        = getBitmapSize(iconInfo.hbmColor);

  MaskBlt(dc, p.x,p.y, sz.cx, sz.cy, srcDC, 0,0, iconInfo.hbmMask, 0,0, MAKEROP4(PATCOPY, SRCCOPY));

  SelectObject(dc   , oldDstGDI);
  DeleteObject(brush);
  SelectObject(srcDC, oldSrcGDI);
  DeleteDC(srcDC);
  closeIconInfo(iconInfo);
}
*/
