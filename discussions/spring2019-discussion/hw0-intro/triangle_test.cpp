#include <string>
#include <iostream>

#include "CGL/viewer.h"
#include "CGL/renderer.h"
#include "CGL/vector3D.h"
#include "CGL/matrix3x3.h"

using namespace std;
using namespace CGL;


/**
 * Part 2:
 * Write your own matrix vector multiplication function. Do not use the built-in CGL function!
 */
Vector3D mult(Matrix3x3 mat, Vector3D input) {
    Vector3D tmp = Vector3D(input);
    tmp.x = tmp.x * mat.column(0)[0] + tmp.x * mat.column(1)[0] + tmp.x * mat.column(2)[0];
    tmp.y = tmp.y * mat.column(0)[1] + tmp.y * mat.column(1)[1] + tmp.y * mat.column(2)[1];
    tmp.z = tmp.z * mat.column(0)[2] + tmp.z * mat.column(1)[2] + tmp.z * mat.column(2)[2];
    cout<<"mult函数内的临时向量"<<tmp<<endl;
    return tmp; // FIXME
}

class TriangleDrawer : public Renderer {
 public:

  TriangleDrawer() : mat(0, 1, 0, -1, 0, 0, 0, 0, 1), a(0, 0.5, 0), b(-.5, -.5, 0.0), c(0.5, -.5, 0.0) { }

  ~TriangleDrawer() { }

  string name() {
    return "Triangle Drawing";
  }

  string info() {
    return "Triangle Drawing";
  }

  void init() {
    return;
  }
  
  void render() {
    glBegin(GL_TRIANGLES);
    glColor3f( 1.0, 1.0, 0.0);

    Vector3D a_trans = mult(mat, a);
    Vector3D b_trans = mult(mat, b);
    Vector3D c_trans = mult(mat, c);
    glVertex3f(a_trans[0], a_trans[1], a_trans[2]);
    glVertex3f(b_trans[0], b_trans[1], b_trans[2]);
    glVertex3f(c_trans[0], c_trans[1], c_trans[2]);
    glEnd();
  }

  void resize(size_t w, size_t h) {
    
    this->w = w;
    this->h = h;

    return;
  }
  
 private:
  // frame buffer size
  size_t w, h; 
  Matrix3x3 mat;
  Vector3D a;
  Vector3D b;
  Vector3D c;

};

int main( int argc, char** argv ) {

  // create viewer
  Viewer viewer = Viewer();

  // defined a user space renderer
  Renderer* renderer = new TriangleDrawer();

  // set user space renderer
  viewer.set_renderer(renderer);

  // start the viewer
  viewer.init();
  viewer.start();

  return 0;
}

