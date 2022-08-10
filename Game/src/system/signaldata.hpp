// Copyright 2022 Pedro Nunes
//
// This file is part of Hyperscape.
//
// Hyperscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hyperscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hyperscape. If not, see <http://www.gnu.org/licenses/>.

#pragma once

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/vec2.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <array>

namespace Hyperscape
{

struct Wavelength
{
private:
    explicit Wavelength(long double value) : m_Value(value) {}

    friend Wavelength operator"" _pm(long double value);
    friend Wavelength operator"" _nm(long double value);
    friend Wavelength operator"" _um(long double value);
    friend Wavelength operator"" _mm(long double value);
    friend Wavelength operator"" _cm(long double value);
    friend Wavelength operator"" _dm(long double value);
    friend Wavelength operator"" _m(long double value);
    friend Wavelength operator"" _km(long double value);
    friend Wavelength operator"" _Mm(long double value);

    long double m_Value { 0 };

public:
    double ToDouble() const { return static_cast<double>(m_Value); }
};

inline Wavelength operator"" _pm(long double value)
{
    return Wavelength(value * 0.000000000001);
}

inline Wavelength operator"" _nm(long double value)
{
    return Wavelength(value * 0.000000001);
}

inline Wavelength operator"" _um(long double value)
{
    return Wavelength(value * 0.000001);
}

inline Wavelength operator"" _mm(long double value)
{
    return Wavelength(value * 0.001);
}

inline Wavelength operator"" _cm(long double value)
{
    return Wavelength(value * 0.01);
}

inline Wavelength operator"" _dm(long double value)
{
    return Wavelength(value * 0.1);
}

inline Wavelength operator"" _m(long double value)
{
    return Wavelength(value);
}

inline Wavelength operator"" _km(long double value)
{
    return Wavelength(value * 1000.0);
}

inline Wavelength operator"" _Mm(long double value)
{
    return Wavelength(value * 1000000.0);
}

class SignalData
{
public:
    SignalData() {}

    void Add(Wavelength wavelength, float intensity);
    Wavelength GetMinimumWavelength() const;
    Wavelength GetMaximumWavelength() const;

    std::array<double, 1001> Values = {};
};

inline void SignalData::Add(Wavelength wavelength, float intensity) 
{
    const double value = wavelength.ToDouble();
    const double logValue = log10(value);
    const double minimum = log10(GetMinimumWavelength().ToDouble());
    const double maximum = log10(GetMaximumWavelength().ToDouble());
    //const double ratio = ImLerp(minimum, maximum, logValue);
    Values[0] = wavelength.ToDouble();
}

inline Wavelength SignalData::GetMinimumWavelength() const 
{
    return 1.0_pm;
}

inline Wavelength SignalData::GetMaximumWavelength() const 
{
    return 100.0_Mm;
}

} // namespace Hyperscape