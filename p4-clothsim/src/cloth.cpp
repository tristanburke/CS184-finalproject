#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
    this->width = width;
    this->height = height;
    this->num_width_points = num_width_points;
    this->num_height_points = num_height_points;
    this->thickness = thickness;
    
    buildGrid();
    buildClothMesh();
}

Cloth::~Cloth() {
    point_masses.clear();
    springs.clear();
    
    if (clothMesh) {
        delete clothMesh;
    }
}

void Cloth::buildGrid() {
    // TODO (Part 1): Build a grid of masses and springs.
    float w_offset = width/num_width_points;
    float h_offset = height/num_height_points;
    float x, y, z_offset;
    y = 0.0;
    for(int j = 0; j < num_height_points; j++){
        x = 0.0;
        for(int i = 0; i < num_width_points; i++){
            bool p = std::find(pinned.begin(), pinned.end(), vector<int> {i, j}) != pinned.end();
            if(orientation==HORIZONTAL){
                point_masses.emplace_back(Vector3D(x, 1, y), p);
            }
            else{
                z_offset = ((double) rand() / (RAND_MAX))*(2./1000)-(1./1000);
                point_masses.emplace_back(Vector3D(x, y, z_offset), p);
            }
            x += w_offset;
        }
        y += h_offset;
    }
    for(int j = 0; j < num_height_points; j++){
        for(int i = 0; i < num_width_points; i++){
            PointMass* current = &point_masses.at(num_width_points*j + i);
            if(i > 0){
                springs.emplace_back(current, &point_masses.at(num_width_points*j + (i-1)), STRUCTURAL);
            }
            if(j > 0){
                springs.emplace_back(current, &point_masses.at(num_width_points*(j-1) + i), STRUCTURAL);
            }
            if(j > 0 && i > 0){
                springs.emplace_back(current, &point_masses.at(num_width_points*(j-1) + (i-1)), SHEARING);
            }
            if(j > 0 && i < num_width_points-1){
                springs.emplace_back(current, &point_masses.at(num_width_points*(j-1) + (i+1)), SHEARING);
            }
            if(i > 1){
                springs.emplace_back(current, &point_masses.at(num_width_points*j + (i-2)), BENDING);
            }
            if(j > 1){
                springs.emplace_back(current, &point_masses.at(num_width_points*(j-2) + i), BENDING);
            }
        }
    }
    
}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
    double mass = width * height * cp->density / num_width_points / num_height_points;
    double delta_t = 1.0f / frames_per_sec / simulation_steps;
    
    // TODO (Part 2): Compute total force acting on each point mass.
    Vector3D total_acceleration = Vector3D();
    for(int i = 0; i < external_accelerations.size(); i++){
        total_acceleration += external_accelerations.at(i);
    }
    Vector3D total_force = total_acceleration*mass;
    for(int i = 0; i < point_masses.size(); i++){
        point_masses.at(i).forces = total_force;
    }
    float Fs;
    for(int i = 0; i < springs.size(); i++){
        Spring &t = springs.at(i);
        if(cp->enable_bending_constraints && t.spring_type==BENDING){
            Fs = cp->ks * ((t.pm_a->position-t.pm_b->position).norm() - t.rest_length);
        }
        else if(cp->enable_shearing_constraints && t.spring_type==SHEARING){
            Fs = cp->ks * ((t.pm_a->position-t.pm_b->position).norm() - t.rest_length);

        }
        else if(cp->enable_structural_constraints && t.spring_type==STRUCTURAL){
            Fs = (0.2*cp->ks) * ((t.pm_a->position-t.pm_b->position).norm() - t.rest_length);
        }
        t.pm_a->forces += (Fs * (t.pm_b->position-t.pm_a->position).unit());
        t.pm_b->forces += (Fs * (t.pm_a->position-t.pm_b->position).unit());
    }
    // TODO (Part 2): Use Verlet integration to compute new point mass positions
    for(int i = 0; i < point_masses.size(); i++){
        PointMass &pm = point_masses.at(i);
        if(pm.pinned){
            continue;
        }
        Vector3D pos = pm.position + (1-(cp->damping/100.))*(pm.position-pm.last_position) + ((pm.forces/mass)*delta_t*delta_t);
        pm.last_position = pm.position;
        pm.position = pos;
    }
    
    // TODO (Part 4): Handle self-collisions.
    build_spatial_map();
    for(int i = 0; i < point_masses.size(); i++){
        PointMass &pm = point_masses.at(i);
        self_collide(pm, simulation_steps);
    }
    
    // TODO (Part 3): Handle collisions with other primitives.
    for(int i = 0; i < point_masses.size(); i++){
        PointMass &pm = point_masses.at(i);
        vector<CollisionObject *> v = *collision_objects;
        for(int c = 0; c < v.size(); c++){
            v.at(c)->collide(pm);
        }
    }
    
    // TODO (Part 2): Constrain the changes to be such that the spring does not change
    // in length more than 10% per timestep [Provot 1995].
    for(int i = 0; i < springs.size(); i++){
        Spring &t = springs.at(i);
        PointMass *pm_a = t.pm_a;
        PointMass *pm_b = t.pm_b;
        Vector3D dist = (pm_b->position - pm_a->position);
        if(dist.norm() > 1.1*t.rest_length){
            if(pm_a->pinned){
                double diff = dist.norm()-(1.1*t.rest_length);
                pm_b->position -= diff*dist.unit();
            }
            else if(pm_b->pinned){
                double diff = dist.norm()-(1.1*t.rest_length);
                pm_a->position += diff*dist.unit();
            }
            else{
                double diff = (dist.norm()-(1.1*t.rest_length))/2.;
                pm_a->position += diff*dist.unit();
                pm_b->position -= diff*dist.unit();
            }
        }
    }
    
}

