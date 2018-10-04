/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "dialog.h"
#include "login1.h"
#include "ckit.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , dbus(0)
    , lidActionBattery(0)
    , lidActionAC(0)
    , criticalActionBattery(0)
    , criticalBattery(0)
    , autoSleepBattery(0)
    , autoSleepAC(0)
    , desktopSS(0)
    , desktopPM(0)
    , showNotifications(0)
    , showSystemTray(0)
    , disableLidAction(0)
    , autoSleepBatteryAction(0)
    , autoSleepACAction(0)
    , lockscreenButton(0)
    , sleepButton(0)
    , hibernateButton(0)
    , poweroffButton(0)
    , lidXrandr(0)
    , hasBacklight(false)
    , backlightSlider(0)
    , backlightWatcher(0)
    , man(0)
    , batteryIcon(0)
    , batteryLabel(0)
    , deviceTree(0)
    , batteryLeftLCD(0)
{
    // setup dialog
    setAttribute(Qt::WA_QuitOnClose, true);
    setWindowTitle(tr("Power Manager"));
    setMinimumSize(QSize(387, 309));

    // setup dbus
    QDBusConnection session = QDBusConnection::sessionBus();
    dbus = new QDBusInterface(PD_SERVICE, PD_PATH, PD_SERVICE,
                              session, this);

    // setup man
    man = new Power(this);

    // setup theme
    Common::setIconTheme();
    setWindowIcon(QIcon::fromTheme(DEFAULT_AC_ICON));

    // setup widgets
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(5);
    layout->setSpacing(0);

    QTabWidget *containerWidget = new QTabWidget(this);

    QWidget *wrapper = new QWidget(this);
    wrapper->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *wrapperLayout = new QVBoxLayout(wrapper);
    wrapperLayout->setMargin(0);
    wrapperLayout->setSpacing(0);
    wrapperLayout->addWidget(containerWidget);

    QGroupBox *batteryContainer = new QGroupBox(this);
    batteryContainer->setTitle(tr("On Battery"));
    batteryContainer->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding);
    QVBoxLayout *batteryContainerLayout = new QVBoxLayout(batteryContainer);
    batteryContainerLayout->setMargin(0);
    batteryContainerLayout->setSpacing(0);

    QWidget *lidActionBatteryContainer = new QWidget(this);
    QHBoxLayout *lidActionBatteryContainerLayout = new QHBoxLayout(lidActionBatteryContainer);
    lidActionBattery = new QComboBox(this);
    QLabel *lidActionBatteryLabel = new QLabel(this);

    QLabel *lidActionBatteryIcon = new QLabel(this);
    lidActionBatteryIcon->setMaximumSize(48, 48);
    lidActionBatteryIcon->setMinimumSize(48, 48);
    lidActionBatteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_VIDEO_ICON)
                                    .pixmap(QSize(48, 48)));
    lidActionBatteryLabel->setText(tr("<h3 style=\"font-weight:normal;\">Lid action</h3>"));
    lidActionBatteryContainerLayout->addWidget(lidActionBatteryIcon);
    lidActionBatteryContainerLayout->addWidget(lidActionBatteryLabel);
    lidActionBatteryContainerLayout->addWidget(lidActionBattery);
    batteryContainerLayout->addWidget(lidActionBatteryContainer);

    QWidget *criticalBatteryContainer = new QWidget(this);
    QHBoxLayout *criticalBatteryContainerLayout = new QHBoxLayout(criticalBatteryContainer);
    criticalBattery = new QSpinBox(this);
    criticalBattery->setMinimum(0);
    criticalBattery->setMaximum(99);
    criticalBattery->setSuffix(tr(" %"));
    QLabel *criticalBatteryLabel = new QLabel(this);

    QWidget *criticalActionBatteryContainer = new QWidget(this);
    criticalActionBatteryContainer->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *criticalActionBatteryContainerLayout = new QVBoxLayout(criticalActionBatteryContainer);
    criticalActionBatteryContainerLayout->setMargin(0);
    criticalActionBatteryContainerLayout->setSpacing(0);
    criticalActionBattery = new QComboBox(this);
    criticalActionBatteryContainerLayout->addWidget(criticalBattery);
    criticalActionBatteryContainerLayout->addWidget(criticalActionBattery);

    QLabel *criticalBatteryIcon = new QLabel(this);
    criticalBatteryIcon->setMaximumSize(48, 48);
    criticalBatteryIcon->setMinimumSize(48, 48);
    criticalBatteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_BATTERY_ICON_CRIT)
                                   .pixmap(QSize(48, 48)));
    criticalBatteryLabel->setText(tr("<h3 style=\"font-weight:normal;\">Critical battery</h3>"));
    criticalBatteryContainerLayout->addWidget(criticalBatteryIcon);
    criticalBatteryContainerLayout->addWidget(criticalBatteryLabel);
    criticalBatteryContainerLayout->addWidget(criticalActionBatteryContainer);

    QWidget *sleepBatteryContainer = new QWidget(this);
    QHBoxLayout *sleepBatteryContainerLayout = new QHBoxLayout(sleepBatteryContainer);
    autoSleepBattery = new QSpinBox(this);
    autoSleepBattery->setMinimum(0);
    autoSleepBattery->setMaximum(1000);
    autoSleepBattery->setSuffix(tr(" min"));
    QLabel *sleepBatteryLabel = new QLabel(this);

    QWidget *sleepActionBatteryContainer = new QWidget(this);
    sleepActionBatteryContainer->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *sleepActionBatteryContainerLayout = new QVBoxLayout(sleepActionBatteryContainer);
    sleepActionBatteryContainerLayout->setMargin(0);
    sleepActionBatteryContainerLayout->setSpacing(0);
    autoSleepBatteryAction = new QComboBox(this);
    sleepActionBatteryContainerLayout->addWidget(autoSleepBattery);
    sleepActionBatteryContainerLayout->addWidget(autoSleepBatteryAction);

    QLabel *sleepBatteryIcon = new QLabel(this);
    sleepBatteryIcon->setMaximumSize(48, 48);
    sleepBatteryIcon->setMinimumSize(48, 48);
    sleepBatteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_SUSPEND_ICON)
                                .pixmap(QSize(48, 48)));
    sleepBatteryLabel->setText(tr("<h3 style=\"font-weight:normal;\">Suspend after</h3>"));
    sleepBatteryContainerLayout->addWidget(sleepBatteryIcon);
    sleepBatteryContainerLayout->addWidget(sleepBatteryLabel);
    sleepBatteryContainerLayout->addWidget(sleepActionBatteryContainer);

    batteryContainerLayout->addWidget(sleepBatteryContainer);
    batteryContainerLayout->addWidget(criticalBatteryContainer);

    batteryContainerLayout->addStretch();

    QGroupBox *acContainer = new QGroupBox(this);
    acContainer->setTitle(tr("On AC"));
    QVBoxLayout *acContainerLayout = new QVBoxLayout(acContainer);
    acContainerLayout->setMargin(0);
    acContainerLayout->setSpacing(0);

    QWidget *lidActionACContainer = new QWidget(this);
    QHBoxLayout *lidActionACContainerLayout = new QHBoxLayout(lidActionACContainer);
    lidActionAC = new QComboBox(this);
    QLabel *lidActionACLabel = new QLabel(this);

    QLabel *lidActionACIcon = new QLabel(this);
    lidActionACIcon->setMaximumSize(48, 48);
    lidActionACIcon->setMinimumSize(48, 48);
    lidActionACIcon->setPixmap(QIcon::fromTheme(DEFAULT_VIDEO_ICON)
                               .pixmap(QSize(48, 48)));
    lidActionACLabel->setText(tr("<h3 style=\"font-weight:normal;\">Lid action</h3>"));
    lidActionACContainerLayout->addWidget(lidActionACIcon);
    lidActionACContainerLayout->addWidget(lidActionACLabel);
    lidActionACContainerLayout->addWidget(lidActionAC);
    acContainerLayout->addWidget(lidActionACContainer);

    QWidget *sleepACContainer = new QWidget(this);
    QHBoxLayout *sleepACContainerLayout = new QHBoxLayout(sleepACContainer);
    autoSleepAC = new QSpinBox(this);
    autoSleepAC->setMinimum(0);
    autoSleepAC->setMaximum(1000);
    autoSleepAC->setSuffix(tr(" min"));
    QLabel *sleepACLabel = new QLabel(this);

    sleepACContainerLayout->addWidget(sleepACLabel);
    acContainerLayout->addWidget(sleepACContainer);

    QWidget *sleepActionACContainer = new QWidget(this);
    sleepActionACContainer->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *sleepActionACContainerLayout = new QVBoxLayout(sleepActionACContainer);
    sleepActionACContainerLayout->setMargin(0);
    sleepActionACContainerLayout->setSpacing(0);
    autoSleepACAction = new QComboBox(this);
    sleepActionACContainerLayout->addWidget(autoSleepAC);
    sleepActionACContainerLayout->addWidget(autoSleepACAction);

    QLabel *sleepACIcon = new QLabel(this);
    sleepACIcon->setMaximumSize(48,48);
    sleepACIcon->setMinimumSize(48,48);
    sleepACIcon->setPixmap(QIcon::fromTheme(DEFAULT_SUSPEND_ICON)
                           .pixmap(QSize(48, 48)));
    sleepACLabel->setText(tr("<h3 style=\"font-weight:normal;\">Suspend after</h3>"));
    sleepACContainerLayout->addWidget(sleepACIcon);
    sleepACContainerLayout->addWidget(sleepACLabel);
    sleepACContainerLayout->addWidget(sleepActionACContainer);
    acContainerLayout->addWidget(sleepACContainer);

    acContainerLayout->addStretch();

    QGroupBox *advContainer = new QGroupBox(this);
    advContainer->setTitle(tr("Advanced"));
    QVBoxLayout *advContainerLayout = new QVBoxLayout(advContainer);

    showSystemTray  = new QCheckBox(this);
    showSystemTray->setIcon(QIcon::fromTheme(DEFAULT_TRAY_ICON));
    showSystemTray->setText(tr("Show system tray"));

    showNotifications = new QCheckBox(this);
    showNotifications->setIcon(QIcon::fromTheme(DEFAULT_NOTIFY_ICON));
    showNotifications->setText(tr("Show notifications"));

    desktopSS = new QCheckBox(this);
    desktopSS->setIcon(QIcon::fromTheme(DEFAULT_VIDEO_ICON));
    desktopSS->setText("org.freedesktop.ScreenSaver");

    desktopPM = new QCheckBox(this);
    desktopPM->setIcon(QIcon::fromTheme(DEFAULT_BATTERY_ICON));
    desktopPM->setText("org.freedesktop.PowerManagement");

    lidXrandr = new QCheckBox(this);
    lidXrandr->setIcon(QIcon::fromTheme(DEFAULT_VIDEO_ICON));
    lidXrandr->setText(tr("Switch internal monitor on/off"
                          "\nwith xrandr if lid action disabled"));

    disableLidAction = new QCheckBox(this);
    disableLidAction->setIcon(QIcon::fromTheme(DEFAULT_VIDEO_ICON));
    disableLidAction->setText(tr("Disable lid action if external"
                                 "\nmonitor(s) is connected"));

    advContainerLayout->addWidget(showSystemTray);
    advContainerLayout->addWidget(showNotifications);
    advContainerLayout->addWidget(desktopSS);
    advContainerLayout->addWidget(desktopPM);
    advContainerLayout->addWidget(disableLidAction);
    advContainerLayout->addWidget(lidXrandr);
    advContainerLayout->addStretch();

    QWidget *extraContainer = new QWidget(this);
    QHBoxLayout *extraContainerLayout = new QHBoxLayout(extraContainer);

    lockscreenButton = new QPushButton(this);
    lockscreenButton->setIcon(QIcon::fromTheme(DEFAULT_LOCK_ICON));
    lockscreenButton->setIconSize(QSize(24, 24));
    lockscreenButton->setToolTip(tr("Lock the screen now."));
    if (lockscreenButton->icon().isNull()) {
        lockscreenButton->setText(tr("Lock screen"));
    }

    sleepButton = new QPushButton(this);
    sleepButton->setIcon(QIcon::fromTheme(DEFAULT_SUSPEND_ICON));
    sleepButton->setIconSize(QSize(24, 24));
    sleepButton->setToolTip(tr("Suspend computer now."));
    if (sleepButton->icon().isNull()) {
        sleepButton->setText(tr("Suspend"));
    }

    hibernateButton = new QPushButton(this);
    hibernateButton->setIcon(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON));
    hibernateButton->setIconSize(QSize(24, 24));
    hibernateButton->setToolTip(tr("Hibernate computer now."));
    if (hibernateButton->icon().isNull()) {
        hibernateButton->setText(tr("Hibernate"));
    }

    poweroffButton = new QPushButton(this);
    poweroffButton->setIcon(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON));
    poweroffButton->setIconSize(QSize(24, 24));
    poweroffButton->setToolTip(tr("Shutdown computer now."));
    if (poweroffButton->icon().isNull()) {
        poweroffButton->setText(tr("Shutdown"));
    }

    backlightSlider = new QSlider(this);
    backlightSlider->hide();
    backlightSlider->setSingleStep(1);
    backlightSlider->setOrientation(Qt::Horizontal);
    backlightWatcher = new QFileSystemWatcher(this);

    QLabel *backlightLabel = new QLabel(this);
    backlightLabel->setPixmap(QIcon::fromTheme(DEFAULT_BACKLIGHT_ICON).pixmap(24, 24));

    extraContainerLayout->addWidget(backlightLabel);
    extraContainerLayout->addWidget(backlightSlider);
    extraContainerLayout->addStretch();
    extraContainerLayout->addWidget(lockscreenButton);
    extraContainerLayout->addWidget(sleepButton);
    extraContainerLayout->addWidget(hibernateButton);
    extraContainerLayout->addWidget(poweroffButton);

    QWidget *statusContainer = new QWidget(this);
    QVBoxLayout *statusContainerLayout = new QVBoxLayout(statusContainer);

    QGroupBox *batteryStatusBox = new QGroupBox(this);
    batteryStatusBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *batteryStatusLayout = new QHBoxLayout(batteryStatusBox);

    batteryIcon = new QLabel(this);
    batteryLabel = new QLabel(this);
    batteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_BATTERY_ICON).pixmap(QSize(48, 48)));

    batteryLeftLCD = new QLCDNumber(this);
    batteryLeftLCD->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    batteryLeftLCD->setFrameStyle(QFrame::NoFrame);
    batteryLeftLCD->setSegmentStyle(QLCDNumber::Flat);
    batteryLeftLCD->display("00:00");

    deviceTree = new QTreeWidget(this);
    deviceTree->setStyleSheet("QTreeWidget,QTreeWidget::item,"
                              "QTreeWidget::item:selected"
                              "{background:transparent;border:0;}");
    deviceTree->setHeaderHidden(true);
    deviceTree->setHeaderLabels(QStringList() << "1" << "2");
    deviceTree->setColumnWidth(0, 150);

    QLabel *aboutLabel = new QLabel(this);
    aboutLabel->setText(QString("<p style=\"text-align:center;font-size:small;\">"
                                "<a href=\"https://github.com/rodlie/powerdwarf\">"
                                "powerdwarf</a> %1 &copy;2018 Ole-Andr&eacute; Rodlie")
                        .arg(qApp->applicationVersion()));

    batteryStatusLayout->addWidget(batteryIcon);
    batteryStatusLayout->addWidget(batteryLabel);
    batteryStatusLayout->addStretch();
    batteryStatusLayout->addWidget(batteryLeftLCD);

    statusContainerLayout->addWidget(batteryStatusBox);
    statusContainerLayout->addWidget(deviceTree);
    statusContainerLayout->addStretch();
    statusContainerLayout->addWidget(aboutLabel);

    layout->addWidget(wrapper);
    layout->addWidget(extraContainer);

    QWidget *settingsWidget = new QWidget(this);
    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsWidget);
    QScrollArea *settingsContainerArea = new QScrollArea(this);
    settingsContainerArea->setSizePolicy(QSizePolicy::Expanding,
                                         QSizePolicy::Expanding);
    settingsContainerArea->setStyleSheet("QScrollArea {border:0;}");
    settingsContainerArea->setWidgetResizable(true);
    settingsContainerArea->setWidget(settingsWidget);
    settingsLayout->addWidget(batteryContainer);
    settingsLayout->addWidget(acContainer);
    settingsLayout->addWidget(advContainer);

    containerWidget->addTab(statusContainer,
                            QIcon::fromTheme(DEFAULT_INFO_ICON),
                            tr("Status"));
    containerWidget->addTab(settingsContainerArea,
                            QIcon::fromTheme(DEFAULT_BATTERY_ICON),
                            tr("Settings"));

    populate(); // populate boxes
    loadSettings(); // load settings

    backlightSlider->setFocus();

    // connect various widgets
    connect(lockscreenButton, SIGNAL(released()),
            this, SLOT(handleLockscreenButton()));
    connect(sleepButton, SIGNAL(released()),
            this, SLOT(handleSleepButton()));
    connect(hibernateButton, SIGNAL(released()),
            this, SLOT(handleHibernateButton()));
    connect(poweroffButton, SIGNAL(released()),
            this, SLOT(handlePoweroffButton()));
    connect(lidActionBattery, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleLidActionBattery(int)));
    connect(lidActionAC, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleLidActionAC(int)));
    connect(criticalActionBattery, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleCriticalAction(int)));
    connect(criticalBattery, SIGNAL(valueChanged(int)),
            this, SLOT(handleCriticalBattery(int)));
    connect(autoSleepBattery, SIGNAL(valueChanged(int)),
            this, SLOT(handleAutoSleepBattery(int)));
    connect(autoSleepAC, SIGNAL(valueChanged(int)),
            this, SLOT(handleAutoSleepAC(int)));
    connect(desktopSS, SIGNAL(toggled(bool)),
            this, SLOT(handleDesktopSS(bool)));
    connect(desktopPM, SIGNAL(toggled(bool)),
            this, SLOT(handleDesktopPM(bool)));
    connect(showNotifications, SIGNAL(toggled(bool)),
            this, SLOT(handleShowNotifications(bool)));
    connect(showSystemTray, SIGNAL(toggled(bool)),
            this, SLOT(handleShowSystemTray(bool)));
    connect(disableLidAction, SIGNAL(toggled(bool)),
            this, SLOT(handleDisableLidAction(bool)));
    connect(autoSleepBatteryAction, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleAutoSleepBatteryAction(int)));
    connect(autoSleepACAction, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleAutoSleepACAction(int)));
    connect(lidXrandr, SIGNAL(toggled(bool)),
            this, SLOT(handleLidXrandr(bool)));
    connect(backlightSlider, SIGNAL(valueChanged(int)),
            this, SLOT(handleBacklightSlider(int)));
    connect(backlightWatcher, SIGNAL(fileChanged(QString)),
            this, SLOT(updateBacklight(QString)));
    connect(man, SIGNAL(updatedDevices()),
            this, SLOT(checkDevices()));
}

