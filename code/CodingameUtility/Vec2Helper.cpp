#include "CodingameUtility_PCH.h"
#include "Vec2Helper.h"

char const* ToString(SVec2i const vec2)
{
    static char buff[32];
    sprintf(buff, "{%i:%i}", vec2[0], vec2[1]);
    return buff;
}

char const* ToString(SVec2 const vec2)
{
    static char buff[32];
    sprintf(buff, "{%i:%i}", (int)vec2[0], (int)vec2[1]);
    return buff;
}

std::ostream& operator<<(std::ostream& out, SVec2 const& vec)
{
    out << vec[0] << " " << vec[1];
    return out;
}

std::istream& operator>>(std::istream& in, SVec2& vec)
{
    in >> vec[0] >> vec[1];
    return in;
}