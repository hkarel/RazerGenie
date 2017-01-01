/*
 * Copyright (C) 2016  Luca Weiss <luca (at) z3ntu (dot) xyz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDebug>
#include <QDomDocument>

#include <iostream>

#include "razermethods.h"

namespace razermethods
{
/* General methods */
QStringList getConnectedDevices()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "getDevices");
    return QDBusMessageToStringList(m);
}

bool syncDevices(bool yes)
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "syncEffects");

    // Set arguments
    QList<QVariant> args;
    yes ? args.append("True") : args.append("False"); // maybe bool works here
    m.setArguments(args);

    bool queued = QDBusConnection::sessionBus().send(m);
    std::cout << "Queued: " << queued << std::endl;
    return queued;
}

QString getDriverVersion()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.daemon", "version");
    return QDBusMessageToString(m);
}

/* Device class methods */
Device::Device(QString s)
{
    serial = s;
    Introspect();
    setupCapabilities();
}

QString Device::getDeviceName()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDeviceName");
    return QDBusMessageToString(m);
}

QString Device::getDeviceType()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDeviceType");
    return QDBusMessageToString(m);
}

QString Device::getPngFilename()
{
    return urlLookup.value(getDeviceName()) + ".png";
}

void Device::setLogoStatic(int r, int g, int b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoStatic");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    QDBusMessageToVoid(m);
}

void Device::Introspect()
{
    QStringList intr;

    QDBusMessage m = prepareDeviceQDBusMessage("org.freedesktop.DBus.Introspectable", "Introspect");
    //introspection = QDBusMessageToXML(m);
    QDomDocument doc = QDBusMessageToXML(m);

    QDomNodeList nodes = doc.documentElement().childNodes();
    //for(QDomNode node : docElem.childNodes()) {
    for(int i = 0; i<nodes.count(); i++) {
        // Check if "interface" and also not org.freedesktop.DBus.Introspectable
        QDomElement element = nodes.at(i).toElement();
        QString interfacename = element.attributeNode("name").value();

        QDomNodeList methodnodes = element.childNodes();
        for(int ii = 0; ii<methodnodes.count(); ii++) {
            QDomElement methodelement = methodnodes.at(ii).toElement();
            intr.append(interfacename + ";" + methodelement.attributeNode("name").value());
        }
        intr.append(interfacename);
    }
    introspection = intr;
}