Dialog::~Dialog()
{
    Common::savePowerSettings("dialog_geometry", saveGeometry());
}

// populate widgets with default values
void Dialog::populate()
{
    lidActionBattery->clear();
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                              tr("None"), lidNone);
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_LOCK_ICON),
                              tr("Lock Screen"), lidLock);
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                              tr("Sleep"), lidSleep);
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                              tr("Hibernate"), lidHibernate);

    lidActionAC->clear();
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                         tr("None"), lidNone);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_LOCK_ICON),
                         tr("Lock Screen"), lidLock);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                         tr("Sleep"), lidSleep);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                         tr("Hibernate"), lidHibernate);

    criticalActionBattery->clear();
    criticalActionBattery->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                                   tr("None"), criticalNone);
    criticalActionBattery->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                                   tr("Hibernate"), criticalHibernate);
    criticalActionBattery->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                                   tr("Shutdown"), criticalShutdown);

    autoSleepBatteryAction->clear();
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                                    tr("None"), suspendNone);
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                                    tr("Sleep"), suspendSleep);
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                                    tr("Hibernate"), suspendHibernate);
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                                    tr("Shutdown"), suspendShutdown);

    autoSleepACAction->clear();
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                               tr("None"), suspendNone);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                               tr("Sleep"), suspendSleep);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                               tr("Hibernate"), suspendHibernate);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                               tr("Shutdown"), suspendShutdown);
}

