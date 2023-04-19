/*
 * Copyright (C) 2002-2007 Keisuke Nishida
 * Copyright (C) 2007 Roger While
  * Copyright (C) 2008-2020 Cobol-IT
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; see the file COPYING.LIB.  If
 * not, write to the Free Software Foundation, 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1301 USA
 */

#ifndef COB_TERMIO_H
#define COB_TERMIO_H

#include <stdio.h>
#include <libcob/common.h>

#define DISPLAY_ONE_FIELD_MODE_MASK         0x00FF
#define DISPLAY_ONE_FIELD_MODE_FILE         0x0000
#define DISPLAY_ONE_FIELD_MODE_BUFFER       0x0001
#define DISPLAY_ONE_FIELD_MODE_DEBUG        0x0002
#define DISPLAY_ONE_FIELD_SOURCE_MASK       0xFF00
#define DISPLAY_ONE_FIELD_SOURCE_DISPLAY    0X0000
#define DISPLAY_ONE_FIELD_SOURCE_XML        0x0100


/* image valid until next call to cob_get_field_display_image or next display*/
COB_DLL_EXPIMP unsigned char * cob_get_field_display_image   (COB_RTD, cob_field *f);


COB_DLL_EXPIMP void cob_one_field_display   (COB_RTD, cob_field *f, FILE *fp, int data_offset);
COB_DLL_EXPIMP void cob_one_field_display_extended (COB_RTD, cob_field *f, FILE *fp, int data_offset, int mode);
COB_DLL_EXPIMP void cob_one_field_display_prinf (COB_RTD, FILE *fp, char *s, int mode);
COB_DLL_EXPIMP void cob_display             (COB_RTD, const int upon_device, const int neednewline, const int varcnt, ...);
COB_DLL_EXPIMP void cob_accept              (COB_RTD, const int upon_device, cob_field *f);
#endif /* COB_TERMIO_H */
