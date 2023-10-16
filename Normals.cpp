#include "Normals.h"

/// <summary>
/// <para>Used to calculate the smooth shading normals by:</para>
/// <para>  -   computing the cross product for each triangle in the vertex</para>
/// <para>  -   add that to the normal of each vertex of that triangle</para>
/// <para>  -   normalize the normal of each vertex</para>
/// <para>This whole function is made possible thanks to Julien Guertault on stack exchange: https://computergraphics.stackexchange.com/a/4032 </para>
/// </summary>
/// <param name="Vertices">:    an std::vector* of SimpleVertex s. </param>
/// <param name="Indices">: an array of indices used to get the vertices of each triangle</param>
/// <param name="IndicesCount">:    the number of indices (i.e. the length of indices)</param>
void CalculateSmoothNormals(std::vector<SimpleVertex>* Vertices, std::vector<unsigned short>* Indices)
{
    //  The function is based off of the following psuedocode
    //  for each traingle abc
    //      perpendicular - crossProduct(triangle.b - triangle.a, triangle.c - triangle.a)
    //      a.normal += perpendicular
    //      b.normal += perpendicular
    //      c.normal += perpendicular
    //  for each vertex
    //      vertex.normal = normalize(vertex.normal)

    //For each triangle...
    for (int i = 0; i < Indices->size(); i += 3)
    {
        //Load the positions into temporary vectors
        SimpleVertex_Vector a = { XMLoadFloat3(&Vertices->at(Indices->at(i)).Pos), XMLoadFloat3(&Vertices->at(Indices->at(i)).Normal) };
        SimpleVertex_Vector b = { XMLoadFloat3(&Vertices->at(Indices->at(i + 1)).Pos), XMLoadFloat3(&Vertices->at(Indices->at(i + 1)).Normal) };
        SimpleVertex_Vector c = { XMLoadFloat3(&Vertices->at(Indices->at(i + 2)).Pos), XMLoadFloat3(&Vertices->at(Indices->at(i + 2)).Normal) };

        //Find the perpendicular vector to the triangle
        XMVECTOR P = XMVector3Cross(b.Pos - a.Pos, c.Pos - a.Pos);

        //Add the result to the already exisiting normal and then store that result into the original vertex array's normal
        XMStoreFloat3(&Vertices->at(Indices->at(i)).Normal, P + a.Normal);
        XMStoreFloat3(&Vertices->at(Indices->at(i + 1)).Normal, P + b.Normal);
        XMStoreFloat3(&Vertices->at(Indices->at(i + 2)).Normal, P + c.Normal);
    }
    //For each vertex's normal
    for (int i = 0; i < Vertices->size(); i++)
    {
        //Normalize that vertex's normal and store it where it was.
        XMStoreFloat3(&Vertices->at(i).Normal, XMVector3Normalize(XMLoadFloat3(&Vertices->at(i).Normal)));
    }
}

void CalculateFlatNormals(std::vector<SimpleVertex>* Vertices, std::vector<unsigned short>* Indices)
{
    //  The function is based off of the following psuedocode
    //  for each traingle abc
    //      perpendicular = normalize(crossProduct(triangle.b - triangle.a, triangle.c - triangle.a))
    //      a.normal = perpendicular
    //      b.normal = perpendicular
    //      c.normal = perpendicular

    // For each triangle...
    int i = 0;
    while (i < Indices->size())
    {
        //Load the positions into temporary vectors
        SimpleVertex_Vector a = { XMLoadFloat3(&Vertices->at(Indices->at(i)).Pos), XMLoadFloat3(&Vertices->at(Indices->at(i)).Normal) };
        SimpleVertex_Vector b = { XMLoadFloat3(&Vertices->at(Indices->at(i + 1)).Pos), XMLoadFloat3(&Vertices->at(Indices->at(i + 1)).Normal) };
        SimpleVertex_Vector c = { XMLoadFloat3(&Vertices->at(Indices->at(i + 2)).Pos), XMLoadFloat3(&Vertices->at(Indices->at(i + 2)).Normal) };

        //Find the perpendicular vector to the triangle
        XMVECTOR P = XMVector3Normalize(XMVector3Cross(b.Pos - a.Pos, c.Pos - a.Pos));

        //Store that result to each vertex in the triangle and move onto the next triangle
        XMStoreFloat3(&Vertices->at(Indices->at(i)).Normal, P);
        XMStoreFloat3(&Vertices->at(Indices->at(i + 1)).Normal, P);
        XMStoreFloat3(&Vertices->at(Indices->at(i + 2)).Normal, P);
        i += 3;
    }
}
