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

#ifndef BLOCKYMANAGER_H
#define BLOCKYMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>

class BlockyManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool apiEnabled READ apiEnabled WRITE setApiEnabled NOTIFY apiEnabledChanged)

public:
    explicit BlockyManager(QObject *parent = 0);
    virtual ~BlockyManager();

    Q_INVOKABLE QString readConfig();

    Q_INVOKABLE void saveConfig(const QString content);

    Q_INVOKABLE QStringList upstreams();
    Q_INVOKABLE void setUpstreams(const QStringList &servers);

    Q_INVOKABLE QStringList denylist();
    Q_INVOKABLE void setDenylist(const QStringList &urls);

    Q_INVOKABLE QVariantList mappings();
    Q_INVOKABLE void setMappings(const QVariantList &mappings);

    Q_INVOKABLE QString fullConfig();

    Q_INVOKABLE void saveFromEntries(const QStringList &upstreamServers,
                                     const QStringList &denylistUrls,
                                     const QVariantList &mappings);

    Q_INVOKABLE bool apiEnabled();
    Q_INVOKABLE void setApiEnabled(bool enabled);

    Q_INVOKABLE int disableDuration();
    Q_INVOKABLE void setDisableDuration(int seconds);

    Q_INVOKABLE void resetConfig();

Q_SIGNALS:
    void migratedConfig();
    void apiEnabledChanged();

private:
    QStringList parseList(const QString &section, const QString &listName) const;
    QString generateConfig(const QStringList &upstreamServers,
                           const QStringList &denylistUrls,
                           const QVariantList &mappings) const;
    QString m_settingsPath;
};

#endif // BLOCKYMANAGER_H
