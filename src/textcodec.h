
#include <QTextCodec>
#include <QByteArray>

#include <QDebug>

namespace TextCodec
{

QTextCodec* codecForByteArray(const QByteArray & bytes)
{
    // use first 16 bytes max to allow BOM detection of codec
    QByteArray bom(bytes.data(), qMin(16, bytes.size()));
    QTextCodec *codecForBOM = QTextCodec::codecForUtfText(bom, nullptr);
    if (codecForBOM) {
        qDebug() << "Codec for BOM:" << codecForBOM->name();
        return codecForBOM;
    }

    QTextCodec *codecForHTML = QTextCodec::codecForHtml(bytes, nullptr);
    if (codecForHTML) {
        qDebug() << "Codec for HTML: " << codecForHTML->name();
        return codecForHTML;
    }
    
    qDebug() << "Codec for locale";
    return QTextCodec::codecForLocale();
}


QString encode(const QString& content, QTextCodec* fromCodec, QTextCodec* toCodec)
{
    if (fromCodec == toCodec) {
        return content;
    }
    
    QByteArray encodedData = fromCodec->fromUnicode(content);
    QTextCodec::ConverterState state;
    QString decodedString = toCodec->toUnicode(encodedData.constData(), encodedData.size(), &state);
    return decodedString;    
}

};
