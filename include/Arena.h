#pragma once
#include <cstddef>
#include <cstdlib>
using namespace std;
class Arena_allocator
{
public:
    Arena_allocator(size_t bytes)
        :m_size(bytes)
    {
        m_buffer=static_cast<byte*>(malloc(m_size));
        m_offset=m_buffer;
    }
    template<typename T>
    T* alloc()
    {
        void* offset=m_offset;
        m_offset+=sizeof(T);
        return static_cast<T*>(offset);
    }
    Arena_allocator(Arena_allocator& other)=delete;
    Arena_allocator operator=(Arena_allocator& other)=delete;
    ~Arena_allocator()
    {
        free(m_buffer);
    }
private:
    size_t m_size;
    byte* m_offset;
    byte* m_buffer;
};