void Cloth::build_spatial_map() {
    for (const auto &entry : map) {
        delete(entry.second);
    }
    map.clear();
    
    // TODO (Part 4): Build a spatial map out of all of the point masses.
    for(int i = 0; i < point_masses.size(); i++){
        float hash = hash_position(point_masses.at(i).position);
        if(map.find(hash) == map.end()){
            map[hash] = new vector<PointMass *>();
        }
        map[hash]->push_back(&point_masses.at(i));
    }
}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
    // TODO (Part 4): Handle self-collision for a given point mass.
    vector<PointMass *> * v = map[hash_position(pm.position)];
    Vector3D correction = Vector3D();
    int count = 0;
    for(int i = 0; i < v->size(); i++){
        PointMass c = *(v->at(i));
        double dist = (pm.position - c.position).norm();
        if(dist > 0 && dist < 2*thickness){
            //is not same pm and is self folding
            Vector3D dir = (pm.position - c.position).unit();
            correction += dir*(2*thickness - dist);
            count++;
        }
    }
    if(count > 0){
        pm.position = pm.position +(correction/count/simulation_steps);
    }
}

float Cloth::hash_position(Vector3D pos) {
    // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.
    float x_h = pos.x - fmod(pos.x, 3*width/num_width_points);
    float y_h = pos.y - fmod(pos.y, 3*height/num_height_points);
    float z_h = pos.z - fmod(pos.z, max(3*width/num_width_points, 3*height/num_height_points));
    return x_h + (113.*y_h) + (113.*113.*z_h);
    return pow(2, x_h) + pow(3, y_h) + pow(5, z_h);
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
    PointMass *pm = &point_masses[0];
    for (int i = 0; i < point_masses.size(); i++) {
        pm->position = pm->start_position;
        pm->last_position = pm->start_position;
        pm++;
    }
}

