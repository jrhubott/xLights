#include "ValueCurve.h"
#include <sstream>
#include <iostream>
#include <wx/wx.h>
#include <wx/string.h>

float ValueCurve::SafeParameter(size_t p, float v)
{
    return std::min(100.0f, std::max(0.0f, v));
}

float ValueCurve::Safe01(float v)
{
    return std::min(1.0f, std::max(0.0f, v));
}

void ValueCurve::RenderType()
{
    if (_type != "Custom")
    {
        _values.clear();
    }
    if (_type == "Flat")
    {
        _values.push_back(vcSortablePoint(0.0f, (float)_parameter1 / 100.0));
        _values.push_back(vcSortablePoint(1.0f, (float)_parameter1 / 100.0));
    }
    else if (_type == "Ramp")
    {
        _values.push_back(vcSortablePoint(0.0f, (float)_parameter1 / 100.0));
        _values.push_back(vcSortablePoint(1.0f, (float)_parameter2 / 100.0));
    }
    else if (_type == "Ramp Up/Down")
    {
        _values.push_back(vcSortablePoint(0.0f, (float)_parameter1 / 100.0));
        _values.push_back(vcSortablePoint(0.5f, (float)_parameter2 / 100.0));
        _values.push_back(vcSortablePoint(1.0f, (float)_parameter3 / 100.0));
    }
    else if (_type == "Ramp Up/Down Hold")
    {
        _values.push_back(vcSortablePoint(0.0f, (float)_parameter1 / 100.0));
        _values.push_back(vcSortablePoint(0.5f - ((0.5f * (float)_parameter3) / 100.0), (float)_parameter2 / 100.0));
        _values.push_back(vcSortablePoint(0.5f + ((0.5f * (float)_parameter3) / 100.0), (float)_parameter2 / 100.0));
        _values.push_back(vcSortablePoint(1.0f, (float)_parameter1 / 100.0));
    }
    else if (_type == "Saw Tooth")
    {
        int count = _parameter3;
        if (count < 1)
        {
            count = 1;
        }
        float per = 1.0f / count;
        _values.push_back(vcSortablePoint(0.0f, (float)_parameter1 / 100.0));
        for (int i = 0; i < count; i++)
        {
            _values.push_back(vcSortablePoint(i * per + per / 2.0f, (float)_parameter2 / 100.0));
            _values.push_back(vcSortablePoint((i + 1) * per, (float)_parameter1 / 100.0));
        }
    }
    else if (_type == "Parabolic Down")
    {
        int a = _parameter1;
        if (_parameter1 == 0)
        {
            a = 1;
        }
        // y = a(x-h)^2 + bx + k
        // h = 0.5
        // k = 0.0
        for (double i = 0.0; i <= 1.01; i += 0.05)
        {
            if (i > 1.0) i = 1.0;
            _values.push_back(vcSortablePoint(i, Safe01(a * (i - 0.5f) * (i - 0.5f) + (float)_parameter2 / 100.0)));
        }
    }
    else if (_type == "Parabolic Up")
    {
        int a = -1 * _parameter1;
        if (_parameter1 == 0)
        {
            a = -1;
        }
        // y = -1 * a(x-h)^2 + bx + k
        // h = 0.5
        // k = c
        for (double i = 0.0; i <= 1.01; i += 0.05)
        {
            if (i > 1.0) i = 1.0;
            _values.push_back(vcSortablePoint(i, Safe01(a * (i - 0.5f) * (i - 0.5f) + (float)_parameter2 / 100.0)));
        }
    }
    else if (_type == "Logarithmic Up")
    {
        // p1 rate
        float a = (float)_parameter1 / 25.0f;
        if (_parameter1 == 0)
        {
            a = 0.04f;
        }
        // y = log(ax - 1);
        for (double i = 0.0; i <= 1.01; i += 0.05)
        {
            if (i > 1.0) i = 1.0;
            _values.push_back(vcSortablePoint(i, Safe01(((float)_parameter2 - 50.0) / 50.0 + log(a + a*i)+1.0f)));
        }
    }
    else if (_type == "Logarithmic Down")
    {
        // p1 rate
        float a = (float)_parameter1 / 10.0f;
        if (_parameter1 == 0)
        {
            a = 0.1f;
        }
        // y = 1.0+ -2^(ax-1.0);
        for (double i = 0.0; i <= 1.01; i += 0.05)
        {
            if (i > 1.0) i = 1.0;
            _values.push_back(vcSortablePoint(i, Safe01(((float)_parameter2 - 50.0) / 50.0 + 1.5f + -1 * exp2(a * i - 1.0f))));
        }
    }
    else if (_type == "Exponential Up")
    {
        // p1 rate
        float a = (float)_parameter1 / 10.0f;
        if (_parameter1 == 0)
        {
            a = 0.1f;
        }
        // y = 1.0+ -2^(ax-1.0);
        for (double i = 0.0; i <= 1.01; i += 0.05)
        {
            if (i > 1.0) i = 1.0;
            _values.push_back(vcSortablePoint(i, Safe01(((float)_parameter2 - 50.0) / 50.0 + (exp(a*i) - 1.0) / (exp(a) - 1.0))));
        }
    }
    else if (_type == "Exponential Down")
    {
        // p1 rate
        float a = (float)_parameter1 / 10.0f;
        if (_parameter1 == 0)
        {
            a = 0.1f;
        }
        // y = 1.0+ -2^(ax-1.0);
        for (double i = 0.0; i <= 1.01; i += 0.05)
        {
            if (i > 1.0) i = 1.0;
            _values.push_back(vcSortablePoint(i, Safe01(((float)_parameter2 - 50.0) / 50.0 + 1.0 - (exp(a*i) - 1.0) / (exp(a) - 1.0))));
        }
    }
    else if (_type == "Sine")
    {
        // p1 - offset in cycle
        // p2 - maxy
        // p3 - cycles
        // one cycle = 2* PI
        static const double pi2 = 6.283185307;
        float maxx = pi2 * std::max((float)_parameter3 / 10.0f, 0.1f);
        for (double i = 0.0; i <= 1.01; i += 0.025)
        {
            if (i > 1.0) i = 1.0;
            float r = i * maxx + (((float)_parameter1 * pi2) / 100.0f);
            _values.push_back(vcSortablePoint(i, Safe01(((float)_parameter4 - 50.0) / 50.0 + (sin(r) * (std::max((float)_parameter2, 1.0f) / 200.0f)) + 0.5f)));
        }
    }
    else if (_type == "Abs Sine")
    {
        // p1 - offset in cycle
        // p2 - maxy
        // p3 - cycles
        // one cycle = 2* PI
        static const double pi2 = 6.283185307;
        float maxx = pi2 * std::max((float)_parameter3 / 10.0f, 0.1f);
        for (double i = 0.0; i <= 1.01; i += 0.025)
        {
            if (i > 1.0) i = 1.0;
            float r = i * maxx + (((float)_parameter1 * pi2) / 100.0f);
            _values.push_back(vcSortablePoint(i, Safe01(((float)_parameter4 - 50.0) / 50.0 + (std::abs(sin(r) * (std::max((float)_parameter2, 1.0f) / 100.0f))))));
        }
    }
    _values.sort();
}

