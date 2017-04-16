#include "pch.h"
#include <D3DGraphics/D3ToString.h>

#define caseStr(f) case f: return _T(#f)

String toString(D3DFILLMODE mode) {
  switch(mode) {
  caseStr(D3DFILL_POINT    );
  caseStr(D3DFILL_WIREFRAME);
  caseStr(D3DFILL_SOLID    );
  default: return format(_T("Unknown fillMode:%d"), mode);
  }
}

String toString(D3DSHADEMODE mode) {
  switch(mode) {
  caseStr(D3DSHADE_FLAT    );
  caseStr(D3DSHADE_GOURAUD );
  caseStr(D3DSHADE_PHONG   );
  default: return format(_T("Unknown shadeMode:%d"), mode);
  }
}

String toString(D3DFORMAT f) {
  switch(f) {
  caseStr(D3DFMT_UNKNOWN              );
  caseStr(D3DFMT_R8G8B8               );
  caseStr(D3DFMT_A8R8G8B8             );
  caseStr(D3DFMT_X8R8G8B8             );
  caseStr(D3DFMT_R5G6B5               );
  caseStr(D3DFMT_X1R5G5B5             );
  caseStr(D3DFMT_A1R5G5B5             );
  caseStr(D3DFMT_A4R4G4B4             );
  caseStr(D3DFMT_R3G3B2               );
  caseStr(D3DFMT_A8                   );
  caseStr(D3DFMT_A8R3G3B2             );
  caseStr(D3DFMT_X4R4G4B4             );
  caseStr(D3DFMT_A2B10G10R10          );
  caseStr(D3DFMT_A8B8G8R8             );
  caseStr(D3DFMT_X8B8G8R8             );
  caseStr(D3DFMT_G16R16               );
  caseStr(D3DFMT_A2R10G10B10          );
  caseStr(D3DFMT_A16B16G16R16         );
  caseStr(D3DFMT_A8P8                 );
  caseStr(D3DFMT_P8                   );
  caseStr(D3DFMT_L8                   );
  caseStr(D3DFMT_A8L8                 );
  caseStr(D3DFMT_A4L4                 );
  caseStr(D3DFMT_V8U8                 );
  caseStr(D3DFMT_L6V5U5               );
  caseStr(D3DFMT_X8L8V8U8             );
  caseStr(D3DFMT_Q8W8V8U8             );
  caseStr(D3DFMT_V16U16               );
  caseStr(D3DFMT_A2W10V10U10          );
  caseStr(D3DFMT_UYVY                 );
  caseStr(D3DFMT_R8G8_B8G8            );
  caseStr(D3DFMT_YUY2                 );
  caseStr(D3DFMT_G8R8_G8B8            );
  caseStr(D3DFMT_DXT1                 );
  caseStr(D3DFMT_DXT2                 );
  caseStr(D3DFMT_DXT3                 );
  caseStr(D3DFMT_DXT4                 );
  caseStr(D3DFMT_DXT5                 );
  caseStr(D3DFMT_D16_LOCKABLE         );
  caseStr(D3DFMT_D32                  );
  caseStr(D3DFMT_D15S1                );
  caseStr(D3DFMT_D24S8                );
  caseStr(D3DFMT_D24X8                );
  caseStr(D3DFMT_D24X4S4              );
  caseStr(D3DFMT_D16                  );
  caseStr(D3DFMT_D32F_LOCKABLE        );
  caseStr(D3DFMT_D24FS8               );
  caseStr(D3DFMT_D32_LOCKABLE         );
  caseStr(D3DFMT_S8_LOCKABLE          );
  caseStr(D3DFMT_L16                  );
  caseStr(D3DFMT_VERTEXDATA           );
  caseStr(D3DFMT_INDEX16              );
  caseStr(D3DFMT_INDEX32              );
  caseStr(D3DFMT_Q16W16V16U16         );
  caseStr(D3DFMT_MULTI2_ARGB8         );
  caseStr(D3DFMT_R16F                 );
  caseStr(D3DFMT_G16R16F              );
  caseStr(D3DFMT_A16B16G16R16F        );
  caseStr(D3DFMT_R32F                 );
  caseStr(D3DFMT_G32R32F              );
  caseStr(D3DFMT_A32B32G32R32F        );
  caseStr(D3DFMT_CxV8U8               );
  caseStr(D3DFMT_A1                   );
  caseStr(D3DFMT_A2B10G10R10_XR_BIAS  );
  caseStr(D3DFMT_BINARYBUFFER         );
  default: return format(_T("Unknown format:%d"), f);
  }
}

