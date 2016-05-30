#pragma once

#include <ddraw.h>
#include <d3drm.h>
#include <vfw.h>
#include "Array.h"
#include "Math/Point2D.h"
#include "Math/Rectangle2D.h"

#define WHITE RGB_MAKE(255, 255, 255)
#define BLACK RGB_MAKE(0, 0, 0)

class PixRect;
class PixRectFont;
class GlyphCurveData;

class PointOperator {
public:
  virtual void apply(const CPoint &p) = 0;
};

class ColorComparator {
public:
  virtual bool equals(const D3DCOLOR &c1, const D3DCOLOR &c2) = 0;
};

double colorDistance(const D3DCOLOR &c1, const D3DCOLOR &c2);

class CurveOperator : public Point2DOperator {
private:
  Point2D m_currentPoint;
  bool    m_firstTime;
public:
  CurveOperator() {
    beginCurve();
  }
  
  void apply(const Point2D &p);
  virtual void line(const Point2D &from, const Point2D &to) = 0;
  
  virtual void beginCurve() {
    m_firstTime = true;
  }
  
  virtual void endCurve() {};
  
  const Point2D &getCurrentPoint() const {
    return m_currentPoint;
  }
  
  bool firstPointInCurve() const {
    return m_firstTime;
  }
};

class PointCollector : public CurveOperator {
public:
  Point2DArray m_result;
  void apply(const Point2D &p) {
    m_result.add(p);
  }
  void line(const Point2D &from, const Point2D &to) {
  };
};

class TextOperator : public CurveOperator {
public:
  virtual void beginGlyph(const Point2D &offset) = 0;
  virtual void endGlyph() {
  };
};

class PixelAccessor {
protected:
  PixRect      *m_pixRect;
  DDSURFACEDESC m_ddsd;
public:
  PixelAccessor(PixRect *pixRect);
  virtual ~PixelAccessor();
  static PixelAccessor *createPixelAccessor(PixRect *pixRect);

  virtual void setPixel(unsigned int x, unsigned int y, D3DCOLOR color) = 0;
  virtual D3DCOLOR getPixel(unsigned int x, unsigned int y)             = 0;
  virtual void setPixel(const CPoint &p, D3DCOLOR color)                = 0;
  virtual D3DCOLOR getPixel(const CPoint &p)                            = 0;

  const PixRect *getPixRect() const {
    return m_pixRect;
  }

  void fill(const CPoint &p, D3DCOLOR color, ColorComparator &cmp);
  void fill(const CPoint &p, D3DCOLOR color);
};

class PixRectOperator : public PointOperator {
private:
  void init();
protected:
  PixRect       *m_pixRect;
  PixelAccessor *m_pixelAccessor;
public:
  PixRectOperator(PixRect *pr = NULL);
  virtual ~PixRectOperator();
  virtual void setPixRect(PixRect *pixRect);
};

class PixRectFilter : public PixRectOperator {
private:
  void init();
protected:
  PixRect       *m_result;
  PixelAccessor *m_resultPixelAccessor;
  friend class PixRect;
public:
  PixRectFilter() { init(); }
  void setPixRect(PixRect *pixRect); // set m_result = m_pixRect = pixRect
  virtual CRect getRect() const; // returns default CRect(0, 0, m_pixRrect->getWidth(), m_pixRect->getHeight())
};

class SetColor : public PixRectOperator {
private:
  const D3DCOLOR m_color;
public:
  SetColor(D3DCOLOR color, PixRect *pr = NULL) : m_color(color), PixRectOperator(pr) {
  }
  void apply(const CPoint &p);
};

class SetAlpha : public PixRectOperator {
private:
  const unsigned char m_alpha;
public:
  SetAlpha(unsigned char alpha, PixRect *pr = NULL) : m_alpha(alpha), PixRectOperator(pr) {
  }
  void apply(const CPoint &p);
};

class InvertColor : public PixRectOperator {
public:
  InvertColor(PixRect *pr = NULL) : PixRectOperator(pr) {
  }
  void apply(const CPoint &p);
};

