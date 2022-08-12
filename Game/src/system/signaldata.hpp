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

#include <math/misc.h>

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
    SignalData();

    void Add(Wavelength wavelength, float intensity);
    void Add(double wavelength, float intensity);
    Wavelength GetMinimumWavelength() const;
    Wavelength GetMaximumWavelength() const;

    // Kept as two separate arrays so they can be used directly by ImPlot.
    static const size_t sNumEntries = 1000;
    std::array<double, sNumEntries> Wavelengths = { 0.0 };
    std::array<double, sNumEntries> Intensities = { 0.0 };
};

inline SignalData::SignalData() 
{
    const double minimum = log10(GetMinimumWavelength().ToDouble());
    const double maximum = log10(GetMaximumWavelength().ToDouble());
    float ratio = 0.0f;

    for (size_t i = 0; i < sNumEntries; ++i)
    {
        ratio = static_cast<float>(i) / static_cast<float>(sNumEntries);
        double logValue = gLerp<double>(minimum, maximum, ratio);
        double wavelength = pow(10.0f, logValue);
        Add(wavelength, 0.0f);
    }
}

inline void SignalData::Add(Wavelength wavelength, float intensity) 
{
    Add(wavelength.ToDouble(), intensity);
}

inline void SignalData::Add(double wavelength, float intensity) 
{
    const double logValue = log10(wavelength);
    const double minimum = log10(GetMinimumWavelength().ToDouble());
    const double maximum = log10(GetMaximumWavelength().ToDouble());
    const double ratio = (logValue - minimum) / (maximum - minimum);
    const unsigned int index = static_cast<unsigned int>(ratio * (sNumEntries - 1));
    Wavelengths[index] = wavelength;
    Intensities[index] = intensity;
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