String toString(D3DPOOL pool) {
  switch(pool) {
  caseStr(D3DPOOL_DEFAULT             );
  caseStr(D3DPOOL_MANAGED             );
  caseStr(D3DPOOL_SYSTEMMEM           );
  caseStr(D3DPOOL_SCRATCH             );
  default: return format(_T("Unknown pool:%d"), pool);
  }
}

String toString(D3DRESOURCETYPE type) {
  switch(type) {
  caseStr(D3DRTYPE_SURFACE            );
  caseStr(D3DRTYPE_VOLUME             );
  caseStr(D3DRTYPE_TEXTURE            );
  caseStr(D3DRTYPE_VOLUMETEXTURE      );
  caseStr(D3DRTYPE_CUBETEXTURE        );
  caseStr(D3DRTYPE_VERTEXBUFFER       );
  caseStr(D3DRTYPE_INDEXBUFFER        );
  default: return format(_T("Unknown resourcetype:%d"), type);
  }
}

String usageToString(DWORD usage) {
  if(usage == 0) {
    return _T("0");
  }
  String result, delim;

#undef  addFlag
#define addFlag(flag) if(usage & flag) { result += delim + _T(#flag); delim = _T("|"); }

  addFlag(D3DUSAGE_RENDERTARGET                    );
  addFlag(D3DUSAGE_DEPTHSTENCIL                    );
  addFlag(D3DUSAGE_DYNAMIC                         );

#if !defined(D3D_DISABLE_9EX)
  addFlag(D3DUSAGE_NONSECURE                       );
#endif

// When passed to CheckDeviceFormat, D3DUSAGE_AUTOGENMIPMAP may return
// D3DOK_NOAUTOGEN if the device doesn't support autogeneration for that format.
// D3DOK_NOAUTOGEN is a success code, not a failure code... the SUCCEEDED and FAILED macros
// will return true and false respectively for this code.
  addFlag(D3DUSAGE_AUTOGENMIPMAP                   );
  addFlag(D3DUSAGE_DMAP                            );

  addFlag(D3DUSAGE_QUERY_LEGACYBUMPMAP             );
  addFlag(D3DUSAGE_QUERY_SRGBREAD                  );
  addFlag(D3DUSAGE_QUERY_FILTER                    );
  addFlag(D3DUSAGE_QUERY_SRGBWRITE                 );
  addFlag(D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING  );
  addFlag(D3DUSAGE_QUERY_VERTEXTEXTURE             );
  addFlag(D3DUSAGE_QUERY_WRAPANDMIP                );

/* Usages for Vertex/Index buffers */
  addFlag(D3DUSAGE_WRITEONLY                       );
  addFlag(D3DUSAGE_SOFTWAREPROCESSING              );
  addFlag(D3DUSAGE_DONOTCLIP                       );
  addFlag(D3DUSAGE_POINTS                          );
  addFlag(D3DUSAGE_RTPATCHES                       );
  addFlag(D3DUSAGE_NPATCHES                        );

#if !defined(D3D_DISABLE_9EX)
  addFlag(D3DUSAGE_TEXTAPI                         );
  addFlag(D3DUSAGE_RESTRICTED_CONTENT              );
  addFlag(D3DUSAGE_RESTRICT_SHARED_RESOURCE        );
  addFlag(D3DUSAGE_RESTRICT_SHARED_RESOURCE_DRIVER );
#endif
  return result;
}

