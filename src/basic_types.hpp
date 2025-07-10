#ifndef BASIC_TYPES_HPP
#define BASIC_TYPES_HPP 

class Vector4{
public:
    float x, y, z, w;
    Vector4(float x, float y, float z, float w)
        :x(x), y(y), z(z), w(w){}
    Vector4(): x(0), y(0), z(0), w(0){}
};

class Vector3{
public:
    float x, y, z;
    Vector3(float x, float y, float z)
        :x(x), y(y), z(z){}
    Vector3(): x(0), y(0), z(0){}
};

class Vector2{
public:
    float x, y;
    Vector2(float x, float y)
        :x(x), y(y){}
    Vector2(): x(0), y(0){}
};


#endif // BASIC_TYPES_HPP