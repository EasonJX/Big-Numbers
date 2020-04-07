/// \file ijkxitIO.h
/// I/O routines for .xit file
/// .xit is an xml format 
/// Version 0.1.1

/*
  IJK: Isosurface Jeneration Kode
  Copyright (C) 2006, 2003, 2001 Rephael Wenger

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  (LGPL) as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#pragma once

#include <iostream>

#include "ijktable.h"

namespace IJKXIO {
  void read_xit( istream &in ,       IJKTABLE::ISOSURFACE_TABLE &table);
  void write_xit(ostream &out, const IJKTABLE::ISOSURFACE_TABLE &table);
  void write_cpp(ostream &out, const IJKTABLE::ISOSURFACE_TABLE &table);
};
