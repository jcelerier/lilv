/*
  Copyright 2007-2011 David Robillard <http://drobilla.net>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/**
   @file lilv.h API for Lilv, a lightweight LV2 host library.
*/

#ifndef LILV_LILV_H
#define LILV_LILV_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#ifdef LILV_SHARED
#    ifdef __WIN32__
#        define LILV_LIB_IMPORT __declspec(dllimport)
#        define LILV_LIB_EXPORT __declspec(dllexport)
#    else
#        define LILV_LIB_IMPORT __attribute__((visibility("default")))
#        define LILV_LIB_EXPORT __attribute__((visibility("default")))
#    endif
#    ifdef LILV_INTERNAL
#        define LILV_API LILV_LIB_EXPORT
#    else
#        define LILV_API LILV_LIB_IMPORT
#    endif
#else
#    define LILV_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define LILV_NAMESPACE_LV2      "http://lv2plug.in/ns/lv2core#"
#define LILV_PORT_CLASS_PORT    "http://lv2plug.in/ns/lv2core#Port"
#define LILV_PORT_CLASS_INPUT   "http://lv2plug.in/ns/lv2core#InputPort"
#define LILV_PORT_CLASS_OUTPUT  "http://lv2plug.in/ns/lv2core#OutputPort"
#define LILV_PORT_CLASS_CONTROL "http://lv2plug.in/ns/lv2core#ControlPort"
#define LILV_PORT_CLASS_AUDIO   "http://lv2plug.in/ns/lv2core#AudioPort"
#define LILV_PORT_CLASS_EVENT   "http://lv2plug.in/ns/ext/event#EventPort"
#define LILV_EVENT_CLASS_MIDI   "http://lv2plug.in/ns/ext/midi#MidiEvent"

typedef struct _LilvPlugin*       LilvPlugin;       /**< LV2 Plugin. */
typedef struct _LilvPluginClass*  LilvPluginClass;  /**< Plugin Class. */
typedef struct _LilvPort*         LilvPort;         /**< Port. */
typedef struct _LilvScalePoint*   LilvScalePoint;   /**< Scale Point (Notch). */
typedef struct _LilvUI*           LilvUI;           /**< Plugin UI. */
typedef struct _LilvValue*        LilvValue;        /**< Typed Value. */
typedef struct _LilvWorld*        LilvWorld;        /**< Lilv World. */

typedef void* LilvPluginClasses;  /**< set<PluginClass>. */
typedef void* LilvPlugins;        /**< set<Plugin>. */
typedef void* LilvScalePoints;    /**< set<ScalePoint>. */
typedef void* LilvUIs;            /**< set<UI>. */
typedef void* LilvValues;         /**< set<Value>. */

/**
   @defgroup lilv Lilv
   Lilv is a simple yet powerful C API for using LV2 plugins.

   For more information about LV2, see <http://lv2plug.in>.
   For more information about Lilv, see <http://drobilla.net/software/lilv>.
   @{
*/

/**
   @name Value
   @{
*/

/**
   Convert a file URI string to a local path string.
   For example, "file://foo/bar/baz.ttl" returns "/foo/bar/baz.ttl".
   Return value is shared and must not be deleted by caller.
   @return @a uri converted to a path, or NULL on failure (URI is not local).
*/
LILV_API
const char*
lilv_uri_to_path(const char* uri);

/**
   Create a new URI value.
   Returned value must be freed by caller with lilv_value_free.
*/
LILV_API
LilvValue
lilv_value_new_uri(LilvWorld world, const char* uri);

/**
   Create a new string value (with no language).
   Returned value must be freed by caller with lilv_value_free.
*/
LILV_API
LilvValue
lilv_value_new_string(LilvWorld world, const char* str);

/**
   Create a new integer value.
   Returned value must be freed by caller with lilv_value_free.
*/
LILV_API
LilvValue
lilv_value_new_int(LilvWorld world, int val);

/**
   Create a new floating point value.
   Returned value must be freed by caller with lilv_value_free.
*/
LILV_API
LilvValue
lilv_value_new_float(LilvWorld world, float val);

/**
   Create a new boolean value.
   Returned value must be freed by caller with lilv_value_free.
*/
LILV_API
LilvValue
lilv_value_new_bool(LilvWorld world, bool val);

/**
   Free an LilvValue.
*/
LILV_API
void
lilv_value_free(LilvValue val);

/**
   Duplicate an LilvValue.
*/
LILV_API
LilvValue
lilv_value_duplicate(LilvValue val);

/**
   Return whether two values are equivalent.
*/
LILV_API
bool
lilv_value_equals(LilvValue value, LilvValue other);

