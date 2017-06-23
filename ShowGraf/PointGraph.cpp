#include "stdafx.h"
#include <QueueList.h>
#include "Graph.h"

PointGraph::PointGraph(GraphParameters *param) : Graph(param) {
  m_dataProcessed = false;
}

void PointGraph::updateDataRange() {
  if(isEmpty()) {
    m_range.setToDefault();
  } else {
    m_range = m_pointArray;
  }
}

const Point2DArray &PointGraph::getProcessedData() const {
  if(m_dataProcessed) {
    return m_processedData;
  }

  if(getParam().m_rollAvgSize == 0) {
    m_processedData = m_pointArray;
  } else {
    m_processedData.clear();
    QueueList<double> queue;
    double sum = 0;
    const size_t maxQueueSize = getParam().m_rollAvgSize;
    const size_t n            = m_pointArray.size();
    for(size_t i = 0; i < n; i++) {
      if(queue.size() == maxQueueSize) {
        sum -= queue.get();
      }
      queue.put(m_pointArray[i].y);
      sum += m_pointArray[i].y;
      m_processedData.add(Point2D(m_pointArray[i].x,sum / queue.size()));
    }
  }
  m_dataProcessed = true;
  return m_processedData;
}

void PointGraph::addPoint(const Point2D &p) {
  m_pointArray.add(p);
  m_dataProcessed = false;
}

void PointGraph::setDataPoints(const Point2DArray &a) {
  m_pointArray = a;
  updateDataRange();
  m_dataProcessed = false;
}

void PointGraph::clear() {
  m_pointArray.clear();
  updateDataRange();
  m_dataProcessed = false;
}

double PointGraph::distance(const CPoint &p, const RectangleTransformation &tr) const {
  const Point2DP      tmpp(p);
  const Point2DArray &data = getProcessedData();
  const size_t        n    = data.size();
  switch(getParam().m_style) {
  case GSCURVE:
    { double minDist = -1;
      for(size_t i = 1; i < n; i++) {
        const double dist = distanceFromLineSegment(tr.forwardTransform(data[i-1]),tr.forwardTransform(data[i]),tmpp);
        if(minDist < 0 || dist < minDist) {
          minDist = dist;
        }
      }
      return minDist;
    }
  case GSPOINT:
  case GSCROSS:
    { double minDist = -1;
      for(size_t i = 0; i < n; i++) {
        const double dist = ::distance(tr.forwardTransform(data[i]),tmpp);
        if(minDist < 0 || dist < minDist) {
          minDist = dist;
        }
      }
      return minDist;
    }
  default:
    throwException(_T("Invalid style:%d"), getParam().m_style);
  }
  return 0;
}

void PointGraph::setRollAvgSize(int size) {
  if(size != getParam().m_rollAvgSize) {
    m_dataProcessed = false;
  }
  getParam().m_rollAvgSize = size;
}

void PointGraph::paint(CCoordinateSystem &cs) {
  const Point2DArray &data  = getProcessedData();
  if(data.isEmpty()) return;
  const Point2D      *pp    = &data[0];
  const Point2D      *end   = &data.last();
  Viewport2D         &vp    = cs.getViewport();
  const COLORREF      color = getParam().m_color;
  Point2DArray        tmp;

  switch(getParam().m_style) {
  case GSCURVE :
    if(data.size() > 1) {
      CPen pen;
      pen.CreatePen(PS_SOLID, 1, color);
      vp.SelectObject(&pen);
      bool lastDefined = pointDefined(*pp);
      if(lastDefined) {
        vp.MoveTo(*pp);
        tmp.add(*pp);
      }
      while(pp++ < end) {
        const bool defined = pointDefined(*pp);
        if(defined) {
          if(lastDefined) {
            vp.LineTo(*pp);
            tmp.add(*pp);
          } else {
            vp.MoveTo(*pp);
            tmp.add(*pp);
          }
        } else {
          cs.setOccupiedConnectedPoints(tmp);
          tmp.clear(-1);
        }
        lastDefined = defined;
      }
      cs.setOccupiedConnectedPoints(tmp);
    }
    break;
  case GSPOINT :
    for(;pp <= end; pp++) {
      if(pointDefined(*pp)) {
        vp.SetPixel(*pp, color);
        tmp.add(*pp);
      }
    }
    cs.setOccupiedPoints(tmp);
    break;
  case GSCROSS :
    for(;pp <= end; pp++) {
      if(pointDefined(*pp)) {
        vp.paintCross(*pp, color, 6);
        tmp.add(*pp);
      }
    }
    cs.setOccupiedPoints(tmp);
    break;
  default:
    throwException(_T("Invalid style:%d"), getParam().m_style);
  }
}

double PointGraph::getSmallestPositiveX() const {
  const Point2DArray &data = getProcessedData();
  if(data.isEmpty()) return 0;
  double result = max(0, data[0].x);
  for(size_t i = 1; i < data.size(); i++) {
    result = getMinPositive(data[i].x, result);
  }
  return result;
}

double PointGraph::getSmallestPositiveY() const {
  const Point2DArray &data = getProcessedData();
  if(data.isEmpty()) return 0;
  double result = max(0, data[0].y);
  for(size_t i = 1; i < data.size(); i++) {
    result = getMinPositive(data[i].y, result);
  }
  return result;
}
