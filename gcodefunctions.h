#ifndef GCODEFUNCTIONS_H
#define GCODEFUNCTIONS_H

#include <QString>
#include "optional.hpp"
#include <chrono>

using namespace stx;
using namespace std;
using namespace std::chrono;
//using namespace std::chrono_literals;

class GCodeFunctions
{
protected:
    using OptReal = optional<qreal>;
    using OptInt = optional<int>;

    template <typename T>
    QString cat(char letter, optional<T> value) const;

    template <typename T>
    QString cat(char letter, T value) const;

public:
    QString G1(OptReal x = nullopt, OptReal y = nullopt, OptReal z = nullopt, OptReal f = nullopt) const;
    QString G4(milliseconds p) const;
    QString G28(bool x = false, bool y = false, bool z = false) const;
    QString G29(int s);
    QString G29(int i, int j, qreal z);
    QString G90() const;
    QString G91() const;

    QString M20() const;
};

#endif // GCODEFUNCTIONS_H