class SubstituteColor : public PixRectOperator {
private:
  const D3DCOLOR m_from, m_to;
public:
  SubstituteColor(D3DCOLOR from, D3DCOLOR to, PixRect *pr = NULL) : m_from(from), m_to(to), PixRectOperator(pr) {
  }
  void apply(const CPoint &p);
};

class GrayScaleFilter : public PixRectFilter {
public:
  void apply(const CPoint &p);
};

class SobelFilter : public PixRectFilter {
protected:
  int Gx[3][3], Gy[3][3];
public:
  SobelFilter();
  void setPixRect(PixRect *src);
  CRect getRect() const;
  void apply(const CPoint &p);
};

class LaplaceFilter : public PixRectFilter {
private:
  int MASK[5][5];
public:
  LaplaceFilter();
  void setPixRect(PixRect *src);
  CRect getRect() const;
  void apply(const CPoint &p);
};

class GaussFilter : public PixRectFilter {
private:
  int MASK[5][5];
public:
  GaussFilter();
  void setPixRect(PixRect *src);
  CRect getRect() const;
  void apply(const CPoint &p);
};

class EdgeDirectionFilter : public SobelFilter {
public:
  void setPixRect(PixRect *src);
  CRect getRect() const;
  void apply(const CPoint &p);
};

class CannyEdgeFilter : public PixRectFilter {
public:
  void setPixRect(PixRect *src);
  CRect getRect() const;
  void apply(const CPoint &p);
};

class PixRectTextMaker : public TextOperator {
private:
  PixRect *m_pixRect;
  D3DCOLOR m_color;
  bool     m_invert;
  Point2D  m_textPos, m_glyphPos;
public:
  void line(const Point2D &from, const Point2D &to);
  void beginGlyph(const Point2D &offset);
  PixRectTextMaker(PixRect *pixRect, const Point2D &textPos, D3DCOLOR color, bool invert);
};

class ScaleParameters {
public:
  ScaleParameters();
  ScaleParameters(bool toSize, const Point2D &scale);
  ScaleParameters(bool toSize, double x, double y); // if(toSize) newImage.size = CSize(x, y);
                                                    // else       newImage.size = CSize(oldImage.size.cx*x, oldImage.size.cy*y);
  bool    m_toSize;
  Point2D m_scale;
};

class MyPolygon : public Array<CPoint> {
public:
  void move(const CPoint &dp);
  CRect getBoundsRect() const;
  int contains(const CPoint &p) const; // 1=inside, -1=outside, 0=edge
  void applyToEdge(PointOperator &f, bool closingEdge = true) const;
  bool add(const CPoint &p);
};

class PixRectClipper {
private:
  LPDIRECTDRAWCLIPPER m_clipper;
  friend class PixRect;
public:
  PixRectClipper(HWND hwnd);
 ~PixRectClipper();
};

class PixRect {
private:
  static LPDIRECTDRAW  directDraw;
  static DDPIXELFORMAT defaultPixelFormat; // same format as the screen

  static void initialize();
  static void uninitialize();

  friend class InitPixRect;
  friend class PixelAccessor;;
  friend class PixRectOperator;
  friend class PixRectClipper;

  DDSURFACEDESC       m_ddsd;
  LPDIRECTDRAWSURFACE m_surface;

  static LPDIRECTDRAWSURFACE newSurface(DDSURFACEDESC &ddsd);
  void createSurface(int width, int height, const DDPIXELFORMAT &pixelFormat);
  void destroySurface();

  void drawEllipsePart(const CPoint &start, const CPoint &end, CPoint &center, D3DCOLOR color, bool invert);
  void fill(const CPoint &p, D3DCOLOR color, ColorComparator &cmp);
  void init(HBITMAP src, const DDPIXELFORMAT &pixelFormat);
  void checkHasAlphaChannel() const; // throw Exception if no alpha-channel
public:
  static void reOpenDirectDraw();
  PixRect();                              // create a PixRect to draw directly on the screen
  PixRect(const PixRect &src);            // not defined
  PixRect &operator=(const PixRect &src); // do
  PixRect(unsigned int width, unsigned int height);
  PixRect(unsigned int width, unsigned int height, const DDPIXELFORMAT &pixelFormat);
  PixRect(const CSize &size);
  PixRect(const CSize &size, const DDPIXELFORMAT &pixelFormat);
  PixRect(HBITMAP src);
  PixRect(HBITMAP src, const DDPIXELFORMAT &pixelFormat);
  virtual ~PixRect();
  PixRect *clone(bool cloneImage=false) const;
  static void PixRect::showPixRect(PixRect *pr);

