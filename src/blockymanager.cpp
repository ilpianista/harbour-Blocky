/*
  The MIT License (MIT)

  Copyright (c) 2023 Andrea Scarpino <andrea@scarpino.dev>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "blockymanager.h"

#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>


BlockyManager::BlockyManager(QObject *parent) :
    QObject(parent)
{
}

BlockyManager::~BlockyManager()
{
}

QString BlockyManager::readConfig()
{
    QFile f("/etc/blocky.yaml");

    const QDir configDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    QFile old(configDir.absolutePath() + "/blocky.yaml");
    if (old.exists()) {
        qDebug() << "old config file exist, migrate to /etc";

        if (f.exists()) {
            f.remove();
        }
        QFile::copy(old.fileName(), f.fileName());
        Q_EMIT migratedConfig();

        old.remove();
    }

    f.open(QIODevice::ReadOnly);

    QTextStream in(&f);
    QString content = in.readAll();

    f.close();

    return content;
}

void BlockyManager::saveConfig(const QString content)
{
    QFile f("/etc/blocky.yaml");
    f.open(QIODevice::WriteOnly);
    f.write(content.toUtf8());
    f.close();
}