/**
   Return this value as a Turtle/SPARQL token.
   Returned value must be freed by caller with lilv_value_free.
   <table>
   <caption>Example Turtle Tokens</caption>
   <tr><th>URI</th><td>&lt;http://example.org/foo &gt;</td></tr>
   <tr><th>QName</td><td>doap:name</td></tr>
   <tr><th>String</td><td>"this is a string"</td></tr>
   <tr><th>Float</td><td>1.0</td></tr>
   <tr><th>Integer</td><td>1</td></tr>
   <tr><th>Boolean</td><td>true</td></tr>
   </table>
*/
LILV_API
char*
lilv_value_get_turtle_token(LilvValue value);

/**
   Return whether the value is a URI (resource).
*/
LILV_API
bool
lilv_value_is_uri(LilvValue value);

/**
   Return this value as a URI string, e.g. "http://example.org/foo".
   Valid to call only if lilv_value_is_uri(@a value) returns true.
   Returned value is owned by @a value and must not be freed by caller.
*/
LILV_API
const char*
lilv_value_as_uri(LilvValue value);

/**
   Return whether the value is a blank node (resource with no URI).
*/
LILV_API
bool
lilv_value_is_blank(LilvValue value);

/**
   Return this value as a blank node identifier, e.g. "genid03".
   Valid to call only if lilv_value_is_blank(@a value) returns true.
   Returned value is owned by @a value and must not be freed by caller.
*/
LILV_API
const char*
lilv_value_as_blank(LilvValue value);

/**
   Return whether this value is a literal (i.e. not a URI).
   Returns true if @a value is a string or numeric value.
*/
LILV_API
bool
lilv_value_is_literal(LilvValue value);

/**
   Return whether this value is a string literal.
   Returns true if @a value is a string value (and not numeric).
*/
LILV_API
bool
lilv_value_is_string(LilvValue value);

/**
   Return @a value as a string.
*/
LILV_API
const char*
lilv_value_as_string(LilvValue value);

/**
   Return whether this value is a decimal literal.
*/
LILV_API
bool
lilv_value_is_float(LilvValue value);

/**
   Return @a value as a float.
   Valid to call only if lilv_value_is_float(@a value) or
   lilv_value_is_int(@a value) returns true.
*/
LILV_API
float
lilv_value_as_float(LilvValue value);

/**
   Return whether this value is an integer literal.
*/
LILV_API
bool
lilv_value_is_int(LilvValue value);

/**
   Return @a value as an integer.
   Valid to call only if lilv_value_is_int(@a value) returns true.
*/
LILV_API
int
lilv_value_as_int(LilvValue value);

/**
   Return whether this value is a boolean.
*/
LILV_API
bool
lilv_value_is_bool(LilvValue value);

/**
   Return @a value as a bool.
   Valid to call only if lilv_value_is_bool(@a value) returns true.
*/
LILV_API
bool
lilv_value_as_bool(LilvValue value);

/**
   @}
   @name Collections
   Lilv has several collection types for holding various types of value:
   <ul>
   <li>LilvPlugins (function prefix "lilv_plugins_")</li>
   <li>LilvPluginClasses (function prefix "lilv_plugin_classes_")</li>
   <li>LilvScalePoints (function prefix "lilv_scale_points_")</li>
   <li>LilvValues (function prefix "lilv_values_")</li>
   <li>LilvUIs (function prefix "lilv_uis_")</li>
   </ul>

   Each collection type supports a similar basic API:
   <ul>
   <li>void PREFIX_free (coll)</li>
   <li>unsigned PREFIX_size (coll)</li>
   <li>LilvIter PREFIX_begin (coll)</li>
   </ul>
   @{
*/

/* Collections */

typedef void* LilvIter;

