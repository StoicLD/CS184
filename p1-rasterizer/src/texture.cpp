#include "texture.h"
#include "CGL/color.h"

#include <cmath>
#include <algorithm>

namespace CGL {

Color Texture::sample(const SampleParams &sp) {
  // Parts 5 and 6: Fill this in.
  // 检查sp的采样方式，决定采样的类型格式之类的
  // Should return a color sampled based on the psm and lsm parameters given
  //std::cout<<"Texture::sample时纹理坐标是 "<<sp.p_uv<<std::endl;
  //直接用原本大小的纹理图片，也就是level0
  float levelD = 0;

  /*
   * 首先确定level，默认情况下level是0
   */
  //level为0
  if(sp.lsm == L_ZERO)
  {
    if (sp.psm == P_NEAREST)
    {
      //最邻近采样
      return this->sample_nearest(sp.p_uv, (int)levelD);
    }
    else
    {
      //双线性插值的采样
      return this->sample_bilinear(sp.p_uv, (int)levelD);
    }
  }
  //使用最接近的level
  else if(sp.lsm == L_NEAREST)
  {
    //这个就直接选择一个最近的level就OK了
    //也就是round一下,比如 0.7就round到1，0.2就round到0
    levelD = round(get_level(sp));
    if (sp.psm == P_NEAREST)
    {
      //最邻近采样，我直接比较平方和的大小（距离），选择就近的纹理点
      return this->sample_nearest(sp.p_uv, (int)levelD);
    }
    else
    {
      //双线性插值的采样
      return this->sample_bilinear(sp.p_uv, (int)levelD);
    }
  }
  //采用线性插值level，在两个level的纹理图片中插值
  else
  {
    levelD = get_level(sp);
    if (sp.psm == P_NEAREST)
    {
      Color c0 = this->sample_nearest(sp.p_uv, (int)floor(levelD));
      Color c1 = this->sample_nearest(sp.p_uv, (int)ceil(levelD));
      float factor;
      float level_up = ceil(levelD);
      float level_down = floor(levelD);
      if(level_up == level_down)
        factor = 0;
      else
        factor = (levelD - level_down) / (level_up - level_down);

      Color final_color(c0 + (-factor * c0) + factor * c1);
      final_color = Color(std::max<float>(0, final_color.r),
                          std::max<float>(0, final_color.g),
                          std::max<float>(0, final_color.b));
      return final_color;
    }
    else {
      //双线性插值的采样
      return this->sample_bilinear(sp.p_uv, levelD);
    }
  }
}

/**
 * 获取的level是一个float类型的，之后会根据nearest还是linear来
 * 得到最终的值
 * @param sp
 * @return
 */
float Texture::get_level(const SampleParams &sp) {
  // Optional helper function for Parts 5 and 6
  // 这是两个很小的向量（看作是向量微元）
  Vector2D dx_uv = sp.p_dx_uv - sp.p_uv;
  Vector2D dy_uv = sp.p_dy_uv - sp.p_uv;

  //L2是L的平方，而L就是一个pixel在纹理贴图中占据多少个texl
  Vector2D dx_uv_2(dx_uv.x * width, dx_uv.y *height);
  Vector2D dy_uv_2(dy_uv.x * width, dy_uv.y *height);
  float L2 = std::max<float>(dx_uv_2.norm2(), dy_uv_2.norm2());
  if(L2 <= 1)
    return 0;
//  std::cout<<"L2s是"<<L2<<std::endl;
//  std::cout<<"L是"<<sqrt(L2)<<std::endl;
  float levelD = log2(sqrt(L2));

  return levelD;
}

// Returns the nearest sample given a particular level and set of uv coords
Color Texture::sample_nearest(Vector2D uv, int level) {
      // Optional helper function for Parts 5 and 6
      // Feel free to ignore or create your own
      if (level >= mipmap.size() || level < 0)
      {
          std::cout << "level越界了！ 此时的level大小是:" <<level<<" 坐标是:"<<uv<<std::endl;
          return Color();
      }
      int tx = static_cast<int>(uv.x * mipmap[level].width);
      int ty = static_cast<int>(uv.y * mipmap[level].height);
      return mipmap[level].get_texel(tx, ty);
}

// Returns the bilinear sample given a particular level and set of uv coords
Color Texture::sample_bilinear(Vector2D uv, int level) {
      // Optional helper function for Parts 5 and 6
      // Feel free to ignore or create your own
      // for part5 先不管level
      if (level >= mipmap.size() || level < 0)
      {
          std::cout << "level越界了！ 此时的level大小是:" <<level<<" 坐标是:"<<uv<<std::endl;
          return Color();
      }
      float tx = static_cast<float>(uv.x * mipmap[level].width);
      float ty = static_cast<float>(uv.y * mipmap[level].height);

      //采集四个附近的点的坐标
      //
      int x0 = static_cast<int>(floor(tx));   //向下取整 2.1 ==> 2.0, 2.0 ==> 2.0
      int y0 = static_cast<int>(floor(ty));
      int x1 = static_cast<int>(ceil(tx));    //向上取整 2.1 ==> 3.0, 2.0 ==> 2.0
      int y1 = static_cast<int>(ceil(ty));

      //获取这四个点的
      Color u00 = mipmap[level].get_texel(x0, y0);
      Color u10 = mipmap[level].get_texel(x1, y0);
      Color u01 = mipmap[level].get_texel(x0, y1);
      Color u11 = mipmap[level].get_texel(x1, y1);

      Color u0;
      Color u1;
      if (x1 == x0)
      {
        u0 = u00;
        u1 = u01;
      }
      else
      {
        u0 = Lerp((tx - x0) / (x1 - x0), u00, u10);
        u1 = Lerp((tx - x0) / (x1 - x0), u01, u11);
      }

  return Lerp(ty - y0, u0, u1);
}

Color Texture::Lerp(float x, CGL::Color c0, CGL::Color c1)
{
    return Color(std::max<float>(c0.r + x * (c1.r - c0.r), 0.0),
                 std::max<float>(c0.g + x * (c1.g - c0.g), 0.0),
                 std::max<float>(c0.b + x * (c1.b - c0.b), 0.0));
}

/****************************************************************************/



inline void uint8_to_float(float dst[3], unsigned char *src) {
  uint8_t *src_uint8 = (uint8_t *)src;
  dst[0] = src_uint8[0] / 255.f;
  dst[1] = src_uint8[1] / 255.f;
  dst[2] = src_uint8[2] / 255.f;
}

inline void float_to_uint8(unsigned char *dst, float src[3]) {
  uint8_t *dst_uint8 = (uint8_t *)dst;
  dst_uint8[0] = (uint8_t)(255.f * max(0.0f, min(1.0f, src[0])));
  dst_uint8[1] = (uint8_t)(255.f * max(0.0f, min(1.0f, src[1])));
  dst_uint8[2] = (uint8_t)(255.f * max(0.0f, min(1.0f, src[2])));
}

void Texture::generate_mips(int startLevel) {

  // make sure there's a valid texture
  if (startLevel >= mipmap.size()) {
    std::cerr << "Invalid start level";
  }

  // allocate sublevels
  int baseWidth = mipmap[startLevel].width;
  int baseHeight = mipmap[startLevel].height;
  int numSubLevels = (int)(log2f((float)max(baseWidth, baseHeight)));

  numSubLevels = min(numSubLevels, kMaxMipLevels - startLevel - 1);
  mipmap.resize(startLevel + numSubLevels + 1);

  int width = baseWidth;
  int height = baseHeight;
  for (int i = 1; i <= numSubLevels; i++) {

    MipLevel &level = mipmap[startLevel + i];

    // handle odd size texture by rounding down
    width = max(1, width / 2);
    //assert (width > 0);
    height = max(1, height / 2);
    //assert (height > 0);

    level.width = width;
    level.height = height;
    level.texels = vector<unsigned char>(3 * width * height);
  }

  // create mips
  int subLevels = numSubLevels - (startLevel + 1);
  for (int mipLevel = startLevel + 1; mipLevel < startLevel + subLevels + 1;
       mipLevel++) {

    MipLevel &prevLevel = mipmap[mipLevel - 1];
    MipLevel &currLevel = mipmap[mipLevel];

    int prevLevelPitch = prevLevel.width * 3; // 32 bit RGB
    int currLevelPitch = currLevel.width * 3; // 32 bit RGB

    unsigned char *prevLevelMem;
    unsigned char *currLevelMem;

    currLevelMem = (unsigned char *)&currLevel.texels[0];
    prevLevelMem = (unsigned char *)&prevLevel.texels[0];

    float wDecimal, wNorm, wWeight[3];
    int wSupport;
    float hDecimal, hNorm, hWeight[3];
    int hSupport;

    float result[3];
    float input[3];

    // conditional differentiates no rounding case from round down case
    if (prevLevel.width & 1) {
      wSupport = 3;
      wDecimal = 1.0f / (float)currLevel.width;
    } else {
      wSupport = 2;
      wDecimal = 0.0f;
    }

    // conditional differentiates no rounding case from round down case
    if (prevLevel.height & 1) {
      hSupport = 3;
      hDecimal = 1.0f / (float)currLevel.height;
    } else {
      hSupport = 2;
      hDecimal = 0.0f;
    }

    wNorm = 1.0f / (2.0f + wDecimal);
    hNorm = 1.0f / (2.0f + hDecimal);

    // case 1: reduction only in horizontal size (vertical size is 1)
    if (currLevel.height == prevLevel.height) {
      //assert (currLevel.height == 1);

      for (int i = 0; i < currLevel.width; i++) {
        wWeight[0] = wNorm * (1.0f - wDecimal * i);
        wWeight[1] = wNorm * 1.0f;
        wWeight[2] = wNorm * wDecimal * (i + 1);

        result[0] = result[1] = result[2] = 0.0f;

        for (int ii = 0; ii < wSupport; ii++) {
          uint8_to_float(input, prevLevelMem + 3 * (2 * i + ii));
          result[0] += wWeight[ii] * input[0];
          result[1] += wWeight[ii] * input[1];
          result[2] += wWeight[ii] * input[2];
        }

        // convert back to format of the texture
        float_to_uint8(currLevelMem + (3 * i), result);
      }

      // case 2: reduction only in vertical size (horizontal size is 1)
    } else if (currLevel.width == prevLevel.width) {
      //assert (currLevel.width == 1);

      for (int j = 0; j < currLevel.height; j++) {
        hWeight[0] = hNorm * (1.0f - hDecimal * j);
        hWeight[1] = hNorm;
        hWeight[2] = hNorm * hDecimal * (j + 1);

        result[0] = result[1] = result[2] = 0.0f;
        for (int jj = 0; jj < hSupport; jj++) {
          uint8_to_float(input, prevLevelMem + prevLevelPitch * (2 * j + jj));
          result[0] += hWeight[jj] * input[0];
          result[1] += hWeight[jj] * input[1];
          result[2] += hWeight[jj] * input[2];
        }

        // convert back to format of the texture
        float_to_uint8(currLevelMem + (currLevelPitch * j), result);
      }

      // case 3: reduction in both horizontal and vertical size
    } else {

      for (int j = 0; j < currLevel.height; j++) {
        hWeight[0] = hNorm * (1.0f - hDecimal * j);
        hWeight[1] = hNorm;
        hWeight[2] = hNorm * hDecimal * (j + 1);

        for (int i = 0; i < currLevel.width; i++) {
          wWeight[0] = wNorm * (1.0f - wDecimal * i);
          wWeight[1] = wNorm * 1.0f;
          wWeight[2] = wNorm * wDecimal * (i + 1);

          result[0] = result[1] = result[2] = 0.0f;

          // convolve source image with a trapezoidal filter.
          // in the case of no rounding this is just a box filter of width 2.
          // in the general case, the support region is 3x3.
          for (int jj = 0; jj < hSupport; jj++)
            for (int ii = 0; ii < wSupport; ii++) {
              float weight = hWeight[jj] * wWeight[ii];
              uint8_to_float(input, prevLevelMem +
                                        prevLevelPitch * (2 * j + jj) +
                                        3 * (2 * i + ii));
              result[0] += weight * input[0];
              result[1] += weight * input[1];
              result[2] += weight * input[2];
            }

          // convert back to format of the texture
          float_to_uint8(currLevelMem + currLevelPitch * j + 3 * i, result);
        }
      }
    }
  }
}

}
