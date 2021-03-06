/**
 * Copyright (C) 2010 Control, Logging, and Data Acquisition Daemon
 * http://www.github.com/geoffjay/cldd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

public class Cldd.Settings : GLib.Object {

    /* XXX this probably doesn't need a property backing field */
    private string _app = "CLDD";
    public string app {
        get { return _app; }
        set { _app = value; }
    }

    /* Default construction */
    public Settings () { }

    /* Construction to load settings from configuration data provided */
    public Settings.with_config (Config config) {
        app = config.get_string_property ("app");
    }
}
