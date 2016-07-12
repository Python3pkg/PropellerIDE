#include "language.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QVariantList>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QRegExp>
#include <QColor>
#include <QIcon>

QHash<QString, LanguageData> Language::_data = QHash<QString, LanguageData>();
QHash<QString, QString> Language::_lookup = QHash<QString, QString>();

Language::Language() 
{
    _language = "";
    load();
}

Language::~Language()
{
}

QRegularExpression Language::buildTokenizer(QStringList tokens)
{
    QString tokenstring = tokens.join("|");
    tokenstring = "("+tokenstring+")";

    if (language().case_sensitive)
        return QRegularExpression(tokenstring);
    else
        return QRegularExpression(tokenstring, 
                QRegularExpression::CaseInsensitiveOption);
}

QStringList Language::matchWholeWord(QStringList list)
{
    QStringList newlist;
    foreach(QString s, list)
    {
        s.prepend("\\b");
        s.append("\\b");
        newlist << s;
    }
    return newlist;
}

QStringList Language::buildWordList(QJsonArray keyarray)
{
    QStringList slist;
    QVariantList vlist = keyarray.toVariantList();

    foreach(QVariant v, vlist)
    {
        slist.append(v.toString());
    }

    return slist;
}

QStringList Language::mergeList(QStringList list)
{
    list.removeDuplicates();
    QString specialchars = "* + ? [ ] ( ) | .";
    QString mergedlist = list.join(" ");

    foreach (QString s, specialchars.split(" "))
    {
        mergedlist = mergedlist.replace(s, "\\"+s);
    }

    return mergedlist.split(QRegExp("\\s+"));
}

ProjectParser * Language::buildParser(QJsonArray projectparser)
{
    ProjectParser * parser = language().parser;
    parser->setCaseInsensitive(true);

    foreach (QJsonValue r, projectparser)
    {
        QList<ProjectParser::Pattern> patterns;

        foreach (QJsonValue pattern, r.toObject()["pattern"].toArray())
        {
            ProjectParser::Pattern p;
            p.regex = pattern.toObject()["regex"].toString();
            foreach (QVariant v, pattern.toObject()["capture"].toArray().toVariantList())
            {
                QMetaType::Type t = (QMetaType::Type) v.type();
                if (t == QMetaType::Double)
                {
                    p.capture << v.toInt();
                }
                else if (t == QMetaType::QString)
                {
                    p.capture << v.toString();
                }
            }

            patterns.append(p);
        }

        parser->addRule(r.toObject()["name"].toString(), patterns,
            QIcon(r.toObject()["icon"].toString()),
            QColor(r.toObject()["color"].toString()));
    }

    return parser;
}

void Language::load()
{
    load("","");
}

void Language::load(QString ext)
{
    ext = ext.toLower();
    qDebug()
        << ext 
        << (_lookup.contains(ext))
        << extensions();
    if (_lookup.contains(ext))
    {
        _language = _lookup[ext];
    }
    else
    {
        load();
    }
}

void Language::load(QString name, QString filename)
{ 
    QFile file(filename);
    name = name.toLower();
    _language = name;

    if (filename.isEmpty())
    {
        _data[_language] = LanguageData();
        return;
    }
    else if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Unsupported language:" << _language;
        _data[_language] = LanguageData();
        return;
    }

    QString text = file.readAll();
    file.close();

    LanguageData data;

    QJsonDocument d = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject lang = d.object();
    QJsonObject syntax = lang["syntax"].toObject();

    data.numbers   = buildWordList(syntax["number"].toArray());
    data.functions = buildWordList(syntax["function"].toArray());
    data.comments  = mergeList(buildWordList(syntax["comment"].toArray()));
    data.strings   = mergeList(buildWordList(syntax["string"].toArray()));

    data.enable_blocks   = syntax["enable_blocks"].toArray().first().toBool();

    if (data.enable_blocks)
    {
        data.blocks = mergeList(buildWordList(syntax["enable_blocks"].toArray().last().toArray()));
    }

    data.case_sensitive  = syntax["case_sensitive"].toBool();

    foreach (QVariant ext, lang["extension"].toArray().toVariantList())
    {
        _lookup[ext.toString().toLower()] = name;
    }

    foreach(QJsonValue m, syntax["mode"].toObject())
    {
        QStringList slist = buildWordList(m.toObject()["keywords"].toArray());
        slist = mergeList(slist);
        slist = matchWholeWord(slist);

        data.keywords.append(slist);

        slist = buildWordList(m.toObject()["operators"].toArray());
        slist = mergeList(slist);

        data.operators.append(slist);
    }

    data.parser = buildParser(lang["project"].toArray());

    _data[_language] = data;
}

QStringList Language::languages()
{
    QStringList langs = _data.keys();
    langs.removeAll("");
    return langs;
}

QStringList Language::extensions()
{
    QStringList exts = _lookup.keys();
    exts.removeAll("");
    return exts;
}


LanguageData Language::language()
{
    if (_data.contains(_language))
        return _data[_language];
    else
        return _data[""];
}

QStringList Language::listKeywords()
{
    return language().keywords;
}

QStringList Language::listOperators()
{
    return language().operators;
}

QStringList Language::listNumbers()
{
    return language().numbers;
}

QStringList Language::listStrings()
{
    return language().strings;
}

QStringList Language::listComments()
{
    return language().comments;
}

QStringList Language::listFunctions()
{
    return language().functions;
}

QStringList Language::listBlocks()
{
    return language().blocks;
}

bool Language::isCaseSensitive()
{
    return language().case_sensitive;
}

ProjectParser * Language::parser()
{
    return language().parser;
}
