/*
 *  This file is part of the WebKit open source project.
 *  This file has been generated by generate-bindings.pl. DO NOT MODIFY!
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "WebKitDOMHTMLAreaElement.h"

#include <WebCore/CSSImportRule.h>
#include "DOMObjectCache.h"
#include <WebCore/Document.h>
#include <WebCore/ExceptionCode.h>
#include <WebCore/ExceptionCodeDescription.h>
#include "GObjectEventListener.h"
#include <WebCore/HTMLNames.h>
#include <WebCore/JSMainThreadExecState.h>
#include "WebKitDOMDOMTokenListPrivate.h"
#include "WebKitDOMEventPrivate.h"
#include "WebKitDOMEventTarget.h"
#include "WebKitDOMHTMLAreaElementPrivate.h"
#include "WebKitDOMNodePrivate.h"
#include "WebKitDOMPrivate.h"
#include "ConvertToUTF8String.h"
#include <wtf/GetPtr.h>
#include <wtf/RefPtr.h>

namespace WebKit {

WebKitDOMHTMLAreaElement* kit(WebCore::HTMLAreaElement* obj)
{
    return WEBKIT_DOM_HTML_AREA_ELEMENT(kit(static_cast<WebCore::Node*>(obj)));
}

WebCore::HTMLAreaElement* core(WebKitDOMHTMLAreaElement* request)
{
    return request ? static_cast<WebCore::HTMLAreaElement*>(WEBKIT_DOM_OBJECT(request)->coreObject) : 0;
}

WebKitDOMHTMLAreaElement* wrapHTMLAreaElement(WebCore::HTMLAreaElement* coreObject)
{
    ASSERT(coreObject);
    return WEBKIT_DOM_HTML_AREA_ELEMENT(g_object_new(WEBKIT_DOM_TYPE_HTML_AREA_ELEMENT, "core-object", coreObject, nullptr));
}

} // namespace WebKit

static gboolean webkit_dom_html_area_element_dispatch_event(WebKitDOMEventTarget* target, WebKitDOMEvent* event, GError** error)
{
    WebCore::Event* coreEvent = WebKit::core(event);
    if (!coreEvent)
        return false;
    WebCore::HTMLAreaElement* coreTarget = static_cast<WebCore::HTMLAreaElement*>(WEBKIT_DOM_OBJECT(target)->coreObject);

    WebCore::ExceptionCode ec = 0;
    gboolean result = coreTarget->dispatchEventForBindings(*coreEvent, ec);
    if (ec) {
        WebCore::ExceptionCodeDescription description(ec);
        g_set_error_literal(error, g_quark_from_string("WEBKIT_DOM"), description.code, description.name);
    }
    return result;
}

static gboolean webkit_dom_html_area_element_add_event_listener(WebKitDOMEventTarget* target, const char* eventName, GClosure* handler, gboolean useCapture)
{
    WebCore::HTMLAreaElement* coreTarget = static_cast<WebCore::HTMLAreaElement*>(WEBKIT_DOM_OBJECT(target)->coreObject);
    return WebKit::GObjectEventListener::addEventListener(G_OBJECT(target), coreTarget, eventName, handler, useCapture);
}

static gboolean webkit_dom_html_area_element_remove_event_listener(WebKitDOMEventTarget* target, const char* eventName, GClosure* handler, gboolean useCapture)
{
    WebCore::HTMLAreaElement* coreTarget = static_cast<WebCore::HTMLAreaElement*>(WEBKIT_DOM_OBJECT(target)->coreObject);
    return WebKit::GObjectEventListener::removeEventListener(G_OBJECT(target), coreTarget, eventName, handler, useCapture);
}

static void webkit_dom_event_target_init(WebKitDOMEventTargetIface* iface)
{
    iface->dispatch_event = webkit_dom_html_area_element_dispatch_event;
    iface->add_event_listener = webkit_dom_html_area_element_add_event_listener;
    iface->remove_event_listener = webkit_dom_html_area_element_remove_event_listener;
}

G_DEFINE_TYPE_WITH_CODE(WebKitDOMHTMLAreaElement, webkit_dom_html_area_element, WEBKIT_DOM_TYPE_HTML_ELEMENT, G_IMPLEMENT_INTERFACE(WEBKIT_DOM_TYPE_EVENT_TARGET, webkit_dom_event_target_init))

enum {
    PROP_0,
    PROP_ALT,
    PROP_COORDS,
    PROP_NO_HREF,
    PROP_SHAPE,
    PROP_TARGET,
    PROP_HREF,
    PROP_PROTOCOL,
    PROP_HOST,
    PROP_HOSTNAME,
    PROP_PORT,
    PROP_PATHNAME,
    PROP_SEARCH,
    PROP_HASH,
};

static void webkit_dom_html_area_element_set_property(GObject* object, guint propertyId, const GValue* value, GParamSpec* pspec)
{
    WebKitDOMHTMLAreaElement* self = WEBKIT_DOM_HTML_AREA_ELEMENT(object);

    switch (propertyId) {
    case PROP_ALT:
        webkit_dom_html_area_element_set_alt(self, g_value_get_string(value));
        break;
    case PROP_COORDS:
        webkit_dom_html_area_element_set_coords(self, g_value_get_string(value));
        break;
    case PROP_NO_HREF:
        webkit_dom_html_area_element_set_no_href(self, g_value_get_boolean(value));
        break;
    case PROP_SHAPE:
        webkit_dom_html_area_element_set_shape(self, g_value_get_string(value));
        break;
    case PROP_TARGET:
        webkit_dom_html_area_element_set_target(self, g_value_get_string(value));
        break;
    case PROP_HREF:
        webkit_dom_html_area_element_set_href(self, g_value_get_string(value));
        break;
    case PROP_PROTOCOL:
        webkit_dom_html_area_element_set_protocol(self, g_value_get_string(value));
        break;
    case PROP_HOST:
        webkit_dom_html_area_element_set_host(self, g_value_get_string(value));
        break;
    case PROP_HOSTNAME:
        webkit_dom_html_area_element_set_hostname(self, g_value_get_string(value));
        break;
    case PROP_PORT:
        webkit_dom_html_area_element_set_port(self, g_value_get_string(value));
        break;
    case PROP_PATHNAME:
        webkit_dom_html_area_element_set_pathname(self, g_value_get_string(value));
        break;
    case PROP_SEARCH:
        webkit_dom_html_area_element_set_search(self, g_value_get_string(value));
        break;
    case PROP_HASH:
        webkit_dom_html_area_element_set_hash(self, g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propertyId, pspec);
        break;
    }
}

static void webkit_dom_html_area_element_get_property(GObject* object, guint propertyId, GValue* value, GParamSpec* pspec)
{
    WebKitDOMHTMLAreaElement* self = WEBKIT_DOM_HTML_AREA_ELEMENT(object);

    switch (propertyId) {
    case PROP_ALT:
        g_value_take_string(value, webkit_dom_html_area_element_get_alt(self));
        break;
    case PROP_COORDS:
        g_value_take_string(value, webkit_dom_html_area_element_get_coords(self));
        break;
    case PROP_NO_HREF:
        g_value_set_boolean(value, webkit_dom_html_area_element_get_no_href(self));
        break;
    case PROP_SHAPE:
        g_value_take_string(value, webkit_dom_html_area_element_get_shape(self));
        break;
    case PROP_TARGET:
        g_value_take_string(value, webkit_dom_html_area_element_get_target(self));
        break;
    case PROP_HREF:
        g_value_take_string(value, webkit_dom_html_area_element_get_href(self));
        break;
    case PROP_PROTOCOL:
        g_value_take_string(value, webkit_dom_html_area_element_get_protocol(self));
        break;
    case PROP_HOST:
        g_value_take_string(value, webkit_dom_html_area_element_get_host(self));
        break;
    case PROP_HOSTNAME:
        g_value_take_string(value, webkit_dom_html_area_element_get_hostname(self));
        break;
    case PROP_PORT:
        g_value_take_string(value, webkit_dom_html_area_element_get_port(self));
        break;
    case PROP_PATHNAME:
        g_value_take_string(value, webkit_dom_html_area_element_get_pathname(self));
        break;
    case PROP_SEARCH:
        g_value_take_string(value, webkit_dom_html_area_element_get_search(self));
        break;
    case PROP_HASH:
        g_value_take_string(value, webkit_dom_html_area_element_get_hash(self));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propertyId, pspec);
        break;
    }
}

static void webkit_dom_html_area_element_class_init(WebKitDOMHTMLAreaElementClass* requestClass)
{
    GObjectClass* gobjectClass = G_OBJECT_CLASS(requestClass);
    gobjectClass->set_property = webkit_dom_html_area_element_set_property;
    gobjectClass->get_property = webkit_dom_html_area_element_get_property;

    g_object_class_install_property(
        gobjectClass,
        PROP_ALT,
        g_param_spec_string(
            "alt",
            "HTMLAreaElement:alt",
            "read-write gchar* HTMLAreaElement:alt",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_COORDS,
        g_param_spec_string(
            "coords",
            "HTMLAreaElement:coords",
            "read-write gchar* HTMLAreaElement:coords",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_NO_HREF,
        g_param_spec_boolean(
            "no-href",
            "HTMLAreaElement:no-href",
            "read-write gboolean HTMLAreaElement:no-href",
            FALSE,
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_SHAPE,
        g_param_spec_string(
            "shape",
            "HTMLAreaElement:shape",
            "read-write gchar* HTMLAreaElement:shape",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_TARGET,
        g_param_spec_string(
            "target",
            "HTMLAreaElement:target",
            "read-write gchar* HTMLAreaElement:target",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_HREF,
        g_param_spec_string(
            "href",
            "HTMLAreaElement:href",
            "read-write gchar* HTMLAreaElement:href",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_PROTOCOL,
        g_param_spec_string(
            "protocol",
            "HTMLAreaElement:protocol",
            "read-write gchar* HTMLAreaElement:protocol",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_HOST,
        g_param_spec_string(
            "host",
            "HTMLAreaElement:host",
            "read-write gchar* HTMLAreaElement:host",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_HOSTNAME,
        g_param_spec_string(
            "hostname",
            "HTMLAreaElement:hostname",
            "read-write gchar* HTMLAreaElement:hostname",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_PORT,
        g_param_spec_string(
            "port",
            "HTMLAreaElement:port",
            "read-write gchar* HTMLAreaElement:port",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_PATHNAME,
        g_param_spec_string(
            "pathname",
            "HTMLAreaElement:pathname",
            "read-write gchar* HTMLAreaElement:pathname",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_SEARCH,
        g_param_spec_string(
            "search",
            "HTMLAreaElement:search",
            "read-write gchar* HTMLAreaElement:search",
            "",
            WEBKIT_PARAM_READWRITE));

    g_object_class_install_property(
        gobjectClass,
        PROP_HASH,
        g_param_spec_string(
            "hash",
            "HTMLAreaElement:hash",
            "read-write gchar* HTMLAreaElement:hash",
            "",
            WEBKIT_PARAM_READWRITE));

}

static void webkit_dom_html_area_element_init(WebKitDOMHTMLAreaElement* request)
{
    UNUSED_PARAM(request);
}

gchar* webkit_dom_html_area_element_get_alt(WebKitDOMHTMLAreaElement* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self), 0);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->attributeWithoutSynchronization(WebCore::HTMLNames::altAttr));
    return result;
}

void webkit_dom_html_area_element_set_alt(WebKitDOMHTMLAreaElement* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self));
    g_return_if_fail(value);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setAttributeWithoutSynchronization(WebCore::HTMLNames::altAttr, convertedValue);
}

gchar* webkit_dom_html_area_element_get_coords(WebKitDOMHTMLAreaElement* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self), 0);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->attributeWithoutSynchronization(WebCore::HTMLNames::coordsAttr));
    return result;
}

void webkit_dom_html_area_element_set_coords(WebKitDOMHTMLAreaElement* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self));
    g_return_if_fail(value);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setAttributeWithoutSynchronization(WebCore::HTMLNames::coordsAttr, convertedValue);
}

gboolean webkit_dom_html_area_element_get_no_href(WebKitDOMHTMLAreaElement* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self), FALSE);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    gboolean result = item->hasAttributeWithoutSynchronization(WebCore::HTMLNames::nohrefAttr);
    return result;
}

void webkit_dom_html_area_element_set_no_href(WebKitDOMHTMLAreaElement* self, gboolean value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self));
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    item->setBooleanAttribute(WebCore::HTMLNames::nohrefAttr, value);
}

gchar* webkit_dom_html_area_element_get_shape(WebKitDOMHTMLAreaElement* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self), 0);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->attributeWithoutSynchronization(WebCore::HTMLNames::shapeAttr));
    return result;
}

void webkit_dom_html_area_element_set_shape(WebKitDOMHTMLAreaElement* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self));
    g_return_if_fail(value);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setAttributeWithoutSynchronization(WebCore::HTMLNames::shapeAttr, convertedValue);
}

gchar* webkit_dom_html_area_element_get_target(WebKitDOMHTMLAreaElement* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self), 0);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->attributeWithoutSynchronization(WebCore::HTMLNames::targetAttr));
    return result;
}

void webkit_dom_html_area_element_set_target(WebKitDOMHTMLAreaElement* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self));
    g_return_if_fail(value);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setAttributeWithoutSynchronization(WebCore::HTMLNames::targetAttr, convertedValue);
}

gchar* webkit_dom_html_area_element_get_href(WebKitDOMHTMLAreaElement* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self), 0);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->getURLAttribute(WebCore::HTMLNames::hrefAttr));
    return result;
}

void webkit_dom_html_area_element_set_href(WebKitDOMHTMLAreaElement* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self));
    g_return_if_fail(value);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setAttributeWithoutSynchronization(WebCore::HTMLNames::hrefAttr, convertedValue);
}

gchar* webkit_dom_html_area_element_get_protocol(WebKitDOMHTMLAreaElement* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self), 0);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->protocol());
    return result;
}

void webkit_dom_html_area_element_set_protocol(WebKitDOMHTMLAreaElement* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self));
    g_return_if_fail(value);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setProtocol(convertedValue);
}

gchar* webkit_dom_html_area_element_get_host(WebKitDOMHTMLAreaElement* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self), 0);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->host());
    return result;
}

void webkit_dom_html_area_element_set_host(WebKitDOMHTMLAreaElement* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self));
    g_return_if_fail(value);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setHost(convertedValue);
}

gchar* webkit_dom_html_area_element_get_hostname(WebKitDOMHTMLAreaElement* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self), 0);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->hostname());
    return result;
}

void webkit_dom_html_area_element_set_hostname(WebKitDOMHTMLAreaElement* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self));
    g_return_if_fail(value);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setHostname(convertedValue);
}

gchar* webkit_dom_html_area_element_get_port(WebKitDOMHTMLAreaElement* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self), 0);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->port());
    return result;
}

void webkit_dom_html_area_element_set_port(WebKitDOMHTMLAreaElement* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self));
    g_return_if_fail(value);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setPort(convertedValue);
}

gchar* webkit_dom_html_area_element_get_pathname(WebKitDOMHTMLAreaElement* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self), 0);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->pathname());
    return result;
}

void webkit_dom_html_area_element_set_pathname(WebKitDOMHTMLAreaElement* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self));
    g_return_if_fail(value);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setPathname(convertedValue);
}

gchar* webkit_dom_html_area_element_get_search(WebKitDOMHTMLAreaElement* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self), 0);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->search());
    return result;
}

void webkit_dom_html_area_element_set_search(WebKitDOMHTMLAreaElement* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self));
    g_return_if_fail(value);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setSearch(convertedValue);
}

gchar* webkit_dom_html_area_element_get_hash(WebKitDOMHTMLAreaElement* self)
{
    WebCore::JSMainThreadNullState state;
    g_return_val_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self), 0);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->hash());
    return result;
}

void webkit_dom_html_area_element_set_hash(WebKitDOMHTMLAreaElement* self, const gchar* value)
{
    WebCore::JSMainThreadNullState state;
    g_return_if_fail(WEBKIT_DOM_IS_HTML_AREA_ELEMENT(self));
    g_return_if_fail(value);
    WebCore::HTMLAreaElement* item = WebKit::core(self);
    WTF::String convertedValue = WTF::String::fromUTF8(value);
    item->setHash(convertedValue);
}