/**
   Iterate over each element of a collection.
   @code
   LILV_FOREACH(plugin_classes, i, classes) {
      LilvPluginClass c = lilv_plugin_classes_get(classes, i);
      // ...
   }
   @endcode
*/
#define LILV_FOREACH(colltype, iter, collection) \
	for (LilvIter (iter) = lilv_ ## colltype ## _begin(collection); \
	     !lilv_ ## colltype ## _is_end(collection, iter); \
	     (iter) = lilv_ ## colltype ## _next(collection, iter))

/* LilvPluginClasses */

LILV_API
void
lilv_plugin_classes_free(LilvPluginClasses collection);

LILV_API
unsigned
lilv_plugin_classes_size(LilvPluginClasses collection);

LILV_API
LilvIter
lilv_plugin_classes_begin(LilvPluginClasses collection);

LILV_API
LilvPluginClass
lilv_plugin_classes_get(LilvPluginClasses collection, LilvIter i);

LILV_API
LilvIter
lilv_plugin_classes_next(LilvPluginClasses collection, LilvIter i);

LILV_API
bool
lilv_plugin_classes_is_end(LilvPluginClasses collection, LilvIter i);

/**
   Get a plugin class from @a classes by URI.
   Return value is shared (stored in @a classes) and must not be freed or
   modified by the caller in any way.
   @return NULL if no plugin class with @a uri is found in @a classes.
*/
LILV_API
LilvPluginClass
lilv_plugin_classes_get_by_uri(LilvPluginClasses classes,
                               LilvValue         uri);

/* ScalePoints */

LILV_API
void
lilv_scale_points_free(LilvScalePoints collection);

LILV_API
unsigned
lilv_scale_points_size(LilvScalePoints collection);

LILV_API
LilvIter
lilv_scale_points_begin(LilvScalePoints collection);

LILV_API
LilvScalePoint
lilv_scale_points_get(LilvScalePoints collection, LilvIter i);

LILV_API
LilvIter
lilv_scale_points_next(LilvScalePoints collection, LilvIter i);

LILV_API
bool
lilv_scale_points_is_end(LilvScalePoints collection, LilvIter i);

/* UIs */

LILV_API
void
lilv_uis_free(LilvUIs collection);

LILV_API
unsigned
lilv_uis_size(LilvUIs collection);

LILV_API
LilvIter
lilv_uis_begin(LilvUIs collection);

LILV_API
LilvUI
lilv_uis_get(LilvUIs collection, LilvIter i);

LILV_API
LilvIter
lilv_uis_next(LilvUIs collection, LilvIter i);

LILV_API
bool
lilv_uis_is_end(LilvUIs collection, LilvIter i);

/**
   Get a UI from @a uis by URI.
   Return value is shared (stored in @a uis) and must not be freed or
   modified by the caller in any way.
   @return NULL if no UI with @a uri is found in @a list.
*/
LILV_API
LilvUI
lilv_uis_get_by_uri(LilvUIs   uis,
                    LilvValue uri);

/* Values */

LILV_API
void
lilv_values_free(LilvValues collection);

LILV_API
unsigned
lilv_values_size(LilvValues collection);

LILV_API
LilvIter
lilv_values_begin(LilvValues collection);

LILV_API
LilvValue
lilv_values_get(LilvValues collection, LilvIter i);

LILV_API
LilvIter
lilv_values_next(LilvValues collection, LilvIter i);

LILV_API
bool
lilv_values_is_end(LilvValues collection, LilvIter i);

LILV_API
LilvValue
lilv_values_get_first(LilvValues collection);

/**
   Return whether @a values contains @a value.
*/
LILV_API
bool
lilv_values_contains(LilvValues values, LilvValue value);

/* Plugins */

LILV_API
unsigned
lilv_plugins_size(LilvPlugins collection);

LILV_API
LilvIter
lilv_plugins_begin(LilvPlugins collection);

LILV_API
LilvPlugin
lilv_plugins_get(LilvPlugins collection, LilvIter i);

LILV_API
LilvIter
lilv_plugins_next(LilvPlugins collection, LilvIter i);

LILV_API
bool
lilv_plugins_is_end(LilvPlugins collection, LilvIter i);

LILV_API
void
lilv_plugins_free(LilvWorld world, LilvPlugins plugins);

/**
   Get a plugin from @a plugins by URI.
   Return value is shared (stored in @a plugins) and must not be freed or
   modified by the caller in any way.
   @return NULL if no plugin with @a uri is found in @a plugins.
*/
LILV_API
LilvPlugin
lilv_plugins_get_by_uri(LilvPlugins plugins,
                        LilvValue   uri);

/**
   @}
   @name World
   The "world" represents all Lilv state, and is used to discover/load/cache
   LV2 data (plugins, UIs, and extensions).
   Normal hosts which just need to load plugins by URI should simply use
   @ref lilv_world_load_all to discover/load the system's LV2 resources.
   @{
*/

/**
   Initialize a new, empty world.
   If initialization fails, NULL is returned.
*/
LILV_API
LilvWorld
lilv_world_new(void);

/**
   Enable/disable language filtering.
   Language filtering applies to any functions that return (a) value(s).
   With filtering enabled, Lilv will automatically return the best value(s)
   for the current LANG.  With filtering disabled, all matching values will
   be returned regardless of language tag.  Filtering is enabled by default.
*/
#define LILV_OPTION_FILTER_LANG "http://drobilla.net/ns/lilv#filter-lang"

/**
   Enable/disable dynamic manifest support.
   Dynamic manifest data will only be loaded if this option is true.
*/
#define LILV_OPTION_DYN_MANIFEST "http://drobilla.net/ns/lilv#dyn-manifest"

/**
   Set an option option for @a world.

   Currently recognized options:
   @ref LILV_OPTION_FILTER_LANG
   @ref LILV_OPTION_DYN_MANIFEST
*/
LILV_API
void
lilv_world_set_option(LilvWorld       world,
                      const char*     uri,
                      const LilvValue value);

/**
   Destroy the world, mwahaha.
   Note that destroying @a world will destroy all the objects it contains
   (e.g. instances of LilvPlugin).  Do not destroy the world until you are
   finished with all objects that came from it.
*/
LILV_API
void
lilv_world_free(LilvWorld world);

/**
   Load all installed LV2 bundles on the system.
   This is the recommended way for hosts to load LV2 data.  It implements the
   established/standard best practice for discovering all LV2 data on the
   system.  The environment variable LV2_PATH may be used to control where
   this function will look for bundles.

   Hosts should use this function rather than explicitly load bundles, except
   in special circumstances (e.g. development utilities, or hosts that ship
   with special plugin bundles which are installed to a known location).
*/
LILV_API
void
lilv_world_load_all(LilvWorld world);

/**
   Load a specific bundle.
   @a bundle_uri must be a fully qualified URI to the bundle directory,
   with the trailing slash, eg. file:///usr/lib/lv2/foo.lv2/

   Normal hosts should not need this function (use lilv_world_load_all).

   Hosts MUST NOT attach any long-term significance to bundle paths
   (e.g. in save files), since there are no guarantees they will remain
   unchanged between (or even during) program invocations. Plugins (among
   other things) MUST be identified by URIs (not paths) in save files.
*/
LILV_API
void
lilv_world_load_bundle(LilvWorld world,
                       LilvValue bundle_uri);

/**
   Get the parent of all other plugin classes, lv2:Plugin.
*/
LILV_API
LilvPluginClass
lilv_world_get_plugin_class(LilvWorld world);

/**
   Return a list of all found plugin classes.
   Returned list is owned by world and must not be freed by the caller.
*/
LILV_API
LilvPluginClasses
lilv_world_get_plugin_classes(LilvWorld world);

/**
   Return a list of all found plugins.
   The returned list contains just enough references to query
   or instantiate plugins.  The data for a particular plugin will not be
   loaded into memory until a call to an lilv_plugin_* function results in
   a query (at which time the data is cached with the LilvPlugin so future
   queries are very fast).

   The returned list and the plugins it contains are owned by @a world
   and must not be freed by caller.
*/
LILV_API
LilvPlugins
lilv_world_get_all_plugins(LilvWorld world);

/**
   Return the plugin with the given @a uri, or NULL if not found.
*/
LILV_API
LilvPlugin
lilv_world_get_plugin_by_uri_string(LilvWorld   world,
                                    const char* uri);

/**
   @}
   @name Plugin
   @{
*/

/**
   Check if @a plugin is valid.
   This is not a rigorous validator, but can be used to reject some malformed
   plugins that could cause bugs (e.g. plugins with missing required fields).

   Note that normal hosts do NOT need to use this - lilv does not
   load invalid plugins into plugin lists.  This is included for plugin
   testing utilities, etc.
   @return true iff @a plugin is valid.
*/
LILV_API
bool
lilv_plugin_verify(LilvPlugin plugin);

/**
   Get the URI of @a plugin.
   Any serialization that refers to plugins should refer to them by this.
   Hosts SHOULD NOT save any filesystem paths, plugin indexes, etc. in saved
   files; save only the URI.

   The URI is a globally unique identifier for one specific plugin.  Two
   plugins with the same URI are compatible in port signature, and should
   be guaranteed to work in a compatible and consistent way.  If a plugin
   is upgraded in an incompatible way (eg if it has different ports), it
   MUST have a different URI than it's predecessor.

   @return A shared URI value which must not be modified or freed.
*/
LILV_API
LilvValue
lilv_plugin_get_uri(LilvPlugin plugin);

/**
   Get the (resolvable) URI of the plugin's "main" bundle.
   This returns the URI of the bundle where the plugin itself was found.
   Note that the data for a plugin may be spread over many bundles, that is,
   lilv_plugin_get_data_uris may return URIs which are not within this bundle.

   Typical hosts should not need to use this function.
   Note this always returns a fully qualified URI.  If you want a local
   filesystem path, use lilv_uri_to_path.
   @return a shared string which must not be modified or freed.
*/
LILV_API
LilvValue
lilv_plugin_get_bundle_uri(LilvPlugin plugin);

/**
   Get the (resolvable) URIs of the RDF data files that define a plugin.
   Typical hosts should not need to use this function.
   Note this always returns fully qualified URIs.  If you want local
   filesystem paths, use lilv_uri_to_path.
   @return a list of complete URLs eg. "file:///foo/ABundle.lv2/aplug.ttl",
   which is shared and must not be modified or freed.
*/
LILV_API
LilvValues
lilv_plugin_get_data_uris(LilvPlugin plugin);

/**
   Get the (resolvable) URI of the shared library for @a plugin.
   Note this always returns a fully qualified URI.  If you want a local
   filesystem path, use lilv_uri_to_path.
   @return a shared string which must not be modified or freed.
*/
LILV_API
LilvValue
lilv_plugin_get_library_uri(LilvPlugin plugin);

/**
   Get the name of @a plugin.
   This returns the name (doap:name) of the plugin.  The name may be
   translated according to the current locale, this value MUST NOT be used
   as a plugin identifier (use the URI for that).
   Returned value must be freed by the caller.
*/
LILV_API
LilvValue
lilv_plugin_get_name(LilvPlugin plugin);

/**
   Get the class this plugin belongs to (ie Filters).
*/
LILV_API
LilvPluginClass
lilv_plugin_get_class(LilvPlugin plugin);

/**
   Get a value associated with the plugin in a plugin's data files.
   @a predicate must be either a URI or a QName.

   Returns the ?object of all triples found of the form:

   <code>&lt;plugin-uri&gt; predicate ?object</code>

   May return NULL if the property was not found, or if object(s) is not
   sensibly represented as an LilvValues (e.g. blank nodes).
   Return value must be freed by caller with lilv_values_free.
*/
LILV_API
LilvValues
lilv_plugin_get_value(LilvPlugin p,
                      LilvValue  predicate);

/**
   Get a value associated with the plugin in a plugin's data files.
   This function is identical to lilv_plugin_get_value, but takes a QName
   string parameter for a predicate instead of an LilvValue, which may be
   more convenient.
*/
LILV_API
LilvValues
lilv_plugin_get_value_by_qname(LilvPlugin  p,
                               const char* predicate);

/**
   Get a value associated with some subject in a plugin's data files.
   @a predicate must be either a URI or a QName.

   Returns the ?object of all triples found of the form:

   <code>subject predicate ?object</code>

   This can be used to investigate URIs returned by lilv_plugin_get_value
   (if information about it is contained in the plugin's data files).

   May return NULL if the property was not found, or if object is not
   sensibly represented as an LilvValues (e.g. blank nodes).
   Return value must be freed by caller with lilv_values_free.
*/
LILV_API
LilvValues
lilv_plugin_get_value_for_subject(LilvPlugin p,
                                  LilvValue  subject_uri,
                                  LilvValue  predicate_uri);

/**
   Return whether a feature is supported by a plugin.
   This will return true if the feature is an optional or required feature
   of the plugin.
*/
LILV_API
bool
lilv_plugin_has_feature(LilvPlugin p,
                        LilvValue  feature_uri);

/**
   Get the LV2 Features supported (required or optionally) by a plugin.
   A feature is "supported" by a plugin if it is required OR optional.

   Since required features have special rules the host must obey, this function
   probably shouldn't be used by normal hosts.  Using lilv_plugin_get_optional_features
   and lilv_plugin_get_required_features separately is best in most cases.

   Returned value must be freed by caller with lilv_values_free.
*/
LILV_API
LilvValues
lilv_plugin_get_supported_features(LilvPlugin p);

/**
   Get the LV2 Features required by a plugin.
   If a feature is required by a plugin, hosts MUST NOT use the plugin if they do not
   understand (or are unable to support) that feature.

   All values returned here MUST be passed to the plugin's instantiate method
   (along with data, if necessary, as defined by the feature specification)
   or plugin instantiation will fail.

   Return value must be freed by caller with lilv_values_free.
*/
LILV_API
LilvValues
lilv_plugin_get_required_features(LilvPlugin p);

/**
   Get the LV2 Features optionally supported by a plugin.
   Hosts MAY ignore optional plugin features for whatever reasons.  Plugins
   MUST operate (at least somewhat) if they are instantiated without being
   passed optional features.

   Return value must be freed by caller with lilv_values_free.
*/
LILV_API
LilvValues
lilv_plugin_get_optional_features(LilvPlugin p);

/**
   Get the number of ports on this plugin.
*/
LILV_API
uint32_t
lilv_plugin_get_num_ports(LilvPlugin p);

/**
   Get the port ranges (minimum, maximum and default values) for all ports.
   @a min_values, @a max_values and @a def_values must either point to an array
   of N floats, where N is the value returned by lilv_plugin_get_num_ports()
   for this plugin, or NULL.  The elements of the array will be set to the
   the minimum, maximum and default values of the ports on this plugin,
   with array index corresponding to port index.  If a port doesn't have a
   minimum, maximum or default value, or the port's type is not float, the
   corresponding array element will be set to NAN.

   This is a convenience method for the common case of getting the range of
   all float ports on a plugin, and may be significantly faster than
   repeated calls to lilv_port_get_range.
*/
LILV_API
void
lilv_plugin_get_port_ranges_float(LilvPlugin p,
                                  float*     min_values,
                                  float*     max_values,
                                  float*     def_values);

/**
   Get the number of ports on this plugin that are members of some class(es).
   Note that this is a varargs function so ports fitting any type 'profile'
   desired can be found quickly.  REMEMBER TO TERMINATE THE PARAMETER LIST
   OF THIS FUNCTION WITH NULL OR VERY NASTY THINGS WILL HAPPEN.
*/
LILV_API
uint32_t
lilv_plugin_get_num_ports_of_class(LilvPlugin p,
                                   LilvValue  class_1, ...);

/**
   Return whether or not the plugin introduces (and reports) latency.
   The index of the latency port can be found with lilv_plugin_get_latency_port
   ONLY if this function returns true.
*/
LILV_API
bool
lilv_plugin_has_latency(LilvPlugin p);

/**
   Return the index of the plugin's latency port.
   It is a fatal error to call this on a plugin without checking if the port
   exists by first calling lilv_plugin_has_latency.

   Any plugin that introduces unwanted latency that should be compensated for
   (by hosts with the ability/need) MUST provide this port, which is a control
   rate output port that reports the latency for each cycle in frames.
*/
LILV_API
uint32_t
lilv_plugin_get_latency_port_index(LilvPlugin p);

/**
   Get a port on @a plugin by @a index.
*/
LILV_API
LilvPort
lilv_plugin_get_port_by_index(LilvPlugin plugin,
                              uint32_t   index);

/**
   Get a port on @a plugin by @a symbol.
   Note this function is slower than lilv_plugin_get_port_by_index,
   especially on plugins with a very large number of ports.
*/
LILV_API
LilvPort
lilv_plugin_get_port_by_symbol(LilvPlugin plugin,
                               LilvValue  symbol);

/**
   Get the full name of the plugin's author.
   Returns NULL if author name is not present.
   Returned value must be freed by caller.
*/
LILV_API
LilvValue
lilv_plugin_get_author_name(LilvPlugin plugin);

/**
   Get the email address of the plugin's author.
   Returns NULL if author email address is not present.
   Returned value must be freed by caller.
*/
LILV_API
LilvValue
lilv_plugin_get_author_email(LilvPlugin plugin);

/**
   Get the email address of the plugin's author.
   Returns NULL if author homepage is not present.
   Returned value must be freed by caller.
*/
LILV_API
LilvValue
lilv_plugin_get_author_homepage(LilvPlugin plugin);

/**
   Return true iff @a plugin has been replaced by another plugin.

   The plugin will still be usable, but hosts should hide them from their
   user interfaces to prevent users from using deprecated plugins.
*/
LILV_API
bool
lilv_plugin_is_replaced(LilvPlugin plugin);

/**
   @}
   @name Port
   @{
*/

/**
   Port analog of lilv_plugin_get_value.
*/
LILV_API
LilvValues
lilv_port_get_value(LilvPlugin plugin,
                    LilvPort   port,
                    LilvValue  predicate);

/**
   Port analog of lilv_plugin_get_value_by_qname.
*/
LILV_API
LilvValues
lilv_port_get_value_by_qname(LilvPlugin  plugin,
                             LilvPort    port,
                             const char* predicate);

/**
   Return the LV2 port properties of a port.
*/
LILV_API
LilvValues
lilv_port_get_properties(LilvPlugin plugin,
                         LilvPort   port);

/**
   Return whether a port has a certain property.
*/
LILV_API
bool
lilv_port_has_property(LilvPlugin p,
                       LilvPort   port,
                       LilvValue  property_uri);

/**
   Return whether a port is an event port and supports a certain event type.
*/
LILV_API
bool
lilv_port_supports_event(LilvPlugin p,
                         LilvPort   port,
                         LilvValue  event_uri);

/**
   Get the symbol of a port.
   The 'symbol' is a short string, a valid C identifier.
   Returned value is owned by @a port and must not be freed.
*/
LILV_API
LilvValue
lilv_port_get_symbol(LilvPlugin plugin,
                     LilvPort   port);

/**
   Get the name of a port.
   This is guaranteed to return the untranslated name (the doap:name in the
   data file without a language tag).  Returned value must be freed by
   the caller.
*/
LILV_API
LilvValue
lilv_port_get_name(LilvPlugin plugin,
                   LilvPort   port);

/**
   Get all the classes of a port.
   This can be used to determine if a port is an input, output, audio,
   control, midi, etc, etc, though it's simpler to use lilv_port_is_a.
   The returned list does not include lv2:Port, which is implied.
   Returned value is shared and must not be destroyed by caller.
*/
LILV_API
LilvValues
lilv_port_get_classes(LilvPlugin plugin,
                      LilvPort   port);

/**
   Determine if a port is of a given class (input, output, audio, etc).
   For convenience/performance/extensibility reasons, hosts are expected to
   create an LilvValue for each port class they "care about".  Well-known type
   URI strings are defined (e.g. LILV_PORT_CLASS_INPUT) for convenience, but
   this function is designed so that Lilv is usable with any port types
   without requiring explicit support in Lilv.
*/
LILV_API
bool
lilv_port_is_a(LilvPlugin plugin,
               LilvPort   port,
               LilvValue  port_class);

/**
   Get the default, minimum, and maximum values of a port.
   @a def, @a min, and @a max are outputs, pass pointers to uninitialized
   (i.e. NOT created with lilv_value_new) LilvValue variables.  These will
   be set to point at new values (which must be freed by the caller using
   lilv_value_free), or NULL if the value does not exist.
*/
LILV_API
void
lilv_port_get_range(LilvPlugin plugin,
                    LilvPort   port,
                    LilvValue* deflt,
                    LilvValue* min,
                    LilvValue* max);

/**
   Get the scale points (enumeration values) of a port.
   This returns a collection of 'interesting' named values of a port
   (e.g. appropriate entries for a UI selector associated with this port).
   Returned value may be NULL if @a port has no scale points, otherwise it
   must be freed by caller with lilv_scale_points_free.
*/
LILV_API
LilvScalePoints
lilv_port_get_scale_points(LilvPlugin plugin,
                           LilvPort   port);

/**
   @}
   @name Scale Point
   @{
*/

/**
   Get the label of this scale point (enumeration value)
   Returned value is owned by @a point and must not be freed.
*/
LILV_API
LilvValue
lilv_scale_point_get_label(LilvScalePoint point);

/**
   Get the value of this scale point (enumeration value)
   Returned value is owned by @a point and must not be freed.
*/
LILV_API
LilvValue
lilv_scale_point_get_value(LilvScalePoint point);

/**
   @}
   @name Plugin Class
   @{
*/

/**
   Get the URI of this class' superclass.
   Returned value is owned by @a plugin_class and must not be freed by caller.
   Returned value may be NULL, if class has no parent.
*/
LILV_API
LilvValue
lilv_plugin_class_get_parent_uri(LilvPluginClass plugin_class);

/**
   Get the URI of this plugin class.
   Returned value is owned by @a plugin_class and must not be freed by caller.
*/
LILV_API
LilvValue
lilv_plugin_class_get_uri(LilvPluginClass plugin_class);

/**
   Get the label of this plugin class, ie "Oscillators".
   Returned value is owned by @a plugin_class and must not be freed by caller.
*/
LILV_API
LilvValue lilv_plugin_class_get_label(LilvPluginClass plugin_class);

/**
   Get the subclasses of this plugin class.
   Returned value must be freed by caller with lilv_plugin_classes_free.
*/
LILV_API
LilvPluginClasses
lilv_plugin_class_get_children(LilvPluginClass plugin_class);

/**
   @}
   @name Plugin Instance
   @{
*/

typedef struct _LilvInstanceImpl* LilvInstanceImpl;

/**
   @cond 0
*/

/* Instance of a plugin.
   This is exposed in the ABI to allow inlining of performance critical
   functions like lilv_instance_run (simple wrappers of functions in lv2.h).
   This is for performance reasons, user code should not use this definition
   in any way (which is why it is not machine documented).
   Truly private implementation details are hidden via @a ref pimpl.
*/
typedef struct _Instance {
	const LV2_Descriptor* lv2_descriptor;
	LV2_Handle            lv2_handle;
	LilvInstanceImpl      pimpl;
}* LilvInstance;

/**
   @endcond
*/

/**
   Instantiate a plugin.
   The returned value is a lightweight handle for an LV2 plugin instance,
   it does not refer to @a plugin, or any other Lilv state.  The caller must
   eventually free it with lilv_instance_free.
   @a features is a NULL-terminated array of features the host supports.
   NULL may be passed if the host supports no additional features.
   @return NULL if instantiation failed.
*/
LILV_API
LilvInstance
lilv_plugin_instantiate(LilvPlugin               plugin,
                        double                   sample_rate,
                        const LV2_Feature*const* features);

/**
   Free a plugin instance.
   @a instance is invalid after this call.
*/
LILV_API
void
lilv_instance_free(LilvInstance instance);

#ifndef LILV_INTERNAL

/**
   Get the URI of the plugin which @a instance is an instance of.
   Returned string is shared and must not be modified or deleted.
*/
static inline const char*
lilv_instance_get_uri(LilvInstance instance)
{
	return instance->lv2_descriptor->URI;
}

/**
   Connect a port to a data location.
   This may be called regardless of whether the plugin is activated,
   activation and deactivation does not destroy port connections.
*/
static inline void
lilv_instance_connect_port(LilvInstance instance,
                           uint32_t     port_index,
                           void*        data_location)
{
	instance->lv2_descriptor->connect_port
		(instance->lv2_handle, port_index, data_location);
}

/**
   Activate a plugin instance.
   This resets all state information in the plugin, except for port data
   locations (as set by lilv_instance_connect_port).  This MUST be called
   before calling lilv_instance_run.
*/
static inline void
lilv_instance_activate(LilvInstance instance)
{
	if (instance->lv2_descriptor->activate)
		instance->lv2_descriptor->activate(instance->lv2_handle);
}

/**
   Run @a instance for @a sample_count frames.
   If the hint lv2:hardRTCapable is set for this plugin, this function is
   guaranteed not to block.
*/
static inline void
lilv_instance_run(LilvInstance instance,
                  uint32_t     sample_count)
{
	instance->lv2_descriptor->run(instance->lv2_handle, sample_count);
}

/**
   Deactivate a plugin instance.
   Note that to run the plugin after this you must activate it, which will
   reset all state information (except port connections).
*/
static inline void
lilv_instance_deactivate(LilvInstance instance)
{
	if (instance->lv2_descriptor->deactivate)
		instance->lv2_descriptor->deactivate(instance->lv2_handle);
}

/**
   Get extension data from the plugin instance.
   The type and semantics of the data returned is specific to the particular
   extension, though in all cases it is shared and must not be deleted.
*/
static inline const void*
lilv_instance_get_extension_data(LilvInstance instance,
                                 const char*  uri)
{
	if (instance->lv2_descriptor->extension_data)
		return instance->lv2_descriptor->extension_data(uri);
	else
		return NULL;
}

/**
   Get the LV2_Descriptor of the plugin instance.
   Normally hosts should not need to access the LV2_Descriptor directly,
   use the lilv_instance_* functions.

   The returned descriptor is shared and must not be deleted.
*/
static inline const LV2_Descriptor*
lilv_instance_get_descriptor(LilvInstance instance)
{
	return instance->lv2_descriptor;
}

/**
   Get the LV2_Handle of the plugin instance.
   Normally hosts should not need to access the LV2_Handle directly,
   use the lilv_instance_* functions.

   The returned handle is shared and must not be deleted.
*/
static inline LV2_Handle
lilv_instance_get_handle(LilvInstance instance)
{
	return instance->lv2_handle;
}

#endif /* LILV_INTERNAL */

/**
   @}
   @name Plugin UI
   @{
*/

/**
   Get all UIs for @a plugin.
   Returned value must be freed by caller using lilv_uis_free.
*/
LILV_API
LilvUIs
lilv_plugin_get_uis(LilvPlugin plugin);

/**
   Get the URI of a Plugin UI.
   @param ui The Plugin UI
   @return a shared value which must not be modified or freed.
*/
LILV_API
LilvValue
lilv_ui_get_uri(LilvUI ui);

/**
   Get the types (URIs of RDF classes) of a Plugin UI.
   @param ui The Plugin UI
   @return a shared value which must not be modified or freed.

   Note that in most cases lilv_ui_is_supported should be used which finds the
   UI type, avoding the need to use this function (and type specific logic).
*/
LILV_API
LilvValues
lilv_ui_get_classes(LilvUI ui);

/**
   Check whether a plugin UI has a given type.
   @param ui        The Plugin UI
   @param class_uri The URI of the LV2 UI type to check this UI against
*/
LILV_API
bool
lilv_ui_is_a(LilvUI ui, LilvValue class_uri);

/**
   Function to determine whether a UI type is supported.

   This is provided by the user and must return non-zero iff using a UI of type
   @c ui_type_uri in a container of type @c container_type_uri is supported.
*/
typedef unsigned (*LilvUISupportedFunc)(const char* container_type_uri,
                                        const char* ui_type_uri);

/**
   Return true iff a Plugin UI is supported as a given widget type.
   @param ui The Plugin UI
   @param supported_func User provided supported predicate.
   @param container_type The widget type to host the UI within.
   @param ui_type (Output) If non-NULL, set to the native type of the UI
   which the caller must free with lilv_value_free.
   @return The embedding quality level returned by @c supported_func.
*/
LILV_API
unsigned
lilv_ui_is_supported(LilvUI              ui,
                     LilvUISupportedFunc supported_func,
                     LilvValue           container_type,
                     LilvValue*          ui_type);

/**
   Get the URI for a Plugin UI's bundle.
   @param ui The Plugin UI
   @return a shared value which must not be modified or freed.
*/
LILV_API
LilvValue
lilv_ui_get_bundle_uri(LilvUI ui);

/**
   Get the URI for a Plugin UI's shared library.
   @param ui The Plugin UI
   @return a shared value which must not be modified or freed.
*/
LILV_API
LilvValue
lilv_ui_get_binary_uri(LilvUI ui);

/**
   @}
   @}
*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LILV_LILV_H */