// load settings and set defaults
void Dialog::loadSettings()
{
    if (Common::validPowerSettings(CONF_DIALOG_GEOMETRY)) {
        restoreGeometry(Common::loadPowerSettings(CONF_DIALOG_GEOMETRY).toByteArray());
    }

    int defaultAutoSleepBattery = AUTO_SLEEP_BATTERY;
    if (Common::validPowerSettings(CONF_SUSPEND_BATTERY_TIMEOUT)) {
        defaultAutoSleepBattery = Common::loadPowerSettings(CONF_SUSPEND_BATTERY_TIMEOUT).toInt();
    }
    setDefaultAction(autoSleepBattery, defaultAutoSleepBattery);

    int defaultAutoSleepBatteryAction = DEFAULT_SUSPEND_BATTERY_ACTION;
    if (Common::validPowerSettings(CONF_SUSPEND_BATTERY_ACTION)) {
        defaultAutoSleepBatteryAction = Common::loadPowerSettings(CONF_SUSPEND_BATTERY_ACTION).toInt();
    }
    setDefaultAction(autoSleepBatteryAction, defaultAutoSleepBatteryAction);

    int defaultAutoSleepAC = 0;
    if (Common::validPowerSettings(CONF_SUSPEND_AC_TIMEOUT)) {
        defaultAutoSleepAC = Common::loadPowerSettings(CONF_SUSPEND_AC_TIMEOUT).toInt();
    }
    setDefaultAction(autoSleepAC, defaultAutoSleepAC);

    int defaultAutoSleepACAction = DEFAULT_SUSPEND_AC_ACTION;
    if (Common::validPowerSettings(CONF_SUSPEND_AC_ACTION)) {
        defaultAutoSleepACAction = Common::loadPowerSettings(CONF_SUSPEND_AC_ACTION).toInt();
    }
    setDefaultAction(autoSleepACAction, defaultAutoSleepACAction);

    int defaultCriticalBattery = CRITICAL_BATTERY;
    if (Common::validPowerSettings(CONF_CRITICAL_BATTERY_TIMEOUT)) {
        defaultCriticalBattery = Common::loadPowerSettings(CONF_CRITICAL_BATTERY_TIMEOUT).toInt();
    }
    setDefaultAction(criticalBattery, defaultCriticalBattery);

    int defaultLidActionBattery = LID_BATTERY_DEFAULT;
    if (Common::validPowerSettings(CONF_LID_BATTERY_ACTION)) {
        defaultLidActionBattery = Common::loadPowerSettings(CONF_LID_BATTERY_ACTION).toInt();
    }
    setDefaultAction(lidActionBattery, defaultLidActionBattery);

    int defaultLidActionAC = LID_AC_DEFAULT;
    if (Common::validPowerSettings(CONF_LID_AC_ACTION)) {
        defaultLidActionAC = Common::loadPowerSettings(CONF_LID_AC_ACTION).toInt();
    }
    setDefaultAction(lidActionAC, defaultLidActionAC);

    int defaultCriticalAction = CRITICAL_DEFAULT;
    if (Common::validPowerSettings(CONF_CRITICAL_BATTERY_ACTION)) {
        defaultCriticalAction = Common::loadPowerSettings(CONF_CRITICAL_BATTERY_ACTION).toInt();
    }
    setDefaultAction(criticalActionBattery, defaultCriticalAction);

    bool defaultDesktopSS = true;
    if (Common::validPowerSettings(CONF_FREEDESKTOP_SS)) {
        defaultDesktopSS = Common::loadPowerSettings(CONF_FREEDESKTOP_SS).toBool();
    }
    desktopSS->setChecked(defaultDesktopSS);

    bool defaultDesktopPM = true;
    if (Common::validPowerSettings(CONF_FREEDESKTOP_PM)) {
        defaultDesktopPM = Common::loadPowerSettings(CONF_FREEDESKTOP_PM).toBool();
    }
    desktopPM->setChecked(defaultDesktopPM);

    bool defaultLidXrandr = false;
    if (Common::validPowerSettings(CONF_LID_XRANDR)) {
        defaultLidXrandr = Common::loadPowerSettings(CONF_LID_XRANDR).toBool();
    }
    lidXrandr->setChecked(defaultLidXrandr);

    bool defaultShowNotifications = true;
    if (Common::validPowerSettings(CONF_TRAY_NOTIFY)) {
        defaultShowNotifications = Common::loadPowerSettings(CONF_TRAY_NOTIFY).toBool();
    }
    showNotifications->setChecked(defaultShowNotifications);

    bool defaultShowTray = true;
    if (Common::validPowerSettings(CONF_TRAY_SHOW)) {
        defaultShowTray = Common::loadPowerSettings(CONF_TRAY_SHOW).toBool();
    }
    showSystemTray->setChecked(defaultShowTray);

    bool defaultDisableLidAction = true;
    if (Common::validPowerSettings(CONF_LID_DISABLE_IF_EXTERNAL)) {
        defaultDisableLidAction = Common::loadPowerSettings(CONF_LID_DISABLE_IF_EXTERNAL).toBool();
    }
    disableLidAction->setChecked(defaultDisableLidAction);

    if (Login1::hasService()) {
        sleepButton->setEnabled(Login1::canSuspend());
        hibernateButton->setEnabled(Login1::canHibernate());
        poweroffButton->setEnabled(Login1::canPowerOff());
    } else {
        sleepButton->setEnabled(UPower::canSuspend());
        hibernateButton->setEnabled(UPower::canHibernate());
        poweroffButton->setEnabled(CKit::canPowerOff());
    }

    checkPerms();

    // backlight
    backlightDevice = Common::backlightDevice();
    hasBacklight = Common::canAdjustBacklight(backlightDevice);
    if (hasBacklight) {
        backlightSlider->setMinimum(1);
        backlightSlider->setMaximum(Common::backlightMax(backlightDevice));
        backlightSlider->setValue(Common::backlightValue(backlightDevice));
        backlightSlider->show();
        backlightSlider->setEnabled(true);
        backlightWatcher->addPath(QString("%1/brightness").arg(backlightDevice));
    } else {
        backlightSlider->hide();
        backlightSlider->setDisabled(true);
    }

    // check devices
    checkDevices();
}

