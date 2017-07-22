/*
 * Author: Chad Froebel <chadfroebel@gmail.com>
 *
 * Port to Nexus 5 : flar2 <asegaert@gmail.com>
 *
 * Port to Osprey : engstk <eng.stk@sapo.pt>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*
 * Possible values for "force_fast_charge" are :
 *
 *   0 - Disabled (default)
 *   1 - Force faster charge
*/

#include <linux/kobject.h>
#include <linux/fastchg.h>
#include <linux/string.h>

int force_fast_charge = 1;

static int __init get_fastcharge_opt(char *ffc)
{
	if (strcmp(ffc, "0") == 0) {
		force_fast_charge = 0;
	} else if (strcmp(ffc, "1") == 0) {
		force_fast_charge = 1;
	} else {
		force_fast_charge = 0;
	}
	return 1;
}

__setup("ffc=", get_fastcharge_opt);
