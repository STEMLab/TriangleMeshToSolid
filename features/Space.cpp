#include "features/Space.h"

#include "compute/SurfacesListComputation.h"
#include "compute/VertexComputation.h"

#include <cstdio>
#include <queue>
#include <climits>
#include <algorithm>
#include <cmath>
#include <compute/SurfaceComputation.h>
#include "cgal/SurfaceIntersection.h"
#include "HalfEdge.h"

Space::Space(){
    generation = 0;
}

Space::Space(string pname)
{
    name = pname;
    generation = 0;
}

Space::~Space()
{
    //dtor
}


int Space::convertTrianglesToSurfaces(vector<Triangle*>& triangles){
    vector<Surface*> c_list;
    ull size = triangles.size();
    for (ull i = 0 ; i < size; i++){
        Surface* newcp = new Surface(*triangles[i]);
        c_list.push_back(newcp);
    }

    this->surfacesList.insert(this->surfacesList.end(), c_list.begin(), c_list.end());

    cout << "\ndone make Surfaces" << endl;
    return 0;
}


int Space::combineSurface(double degree){
    cout << "Combine Surfaces" << endl;

    sort(this->surfacesList.begin(), this->surfacesList.end(), Surface::compareLength);
    ull p_size = this->surfacesList.size();
    bool* checked = (bool*)malloc(sizeof(bool) * p_size);
    std::fill(checked, checked + p_size, false);

    vector<Surface*> new_poly_list;
    int combined_count = 0;
    for (ull i = 0 ; i < this->surfacesList.size() ; i++)
    {
        if (checked[i]) continue;
        checked[i] = true;

        ll count = -1;
        Surface* newcp = new Surface(this->surfacesList[i]);
        while(count != 0){
            newcp = attachSurfaces(newcp, i+1, checked, count, degree);
            if (newcp == NULL) break;
            printProcess(combined_count, this->surfacesList.size(), "combineSurface");
            combined_count += count;
        }
        if (newcp != NULL) new_poly_list.push_back(newcp);
    }

    freeSurfaces();
    this->surfacesList = new_poly_list;
    return 0;
}

Surface* Space::attachSurfaces(Surface* cp, ull start, bool* checked, ll& count, double degree)
{
    count = 0;
    if (cp->normal == CGAL::NULL_VECTOR) {
        cerr << "Normal Vector is NULL in attach Surfaces" << endl;
        cerr << cp->toJSONString() << endl;
        exit(-1);
    }

    for (ull id = start ; id < this->surfacesList.size() ; id++)
    {
        if (!checked[id])
        {
            Surface* sf = this->surfacesList[id];
            if (TMIC::combine(cp, sf, degree) == 0)
            {
                cout << ".";
                cp->triangles.insert(cp->triangles.end(), sf->triangles.begin(), sf->triangles.end());
                checked[id] = true;
                count++;
            }

        }
    }
    return cp;
}

int Space::updateNormal(){
    cout << "\n------------updateNormal------------\n" << endl;
    for (ull i = 0 ; i < (int)this->surfacesList.size() ; i++)
    {
        Surface* surface = this->surfacesList[i];
        if (!surface->updateNormal())
        {
            cout << surface->toJSONString() <<endl;
            cout << "Cannot make Normal" << endl;
            exit(-1);
        }

    }
    return 0;
}