void Device::setupCapabilities()
{
    capabilites.insert("name", true);
    capabilites.insert("type", true);
    capabilites.insert("firmware_version", true);
    capabilites.insert("serial", true);
    capabilites.insert("brightness", hasCapabilityInternal("razer.device.lighting.brightness"));

    capabilites.insert("macro_logic", hasCapabilityInternal("razer.device.macro"));

    // Default device is a chroma so lighting capabilities
    capabilites.insert("lighting", hasCapabilityInternal("razer.device.lighting.chroma"));
    capabilites.insert("lighting_breath_single", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathSingle"));
    capabilites.insert("lighting_breath_dual", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathDual"));
    capabilites.insert("lighting_breath_triple", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathTriple"));
    capabilites.insert("lighting_breath_random", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathRandom"));
    capabilites.insert("lighting_wave", hasCapabilityInternal("razer.device.lighting.chroma", "setWave"));
    capabilites.insert("lighting_reactive", hasCapabilityInternal("razer.device.lighting.chroma", "setReactive"));
    capabilites.insert("lighting_none", hasCapabilityInternal("razer.device.lighting.chroma", "setNone"));
    capabilites.insert("lighting_spectrum", hasCapabilityInternal("razer.device.lighting.chroma", "setSpectrum"));
    capabilites.insert("lighting_static", hasCapabilityInternal("razer.device.lighting.chroma", "setStatic"));
    capabilites.insert("lighting_ripple", hasCapabilityInternal("razer.device.lighting.custom", "setRipple")); // Thinking of extending custom to do more hence the key check
    capabilites.insert("lighting_ripple_random", hasCapabilityInternal("razer.device.lighting.custom", "setRippleRandomColour"));

    capabilites.insert("lighting_pulsate", hasCapabilityInternal("razer.device.lighting.chroma", "setPulsate"));

    // Get if the device has an LED Matrix, == True as its a DBus boolean otherwise, so for consistency sake we coerce it into a native bool
    // FIXME: 'lighting_led_matrix': self._dbus_interfaces['device'].hasMatrix() == True,
    capabilites.insert("lighting_led_single", hasCapabilityInternal("razer.device.lighting.chroma", "setKey"));

    // Mouse lighting attrs
    capabilites.insert("lighting_logo", hasCapabilityInternal("razer.device.lighting.logo"));
    capabilites.insert("lighting_logo_blinking", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBlinking"));
    capabilites.insert("lighting_logo_brightness", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBrightness"));
    capabilites.insert("lighting_logo_pulsate", hasCapabilityInternal("razer.device.lighting.logo", "setLogoPulsate"));
    capabilites.insert("lighting_logo_spectrum", hasCapabilityInternal("razer.device.lighting.logo", "setLogoSpectrum"));
    capabilites.insert("lighting_logo_static", hasCapabilityInternal("razer.device.lighting.logo", "setLogoStatic"));
    capabilites.insert("lighting_logo_none", hasCapabilityInternal("razer.device.lighting.logo", "setLogoNone"));
    capabilites.insert("lighting_logo_reactive", hasCapabilityInternal("razer.device.lighting.logo", "setLogoReactive"));
    capabilites.insert("lighting_logo_breath_single", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBreathSingle"));
    capabilites.insert("lighting_logo_breath_dual", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBreathDual"));
    capabilites.insert("lighting_logo_breath_random", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBreathRandom"));

    capabilites.insert("lighting_scroll", hasCapabilityInternal("razer.device.lighting.scroll"));
    capabilites.insert("lighting_scroll_blinking", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBlinking"));
    capabilites.insert("lighting_scroll_brightness", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBrightness"));
    capabilites.insert("lighting_scroll_pulsate", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollPulsate"));
    capabilites.insert("lighting_scroll_spectrum", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollSpectrum"));
    capabilites.insert("lighting_scroll_static", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollStatic"));
    capabilites.insert("lighting_scroll_none", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollNone"));
    capabilites.insert("lighting_scroll_reactive", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollReactive"));
    capabilites.insert("lighting_scroll_breath_single", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBreathSingle"));
    capabilites.insert("lighting_scroll_breath_dual", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBreathDual"));
    capabilites.insert("lighting_scroll_breath_random", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBreathRandom"));
}

bool Device::hasCapability(const QString &name)
{
    return capabilites.value(name);
}

bool Device::hasCapabilityInternal(const QString &interface, const QString &method)
{
    if(method.isNull()) {
        return introspection.contains(interface);
    }
    return introspection.contains(interface + ";" + method);
}

QHash<QString, bool> Device::getAllCapabilities()
{
    return capabilites;
}

Device::~Device()
{
    bool hasCapabilityInternal(const QString &interface, const QString &method = QString());

}

/* Helper methods */
QDBusMessage Device::prepareDeviceQDBusMessage(const QString &interface, const QString &method)
{
    return QDBusMessage::createMethodCall("org.razer", "/org/razer/device/" + serial, interface, method);
}

QDBusMessage prepareGeneralQDBusMessage(const QString &interface, const QString &method)
{
    return QDBusMessage::createMethodCall("org.razer", "/org/razer", interface, method);
}

QString QDBusMessageToString(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        // Everything went fine.
        return msg.arguments()[0].toString();
    }
    // TODO: Handle error
    return "error";
}

QStringList QDBusMessageToStringList(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        return msg.arguments()[0].toStringList();
    }
    // TODO: Handle errror
    return msg.arguments()[0].toStringList();
}

QDomDocument QDBusMessageToXML(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    QDomDocument doc;
    if(msg.type() == QDBusMessage::ReplyMessage) {
        doc.setContent(msg.arguments()[0].toString());
    }
    // TODO: Handle errror
    return doc;
}

void QDBusMessageToVoid(const QDBusMessage &message)
{
    QDBusConnection::sessionBus().send(message);
    // TODO: Handle errror
}
}

int main()
{
    std::cout << "Driver version: " << razermethods::getDriverVersion().toStdString() << std::endl;
    QStringList serialnrs = razermethods::getConnectedDevices();
    foreach (const QString &str, serialnrs) {
        std::cout << "-----------------" << std::endl;
        std::cout << "Serial: " << str.toStdString() << std::endl;
        razermethods::Device device = razermethods::Device(str);
        std::cout << "Name:   " << device.getDeviceName().toStdString() << std::endl;
        std::cout << "Type:   " << device.getDeviceType().toStdString() << std::endl;
        //bool logostatic = device.hasCapabilityInternal("razer.device.lighting.logo", "setLogoStatic");
        //std::cout << "Can logo static: " << logostatic << std::endl;
        QHash<QString, bool> hash = device.getAllCapabilities();
        for (QHash<QString, bool>::iterator i = hash.begin(); i != hash.end(); ++i)
            std::cout << i.key().toStdString() << ": " << i.value() << std::endl;
    }
}

