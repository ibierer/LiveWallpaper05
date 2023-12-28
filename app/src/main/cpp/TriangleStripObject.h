//
// Created by Immanuel Bierer on 12/25/2023.
//

#ifndef LIVEWALLPAPER05_TRIANGLESTRIPOBJECT_H
#define LIVEWALLPAPER05_TRIANGLESTRIPOBJECT_H


class TriangleStripObject {
public:

    TriangleStripObject();

    TriangleStripObject(const TriangleStripObject& other);

    TriangleStripObject& operator=(TriangleStripObject other);

    ~TriangleStripObject();

    int getNumVertices();

    template<class T>
    T* getVertices();

protected:

    void* vertices;

    int numVertices;

private:

};


#endif //LIVEWALLPAPER05_TRIANGLESTRIPOBJECT_H