int Space::simplifySegment(){
    cout << "\n------------simplifySegment------------\n" << endl;
    sort(this->surfacesList.begin(), this->surfacesList.end(), Surface::compareLength);
    ull sizeOfSurfaces = this->surfacesList.size();

    for (ull i = 0 ; i < sizeOfSurfaces; i++){
        assert((int) this->surfacesList[i]->getVerticesSize() >= 3);
    }

    for (ull i = 0 ; i < sizeOfSurfaces - 1; i++){
        Surface *&surfaceI = this->surfacesList[i];
        if (!surfaceI->isValid()) continue;
        printProcess(i, sizeOfSurfaces, "");
        for (ull j = i + 1; j < sizeOfSurfaces ; j++){
            Surface *&surfaceJ = this->surfacesList[j];
            if (!surfaceI->isValid()) break;
            if (!surfaceJ->isValid()) continue;
            if (!CGALCalculation::isIntersect_BBOX(surfaceI, surfaceJ)) continue;
            while (TMIC::simplifyLineSegment(this->surfacesList[i], this->surfacesList[j]) == 0){
                if (!surfaceI->isValid() || !surfaceJ->isValid()) break;
            }
        }
        /* v0.3.8
        for (ull j = i + 1; j < sizeOfSurfaces ; j++)
        {
            int loop_count = 0;
            int j_sizeOfVertices = (int) this->surfacesList[j]->getVerticesSize();
            if (!CGALCalculation::isIntersect_BBOX(surfaceI, this->surfacesList[j])) continue;
            while (SurfacePairComputation::simplifyLineSegment(this->surfacesList[i], this->surfacesList[j]) == 0)
            {
                if (!this->surfacesList[j]->isValid()){
                    cout << this->surfacesList[j]->toJSONString() << endl;
                    delete this->surfacesList[j];
                    this->surfacesList.erase(this->surfacesList.begin() + j);
                    Checker::num_of_invalid += 1;
                    cout << "Erase invalid surface" << endl;
                    return simplifySegment();
                }
                assert (this->surfacesList[i]->isValid());

                loop_count++;
                assert(loop_count <= j_sizeOfVertices);
                j_sizeOfVertices = (int) this->surfacesList[j]->getVerticesSize();

            }
        }
        */
    }
    sizeOfSurfaces = this->surfacesList.size();
    /* v0.3.8
    for (ull i = 0 ; i < sizeOfSurfaces; i++){
        assert((int) this->surfacesList[i]->getVerticesSize() >= 3);
        for (HalfEdge* he : this->surfacesList[i]->boundaryEdges){
            assert(he->parent == this->surfacesList[i]);
        }
    }
    */

    bool hasRemoved = false;
    for (int i = sizeOfSurfaces - 1; i >= 0 ; i--){
        if (this->surfacesList[i]->isValid()){

        }
        else{
            hasRemoved = true;
            delete this->surfacesList[i];
            this->surfacesList.erase(this->surfacesList.begin() + i);
        }
    }

    if (hasRemoved)
        return simplifySegment();
    else
        return 0;
}

int Space::checkSurfaceValid() {
    cout << "\n------------- check whether surface is valid --------------\n" << endl;
    for (vector<Surface*>::size_type i = 0 ; i < this->surfacesList.size(); )
    {
        Surface* surface = this->surfacesList[i];
        surface->updateMBB();

        if (surface->isValid()){
            i++;
        }
        else{
            return -1;
        }
    }
    return 0;
}


int Space::removeStraight(){
    cout << "\n------------- removeStraight --------------\n" << endl;
    for (vector<Surface*>::size_type i = 0 ; i < this->surfacesList.size(); ){
        Surface* surface = this->surfacesList[i];
        SurfaceComputation::removeStraight(surface);
        if (surface->isValid()){
            i++;
        }
        else{
            delete surface;
            this->surfacesList.erase(this->surfacesList.begin() + i);
            Checker::num_of_invalid += 1;
            cout << "Erase invalid surface" << endl;
        }
    }
    return 0;
}

int Space::translateSpaceToOrigin(){
    cout << "\n------------- translateSpaceToOrigin --------------\n" << endl;

    updateMBB();
    double diff[3];
    for (int i = 0 ; i < 3 ; i++){
        diff[i] = -this->min_coords[i];
    }

    for (ull i = 0 ; i < this->surfacesList.size() ; i++)
    {
        this->surfacesList[i]->translate(diff);
    }

    for (ull i = 0 ; i < this->vertices.size() ; i++){
        this->vertices[i]->translate(diff);
    }

    return 0;
}

void Space::updateMBB(){
    vector<vector<double> > min_max;
    min_max = SurfacesListComputation::getMBB(this->surfacesList);
    for (int i = 0 ; i < 3 ; i++){
        this->min_coords[i] = min_max[0][i];
        this->max_coords[i] = min_max[1][i];
    }
}

void Space::freeSurfaces(){
    for (ull i = 0 ; i < this->surfacesList.size() ; i++)
    {
        delete(this->surfacesList[i]);
    }
    this->surfacesList.clear();
}


int Space::checkDuplicateVertexInSurfaces() {
    for (unsigned int s_i = 0 ; s_i < this->surfacesList.size() ;s_i++){
        if (surfacesList[s_i]->checkDuplicate()){
            cout << "it has duplicate Vertex" << endl;
            return -1;
        }
    }
    return 0;
}

int Space::makeSurfacesPlanar() {
    for (ull i = 0 ; i < this->surfacesList.size() ; i++){
        SurfaceComputation::flatten(this->surfacesList[i]);
    }
    return 0;
}

void Space::sortSurfacesByArea() {
    sort(this->surfacesList.begin(), this->surfacesList.end(), Surface::compareArea);
}

