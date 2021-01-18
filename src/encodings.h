/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#ifndef ENCODINGS_H
#define ENCODINGS_H


#include <QTextCodec>
#include <QRegularExpression>


class Encodings
{
public:
    Encodings();
    
    static QVector<QTextCodec *> findCodecs();
    static QString convert(const QString& content, QTextCodec* from, QTextCodec* to);
};

#endif // ENCODINGS_H