  Array<DDSURFACEDESC> getDisplayModes() const;
  static DDCAPS getDriverCaps();
  static DDCAPS getEmulatorCaps();
  PixelAccessor *getPixelAccessor() {
    return PixelAccessor::createPixelAccessor(this);
  }

  static const DDPIXELFORMAT &getDefaultPixelFormat() {
    return defaultPixelFormat;
  }

  LPDIRECTDRAWSURFACE cloneSurface() const;

  int getWidth()  const {
    return m_ddsd.dwWidth;
  }
  
  int getHeight() const {
    return m_ddsd.dwHeight;
  }
  
  CSize getSize() const {
    return CSize(getWidth(), getHeight());
  }

  void setSize(const CSize &size); // in pixels

  CSize getSizeInMillimeters(HDC hdc = NULL) const;

  CRect getRect() const {
    return CRect(0, 0, getWidth(), getHeight());
  }

  void setPixel(unsigned int x, unsigned int y, D3DCOLOR color);
  D3DCOLOR getPixel(unsigned int x, unsigned int y) const;
  void     setPixel(const CPoint &p, D3DCOLOR color);
  D3DCOLOR getPixel(const CPoint &p) const;
  D3DCOLOR getAverageColor(const Rectangle2D &rect) const;
  D3DCOLOR getAverageColor() const;

  void line(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert=false);
  void line(const CPoint &p1, const CPoint &p2, D3DCOLOR color, bool invert=false);
  void rectangle(const CRect &rect, D3DCOLOR color, bool invert=false);
  void rectangle(const CPoint &p0, const CSize &size, D3DCOLOR color, bool invert=false);
  void rectangle(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert=false);
  void drawDragRect(CRect *newRect, const CSize &newSize, CRect *lastRect, const CSize &lastSize);

  void polygon(const MyPolygon &polygon, D3DCOLOR color, bool invert=false, bool closingEdge=true);
  void ellipse(const CRect &rect, D3DCOLOR color, bool invert=false);
  void bezier(const Point2D &start, const Point2D &cp1, const Point2D &cp2, const Point2D &end, D3DCOLOR color, bool invert=false);
  void fillRect(const CRect &rect, D3DCOLOR color, bool invert=false);
  void fillRect(const CPoint &p0, const CSize &size, D3DCOLOR color, bool invert=false);
  void fillRect(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert=false);
  void fillPolygon(const MyPolygon &polygon, D3DCOLOR color, bool invert=false);
  void fillEllipse(const CRect &rect, D3DCOLOR color, bool invert=false);
  static PixRect *mirror(     const PixRect *src, bool vertical);
  static PixRect *rotateImage(const PixRect *src, D3DCOLOR background, double degree);
  static PixRect *scaleImage( const PixRect *src, const ScaleParameters &param);
  static CSize    getRotatedSize(const CSize &size, double degree);
  void text(const CPoint &p, const char *text, const PixRectFont &font, D3DCOLOR color, bool invert=false);
  void drawGlyph(const CPoint &p, const GlyphCurveData &glyphCurve, D3DCOLOR color, bool invert=false);
  void drawText(const CPoint &p, const char *text, const PixRectFont &font, D3DCOLOR color, bool invert=false);
  void copy(VIDEOHDR &videoHeader);  

  inline bool contains(const CPoint &p) const {
    return (unsigned long)p.x < m_ddsd.dwWidth && (unsigned long)p.y < m_ddsd.dwHeight;
  }