void Cloth::buildClothMesh() {
    if (point_masses.size() == 0) return;
    
    ClothMesh *clothMesh = new ClothMesh();
    vector<Triangle *> triangles;
    
    // Create vector of triangles
    for (int y = 0; y < num_height_points - 1; y++) {
        for (int x = 0; x < num_width_points - 1; x++) {
            PointMass *pm = &point_masses[y * num_width_points + x];
            // Get neighboring point masses:
            /*                      *
             * pm_A -------- pm_B   *
             *             /        *
             *  |         /   |     *
             *  |        /    |     *
             *  |       /     |     *
             *  |      /      |     *
             *  |     /       |     *
             *  |    /        |     *
             *      /               *
             * pm_C -------- pm_D   *
             *                      *
             */
            
            float u_min = x;
            u_min /= num_width_points - 1;
            float u_max = x + 1;
            u_max /= num_width_points - 1;
            float v_min = y;
            v_min /= num_height_points - 1;
            float v_max = y + 1;
            v_max /= num_height_points - 1;
            
            PointMass *pm_A = pm                       ;
            PointMass *pm_B = pm                    + 1;
            PointMass *pm_C = pm + num_width_points    ;
            PointMass *pm_D = pm + num_width_points + 1;
            
            Vector3D uv_A = Vector3D(u_min, v_min, 0);
            Vector3D uv_B = Vector3D(u_max, v_min, 0);
            Vector3D uv_C = Vector3D(u_min, v_max, 0);
            Vector3D uv_D = Vector3D(u_max, v_max, 0);
            
            
            // Both triangles defined by vertices in counter-clockwise orientation
            triangles.push_back(new Triangle(pm_A, pm_C, pm_B,
                                             uv_A, uv_C, uv_B));
            triangles.push_back(new Triangle(pm_B, pm_C, pm_D,
                                             uv_B, uv_C, uv_D));
        }
    }
    
    // For each triangle in row-order, create 3 edges and 3 internal halfedges
    for (int i = 0; i < triangles.size(); i++) {
        Triangle *t = triangles[i];
        
        // Allocate new halfedges on heap
        Halfedge *h1 = new Halfedge();
        Halfedge *h2 = new Halfedge();
        Halfedge *h3 = new Halfedge();
        
        // Allocate new edges on heap
        Edge *e1 = new Edge();
        Edge *e2 = new Edge();
        Edge *e3 = new Edge();
        
        // Assign a halfedge pointer to the triangle
        t->halfedge = h1;
        
        // Assign halfedge pointers to point masses
        t->pm1->halfedge = h1;
        t->pm2->halfedge = h2;
        t->pm3->halfedge = h3;
        
        // Update all halfedge pointers
        h1->edge = e1;
        h1->next = h2;
        h1->pm = t->pm1;
        h1->triangle = t;
        
        h2->edge = e2;
        h2->next = h3;
        h2->pm = t->pm2;
        h2->triangle = t;
        
        h3->edge = e3;
        h3->next = h1;
        h3->pm = t->pm3;
        h3->triangle = t;
    }
    
    // Go back through the cloth mesh and link triangles together using halfedge
    // twin pointers
    
    // Convenient variables for math
    int num_height_tris = (num_height_points - 1) * 2;
    int num_width_tris = (num_width_points - 1) * 2;
    
    bool topLeft = true;
    for (int i = 0; i < triangles.size(); i++) {
        Triangle *t = triangles[i];
        
        if (topLeft) {
            // Get left triangle, if it exists
            if (i % num_width_tris != 0) { // Not a left-most triangle
                Triangle *temp = triangles[i - 1];
                t->pm1->halfedge->twin = temp->pm3->halfedge;
            } else {
                t->pm1->halfedge->twin = nullptr;
            }
            
            // Get triangle above, if it exists
            if (i >= num_width_tris) { // Not a top-most triangle
                Triangle *temp = triangles[i - num_width_tris + 1];
                t->pm3->halfedge->twin = temp->pm2->halfedge;
            } else {
                t->pm3->halfedge->twin = nullptr;
            }
            
            // Get triangle to bottom right; guaranteed to exist
            Triangle *temp = triangles[i + 1];
            t->pm2->halfedge->twin = temp->pm1->halfedge;
        } else {
            // Get right triangle, if it exists
            if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
                Triangle *temp = triangles[i + 1];
                t->pm3->halfedge->twin = temp->pm1->halfedge;
            } else {
                t->pm3->halfedge->twin = nullptr;
            }
            
            // Get triangle below, if it exists
            if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
                Triangle *temp = triangles[i + num_width_tris - 1];
                t->pm2->halfedge->twin = temp->pm3->halfedge;
            } else {
                t->pm2->halfedge->twin = nullptr;
            }
            
            // Get triangle to top left; guaranteed to exist
            Triangle *temp = triangles[i - 1];
            t->pm1->halfedge->twin = temp->pm2->halfedge;
        }
        
        topLeft = !topLeft;
    }
    
    clothMesh->triangles = triangles;
    this->clothMesh = clothMesh;
}
