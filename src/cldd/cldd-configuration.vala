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

namespace Cldd {
    public errordomain ConfigError {
        FILE_NOT_FOUND,
        XML_DOCUMENT_EMPTY,
        INVALID_XPATH_EXPR
    }
}

/**
 * Configuration data that loads XML configuration and is used to retrieve user
 * defined settings.
 */
public class Cldd.Config : GLib.Object {

    public string file_name { get; set; default = "cldd.xml"; }

    public bool loaded { get; set; default = false; }

    private Xml.Doc *doc;
    private Xml.XPath.Context *ctx;
    private Xml.XPath.Object *obj;

    /**
     * Used when a property is changed.
     */
    public signal void property_changed (string property);

    /**
     * Default construction, don't really see the point of multiple constructors
     * at this stage.
     */
    public Config (string file_name) {
        this.file_name = file_name;
        load_document ();
    }

    private void load_document () {
        /* Load XML document */
        doc = Xml.Parser.parse_file (file_name);
        if (doc == null) {
            loaded = false;
            throw new Cldd.ConfigError.FILE_NOT_FOUND (
                "File %s not found or permissions incorrect.", file_name
            );
        }

        /* Create context for XPath queries */
        ctx = new Xml.XPath.Context (doc);
        ctx->register_ns ("cld", "urn:libcld");

        /* Assume success if we made it here */
        loaded = true;
    }

    public void save () {
        doc->save_file (file_name);
    }

    public Xml.Node * get_xml_node (string xpath) {
        obj = ctx->eval_expression (xpath);
        if (obj == null) {
            throw new Cldd.ConfigError.INVALID_XPATH_EXPR (
                "The XPath expression %s is invalid.", xpath
            );
        }

        /* XXX add error checking for a set that's > 1 in length */
        Xml.XPath.NodeSet *nodes = obj->nodesetval;

        return nodes->item (0);
    }

    private void edit_node (Xml.Node *node, string value) {
        node->set_content (value);
        if (node->type != Xml.ElementType.NAMESPACE_DECL)
            node = null;
    }

    /* XXX it might be more useful if these all retrieved the values using an
     *     xpath query but that would limit the ability to port to JSON */

    public bool get_boolean_property (string property) {
        string xpath = "//cldd/property[@name=\"%s\"]".printf (property);
        Xml.Node *node = get_xml_node (xpath);

        return bool.parse (node->get_content ());
    }

    public string get_string_property (string property) {
        string xpath = "//cldd/property[@name=\"%s\"]".printf (property);
        Xml.Node *node = get_xml_node (xpath);

        return node->get_content ();
    }

    public int get_int_property (string property) {
        string xpath = "//cldd/property[@name=\"%s\"]".printf (property);
        Xml.Node *node = get_xml_node (xpath);

        return int.parse (node->get_content ());
    }

    public double get_double_property (string property) {
        string xpath = "//cldd/property[@name=\"%s\"]".printf (property);
        Xml.Node *node = get_xml_node (xpath);

        return double.parse (node->get_content ());
    }

    public double get_float_property (string property) {
        string xpath = "//cldd/property[@name=\"%s\"]".printf (property);
        Xml.Node *node = get_xml_node (xpath);

        return (float)double.parse (node->get_content ());
    }

    public void set_boolean_property (string property, bool value) {
        string xpath = "//cldd/property[@name=\"%s\"]".printf (property);
        edit_node (get_xml_node (xpath), value.to_string ());
        property_changed (property);
    }

    public void set_string_property (string property, string value) {
        string xpath = "//cldd/property[@name=\"%s\"]".printf (property);
        edit_node (get_xml_node (xpath), value.to_string ());
        property_changed (property);
    }

    public void set_int_property (string property, int value) {
        string xpath = "//cldd/property[@name=\"%s\"]".printf (property);
        edit_node (get_xml_node (xpath), value.to_string ());
        property_changed (property);
    }

    public void set_double_property (string property, double value) {
        string xpath = "//cldd/property[@name=\"%s\"]".printf (property);
        edit_node (get_xml_node (xpath), value.to_string ());
        property_changed (property);
    }

    public void set_float_property (string property, float value) {
        string xpath = "//cldd/property[@name=\"%s\"]".printf (property);
        edit_node (get_xml_node (xpath), value.to_string ());
        property_changed (property);
    }

    /* XXX not sure how this should be done yet, possibly read the current
     *     matching node and iterate through attributes and elements */
    public void set_xml_node (string xpath, Xml.Node *node) {
        Xml.Node *cld_node = get_xml_node (xpath);
        cld_node->replace (node);
    }
}
