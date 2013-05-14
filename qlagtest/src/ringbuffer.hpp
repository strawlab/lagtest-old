#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <exception>
#include <assert.h>

using namespace std;

template <class T>

class RingBuffer
{

public:
    RingBuffer(int size, bool allowOverwrite = false);
    T* get();
    bool canGet();
    int put(T* e);


private:
    T *data;
    int size;
    int w;
    int r;
    bool overwrite;
};

template <class T>
RingBuffer<T>::RingBuffer(int size, bool allowOverwrite)
{
    assert(size > 0);

    qDebug("Alloctating [%d] elements of size %d", size, sizeof(T) );

    this->size = size + (allowOverwrite?0:1); //If we dont overwrite, there must be a extra element in the ringbuffer

    this->data = (T*)malloc(sizeof(T) * this->size );
    if( this->data == NULL)
        throw std::exception();

    this->w = 1;
    this->r = 0;
    this->overwrite = allowOverwrite;
}

template <class T>
T* RingBuffer<T>::get()
{
    int nr = (r+1)%this->size;

    if( (nr != w) || this->overwrite ){
        r = nr;
        return &(this->data[r]);
    } else {
        return NULL;
    }
}

template <class T>
bool RingBuffer<T>::canGet()
{
    int nr = (r+1)%this->size;

    if(nr != w)
        return true;
    else
        return false;
}

template <class T>
int RingBuffer<T>::put(T *e){

//    qDebug("w=%d, r=%d", w, r);
    if( (w != r) || this->overwrite ){
        memcpy( &(this->data[w]), e, sizeof(T) );
        w = (w+1)%this->size;
        int remaining = (w > r)?(size-w+r):(r-w);
        return remaining;
    } else {
        return -1;
    }
}


/*
    vp = rb.get();
    qDebug("Zero element %p", vp);

    qDebug("Put first: %d left", rb.put(&c[0]) );
//    vp = rb.get();
//    r = *(char*)vp;
    qDebug("Get first: %c ", *(rb.get()) );

    qDebug("Put first: %d left", rb.put(&c[0]) );
    qDebug("Put second: %d left", rb.put(&c[1]) );
    qDebug("Put third: %d left", rb.put(&c[2]) );
    qDebug("Put fourth: %d left", rb.put(&c[3]) );

    qDebug("Get: %c ", *(rb.get()) );
    qDebug("Get: %c ", *(rb.get()) );
    qDebug("Get: %c ", *(rb.get()) );
    qDebug("Get: %c ", *(rb.get()) );

    qDebug("Put first: %d left", rb.put(&c[0]) );
    qDebug("Put second: %d left", rb.put(&c[1]) );
    qDebug("Put third: %d left", rb.put(&c[2]) );
    qDebug("Put fourth: %d left", rb.put(&c[3]) );

    qDebug("Get: %c ", *(rb.get()) );
    qDebug("Get: %c ", *(rb.get()) );
    qDebug("Get: %c ", *(rb.get()) );
    qDebug("Get: %c ", *(rb.get()) );

    qDebug("Put first: %d left", rb.put(&c[0]) );
    qDebug("Put second: %d left", rb.put(&c[1]) );
    qDebug("Put third: %d left", rb.put(&c[2]) );
    qDebug("Put fourth: %d left", rb.put(&c[3]) );
    qDebug("Put first: %d left", rb.put(&c[0]) );
    qDebug("Put second: %d left", rb.put(&c[1]) );
    qDebug("Put third: %d left", rb.put(&c[2]) );
    qDebug("Put fourth: %d left", rb.put(&c[3]) );

    qDebug("Get: %c ", *(rb.get()) );
    qDebug("Get: %c ", *(rb.get()) );
    qDebug("Get: %c ", *(rb.get()) );
    qDebug("Get: %c ", *(rb.get()) );

    qDebug("Put first: %d left", rb.put(&c[0]) );
    qDebug("Put second: %d left", rb.put(&c[1]) );
    qDebug("Put third: %d left", rb.put(&c[2]) );
    qDebug("Put fourth: %d left", rb.put(&c[3]) );

    qDebug("Get: %c ", *(rb.get()) );
    qDebug("Get: %c ", *(rb.get()) );
    qDebug("Get: %c ", *(rb.get()) );
    qDebug("Get: %c ", *(rb.get()) );
    */

#endif // RINGBUFFER_H
