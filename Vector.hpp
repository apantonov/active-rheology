#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <cstddef>
#include <cstdio>
#include <algorithm>

template <typename T>
class Vector {

private:
    size_t length;
    T *data;
    
public:
    Vector(size_t n);
    ~Vector();
    
    Vector& operator= (const Vector<T> & other);
    T& operator [](size_t idx);
    T operator [](size_t idx) const;
    
    void swap(const size_t i, const size_t j);
    void leftShift();
    void rightShift();
    void sort();
    double mean();
    void print();
    void print(FILE *file);
    size_t size();
};

template<typename T>
Vector<T>::Vector(size_t n) {
    length = n;
    data = new T[length]();
}

template < typename T >
Vector<T>::~Vector() {
    delete[] data;
}

template < typename T >
size_t Vector<T>::size() {
    return length;
}

template < typename T >
void Vector<T>::swap(const size_t i, const size_t j) {
    T tmp;
    tmp = data[i];
    data[i] = data[j];
    data[j] = tmp;
}

template < typename T >
Vector<T>& Vector<T>::operator= (const Vector<T> &other)
{
    if (this != &other) // protect against invalid self-assignment
    {
        if (length == other.length) {
            for (size_t i = 0; i < length; ++i) {
                data[i] = other.data[i];
            }
        } else {
            T *new_data = new T[other.length];
            std::copy(other.data, other.data+other.length, new_data);
            delete[] data;
            data = new_data;
            length = other.length;
        }
    }
    return *this;
}

template < typename T >
T& Vector<T>::operator [](size_t idx) {
    return data[idx];
}

template < typename T >
T Vector<T>::operator [](size_t idx) const {
    return data[idx];
}

template < typename T >
void Vector<T>::print() {
    printf("[");
    for (size_t i = 0; i < length-1; ++i) {
        printf("%f, ", data[i]);
    }
    printf("%f]\n", data[length-1]);
}

template < typename T >
void Vector<T>::print(FILE *file) {
    for (size_t i = 0; i < length-1; ++i) {
        fprintf(file, "%f, ", data[i]);
    }
    fprintf(file, "%f", data[length-1]);
}

template < typename T >
void Vector<T>::leftShift() {
    T tmp = data[0];
    for (size_t i = 0; i < length-1; ++i) {
        data[i] = data[i+1];
    }
    data[length-1] = tmp;
}

template < typename T >
void Vector<T>::rightShift() {
    T tmp = data[length-1];
    for (size_t i = length-1; i > 0; --i) {
        data[i] = data[i-1];
    }
    data[0] = tmp;
}

template < typename T >
void Vector<T>::sort() {
    size_t i, k;
    T tmp = 0.0;
    for (i = 1; i < length; ++i) {
        for (k = i; k > 0 && data[k] < data[k-1]; k--) {
            tmp = data[k];
            data[k] = data[k-1];
            data[k-1] = tmp;
        }
    }
}

template < typename T >
double Vector<T>::mean() {
    size_t i;
    double sum = 0.0; 
    for (i = 0; i < length; ++i) {
        sum += data[i];
    }
    return sum / ((double) length);
}

#endif // __PARTICLE_CONFIGURATION_H__