  inline bool contains(unsigned int x, unsigned int y) const {
    return x < m_ddsd.dwWidth && y < m_ddsd.dwHeight;
  }
  bool contains(        const Point2D &p) const;
  bool containsExtended(const Point2D &p) const;

  
  void rop( const CRect  &dr,                    unsigned long op, const PixRect *src, const CPoint &sp);
  void rop( const CRect  &dr,                    unsigned long op, const PixRect *src, const CRect  &sr);
  void rop( const CPoint &dp, const CSize &size, unsigned long op, const PixRect *src, const CPoint &sp);
  void rop( int x, int y, int w, int h,          unsigned long op, const PixRect *src, int sx, int sy);
  void mask(const CRect  &dr,                    unsigned long op, const PixRect *src, const CPoint &sp, const PixRect *prMask);
  void mask(const CPoint &dp, const CSize &size, unsigned long op, const PixRect *src, const CPoint &sp, const PixRect *prMask);
  void mask(int x, int y, int w, int h,          unsigned long op, const PixRect *src, int sx, int sy,   const PixRect *prMask);

  static void bitBlt(    HDC      dst, int x, int y, int w, int h,         unsigned long op, const PixRect *src, int sx, int sy);
  static void bitBlt(    PixRect *dst, int x, int y, int w, int h,         unsigned long op,       HDC      src, int sx, int sy);
  static void bitBlt(    HDC      dst, const CPoint &p, const CSize &size, unsigned long op, const PixRect *src, const CPoint &sp);
  static void bitBlt(    PixRect *dst, const CPoint &p, const CSize &size, unsigned long op,       HDC      src, const CPoint &sp);
  static void stretchBlt(HDC      dst, int x, int y, int w, int h,         unsigned long op, const PixRect *src, int sx, int sy, int sw, int sh);
  static void stretchBlt(PixRect *dst, int x, int y, int w, int h,         unsigned long op, const HDC      src, int sx, int sy, int sw, int sh);
  static void stretchBlt(HDC      dst, const CPoint &p, const CSize &size, unsigned long op, const PixRect *src, const CPoint &sp, const CSize &ssize);
  static void stretchBlt(PixRect *dst, const CPoint &p, const CSize &size, unsigned long op, const HDC      src, const CPoint &sp, const CSize &ssize);
  static void stretchBlt(HDC      dst, const CRect &dstRect,               unsigned long op, const PixRect *src, const CRect  &sr);
  static void stretchBlt(PixRect *dst, const CRect &dstRect,               unsigned long op, const HDC      src, const CRect  &sr);
  static void alphaBlend(PixRect &dst, int x, int y, int w, int h,  const PixRect &src, int sx, int sy, int sw, int sh, int srcConstAlpha);
  static void alphaBlend(PixRect &dst, const CRect &dstRect,        const PixRect &src, const CRect &srcRect,           int srcConstAlpha);
  static void alphaBlend(HDC      dst, int x, int y, int w, int h,  const PixRect &src, int sx, int sy, int sw, int sh, int srcConstAlpha);
  static void alphaBlend(HDC      dst, const CRect &dstRect,        const PixRect &src, const CRect &srcRect,           int srcConstAlpha);

  void fastCopy(const CRect &rect, const PixRect *src);
  void replicate(int x, int y, int w, int h, const PixRect *src);
  void fill(const CPoint &p, D3DCOLOR color);
  void fillTransparent(const CPoint &p, unsigned char alpha=255); // alpha = 0 => transparent, 255 = opaque
  void approximateFill(const CPoint &p, D3DCOLOR color, double tolerance);
  void preMultiplyAlpha();
  PixRect &apply(PixRectOperator &op    );                                                // return *this
  PixRect &apply(PixRectFilter   &filter);                                                // return *this

  const DDPIXELFORMAT &getPixelFormat() const;
  static DDPIXELFORMAT getPixelFormat(const BITMAP &bm);

  void setClipper(PixRectClipper *clipper);

  PixRect &operator=(HBITMAP src);
  operator HBITMAP() const;

  HDC getDC() const {
    HDC dc;
    m_surface->GetDC(&dc);
    return dc;
  }

  void releaseDC(HDC dc) const {
    m_surface->ReleaseDC(dc);
  }

  void fromBitmap( CBitmap &src);
  void toBitmap(   CBitmap &dst) const;

