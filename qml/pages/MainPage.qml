/*
    Copyright (C) 2023 Andrea Scarpino <andrea@scarpino.dev>
    All rights reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.DBus 2.0

Page {

    allowedOrientations: Orientation.All

    DBusInterface {
        id: systemd

        bus: DBus.SystemBus
        service: 'org.freedesktop.systemd1'
        path: '/org/freedesktop/systemd1'
        iface: 'org.freedesktop.systemd1.Manager'
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Blocky")
            }

            TextArea {
                id: config
                focus: true
                text: manager.readConfig()
            }

            Button {
                id: start
                text: qsTr("Save and restart")
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    manager.saveConfig(config.text);
                    systemd.typedCall('RestartUnit',
                        [
                            { 'type': 's', 'value': 'blocky.service' },
                            { 'type': 's', 'value': 'fail' }
                        ],
                        function(result) {
                            console.log("Success! Blocky started.");
                        },
                        function(error, message) {
                            console.log("failed (" + error + ") with:", message);
                        }
                    );
                }
            }
        }
    }
}