// set default action in combobox
void Dialog::setDefaultAction(QComboBox *box, int action)
{
    for (int i=0;i<box->count();i++) {
        if (box->itemData(i).toInt() == action) {
            box->setCurrentIndex(i);
            return;
        }
    }
}

// set default value in spinbox
void Dialog::setDefaultAction(QSpinBox *box, int action)
{
    box->setValue(action);
}

// set default value in combobox
void Dialog::setDefaultAction(QComboBox *box, QString value)
{
    for (int i=0;i<box->count();i++) {
        if (box->itemText(i) == value) {
            box->setCurrentIndex(i);
            return;
        }
    }
}

// save current value and update power manager
void Dialog::handleLidActionBattery(int index)
{
    if (index == lidHibernate &&
            (!Common::kernelCanResume() ||
             !hibernateButton->isEnabled())) {
        QMessageBox::warning(this, tr("Error"), tr("Hibernate is not supported"));
        return;
    } else if (index == lidSleep &&
               !sleepButton->isEnabled()) {
        QMessageBox::warning(this, tr("Error"), tr("Suspend is not supported"));
        return;
    }
    Common::savePowerSettings(CONF_LID_BATTERY_ACTION, index);
}

void Dialog::handleLidActionAC(int index)
{
    if (index == lidHibernate &&
            (!Common::kernelCanResume() ||
             !hibernateButton->isEnabled())) {
        QMessageBox::warning(this, tr("Error"), tr("Hibernate is not supported"));
        return;
    } else if (index == lidSleep &&
               !sleepButton->isEnabled()) {
        QMessageBox::warning(this, tr("Error"), tr("Suspend is not supported"));
        return;
    }
    Common::savePowerSettings(CONF_LID_AC_ACTION, index);
}