ValueCurve::ValueCurve(const std::string& id, float min, float max, const std::string type, float parameter1, float parameter2, float parameter3, float parameter4)
{
    _type = type;
    _id = id;
    _min = min;
    _max = max;
    _parameter1 = SafeParameter(1, parameter1);
    _parameter2 = SafeParameter(2, parameter2);
    _parameter3 = SafeParameter(3, parameter3);
    _parameter4 = SafeParameter(4, parameter4);
    _active = false;
    RenderType();
}

void ValueCurve::SetDefault(float min, float max)
{
    _type = "Flat";
    _min = min;
    _max = max;
    _parameter1 = SafeParameter(1, 0);
    _parameter2 = SafeParameter(2, 0);
    _parameter3 = SafeParameter(3, 0);
    _parameter4 = SafeParameter(4, 0);
    _active = false;
    RenderType();
}

void ValueCurve::Deserialise(std::string s)
{
    if (s == "")
    {
        _active = false;
    }
    else
    {
        _active = true;
        _values.clear();
        _type = "Flat";
        _parameter1 = 0.0f;
        _parameter2 = 0.0f;
        _parameter3 = 0.0f;
        _parameter4 = 0.0f;
        wxArrayString v = wxSplit(wxString(s.c_str()), '|');
        for (auto vs = v.begin(); vs != v.end(); vs++)
        {
            wxArrayString v1 = wxSplit(*vs, '=');
            if (v1.size() == 2)
            {
                SetSerialisedValue(v1[0].ToStdString(), v1[1].ToStdString());
            }
        }
        RenderType();
    }
}
std::string ValueCurve::Serialise()
{
    std::string res = "";

    if (IsActive())
    {
        res += "Id=" + _id + "|";
        if (_type != "Flat")
        {
            res += "Type=" + _type + "|";
        }
        res += "Min=" + std::string(wxString::Format("%.2f", _min).c_str()) + "|";
        res += "Max=" + std::string(wxString::Format("%.2f", _max).c_str()) + "|";
        if (_parameter1 != 0)
        {
            res += "P1=" + std::string(wxString::Format("%d", (int)_parameter1).c_str()) + "|";
        }
        if (_parameter2 != 0)
        {
            res += "P2=" + std::string(wxString::Format("%d", (int)_parameter2).c_str()) + "|";
        }
        if (_parameter3 != 0)
        {
            res += "P3=" + std::string(wxString::Format("%d", (int)_parameter3).c_str()) + "|";
        }
        if (_parameter4 != 0)
        {
            res += "P4=" + std::string(wxString::Format("%d", (int)_parameter4).c_str()) + "|";
        }
        if (_type == "Custom")
        {
            RemoveExcessCustomPoints();
            res += "Values=";
            for (auto it = _values.begin(); it != _values.end(); ++it)
            {
                res += std::string(wxString::Format("%.2f", it->x).c_str()) + ":" + std::string(wxString::Format("%.2f", it->y).c_str());
                if (!(*it == _values.back()))
                {
                    res += ";";
                }
            }
        }
    }
    return res;
}

