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
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>

static const QString CONFIG_PATH = QStringLiteral("/etc/blocky.yaml");

BlockyManager::BlockyManager(QObject *parent)
    : QObject(parent)
{
    m_settingsPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
                     + QDir::separator() + QCoreApplication::applicationName() + ".conf";
}

BlockyManager::~BlockyManager() {}

QString BlockyManager::readConfig()
{
    QFile f(CONFIG_PATH);

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
    QFile f(CONFIG_PATH);
    f.open(QIODevice::WriteOnly);
    f.write(content.toUtf8());
    f.close();
}

QStringList BlockyManager::upstreams()
{
    return parseList(QStringLiteral("upstreams"), QStringLiteral("groups.default"));
}

void BlockyManager::setUpstreams(const QStringList &servers)
{
    QStringList d = denylist();
    saveConfig(generateConfig(servers, d));
}

QStringList BlockyManager::denylist()
{
    return parseList(QStringLiteral("blocking"), QStringLiteral("denylists.ads"));
}

void BlockyManager::setDenylist(const QStringList &urls)
{
    QStringList u = upstreams();
    saveConfig(generateConfig(u, urls));
}

QString BlockyManager::fullConfig()
{
    return readConfig();
}

void BlockyManager::saveFromEntries(const QStringList &upstreamServers,
                                    const QStringList &denylistUrls)
{
    saveConfig(generateConfig(upstreamServers, denylistUrls));
}

bool BlockyManager::apiEnabled()
{
    QSettings settings(m_settingsPath, QSettings::IniFormat);
    return settings.value(QStringLiteral("apiEnabled"), false).toBool();
}

void BlockyManager::setApiEnabled(bool enabled)
{
    QSettings settings(m_settingsPath, QSettings::IniFormat);
    settings.setValue(QStringLiteral("apiEnabled"), enabled);
    settings.sync();
    Q_EMIT apiEnabledChanged();
}

int BlockyManager::disableDuration()
{
    QSettings settings(m_settingsPath, QSettings::IniFormat);
    return settings.value(QStringLiteral("disableDuration"), 300).toInt();
}

void BlockyManager::setDisableDuration(int seconds)
{
    QSettings settings(m_settingsPath, QSettings::IniFormat);
    settings.setValue(QStringLiteral("disableDuration"), seconds);
    settings.sync();
}

void BlockyManager::resetConfig()
{
    const QString reference = QStringLiteral("/usr/share/%1/blocky.yaml")
                                  .arg(QCoreApplication::applicationName());
    QFile ref(reference);
    if (!ref.exists()) {
        qWarning() << "reference config not found at" << reference;
        return;
    }
    QFile::remove(CONFIG_PATH);
    QFile::copy(reference, CONFIG_PATH);
}

QStringList BlockyManager::parseList(const QString &section, const QString &listName) const
{
    QStringList result;
    QFile f(CONFIG_PATH);

    if (!f.open(QIODevice::ReadOnly)) {
        return result;
    }

    QStringList lines;
    QTextStream in(&f);
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }
    f.close();

    QStringList pathParts = listName.split(QLatin1Char('.'));
    QStringList fullPath;
    fullPath.append(section);
    fullPath.append(pathParts);

    int depth = 0;
    bool inTarget = false;
    int targetIndent = -1;
    int prevIndent = -1;

    for (int i = 0; i < lines.size(); ++i) {
        const QString &line = lines.at(i);
        if (line.trimmed().isEmpty() || line.trimmed().startsWith(QLatin1Char('#'))) {
            continue;
        }

        int indent = 0;
        for (int j = 0; j < line.size(); ++j) {
            if (line.at(j) == QLatin1Char(' ')) {
                ++indent;
            } else {
                break;
            }
        }

        QString trimmed = line.trimmed();

        if (inTarget) {
            if (indent > targetIndent && trimmed.startsWith(QLatin1String("- "))) {
                result.append(trimmed.mid(2).trimmed());
            } else if (indent <= targetIndent) {
                break;
            }
            continue;
        }

        if (depth < fullPath.size() && indent > prevIndent) {
            QString key = trimmed;
            if (key.endsWith(QLatin1Char(':'))) {
                key.chop(1);
            }
            if (key == fullPath.at(depth)) {
                ++depth;
                prevIndent = indent;
                if (depth == fullPath.size()) {
                    inTarget = true;
                    targetIndent = indent;
                }
            }
        }
    }

    return result;
}

QString BlockyManager::generateConfig(const QStringList &upstreamServers,
                                      const QStringList &denylistUrls) const
{
    QString config;

    config += QLatin1String("upstreams:\n");
    config += QLatin1String("  groups:\n");
    config += QLatin1String("    default:\n");
    for (const QString &s : upstreamServers) {
        config += QLatin1String("      - ") + s + QLatin1Char('\n');
    }

    config += QLatin1String("blocking:\n");
    config += QLatin1String("  denylists:\n");
    config += QLatin1String("    ads:\n");
    for (const QString &u : denylistUrls) {
        config += QLatin1String("      - ") + u + QLatin1Char('\n');
    }
    config += QLatin1String("  clientGroupsBlock:\n");
    config += QLatin1String("    default:\n");
    config += QLatin1String("      - ads\n");
    config += QLatin1String("  loading:\n");
    config += QLatin1String("    strategy: fast\n");

    config += QLatin1String("ports:\n");
    config += QLatin1String("  dns: 127.0.0.1:53\n");

    QSettings settings(m_settingsPath, QSettings::IniFormat);
    if (settings.value(QStringLiteral("apiEnabled"), true).toBool()) {
        config += QLatin1String("  http: 127.0.0.1:4000\n");
    }

    return config;
}
