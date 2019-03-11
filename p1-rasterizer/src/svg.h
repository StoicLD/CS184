// Original file Copyright CMU462 Fall 2015: 
// Kayvon Fatahalian, Keenan Crane,
// Sky Gao, Bryce Summers, Michael Choquette.
#ifndef CGL_SVG_H
#define CGL_SVG_H

#include <map>
#include <vector>
#include <string>

#include "CGL/color.h"
// #include "texture.h"
#include "CGL/vector2D.h"
#include "CGL/matrix3x3.h"
#include "CGL/tinyxml2.h"
using namespace tinyxml2;

#include "transforms.h"
#include "texture.h"

namespace CGL {

class DrawRend;

//一个用来表示不同类型元素的枚举
//包括三角形，点，线，长方形，图片等等。。。
typedef enum e_SVGElementType {
  NONE = 0,
  POINT,
  LINE,
  POLYLINE,
  RECT,
  POLYGON,
  ELLIPSE,
  IMAGE,
  GROUP,
  TRIANGLE
} SVGElementType;

struct Style {
  Color strokeColor;
  Color fillColor;
  float strokeWidth;
  float miterLimit;
  bool strokeVisible;
};

//svg元素，包含绘制类型，style，变换矩阵
struct SVGElement {

  SVGElement( SVGElementType _type ) 
    : type( _type ), transform( Matrix3x3::identity() ) { }
  //虚析构函数，基类的析构函数应该写成虚函数的形式
  virtual ~SVGElement() { }

  //=0表示纯虚函数, 纯虚函数无需定义
  virtual void draw(DrawRend *dr, Matrix3x3 global_transform) = 0;

  // primitive type, 一个枚举
  SVGElementType type;

  // styling
  Style style;

  // transformation list
  Matrix3x3 transform;
  
};

//三角形继承自SVG，多了三个点
//重载了draw，添加了一个color函数
struct Triangle : SVGElement {

  Triangle(): SVGElement (TRIANGLE ) { }
  Vector2D p0_svg, p1_svg, p2_svg;

  void draw(DrawRend *dr, Matrix3x3 global_transform);
  virtual Color color(Vector3D p_bary, Vector3D p_dx_bary = Vector3D(), Vector3D p_dy_bary = Vector3D(), 
                        SampleParams sp = SampleParams()) = 0;
};

struct ColorTri : Triangle { 

  // Should return appropriate color given barycentric 
  // coordinates x and y corresponding to vertices 'a' and 'b'.
  // Ignore the dx and dy arguments -- they are for TexTri to 
  // do trilinear filtering.
  Color color(Vector3D p_bary, Vector3D p_dx_bary = Vector3D(), Vector3D p_dy_bary = Vector3D(), 
                        SampleParams sp = SampleParams());

  // Per-vertex colors. Should be interpolated between using 
  // barycentric coordinates.
  Color p0_col, p1_col, p2_col;
};

struct TexTri : Triangle {

  // Should return appropriate color given barycentric 
  // coordinates x and y corresponding to vertices 'a' and 'b'.
  // Use the dx and dy arguments for mipmap
  // level selection in trilinear filtering.
  Color color(Vector3D p_bary, Vector3D p_dx_bary = Vector3D(), Vector3D p_dy_bary = Vector3D(), 
                        SampleParams sp = SampleParams());

  // Per-vertex uv coordinates. 
  // Should be interpolated between using barycentric coordinates.
  Vector2D p0_uv, p1_uv, p2_uv;
  Texture *tex;
};

struct Group : SVGElement {

  Group() : SVGElement  ( GROUP ) { }
  std::vector<SVGElement*> elements;

  void draw(DrawRend *dr, Matrix3x3 global_transform);

  ~Group();

};

struct Point : SVGElement {

  Point() : SVGElement ( POINT ) { }
  Vector2D position;

  void draw(DrawRend *dr, Matrix3x3 global_transform);

};

struct Line : SVGElement {

  Line() : SVGElement ( LINE ) { }  
  Vector2D from;
  Vector2D to;

  void draw(DrawRend *dr, Matrix3x3 global_transform);

};

struct Polyline : SVGElement {

  Polyline() : SVGElement  ( POLYLINE ) { }
  std::vector<Vector2D> points;

  void draw(DrawRend *dr, Matrix3x3 global_transform);

};

struct Rect : SVGElement {

  Rect() : SVGElement ( RECT ) { }
  Vector2D position;
  Vector2D dimension;

  void draw(DrawRend *dr, Matrix3x3 global_transform);

};

struct Polygon : SVGElement {

  Polygon() : SVGElement  ( POLYGON ) { }
  std::vector<Vector2D> points;

  void draw(DrawRend *dr, Matrix3x3 global_transform);

};

struct Image : SVGElement {

  Image() : SVGElement  ( IMAGE ) { }
  Vector2D position;
  Vector2D dimension;
  Texture tex;

  void draw(DrawRend *dr, Matrix3x3 global_transform);
  
};

struct SVG {

  ~SVG();
  float width, height;
  std::vector<SVGElement*> elements;
  std::map<std::string, Texture*> textures;

  void draw(DrawRend *dr, Matrix3x3 global_transform) {
    for (int i = 0; i < elements.size(); ++i)
      elements[i]->draw(dr, global_transform);
  }

};

} // namespace CGL

#endif // CGL_SVG_H
