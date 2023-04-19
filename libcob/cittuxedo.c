/*
   * Copyright (C) 2008-2020 Cobol-IT
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1301 USA
 */
/* This module is only a fake to use with BEA Tuxedo cobatmi library
 * to force a reference to the library.
 */
extern void TPCALL (void);
/*
 * DO NEVER CALL this function... just link the module with your program using 
 * Tuxedo cobatmi  library
 */
void CIT_TuxedoStub (void)
{
	TPCALL();
}