void ValueCurve::SetSerialisedValue(std::string k, std::string s)
{
    wxString kk = wxString(k.c_str());
    if (kk == "Id")
    {
        _id = s;
    }
    else if (kk == "Type")
    {
        _type = s;
    }
    else if (kk == "Min")
        {
            _min = wxAtof(wxString(s.c_str()));
        }
        else if (kk == "Max")
        {
            _max = wxAtof(wxString(s.c_str()));
        }
        else if (kk == "P1")
        {
            _parameter1 = (float)wxAtoi(wxString(s.c_str()));
        }
        else if (kk == "P2")
        {
            _parameter2 = (float)wxAtoi(wxString(s.c_str()));
        }
        else if (kk == "P3")
        {
            _parameter3 = (float)wxAtoi(wxString(s.c_str()));
        }
        else if (kk == "P4")
        {
            _parameter4 = (float)wxAtoi(wxString(s.c_str()));
        }
        else if (kk == "Values")
        {
            wxArrayString points = wxSplit(s, ';');

            for (auto p = points.begin(); p != points.end(); p++)
            {
                wxArrayString xy = wxSplit(*p, ':');
                _values.push_back(vcSortablePoint(wxAtof(wxString(xy.front().c_str())), wxAtof(wxString(xy.back().c_str()))));
            }
        }
    
    _values.sort();
    _active = true;
}

void ValueCurve::SetType(std::string type)
{
    _type = type;
    RenderType();
}

float ValueCurve::GetOutputValueAt(float offset)
{
    return _min + (_max - _min) * GetValueAt(offset);
}

float ValueCurve::GetValueAt(float offset)
{
    if (_values.size() < 2) return 1.0f;
    if (!_active) return 1.0f;

    vcSortablePoint last = _values.front();
    auto it = _values.begin();
    it++;
    while (it != _values.end() && *it < offset)
    {
        last = *it;
        ++it;
    }

    if (it == _values.end())
    {
        return _values.back().y;
    }

    // straight line interpolation between last and *it
    if (it->x == last.x)
    {
        // this should not be possible
        return it->y;
    }
    else
    {
        return last.y + (it->y - last.y) * (offset - last.x) / (it->x - last.x);
    }
}

bool ValueCurve::IsSetPoint(float offset)
{
    auto it = _values.begin();
    while (it != _values.end() && *it <= offset)
    {
        if (*it == offset)
        {
            return true;
        }
        it++;
    }

    return false;
}

void ValueCurve::DeletePoint(float offset)
{
    if (GetPointCount() > 2)
    {
        auto it = _values.begin();
        while (it != _values.end() && *it <= offset)
        {
            if (*it == offset)
            {
                _values.remove(*it);
                break;
            }
            it++;
        }
    }
}

void ValueCurve::RemoveExcessCustomPoints()
{
    // go through list and remove middle points where 3 in a row have the same value
    auto it1 = _values.begin();
    auto it2 = it1;
    it2++;
    auto it3 = it2;
    it3++;

    while (it1 != _values.end() && it2 != _values.end() && it3 != _values.end())
    {
        if (it1->y == it2->y && it2->y == it3->y)
        {
            _values.remove(*it2);
            it2 = it1;
            it2++;
            it3 = it2;
            it3++;
        }
        else
        {
            it1++;
            it2++;
            it3++;
        }
    }
}

void ValueCurve::SetValueAt(float offset, float value)
{
    auto it = _values.begin();
    while (it != _values.end() && *it <= offset)
    {
        if (*it == offset)
        {
            _values.remove(*it);
            break;
        }
        it++;
    }
    _values.push_back(vcSortablePoint(offset, value));
    _values.sort();
}