#pragma once

#include <CompactArray.h>

class PropertyContainer;

class PropertyChangeListener {
public:
  virtual void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) = 0;
};

class PropertyContainer {
private:
  CompactArray<PropertyChangeListener*> m_listeners;
protected:
  void notifyPropertyChanged(int id, const void *oldValue, const void *newValue) const;
public:
  void addPropertyChangeListener(   PropertyChangeListener *listener, bool first = false);
  void removePropertyChangeListener(PropertyChangeListener *listener);
};
