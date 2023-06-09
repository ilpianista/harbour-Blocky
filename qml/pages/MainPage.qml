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

Page {

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: "Blocky"
            }

            TextArea {
                id: config
                focus: true
                text: manager.readConfig()
            }

            BusyIndicator {
                id: busy
                visible: false
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                id: errorMsg
                visible: false
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Button {
                id: start
                text: qsTr("Save and restart")
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    enabled = config.enabled = errorMsg.visible = false;
                    busy.visible = busy.running = true;
                    manager.saveConfig(config.text);
                    appWindow.restartBlocky();
                    enabled = config.enabled = true;
                    busy.visible = busy.running = false;
                }
            }
        }
    }
}
