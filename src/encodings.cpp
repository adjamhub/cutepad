/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "encodings.h"


Encodings::Encodings()
{}


QVector<QTextCodec *> Encodings::findCodecs()
{
    QVector<QTextCodec *> availableCodecs;
    
    QMap<QString, QTextCodec *> codecMap;
    QRegularExpression iso8859RegExp("^ISO[- ]8859-([0-9]+).*$");
    QRegularExpressionMatch match;
    
    const QList<int> mibs = QTextCodec::availableMibs();
    for (int mib : mibs) {
        QTextCodec *codec = QTextCodec::codecForMib(mib);
    
        QString sortKey = codec->name().toUpper();
        char rank;
    
        if (sortKey.startsWith(QLatin1String("UTF-8"))) {
            rank = 1;
        } else if (sortKey.startsWith(QLatin1String("UTF-16"))) {
            rank = 2;
        } else if ((match = iso8859RegExp.match(sortKey)).hasMatch()) {
            if (match.capturedRef(1).size() == 1)
                rank = 3;
            else
                rank = 4;
        } else {
            rank = 5;
        }
        sortKey.prepend(QLatin1Char('0' + rank));
    
        codecMap.insert(sortKey, codec);
    }
    
    for (const auto &codec : qAsConst(codecMap))
        availableCodecs += codec;
      
     return availableCodecs;
}


QString Encodings::convert(const QString& content, QTextCodec* from, QTextCodec* to)
{
    QByteArray encodedData = from->fromUnicode(content);

    QTextCodec::ConverterState state;
    QString decodedString = to->toUnicode(encodedData.constData(), encodedData.size(), &state);

    return decodedString;
}

