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
    bool get(T *data);
    bool canGet();
    int put(T* e);
    int getSize() { return this->size; }
    static void test();

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
bool RingBuffer<T>::get(T* data)
{
    //qDebug("w=%d, r=%d", w, r);
    int nr = (r+1)%this->size;

    if( (nr != w) || this->overwrite ){
        //memcpy( data, &(this->data[r]), sizeof(T) );
        r = nr;
        *data = this->data[r];
        return true;
    } else {
        return false;
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

    //qDebug("w=%d, r=%d", w, r);
    if( (w != r) || this->overwrite ){
        //memcpy( &(this->data[w]), e, sizeof(T) );
        this->data[w] = *e;
        w = (w+1)%this->size;
        int remaining = (w > r)?(size-w+r):(r-w);
        return remaining;
    } else {
        return -1;
    }
}

template <class T>
void RingBuffer<T>::test()
{
    char c[] = {'a','b','c','d'};
    char t; bool b;
    RingBuffer rb = RingBuffer<char>(4);

    b = rb.get(&t);
    qDebug("Zero element %d = %p", b, t);

    qDebug("Can get %d", rb.canGet() );

    qDebug("Put first: %d left", rb.put(&c[0]) );
    b = rb.get(&t);
    qDebug("Get first: %d = %c ", b, t );

    qDebug("Fill and empty ...");
    qDebug("Put first: %d left", rb.put(&c[0]) );
    qDebug("Put second: %d left", rb.put(&c[1]) );
    qDebug("Put third: %d left", rb.put(&c[2]) );
    qDebug("Put fourth: %d left", rb.put(&c[3]) );

    while( rb.canGet() ){
        b = rb.get(&t);
        qDebug("Get: %d = %c ", b, t );
    }

    qDebug("Overfill ...");
    qDebug("Put first: %d left", rb.put(&c[0]) );
    qDebug("Put second: %d left", rb.put(&c[1]) );
    qDebug("Put third: %d left", rb.put(&c[2]) );
    qDebug("Put fourth: %d left", rb.put(&c[3]) );
    qDebug("Put first: %d left", rb.put(&c[0]) );
    qDebug("Put second: %d left", rb.put(&c[1]) );
    qDebug("Put third: %d left", rb.put(&c[2]) );
    qDebug("Put fourth: %d left", rb.put(&c[3]) );

    while( rb.canGet() ){
        b = rb.get(&t);
        qDebug("Get: %d = %c ", b, t );
    }
    return;
    /*
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
}

#endif // RINGBUFFER_H
