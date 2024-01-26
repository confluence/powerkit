/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWER_CLIENT_H
#define POWER_CLIENT_H

#include <QDBusInterface>

class PowerClient
{
public:
    static double getBatteryLeft(QDBusInterface *iface);
    static bool hasBattery(QDBusInterface *iface);
    static bool onBattery(QDBusInterface *iface);
    static qlonglong timeToEmpty(QDBusInterface *iface);
    static bool canHibernate(QDBusInterface *iface);
    static bool canSuspend(QDBusInterface *iface);
    static bool canRestart(QDBusInterface *iface);
    static bool canPowerOff(QDBusInterface *iface);
    static bool lidIsPresent(QDBusInterface *iface);
    static void lockScreen(QDBusInterface *iface);
    static void hibernate(QDBusInterface *iface);
    static void suspend(QDBusInterface *iface);
    static bool restart(QDBusInterface *iface);
    static bool poweroff(QDBusInterface *iface);
};

#endif // POWER_CLIENT_H
