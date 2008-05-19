/*
 * upnp.c
 *
 * Copyright (C) 2007 Sebastian Gottschall <gottschall@dd-wrt.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Id:
 */
#ifdef HAVE_UPNP
#include <stdlib.h>
#include <bcmnvram.h>
#include <shutils.h>
#include <utils.h>
#include <syslog.h>
#include <signal.h>

int
start_upnp (void)
{
  char *wan_ifname = get_wan_face ();
  int ret;

  if (nvram_match ("upnp_enable", "0"))
    {
      stop_upnp ();
      return 0;
    }
  /* Make sure its not running first */
  ret = killall ("upnp", SIGUSR1);
  if (ret != 0)
    {
      ret = eval ("upnp", "-D",
		  "-L", nvram_safe_get ("lan_ifname"),
		  "-W", wan_ifname,
		  "-I", nvram_safe_get ("upnp_ssdp_interval"),
		  "-A", nvram_safe_get ("upnp_max_age"));
      syslog (LOG_INFO, "upnp : upnp daemon successfully started\n");
    }

  cprintf ("done\n");
  return ret;
}

int
stop_upnp (void)
{
  if (pidof ("upnp") > 0)
  {
  syslog (LOG_INFO, "upnp : upnp daemon successfully stopped\n");
  killall ("upnp", SIGUSR1);
  killall ("upnp", SIGTERM);
  cprintf ("done\n");
  }
  return 0;
}
#endif