  void writeAsBMP( const String &fileName);
  void writeAsBMP( FILE *f);
  void writeAsJPG( const String &fileName);
  void writeAsJPG( FILE *f);
  void writeAsPNG( const String &fileName);
  void writeAsPNG( FILE *f);
  void writeAsTIFF(const String &fileName);
  void writeAsTIFF(FILE *f);

  static PixRect *load(const String &fileName);
  static PixRect *load(FILE *f);

  friend bool operator==(const PixRect &p1, const PixRect &p2);
  friend bool operator!=(const PixRect &p1, const PixRect &p2);
};


class PolygonCurve {
private:
  short          m_type; // TT_PRIM_LINE, TT_PRIM_QSPLINE or TT_PRIM_CSPLINE
  Point2DArray   m_points;
public:
  void addPoint(const Point2D &p) {
    m_points.add(p);
  }

  PolygonCurve(short type) {
    m_type = type;
  }

  Rectangle2D getBoundingBox() const;
  inline const Point2DArray &getAllPoints() const {
    return m_points;
  }
  void move(const Point2D &dp);
  inline short getType() const {
    return m_type;
  }
  String toString() const;
  String toXML();
};

class GlyphPolygon {
public:
  Point2D             m_start;
  Array<PolygonCurve> m_polygonCurveArray;
  
  GlyphPolygon(const Point2D &start) {
    m_start = start;
  }

  void addCurve(PolygonCurve &curve) {
    m_polygonCurveArray.add(curve);
  }
  
  Rectangle2D getBoundingBox() const;
  Point2DArray getAllPoints() const;
  void move(const Point2D &dp);
  String toString() const;
  String toXML();
};

class GlyphCurveData {
private:
  Array<GlyphPolygon> m_glyphPolygonArray;
public:

  GlyphCurveData(HDC hdc, unsigned char ch, const MAT2 &m);
  GlyphCurveData();

  String toString() const;
  String toXML();
  Rectangle2D getBoundingBox() const;
  Point2DArray getAllPoints() const;
  Point2DArray getLinePoints() const;
  
  void addPolygon(const GlyphPolygon &polygon) {
    m_glyphPolygonArray.add(polygon);
  }
  
  void addLine(const Point2D &p1, const Point2D &p2);
  void move(const Point2D &dp);
  const Array<GlyphPolygon> &getPolygonArray() const {
    return m_glyphPolygonArray;
  }
};

class GlyphData {
public:
  PixRect         *m_pixRect;
  GLYPHMETRICS     m_metrics;
  GlyphCurveData   m_glyphCurveData;
  unsigned char    m_ch;

  GlyphData(HDC hdc, unsigned char ch, const MAT2 &m);
  ~GlyphData();
};

class PixRectFont {
private:
  void initGlyphData(float orientation);
protected:
  CFont            m_font;
  TEXTMETRIC       m_textMetrics;
  GlyphData       *m_glyphData[256];
public:
  PixRectFont();
  PixRectFont(const LOGFONT &logfont, float orientation=0);
 ~PixRectFont();

  void getLogFont(LOGFONT &logfont);
  
  CFont &getFont() {
    return m_font;
  }
  
  const TEXTMETRIC getTextMetrics() const {
    return m_textMetrics;
  }
  
  const GlyphData *getGlyphData(unsigned char index) const;
};

void applyToLine(int x1, int y1, int x2, int y2, PointOperator &op);
void applyToLine(const CPoint &p1, const CPoint &p2, PointOperator &op);
void applyToRectangle(const CRect &rect, PointOperator &op);
void applyToFullRectangle(const CRect &rect, PointOperator &op);
void applyToEllipse(const CRect &rect, PointOperator &op);
void applyToBezier(const Point2D &start, const Point2D &cp1, const Point2D &cp2, const Point2D &end, CurveOperator &op, bool applyStart=true);
void applyToGlyphPolygon(const GlyphPolygon &glyphPolygon, CurveOperator &op);
void applyToGlyph(const GlyphCurveData &glyphCurve, CurveOperator &op);
void applyToText(const char *text, const PixRectFont &font, TextOperator &op);

class DebugPixRect {
public:
  static void showPixRect(const PixRect *pr); // paint pr in upper left corner of the screen.
};
