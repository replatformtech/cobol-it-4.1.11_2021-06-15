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

#ifndef COB_STRINGS_H
#define COB_STRINGS_H

#include <libcob/common.h>

COB_DLL_EXPIMP void cob_inspect_init        (COB_RTD, cob_field *var, const int replacing);
COB_DLL_EXPIMP void cob_inspect_start       (COB_RTD);
COB_DLL_EXPIMP void cob_inspect_before      (COB_RTD, cob_field *str);
COB_DLL_EXPIMP void cob_inspect_after       (COB_RTD, cob_field *str);
COB_DLL_EXPIMP void cob_inspect_characters  (COB_RTD, cob_field *f1);
COB_DLL_EXPIMP void cob_inspect_all         (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void cob_inspect_leading     (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void cob_inspect_first       (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void cob_inspect_trailing    (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void cob_inspect_converting  (COB_RTD, cob_field *f1, cob_field *f2);
COB_DLL_EXPIMP void cob_inspect_finish      (COB_RTD);

COB_DLL_EXPIMP void cob_string_init         (COB_RTD, cob_field *dst, cob_field *ptr);
COB_DLL_EXPIMP void cob_string_delimited    (COB_RTD, cob_field *dlm);
COB_DLL_EXPIMP void cob_string_append       (COB_RTD, cob_field *src);
COB_DLL_EXPIMP void cob_string_finish       (COB_RTD);

COB_DLL_EXPIMP void cob_unstring_init       (COB_RTD, cob_field *src, cob_field *ptr, const size_t num_dlm);
COB_DLL_EXPIMP void cob_unstring_delimited  (COB_RTD, cob_field *dlm, const int all);
COB_DLL_EXPIMP void cob_unstring_into       (COB_RTD, cob_field *dst, cob_field *dlm, cob_field *cnt);
COB_DLL_EXPIMP void cob_unstring_tallying   (COB_RTD, cob_field *f);
COB_DLL_EXPIMP void cob_unstring_finish     (COB_RTD);

#endif /* COB_STRINGS_H */
