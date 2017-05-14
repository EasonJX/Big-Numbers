#include "stdafx.h"
#include "DataGraph.h"

Point2D DataGraph::tr1(const TCHAR *sx, const TCHAR *sy) {
  DataGraphParameters *param = (DataGraphParameters*)m_param;
  Point2D result;
  result.x = param->m_xReader.convertString(sx);
  result.y = param->m_yReader.convertString(sy);
  if(param->m_xRelativeToFirst || param->m_yRelativeToFirst) {
    if(m_hasFirstDataPoint) {
      if(param->m_xRelativeToFirst) {
        result.x -= m_firstPoint.x;
      }
      if(param->m_yRelativeToFirst) {
        result.x -= m_firstPoint.y;
      }
    } else {
      m_firstPoint = result;
      m_hasFirstDataPoint = true;
      if(param->m_xRelativeToFirst) {
        result.x = 0;
      }
      if(param->m_yRelativeToFirst) {
        result.y = 0;
      }
    }
  }
  return result;
}

void DataGraph::readData(FILE *f) {
  DataGraphParameters *param = (DataGraphParameters*)m_param;
  int lineCount = 0;
  double currentX = 1;
  TCHAR line[4000];
  m_hasFirstDataPoint = false;
  while(FGETS(line,ARRAYSIZE(line),f)) {
    lineCount++;

    TCHAR sx[1024],sy[1024];
    if(param->m_onePerLine) {
      if(_stscanf(line, _T("%s"), sy) != 1) {
        if(param->m_ignoreErrors) {
          continue;
        }
        throwException(_T("Invalid input in %s line %d:%s"),param->getFileName().cstr(),lineCount,line);
      }
      _stprintf(sx,_T("%lg"),currentX++);
    } else if(_stscanf(line,_T("%s %s"), sx, sy) != 2) {
      if(param->m_ignoreErrors) {
        continue;
      }
      throwException(_T("Invalid input in %s line %d:%s"),param->getFileName().cstr(),lineCount,line);
    }
    try {
      addPoint(tr1(sx, sy));
    } catch(Exception e) {
      if(param->m_ignoreErrors) {
        continue;
      }
      throwException(_T("Invalid input in %s line %d:<%s>:\n\r%s\n"),param->getFileName().cstr(),lineCount,line,e.what());
    }
  }
  updateDataRange();
}

DataGraph::DataGraph(DataGraphParameters &param) : PointGraph(new DataGraphParameters(param)) {
  FILE *f = (param.getFileName() == _T("stdin")) ? stdin : fopen(param.getFileName(),_T("r"));
  if(f != NULL) {
    readData(f);
    if(f != stdin) {
      fclose(f);
    }
  }
}