void Space::tagID() {
    SurfacesListComputation::tagID(this->surfacesList);
}

void Space::resolveIntersectionINTRASurface() {
    int newSurfaceCount = 0;
    for (int sfID = 0 ; sfID < this->surfacesList.size(); ) {
        vector<Surface*> newSurfaces = SurfaceIntersection::resolveSelfIntersection(this->surfacesList[sfID]);
    }
    cout << "Intersect Surfaces : " << this->surfacesList.size() << endl;
}


void Space::clearTrianglesListInSurfaces() {
    for (unsigned int sfID = 0 ; sfID < this->surfacesList.size(); sfID++) {
        this->surfacesList[sfID]->clearTriangleList();
    }
}

void Space::triangulateSurfaces() {
    this->hasTriangulation = true;
    for (unsigned int sfID = 0 ; sfID < this->surfacesList.size(); ) {
        Surface* pSurface = this->surfacesList[sfID];

        if (SurfaceComputation::triangulate(pSurface)){
            cerr << "Triangulation Error" << endl;
            this->surfacesList.erase(this->surfacesList.begin() + sfID);
        }
        else {
            sfID++;
        }
    }
}

int Space::checkSelfIntersection() {
    int count = 0;
    for (unsigned int sfID = 0 ; sfID < this->surfacesList.size(); sfID++) {
        Surface* pSurface = this->surfacesList[sfID];
        if (SurfaceIntersection::checkSelfIntersection(pSurface)){
            cerr << "Self Intersection in Surface " << sfID << endl;
            cerr << pSurface->toJSONString() << endl;
            count++;
        }
        else {
        }
    }
    cout << this->name << " Self Intersection Count : " << count << endl;
    return 0;
}

vector<Triangle *> Space::getTriangleListOfAllSurfaces() {
    vector<Triangle *> triangles;
    for (unsigned int sfID = 0 ; sfID < this->surfacesList.size(); sfID++) {
        Surface* pSurface = this->surfacesList[sfID];
        int index = 0;
        for (Triangle* triangle : pSurface->triangles){
            triangle->sf_id = to_string(sfID) + "_" + to_string(index++);
        }
        triangles.insert(triangles.end(),pSurface->triangles.begin(),pSurface->triangles.end());
    }
    return triangles;
}

/*

void Space::rotateSpaceByFloorTo00(){
    cout << " ---------- rotate -------------" << endl;
    sort(this->surfacesList.begin(), this->surfacesList.end(), Surface::compareArea);
    int floor_index = SLC::findFirstSurfaceIndexSimilarWithAxis(this->surfacesList, 2);
    Surface* floor = this->surfacesList[floor_index];
    floor->updateMBB();
    //Plane_3 plane(this->surfacesList[floor_index]->v_list[0]->getCGALPoint(), floor->av_normal);
    //floor->makePlanar(plane);

    Vector_3 vector_z(0,0,1);
    double angle = -CGALCalculation::getAngle(floor->normal, vector_z);
    if (angle == 0.0){
        cout << "angle is 0.0" << endl;
        return;
    }

    Vector_3 unit_vector = CGAL::cross_product(vector_z, floor->normal);
    unit_vector = unit_vector / sqrt(unit_vector.squared_length());
    cout << "rotate " << angle << ", " << unit_vector.squared_length()<< endl;
    assert(unit_vector.squared_length() < 1.000001 && unit_vector.squared_length() > 0.99999);

    double cos_value = cos(angle * PI /180.0);
    double sin_value = sin(angle * PI /180.0);

    double ux = unit_vector.x();
    double uy = unit_vector.y();
    double uz = unit_vector.z();

    Transformation rotateZ(cos_value + ux*ux *(1-cos_value), ux*uy*(1-cos_value) - (uz * sin_value), ux*uz*(1-cos_value) + uy*sin_value,
                            uy*ux*(1-cos_value) + uz * sin_value,cos_value + uy*uy*(1-cos_value), uy*uz*(1-cos_value)- (ux*sin_value),
                            uz*ux*(1-cos_value)-uy*sin_value , uz*uy*(1-cos_value) + ux * sin_value, cos_value + uz*uz*(1-cos_value),
                            1);

    for (ull i = 0 ; i < this->p_vertexList->size() ; i++){
        Point_3 p = CGAL_User::getCGALPoint(this->p_vertexList->at(i));
        p = p.transform(rotateZ);
        this->p_vertexList->at(i)->setCoords(p.x(), p.y(), p.z());
    }
}
*/
