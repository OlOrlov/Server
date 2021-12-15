#ifndef HCOMMON_H
#define HCOMMON_H

const quint8 maxConnects = 75;
const quint8 maxMessageSize = 16;//TO_DELETE?
const quint16 portForAuthorization = 8000;
const quint16 portForLogRecord = 8001;
const quint16 portForSending = 8002;
const QByteArray errMsg{"ERR"};
const QString authWord = "[Auth]";
const quint8 authWordLength = 6;
const QString loginWord = "[Login]";
const quint8 loginWordLength = 7;
const quint8 maxLoginSize = 16;
const QString tokenWord = "[Token]";
const quint8 tokenWordLength = 7;
const quint8 tokenSize = 4;
const QString msgWord = "[Msg]";
const quint8 msgWordLength = 5;

#endif // HCOMMON_H
