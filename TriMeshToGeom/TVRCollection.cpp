//
//  TVRCollection.cpp
//  TriMeshToGeom
//
//  Created by Dong's MacBook Pro on 2017. 6. 21..
//  Copyright © 2017년 Dong's MacBook Pro. All rights reserved.
//

#include "TVRCollection.hpp"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#define FLOATSIZE 1000000

using namespace std;

vector<pair<string, vector<CombinedPolygon*>> > TVRCollection::makeSurfaces(Checker* check){
    vector<pair<string, vector<CombinedPolygon*>> > result;
    for (auto it=this->obj_list.begin() ; it != this->obj_list.end() ; it++){
        cout << (it->first) << " is converting..." << endl;
        it->second->setChecker(check);
//        if (it->second->checkDuplicateVertex()) {
//            cout << "Duplicate Vertex" << endl;
//            return {};
//        }
        vector<CombinedPolygon*> polygons = it->second->makePolygons();
        result.push_back(make_pair(it->first, polygons));
    }

    return result;
}

void TVRCollection::print(){
    for (auto it=this->obj_list.begin() ; it != this->obj_list.end() ; it++){
        cout << "\n\n\n-------" << it->first << "-------\n" << endl;
        it->second->print();
    }
}

int TVRCollection::loadFile(char *filename, Checker* check){
    ifstream fin;
    fin.open(filename);

    if (!fin) return -1;//error

    string inputstr;
    getline(fin, inputstr);
    if (inputstr.find("TVR0") == string::npos){
        cout << "different version : " <<  inputstr << endl;
        return -1;
    }

    obj_type* obj = NULL;
    string group_name;
    int v_count = 0;
    int f_count = 0;

    while(!fin.eof()){
        getline(fin, inputstr);
        if (inputstr.size() < 3) continue;
        switch(inputstr.c_str()[0]){
            case 'c':{
                break;
            }
            case 'v':{
                v_count++;
                if (v_count % 5000 == 0) cout << "Loaded vertex : " << v_count << endl;
                vertex_type vt;
                this->makeVertex(inputstr, vt);

                vertex_type* pt_v = this->findSameVertex(check, vt);
                this->vertex.push_back(pt_v);
                break;
            }
            case 'g':{
                if (obj != NULL){
                    obj->setVertexList(&this->vertex);
                    this->obj_list.push_back(make_pair(group_name, obj));
                }
                obj = new obj_type();
                obj->setCollection(this);
                group_name = this->getGroupName(inputstr);
                break;
            }
            case 'f':{
                f_count++;
                if (f_count % 5000 == 0) cout << "Loaded faces : " << f_count << endl;
                polygon_type pt;
                this->makeTriangle(inputstr, pt);
                obj->tri_list.push_back(pt);
                break;
            }
        }
    }
    obj->setVertexList(&this->vertex);
    if (group_name.find('\r') != string::npos) group_name.erase(group_name.find('\r'));
    this->obj_list.push_back(make_pair(group_name, obj));

    return 0;
}

vertex_type* TVRCollection::findSameVertex(Checker* check, vertex_type& vt){
    for (int i = 0 ; i < this->vertex.size() ; i++){
        if (check->isSameVertex(*this->vertex[i], vt) ){
            return this->vertex[i];
        }
    }

    vertex_type* new_vt = new vertex_type();
    new_vt->x = vt.x;
    new_vt->y = vt.y;
    new_vt->z = vt.z;
    return new_vt;
}

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

void TVRCollection::makeTriangle(string& input, polygon_type& pt){
    std::vector<std::string> x = split(input, ' ');

    pt.a = stol(x[1]);
    pt.b = stol(x[2]);
    pt.c = stol(x[3]);
}

void TVRCollection::makeVertex(string& input, vertex_type& vt){
    std::stringstream ss;
    ss.str(input);

    string line;
    getline(ss, line, '\t');
    std::vector<std::string> x = split(line, ' ');

    //vt.x = stod(x[1]);
    vt.x = atof(x[1].c_str());
    vt.y = stod(x[2]);
    vt.z = stod(x[3]);
}

string TVRCollection::getGroupName(string& input){
    std::vector<std::string> x = split(input, ' ');
    return x[1];
}