void Dialog::handleCriticalAction(int index)
{
    if (index == criticalHibernate &&
            (!Common::kernelCanResume() ||
             !hibernateButton->isEnabled())) {
        QMessageBox::warning(this, tr("Error"), tr("Hibernate is not supported"));
        return;
    }
    Common::savePowerSettings(CONF_CRITICAL_BATTERY_ACTION, index);
}

void Dialog::handleCriticalBattery(int value)
{
    Common::savePowerSettings(CONF_CRITICAL_BATTERY_TIMEOUT, value);
}

void Dialog::handleAutoSleepBattery(int value)
{
    Common::savePowerSettings(CONF_SUSPEND_BATTERY_TIMEOUT, value);
 }

void Dialog::handleAutoSleepAC(int value)
{
    Common::savePowerSettings(CONF_SUSPEND_AC_TIMEOUT, value);
}

void Dialog::handleDesktopSS(bool triggered)
{
    Common::savePowerSettings(CONF_FREEDESKTOP_SS, triggered);
    QMessageBox::information(this, tr("Restart required"),
                             tr("You must restart the power daemon to apply this setting"));
    // TODO: add restart now?
}

void Dialog::handleDesktopPM(bool triggered)
{
    Common::savePowerSettings(CONF_FREEDESKTOP_PM, triggered);
    QMessageBox::information(this, tr("Restart required"),
                             tr("You must restart the power daemon to apply this setting"));
    // TODO: add restart now?
}