String vertexToString(const char *v, DWORD fvf, int dec) {
  const int    width = dec + 3;
  String       result;
  const TCHAR *delimiter = EMPTYSTRING;

#define addTagName(        tag ) { result += delimiter; result += tag; result += _T(":"); }
#define addTypeStr(        type) { result += toString(*(type*)v);             v += sizeof(type); delimiter = _T(", "); }
#define addTypeStrD(       type) { result += toString(*(type*)v, dec);        v += sizeof(type); delimiter = _T(", "); }
#define addTypeStrDW(      type) { result += toString(*(type*)v, dec, width); v += sizeof(type); delimiter = _T(", "); }
#define caseAddElement( s, type) case D3DFVF##_##s:     { addTagName(_T(#s)); addTypeStr( type); } break
#define caseAddElementD(s, type) case D3DFVF##_##s:     { addTagName(_T(#s)); addTypeStrD(type); } break
#define addFlagElement( s, type) if(fvf & D3DFVF##_##s) { addTagName(_T(#s)); addTypeStr( type); }
#define addFlagElementD(s, type) if(fvf & D3DFVF##_##s) { addTagName(_T(#s)); addTypeStrD(type); }
#define addTextureStr(    index) addTagName(format(_T("TEX%d"), index)); \
                                 addTypeStrD(TextureVertex);             \
                                 break

#define caseTexture(coordIndex) case D3DFVF_TEX##coordIndex: addTextureStr(coordIndex)
  switch(fvf & D3DFVF_POSITION_MASK) {
  caseAddElementD(XYZ              ,D3DXVECTOR3    );
  caseAddElementD(XYZRHW           ,D3DXVECTOR4    );
/*
  caseAddBytes(XYZB1            )
  caseAddBytes(XYZB2            )
  caseAddBytes(XYZB3            )
  caseAddBytes(XYZB4            )
  caseAddBytes(XYZB5            )
*/
  caseAddElementD(XYZW             ,D3DXVECTOR4    );
  }
  addFlagElementD(NORMAL           ,D3DXVECTOR3    );
  addFlagElementD(PSIZE            ,float          );
  addFlagElement( DIFFUSE          ,D3PCOLOR       );
  addFlagElement( SPECULAR         ,D3PCOLOR       );

  switch(fvf & D3DFVF_TEXCOUNT_MASK) {
  caseTexture(1);
  caseTexture(2);
  caseTexture(3);
  caseTexture(4);
  caseTexture(5);
  caseTexture(6);
  caseTexture(7);
  caseTexture(8);
  }
/*
  addFlagBytes(LASTBETA_UBYTE4  );
  addFlagBytes(LASTBETA_D3DCOLOR);
*/
  return result;
}

int formatToSize(D3DFORMAT f) {
  switch(f) {
  case D3DFMT_INDEX16: return sizeof(short);
  case D3DFMT_INDEX32: return sizeof(long );
  default            : throwInvalidArgumentException(__TFUNCTION__, _T("f=%d"), f);
                       return 1;
  }
}

String toString(const D3DVERTEXBUFFER_DESC &desc) {
  return format(_T("Type      :%s\n"
                   "Format    :%s\n"
                   "Pool      :%s\n"
                   "Usage     :%s\n"
                   "FVF       :%s\n"
                   "Buffersize:%s bytes\n")
               ,toString(     desc.Type  ).cstr()
               ,toString(     desc.Format).cstr()
               ,toString(     desc.Pool  ).cstr()
               ,usageToString(desc.Usage ).cstr()
               ,FVFToString(  desc.FVF   ).cstr()
               ,format1000(   desc.Size  ).cstr()
               );
}

String toString(const D3DINDEXBUFFER_DESC &desc) {
  return format(_T("Type      :%s\n"
                   "Format    :%s\n"
                   "Pool      :%s\n"
                   "Usage     :%s\n"
                   "Buffersize:%s bytes\n")
               ,toString(     desc.Type  ).cstr()
               ,toString(     desc.Format).cstr()
               ,toString(     desc.Pool  ).cstr()
               ,usageToString(desc.Usage ).cstr()
               ,format1000(   desc.Size  ).cstr()
               );
}

String toString(const D3DDISPLAYMODE &mode) {
  return format(_T("(W,H)       :(%d,%d)\n"
                   "Refresh rate:%d\n"
                   "Format      :%s\n")
               ,mode.Width, mode.Height
               ,mode.RefreshRate
               ,toString(mode.Format).cstr()
               );
}

String toString(const D3DCOLORVALUE &c, bool showAlpha) {
  if(showAlpha) {
    return format(_T("R:%.2f G:%.2f B:%.2f A:%.2f"), c.r,c.g,c.b, c.a);
  } else {
    return format(_T("R:%.2f G:%.2f B:%.2f"), c.r,c.g,c.b);
  }
}
