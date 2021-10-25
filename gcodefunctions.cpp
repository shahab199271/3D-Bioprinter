#include "gcodefunctions.h"

template<typename Type>
QString GCodeFunctions::cat(char letter, optional<Type> value) const
{
    if(value.has_value())
        return QString("%1%2").arg(letter).arg(value.value());
    else
        return "";
}

template<typename Type>
QString GCodeFunctions::cat(char letter, Type value) const
{
    return QString("%1%2").arg(letter).arg(value);
}

QString GCodeFunctions::G1(GCodeFunctions::OptReal x, GCodeFunctions::OptReal y, GCodeFunctions::OptReal z, GCodeFunctions::OptReal f) const
{
    return QString("G1 %1 %2 %3 %4")
            .arg(cat('X', x))
            .arg(cat('Y', y))
            .arg(cat('Z', z))
            .arg(cat('F', f));
}

QString GCodeFunctions::G4(std::chrono::milliseconds p) const
{
    return QString("G4 %1")
            .arg(cat('P', p.count()));
}

QString GCodeFunctions::G28(bool x, bool y, bool z) const
{
    return QString("G28 %1 %2 %3")
            .arg(x ? "X" : "")
            .arg(y ? "Y" : "")
            .arg(z ? "Z" : "");
}

QString GCodeFunctions::G29(int s)
{
    return QString("G29 S%1")
            .arg(s);
}

QString GCodeFunctions::G29(int i, int j, qreal z)
{
    return QString("G29 S3 I%1 J%2 Z%3")
            .arg(i)
            .arg(j)
            .arg(z);
}

QString GCodeFunctions::G90() const
{
    return QStringLiteral("G90");
}

QString GCodeFunctions::G91() const
{
    return QStringLiteral("G91");
}

QString GCodeFunctions::M20() const
{
    return QStringLiteral("M20");
}