void Dialog::handleLidXrandr(bool triggered)
{
    Common::savePowerSettings(CONF_LID_XRANDR, triggered);
}

void Dialog::handleShowNotifications(bool triggered)
{
    Common::savePowerSettings(CONF_TRAY_NOTIFY, triggered);
}

void Dialog::handleShowSystemTray(bool triggered)
{
    Common::savePowerSettings(CONF_TRAY_SHOW, triggered);
}

void Dialog::handleDisableLidAction(bool triggered)
{
    if (triggered && !lidXrandr->isEnabled()) {
        lidXrandr->setEnabled(true);
    } else { lidXrandr->setDisabled(true); }
    Common::savePowerSettings(CONF_LID_DISABLE_IF_EXTERNAL, triggered);
}

void Dialog::handleAutoSleepBatteryAction(int index)
{
    if (index == suspendHibernate &&
            (!Common::kernelCanResume() ||
             !hibernateButton->isEnabled())) {
        QMessageBox::warning(this, tr("Error"), tr("Hibernate is not supported"));
        return;
    } else if (index == suspendSleep &&
               !sleepButton->isEnabled()) {
        QMessageBox::warning(this, tr("Error"), tr("Suspend is not supported"));
        return;
    }
    Common::savePowerSettings(CONF_SUSPEND_BATTERY_ACTION, index);
}

