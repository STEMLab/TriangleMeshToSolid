//
// Created by dongmin on 18. 1. 18.
//

#ifndef TRIANGLEMESHTOCLEARSOLID_SURFACECOMPUTATION_H
#define TRIANGLEMESHTOCLEARSOLID_SURFACECOMPUTATION_H

#include "features/Surface.h"
#include "cgal/Types.h"

class SurfaceComputation {
public:
    static void flatten(Surface *&sf);

    static Vertex* getCenterPoint(Surface *pSurface);

    static Plane_3 getPlane3(Surface *&pSurface);

    static void triangulate(Surface *&pSurface);

    static std::vector<Point_2> to2D(Surface *&pSurface, Plane_3 plane);

    static void removeStraight(Surface *&pSurface);
};


#endif //TRIANGLEMESHTOCLEARSOLID_SURFACECOMPUTATION_H