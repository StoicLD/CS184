#ifndef CGL_TEXTURE_H
#define CGL_TEXTURE_H

#include <vector>
#include "CGL/CGL.h"
#include "CGL/color.h"
#include "CGL/vector2D.h"

#include <iostream>
using namespace std;

namespace CGL {

typedef enum PixelSampleMethod { P_NEAREST = 0, P_LINEAR = 1 } PixelSampleMethod;
typedef enum LevelSampleMethod { L_ZERO = 0, L_NEAREST = 1, L_LINEAR = 2 } LevelSampleMethod;

struct SampleParams {
  Vector2D p_uv;
  Vector2D p_dx_uv, p_dy_uv;
  PixelSampleMethod psm;
  LevelSampleMethod lsm;
};

static const int kMaxMipLevels = 14;

struct MipLevel {
	size_t width;
	size_t height;
  // RGB color values
  std::vector<unsigned char> texels;

  //实现一下
  Color get_texel(int tx, int ty)
  {
      tx = std::min<int>(tx, width - 1);
      ty = std::min<int>(ty, height - 1);
      int index = static_cast<int>(3 * (ty * width + tx));
      //std::cout<<"index下标是"<<index<<std::endl;
      if(index < 0 || index + 2 >= texels.size())
      {
          std::cout<<"获取纹理坐标时下标越界！"<<std::endl;
          std::cout<<"坐标是"<<index<<"最大下标是"<<texels.size()<<std::endl;
          std::cout<<"此时ty和tx分别是("<<tx<<","<<ty<<") "<<std::endl;
          return Color();
      }
      float r = static_cast<float>((float)texels[index] / 255.0);
      float g = static_cast<float>((float)texels[index + 1] / 255.0);
      float b = static_cast<float>((float)texels[index + 2] / 255.0);
      Color c(r, g, b);
      //std::cout<<"颜色是"<<c<<std::endl;
      return c;
  }

};

//纹理的结构体
struct Texture {
  size_t width;
  size_t height;
  std::vector<MipLevel> mipmap;

  void init(const vector<unsigned char>& pixels, const size_t& w, const size_t& h) {
    width = w; height = h;

    // A fancy C++11 feature. emplace_back constructs the element in place,
    // and in this case it uses the new {} list constructor syntax.
    mipmap.emplace_back(MipLevel{width, height, pixels});

    generate_mips();
  }

  // Generates up to kMaxMipLevels of mip maps. Level 0 contains
  // the unfiltered original pixels.
  void generate_mips(int startLevel = 0);

  Color sample(const SampleParams &sp);
  float get_level(const SampleParams &sp);

  Color sample_nearest(Vector2D uv, int level = 0);

  Color sample_bilinear(Vector2D uv, int level = 0);

  Color Lerp(float x, Color c0, Color c1);
};

}

#endif // CGL_TEXTURE_H