void Dialog::handleAutoSleepACAction(int index)
{
    if (index == suspendHibernate &&
            (!Common::kernelCanResume() ||
             !hibernateButton->isEnabled())) {
        QMessageBox::warning(this, tr("Error"), tr("Hibernate is not supported"));
        return;
    } else if (index == suspendSleep &&
               !sleepButton->isEnabled()) {
        QMessageBox::warning(this, tr("Error"), tr("Suspend is not supported"));
        return;
    }
    Common::savePowerSettings(CONF_SUSPEND_AC_ACTION, index);
}

void Dialog::handleLockscreenButton()
{
    QProcess proc;
    proc.start(XSCREENSAVER_LOCK);
    proc.waitForFinished();
    proc.close();
}

void Dialog::handleSleepButton()
{
    if (Login1::hasService()) {
        if (Login1::canSuspend()) { Login1::suspend(); }
    } else if (UPower::canSuspend()) { UPower::suspend(); }
    else {
        QMessageBox::information(this,
                                 tr("Power Action"),
                                 tr("System denied power request."
                                    " Maybe the required daemon is not running,"
                                    " or you may not have the required permissions."));
    }
}

void Dialog::handleHibernateButton()
{
    if (Login1::hasService()) {
        if (Login1::canHibernate()) { Login1::hibernate(); }
    } else if (UPower::canHibernate()) { UPower::hibernate(); }
    else {
        QMessageBox::information(this,
                                 tr("Power Action"),
                                 tr("System denied power request."
                                    " Maybe the required daemon is not running,"
                                    " or you may not have the required permissions."));
    }
}

void Dialog::handlePoweroffButton()
{
    if (Login1::hasService()) {
        if (Login1::canPowerOff()) { Login1::poweroff(); }
    } else if (CKit::hasService()) {
        if (CKit::canPowerOff()) { CKit::poweroff(); }
    } else {
        QMessageBox::information(this,
                                 tr("Power Action"),
                                 tr("System denied power request."
                                    " Maybe the required daemon is not running,"
                                    " or you may not have the required permissions."));
    }
}

void Dialog::checkPerms()
{
    if (!Common::kernelCanResume() || !hibernateButton->isEnabled()) {
        QMessageBox::warning(this, tr("Hibernate not supported"),
                             tr("The kernel command line does not contain resume=<swap partition/file>."
                                "Add resume=<swap partition/file> to the boot loader configuration"
                                " (GRUB/LILO etc) to enable hibernate."));
        hibernateButton->setDisabled(true);
        if (criticalActionBattery->currentIndex() == criticalHibernate) {
            criticalActionBattery->setCurrentIndex(criticalShutdown);
            handleCriticalAction(criticalShutdown);
        }
        if (lidActionAC->currentIndex() == lidHibernate) {
            lidActionAC->setCurrentIndex(lidLock);
            handleLidActionAC(lidLock);
        }
        if (lidActionBattery->currentIndex() == lidHibernate) {
            lidActionBattery->setCurrentIndex(lidLock);
            handleLidActionBattery(lidLock);
        }
        if (autoSleepACAction->currentIndex() == suspendHibernate) {
            autoSleepACAction->setCurrentIndex(suspendNone);
            handleAutoSleepACAction(suspendNone);
        }
        if (autoSleepBatteryAction->currentIndex() == suspendHibernate) {
            autoSleepBatteryAction->setCurrentIndex(suspendNone);
            handleAutoSleepBatteryAction(suspendNone);
        }
    }
    if (!sleepButton->isEnabled()) {
        if (lidActionAC->currentIndex() == lidSleep) {
            lidActionAC->setCurrentIndex(lidLock);
            handleLidActionAC(lidLock);
        }
        if (lidActionBattery->currentIndex() == lidSleep) {
            lidActionBattery->setCurrentIndex(lidLock);
            handleLidActionBattery(lidLock);
        }
        if (autoSleepACAction->currentIndex() == suspendSleep) {
            autoSleepACAction->setCurrentIndex(suspendNone);
            handleAutoSleepACAction(suspendNone);
        }
        if (autoSleepBatteryAction->currentIndex() == suspendSleep) {
            autoSleepBatteryAction->setCurrentIndex(suspendNone);
            handleAutoSleepBatteryAction(suspendNone);
        }
    }
}

