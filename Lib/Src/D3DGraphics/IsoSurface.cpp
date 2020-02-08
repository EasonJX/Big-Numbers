#include "pch.h"
#include <XMLUtil.h>
#include <D3DGraphics/D3XML.h>
#include "D3DGraphics/IsoSurface.h"

IsoSurfaceParameters::IsoSurfaceParameters() {
  m_cellSize         = 0.25;
  m_boundingBox      = Cube3D(Point3D(-5,-5,-5), Point3D( 5, 5, 5));
  m_tetrahedral      = true;
  m_tetraOptimize4   = false;
  m_adaptiveCellSize = false;
  m_originOutside    = false;
  m_machineCode      = true;
  m_doubleSided      = false;
  m_includeTime      = false;
  m_timeInterval     = DoubleInterval(0,10);
  m_frameCount       = 20;
}

void IsoSurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.createRoot(_T("IsoSurface"));
  doc.setValue(  root, _T("expr"              ), m_expr            );
  doc.setValue(  root, _T("cellsize"          ), m_cellSize        );
  setValue( doc, root, _T("boundingbox"       ), m_boundingBox     );
  doc.setValue(  root, _T("tetrahedral"       ), m_tetrahedral     );
  doc.setValue(  root, _T("tetraoptimize4"    ), m_tetraOptimize4  );
  doc.setValue(  root, _T("adaptivecellsize"  ), m_adaptiveCellSize);
  doc.setValue(  root, _T("originoutside"     ), m_originOutside   );
  doc.setValue(  root, _T("machinecode"       ), m_machineCode     );
  doc.setValue(  root, _T("doublesided"       ), m_doubleSided     );
  doc.setValue(  root, _T("includetime"       ), m_includeTime     );
  if(m_includeTime) {
    setValue(doc, root, _T("timeinterval"     ), m_timeInterval    );
    doc.setValue( root, _T("framecount"       ), m_frameCount      );
  }
}

void IsoSurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  checkTag(root, _T("IsoSurface"));
  doc.getValueLF(root, _T("expr"              ), m_expr            );
  doc.getValue(  root, _T("cellsize"          ), m_cellSize        );
  getValue( doc, root, _T("boundingbox"       ), m_boundingBox     );
  doc.getValue(  root, _T("tetrahedral"       ), m_tetrahedral     );
  doc.getValue(  root, _T("tetraoptimize4"    ), m_tetraOptimize4  );
  doc.getValue(  root, _T("adaptivecellsize"  ), m_adaptiveCellSize);
  doc.getValue(  root, _T("originoutside"     ), m_originOutside   );
  doc.getValue(  root, _T("machinecode"       ), m_machineCode     );
  doc.getValue(  root, _T("doublesided"       ), m_doubleSided     );
  doc.getValue(  root, _T("includetime"       ), m_includeTime     );
  if(m_includeTime) {
    getValue(doc, root, _T("timeinterval"     ), m_timeInterval    );
    doc.getValue( root, _T("framecount"       ), m_frameCount      );
  }
}
