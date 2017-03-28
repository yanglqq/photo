#include "Slipper.h"

Slipper::Slipper(QObject *parent)
    : QObject(parent),
      m_x(0),
      m_y(0),
      m_lockType(Free)
{
}
