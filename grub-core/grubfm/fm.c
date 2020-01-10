/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2019  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <grub/types.h>
#include <grub/misc.h>
#include <grub/mm.h>
#include <grub/err.h>
#include <grub/dl.h>
#include <grub/extcmd.h>
#include <grub/i18n.h>

#include <grub/term.h>
#include <grub/video.h>
#include <grub/bitmap.h>
#include <grub/gfxmenu_view.h>

#include "fm.h"

GRUB_MOD_LICENSE ("GPLv3+");

static int init = 0;
char grubfm_root[20] = "memdisk";

static void
grubfm_init (void)
{
  if (!init)
  {
    grubfm_ini_enum (grubfm_root);
    init = 1;
  }
}

static grub_err_t
grub_cmd_grubfm (grub_extcmd_context_t ctxt __attribute__ ((unused)),
        int argc, char **args)
{
  grubfm_init ();
  grubfm_clear_menu ();
  if (argc == 0)
    grubfm_enum_device ();
  else
    grubfm_enum_file (args[0]);
  char *src = NULL;
  src = grub_xasprintf ("source (%s)/boot/grub/global.sh\n",
                        grubfm_root);
  if (!src)
    return 0;
  grub_script_execute_sourcecode (src);
  grub_free (src);
  return 0;
}

static grub_err_t
grub_cmd_grubfm_open (grub_extcmd_context_t ctxt __attribute__ ((unused)),
        int argc, char **args)
{
  grubfm_init ();
  grubfm_clear_menu ();
  if (argc != 1)
    return grub_error (GRUB_ERR_BAD_ARGUMENT, N_("bad argument"));
  grubfm_open_file (args[0]);
  char *src = NULL;
  src = grub_xasprintf ("source (%s)/boot/grub/global.sh\n",
                        grubfm_root);
  if (!src)
    return 0;
  grub_script_execute_sourcecode (src);
  grub_free (src);
  return 0;
}

static const struct grub_arg_option options_set[] =
{
  {"root", 'r', 0, N_("root"), 0, 0},
  {0, 0, 0, 0, 0, 0}
};

enum options_set
{
  FM_SET_ROOT,
};

static grub_err_t
grub_cmd_grubfm_set (grub_extcmd_context_t ctxt,
                     int argc, char **args)
{
  struct grub_arg_list *state = ctxt->state;
  if (state[FM_SET_ROOT].set && argc == 1)
  {
    grub_strncpy(grubfm_root, args[0], 19);
  }
  return 0;
}

static grub_err_t
grub_cmd_grubfm_dbg (grub_extcmd_context_t ctxt __attribute__ ((unused)),
                     int argc __attribute__ ((unused)),
                     char **args __attribute__ ((unused)))
{
  return 0;
}

static grub_err_t
grub_cmd_grubfm_hex (grub_extcmd_context_t ctxt __attribute__ ((unused)),
                     int argc, char **args)
{
  if (argc != 1)
    return grub_error (GRUB_ERR_BAD_ARGUMENT, N_("bad argument"));
  unsigned int w, h;
  grubfm_get_screen_info (&w, &h);
  if (w < 1024 || h < 768)
    return grub_error (GRUB_ERR_BAD_OS, N_("gfxmode (minimum resolution 1024x768) required"));
  grubfm_hexdump (args[0]);
  return 0;
}

static grub_extcmd_t cmd;
static grub_extcmd_t cmd_open;
static grub_extcmd_t cmd_set;
static grub_extcmd_t cmd_dbg;
static grub_extcmd_t cmd_hex;

GRUB_MOD_INIT(grubfm)
{
  cmd = grub_register_extcmd ("grubfm", grub_cmd_grubfm, 0, 
                  N_("[PATH]"),
                  N_("GRUB file manager."), 0);
  cmd_open = grub_register_extcmd ("grubfm_open", grub_cmd_grubfm_open, 0,
                  N_("PATH"),
                  N_("GRUB file manager."), 0);
  cmd_set = grub_register_extcmd ("grubfm_set", grub_cmd_grubfm_set, 0,
                                  N_("--root DEVICE"),
                                  N_("GRUB file manager."),
                                  options_set);
  cmd_dbg = grub_register_extcmd ("grubfm_dbg", grub_cmd_grubfm_dbg, 0, 0,
                  N_("GRUB file manager."), 0);
  cmd_hex = grub_register_extcmd ("grubfm_hex", grub_cmd_grubfm_hex, 0,
                  N_("PATH"),
                  N_("GRUB file manager."), 0);
}

GRUB_MOD_FINI(grubfm)
{
  grub_unregister_extcmd (cmd);
  grub_unregister_extcmd (cmd_open);
  grub_unregister_extcmd (cmd_set);
  grub_unregister_extcmd (cmd_dbg);
  grub_unregister_extcmd (cmd_hex);
}
