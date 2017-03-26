#pragma once

#include <D3D9.h>
#include <D3DX9.h>
#include <Math/MathLib.h>
#include <Math/Spherical.h>
#include <Math/Point2D.h>
#include "Math/Point3D.h"
#include "D3Error.h"

//#define LEFTHANDED 

#ifdef LEFTHANDED

#define D3DXMatrixPerspectiveFov(mat , angel, apsect, zn, fn)  D3DXMatrixPerspectiveFovLH(mat,angel,apsect,zn,fn)
#define D3DXMatrixLookAt(        view, pos  , lookAt, up)      D3DXMatrixLookAtLH(view, pos, lookAt, up)

#else

#define D3DXMatrixPerspectiveFov(mat , angel, apsect, zn, fn)  D3DXMatrixPerspectiveFovRH(mat,angel,apsect,zn,fn)
#define D3DXMatrixLookAt(        view, pos  , lookAt, up)      D3DXMatrixLookAtRH(view, pos, lookAt, up)

#endif


class ParametricSurface : public FunctionTemplate<Point2D, Point3D> {
};

float       operator*(   const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2);
float       length(      const D3DXVECTOR3 &v);
float       angle(       const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2);
D3DXVECTOR3 unitVector(  const D3DXVECTOR3 &v);
D3DXVECTOR3 createUnitVector(int i); // i = [0..2] giving ((1,0,0) or (0,1,0) or (0,0,1)
D3DXVECTOR3 rotate(      const D3DXVECTOR3 &v , const D3DXVECTOR3 &axes, double rad);
D3DXVECTOR3 crossProduct(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2);
D3DXVECTOR3 randomUnitVector();
D3DXVECTOR3 ortonormalVector(const D3DXVECTOR3 &v);
String      toString(    const D3DXVECTOR3 &v, int dec = 3);
String      toString(    const D3DXVECTOR4 &v, int dec = 3);
D3DXMATRIX  transpose( const D3DXMATRIX  &m);
D3DXMATRIX  invers(    const D3DXMATRIX  &m);
D3DXMATRIX  createIdentityMatrix();
D3DXMATRIX  createTranslateMatrix(const D3DXVECTOR3 &v);
D3DXMATRIX  createScaleMatrix(    const D3DXVECTOR3 &s);
D3DXMATRIX  createRotationMatrix( const D3DXVECTOR3 &axis, double rad);
D3DXVECTOR3 operator*( const D3DXMATRIX  &m , const D3DXVECTOR3 &v);
D3DXVECTOR3 operator*( const D3DXVECTOR3 &v,  const D3DXMATRIX  &m);
float       det(       const D3DXMATRIX  &m);
String      toString(  const D3DXMATRIX  &m, int dec = 3);

inline float dist(const D3DXVECTOR3 &p1, const D3DXVECTOR3 &p2) {
  return length(p1 - p2);
}

#define     radians(x) D3DXToRadian(x)
#define     degrees(x) D3DXToDegree(x)

typedef struct Vertex {
  float x, y, z;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ
  };
  inline Vertex() {
  }
  inline Vertex(float  _x, float  _y, float  _z) { x  = _x ; y  = _y ; z  = _z ; }
  inline Vertex(double _x, double _y, double _z) { x  = (float)_x ; y  = (float)_y ; z  = (float)_z ; }
  inline Vertex(     const Point3D     &p)       { x  = (float)p.x; y  = (float)p.y; z  = (float)p.z; }
  inline Vertex(     const D3DXVECTOR3 &v)       { x  = v.x; y  = v.y; z  = v.z; }
  inline void setPos(const D3DXVECTOR3 &v)       { x  = v.x; y  = v.y; z  = v.z; }
  inline operator D3DXVECTOR3() const {
    return D3DXVECTOR3(x,y,z);
  }
  inline Vertex operator-() const { return Vertex(-x,-y,-z); } 
} Vertex;

Vertex createVertex(double x, double y, double z);

class TextureVertex {
public:
  float u, v;
  inline String toString(int dec = 3) const {
    return format(_T("%*f %*f"), dec, u, dec, v);
  }
};

typedef struct {
  float x, y, z;
  float nx, ny, nz;
//  DWORD diffuse, specular;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL/* | D3DFVF_DIFFUSE | D3DFVF_SPECULAR*/
  };
  inline void setPos(double _x, double _y, double _z)    { x  = (float)_x;  y  = (float)_y;  z  =  (float)_z; }
  inline void setPos(   const Vertex      &v)            { x  = v.x; y  = v.y; z  = v.z; }
  inline void setPos(   const D3DXVECTOR3 &v)            { x  = v.x; y  = v.y; z  = v.z; }
  inline void setPos(   const Point3D     &p)            { x  = (float)p.x; y  = (float)p.y; z  = (float)p.z; }

  inline void setNormal(double _x, double _y, double _z) { nx =  (float)_x; ny =  (float)_y; nz =  (float)_z; }
  inline void setNormal(const D3DXVECTOR3 &n)            { nx = n.x; ny = n.y; nz = n.z; }
  inline void setNormal(const Point3D     &n)            { nx = (float)n.x; ny = (float)n.y; nz = (float)n.z; }

  inline void setPosAndNormal(const Vertex      &v, const D3DXVECTOR3 &n, D3DCOLOR diffuse=-1) { // ignore diffuse
    setPos(v); setNormal(n);
  }
  inline void setPosAndNormal(const D3DXVECTOR3 &v, const D3DXVECTOR3 &n, D3DCOLOR diffuse=-1) { // ignore diffuse
    setPos(v); setNormal(n);
  }
  inline void setPosAndNormal(const Point3D     &v, const D3DXVECTOR3 &n, D3DCOLOR diffuse=-1) { // ignore diffuse
    setPos(v); setNormal(n);
  }
  inline void reverseNormal() { nx*=-1; ny*=-1; nz*=-1; }
} VertexNormal;

