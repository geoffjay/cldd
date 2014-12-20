/**
 * Copyright (C) 2010 Geoff Johnson <geoff.jay@gmail.com>
 *
 * This file is part of cldd.
 *
 * cldd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <common.h>

#include "cmdline.h"
#include "log.h"
#include "conf.h"
#include "error.h"

#define USER_CONFIG_FILE_LOCATION1    ".clddconf"
#define USER_CONFIG_FILE_LOCATION2    ".cldd/cldd.conf"

static GQuark
cmdline_quark (void)
{
    return g_quark_from_static_string ("cmdline");
}

G_GNUC_NORETURN
static void version(void)
{
    puts (PACKAGE " (CLDD: Control Logging and Data Acquisition Daemon) "
          VERSION " \n\n"
          "Copyright (C) 2012 Geoff Johnson <geoff.jay@gmail.com>\n"
          "This is free software; see the source for copying conditions.\n"
          "There is NO warranty; not even MERCHANTABILITY or FITNESS FOR A"
          "PARTICULAR PURPOSE.\n\n");

    exit (EXIT_SUCCESS);
}

static const char *summary =
    "Control Logging and Data Acquisition Daemon\n"
    "a daemon for data acquistion and control.";

/* need to add glib or rewrite this function another way */
bool
parse_cmdline (int argc, char **argv, struct options *options, GError **error_r)
{
    GError *error = NULL;
    GOptionContext *context;
    bool ret;
    static gboolean option_version,
                    option_no_daemon,
                    option_no_config;
    const GOptionEntry entries[] = {
        { "kill", 0, 0, G_OPTION_ARG_NONE, &options->kill,
          "kill the currently running cldd session", NULL },
        { "no-config", 0, 0, G_OPTION_ARG_NONE, &option_no_config,
          "don't read from config", NULL },
        { "no-daemon", 0, 0, G_OPTION_ARG_NONE, &option_no_daemon,
          "don't detach from console", NULL },
        { "stdout", 0, 0, G_OPTION_ARG_NONE, &options->log_stderr,
          NULL, NULL },
        { "stderr", 0, 0, G_OPTION_ARG_NONE, &options->log_stderr,
          "print messages to stderr", NULL },
        { "verbose", 'v', 0, G_OPTION_ARG_NONE, &options->verbose,
          "verbose logging", NULL },
        { "version", 'V', 0, G_OPTION_ARG_NONE, &option_version,
          "print version number", NULL },
        { .long_name = NULL }
    };

    options->kill = false;
    options->daemon = true;
    options->log_stderr = false;
    options->verbose = false;

    context = g_option_context_new ("[path/to/cldd.conf]");
    g_option_context_add_main_entries (context, entries, NULL);

    g_option_context_set_summary (context, summary);

    ret = g_option_context_parse (context, &argc, &argv, &error);
    g_option_context_free (context);

    if (!ret)
        CLDD_ERROR("option parsing failed: %s\n", error->message);

    if (option_version)
        version ();

    /* initialize the logging library, so the configuration file
       parser can use it already */
    log_early_init (options->verbose);

    options->daemon = !option_no_daemon;

    if (option_no_config)
    {
        g_debug ("Ignoring config, using daemon defaults\n");
        return true;
    }
    else if (argc <= 1)
    {
        /* default configuration file path */
        char *path1;
        char *path2;

        path1 = g_build_filename (g_get_home_dir (),
                                  USER_CONFIG_FILE_LOCATION1, NULL);
        path2 = g_build_filename (g_get_home_dir (),
                                  USER_CONFIG_FILE_LOCATION2, NULL);

        if (g_file_test (path1, G_FILE_TEST_IS_REGULAR))
            ret = config_read_file (path1, error_r);
        else if (g_file_test (path2, G_FILE_TEST_IS_REGULAR))
            ret = config_read_file (path2, error_r);
        else if (g_file_test (SYSTEM_CONFIG_FILE_LOCATION,
                              G_FILE_TEST_IS_REGULAR))
            ret = config_read_file (SYSTEM_CONFIG_FILE_LOCATION, error_r);

        g_free (path1);
        g_free (path2);

        return ret;
    }
    else if (argc == 2)
    {
        /* specified configuration file */
        return config_read_file (argv[1], error_r);
    }
    else
    {
        g_set_error (error_r, cmdline_quark (), 0,
                     "too many arguments");
        return false;
    }
}
