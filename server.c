/*
 *  Server interfaces
 *  Copyright C 2001 ***REMOVED***.  All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  As a special exception, if other files instantiate generics from this
 *  unit, or you link this unit with other files to produce an executable,
 *  this unit does not by itself cause the resulting executable to be
 *  covered by the GNU General Public License. This exception does not
 *  however invalidate any other reasons why the executable file might be
 *  covered by the GNU General Public License.
 *
 */

/**
 * This is where the actual IRC server handling mechanism
 * interfaces should go.  To some degree, server.c should
 * implement a working IRC server except with a large number
 * of event/callback mechanisms to be handled by the user
 * of the library.
 *
 * server.c will implement the protocol itself, other parts of the
 * library sockets and dns, and mechanisms for internal structure
 * too.
 *
 * the user of the library will be responsible for coordination of
 * high-level things like dealing with messages once they are handled
 * though possibly using utilities in the library itself to do that.
 */