void Dialog::handleBacklightSlider(int value)
{
    if (Common::backlightValue(backlightDevice) != value) {
        Common::adjustBacklight(backlightDevice, value);
    }
}

void Dialog::updateBacklight(QString file)
{
    Q_UNUSED(file);
    if (!hasBacklight) { return; }
    int value = Common::backlightValue(backlightDevice);
    if (value != backlightSlider->value()) {
        backlightSlider->setValue(value);
    }
}

void Dialog::checkDevices()
{
    QIcon icon = QIcon::fromTheme(DEFAULT_BATTERY_ICON);
    double left = man->batteryLeft();
    if (left<= 10) {
        icon = QIcon::fromTheme(man->onBattery()?DEFAULT_BATTERY_ICON_CRIT:DEFAULT_BATTERY_ICON_CRIT_AC);
    } else if (left<=25) {
        icon = QIcon::fromTheme(man->onBattery()?DEFAULT_BATTERY_ICON_LOW:DEFAULT_BATTERY_ICON_LOW_AC);
    } else if (left<=75) {
        icon = QIcon::fromTheme(man->onBattery()?DEFAULT_BATTERY_ICON_GOOD:DEFAULT_BATTERY_ICON_GOOD_AC);
    } else if (left<=90) {
        icon = QIcon::fromTheme(man->onBattery()?DEFAULT_BATTERY_ICON_FULL:DEFAULT_BATTERY_ICON_FULL_AC);
    } else {
        icon = QIcon::fromTheme(man->onBattery()?DEFAULT_BATTERY_ICON_FULL:DEFAULT_BATTERY_ICON_CHARGED);
        if (left>=100 && !man->onBattery()) {
            icon = QIcon::fromTheme(DEFAULT_AC_ICON);
        }
    }

    batteryIcon->setPixmap(icon.pixmap(QSize(48, 48)));
    batteryLabel->setText(QString("<h1 style=\"font-weight:normal;\">%1%</h1>").arg(left));
    batteryLeftLCD->display(QDateTime::fromTime_t(man->timeToEmpty()).toUTC().toString("hh:mm"));

    QMapIterator<QString, Device*> i(man->devices);
    while (i.hasNext()) {
        i.next();
        //qDebug() << i.value()->name << i.value()->model << i.value()->type  << i.value()->isPresent << i.value()->objectName() << i.value()->percentage;
        QString uid = i.value()->path;
        if (!i.value()->isPresent) {
            if (deviceExists(uid)) { deviceRemove(uid); }
            continue;
        }
        if (!deviceExists(uid)) {
            QTreeWidgetItem *item = new QTreeWidgetItem(deviceTree);
            item->setText(0, i.value()->model.isEmpty()?i.value()->name:i.value()->model);
            item->setData(0, DEVICE_UUID, uid);
            item->setFlags(Qt::ItemIsEnabled);
            QIcon itemIcon;
            switch(i.value()->type) {
            case Device::DeviceKeyboard:
                itemIcon = QIcon::fromTheme(DEFAULT_KEYBOARD_ICON);
                break;
            case Device::DeviceMouse:
                itemIcon = QIcon::fromTheme(DEFAULT_MOUSE_ICON);
                break;
            default:
                itemIcon = QIcon::fromTheme(DEFAULT_BATTERY_ICON);
            }
            item->setIcon(0, itemIcon);
            devicesProg[uid] = new QProgressBar(this);
            devicesProg[uid]->setMinimum(0);
            devicesProg[uid]->setMaximum(100);
            devicesProg[uid]->setValue(i.value()->percentage);
            deviceTree->setItemWidget(item, 1, devicesProg[uid]);
        } else {
            devicesProg[i.value()->path]->setValue(i.value()->percentage);
        }
    }
}

bool Dialog::deviceExists(QString uid)
{
    for (int i=0;i<deviceTree->topLevelItemCount();++i) {
        QTreeWidgetItem *item = deviceTree->topLevelItem(i);
        if (!item) { continue; }
        if (item->data(0, DEVICE_UUID) == uid) { return true; }
    }
    return false;
}

void Dialog::deviceRemove(QString uid)
{
    for (int i=0;i<deviceTree->topLevelItemCount();++i) {
        QTreeWidgetItem *item = deviceTree->topLevelItem(i);
        if (!item) { continue; }
        if (item->data(0, DEVICE_UUID) == uid) {
            delete deviceTree->takeTopLevelItem(i);
        }
    }
    if (devicesProg.contains(uid)) {
        devicesProg[uid]->deleteLater();
        devicesProg.remove(uid);
    }
}