typedef struct {
  float x, y, z;
  float nx, ny, nz;
  DWORD diffuse;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE
  };
  inline void setPos(double _x, double _y, double _z)    { x  =  (float)_x; y  =  (float)_y; z  =  (float)_z; }
  inline void setPos(   const Vertex      &v)            { x  = v.x; y  = v.y; z  = v.z; }
  inline void setPos(   const D3DXVECTOR3 &v)            { x  = v.x; y  = v.y; z  = v.z; }
  inline void setPos(   const Point3D     &p)            { x  = (float)p.x; y  = (float)p.y; z  = (float)p.z; }

  inline void setNormal(double _x, double _y, double _z) { nx =  (float)_x; ny =  (float)_y; nz =  (float)_z; }
  inline void setNormal(const D3DXVECTOR3 &n)            { nx = n.x; ny = n.y; nz = n.z; }
  inline void setNormal(const Point3D     &n)            { nx = (float)n.x; ny = (float)n.y; nz = (float)n.z; }

  inline void setPosAndNormal(const Vertex      &v, const D3DXVECTOR3 &n, D3DCOLOR _diffuse) { 
    setPos(v); setNormal(n);
    diffuse = _diffuse;
  }
  inline void setPosAndNormal(const D3DXVECTOR3 &v, const D3DXVECTOR3 &n, D3DCOLOR _diffuse) { 
    setPos(v); setNormal(n);
    diffuse = _diffuse;
  }
  inline void setPosAndNormal(const Point3D     &v, const D3DXVECTOR3 &n, D3DCOLOR _diffuse) {
    setPos(v); setNormal(n);
    diffuse = _diffuse;
  }
  inline void reverseNormal() { nx*=-1; ny*=-1; nz*=-1; }
} VertexNormalDiffuse;

typedef struct Line {
  Vertex m_p1, m_p2;
  inline Line() {}
  inline Line(const Vertex &p1, const Vertex &p2) : m_p1(p1), m_p2(p2) {
  }
  inline Line(const Point3D &p1, const Point3D &p2) : m_p1(p1), m_p2(p2) {
  }
} Line;

typedef CompactArray<Vertex>     VertexArray;
typedef Array<VertexArray>       CurveArray;

class Point3DP : public Point3D {
public:
  inline Point3DP() {
  }
  inline Point3DP(double _x, double _y, double _z) : Point3D(_x, _y, _z) {
  }
  inline Point3DP(const Point3D &p) : Point3D(p) {
  }
  inline Point3DP(const D3DXVECTOR3 &v) : Point3D(v.x, v.y, v.z) {
  }
  inline operator D3DXVECTOR3() const {
    return D3DXVECTOR3((float)x, (float)y, (float)z);
  }
};

class D3PosDirUpScale {
  D3DXVECTOR3 m_pos, m_dir, m_up, m_scale;
  D3DXMATRIX  m_view;
  void updateView();
  void dumpWorld();
public:
  D3PosDirUpScale();
  const D3DXVECTOR3 &getPos() const {
    return m_pos;
  }
  const D3DXVECTOR3 &getDir() const {
    return m_dir;
  }
  const D3DXVECTOR3 &getUp() const {
    return m_up;
  }
  D3DXVECTOR3 getRight() const {
    return crossProduct(m_dir, m_up);
  }
  const D3DXVECTOR3 &getScale() const {
    return m_scale;
  }
  void setPos(const D3DXVECTOR3 &pos);
  void setOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up);
  void setScale(const D3DXVECTOR3 &scale);

  D3DXMATRIX  getWorldMatrix()    const;
  D3DXMATRIX  getViewMatrix()     const;
  D3DXMATRIX  getRotationMatrix() const;
  D3DXMATRIX  getScaleMatrix()    const;
  
  void setWorldMatrix(const D3DXMATRIX &world);
//  void setViewMatrix( const D3DXMATRIX &view );

  String toString() const;
  inline bool operator==(const D3PosDirUpScale &pdus) const {
    return m_pos == pdus.m_pos && m_dir == pdus.m_dir && m_up == pdus.m_up && m_scale == pdus.m_scale;
  }
  inline bool operator!=(const D3PosDirUpScale &pdus) const {
    return !(*this == pdus);
  }
};

class D3Ray {
public:
  D3DXVECTOR3 m_orig;
  D3DXVECTOR3 m_dir;
  D3Ray() : m_orig(0, 0, 0), m_dir(0, 0, 0) {
  }
  D3Ray(const D3DXVECTOR3 &orig, const D3DXVECTOR3 &m_dir) : m_orig(orig), m_dir(unitVector(m_dir)) {
  }
  void clear() {
    m_dir = m_orig = D3DXVECTOR3(0,0,0);
  }
  bool isSet() const {
    return length(m_dir) > 0;
  }
  String toString() const {
    return format(_T("Orig:(%s), Dir:(%s)"), ::toString(m_orig).cstr(), ::toString(m_dir).cstr());;
  }
};

class D3Spherical : public Spherical {
public:
  inline D3Spherical() : Spherical() {
  }
  inline D3Spherical(const Point3D &p) : Spherical(p) {
  }
  inline D3Spherical(double x, double y, double z) : Spherical(x,y,z) {
  }
  inline D3Spherical(const D3DXVECTOR3 &v) {
    init(v.x, v.y, v.z);
  }
  operator D3DXVECTOR3() const;
};

#include "PragmaLib.h"
