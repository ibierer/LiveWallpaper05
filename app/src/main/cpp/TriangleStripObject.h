//
// Created by Immanuel Bierer on 12/25/2023.
//

#ifndef LIVEWALLPAPER05_TRIANGLESTRIPOBJECT_H
#define LIVEWALLPAPER05_TRIANGLESTRIPOBJECT_H


class TriangleStripObject {
public:

    enum AttributeType {
        POSITION_XYZ,
        POSITION_XYZ_NORMAL_XYZ,
        POSITION_XYZ_COLOR_RGB
    };

    TriangleStripObject();

    TriangleStripObject(const TriangleStripObject& other);

    TriangleStripObject& operator=(TriangleStripObject other);

    ~TriangleStripObject();

    int getNumVertices() const;

    template<class T>
    T* getVertices() const;

    AttributeType getAttributeType() const;

protected:

    AttributeType attributeType;

    void* vertices;

    int numVertices;

private:

};


#endif //LIVEWALLPAPER05_TRIANGLESTRIPOBJECT_H
