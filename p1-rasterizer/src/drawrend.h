#ifndef CGL_DRAWREND_H
#define CGL_DRAWREND_H

#include "CGL/CGL.h"
#include "CGL/renderer.h"
#include "CGL/color.h"
#include <vector>
#include <cstring>
#include "GLFW/glfw3.h"
#include "svg.h"

namespace CGL {

class DrawRend : public Renderer {
 public:
  DrawRend(std::vector<SVG*> svgs_):
  svgs(svgs_), current_svg(0)
  {}

  ~DrawRend( void );

  // inherited Renderer interface functions
  void init();
  void render();
  void resize( size_t w, size_t h );
  std::string name() { return "Draw"; }
  std::string info();
  void cursor_event( float x, float y );
  void scroll_event( float offset_x, float offset_y );
  void mouse_event( int key, int event, unsigned char mods );
  void keyboard_event( int key, int event, unsigned char mods );

  void set_gl(bool gl_) { gl = gl_; }

  // write current pixel buffer to disk
  void write_screenshot();

  // write only framebuffer to disk
  void write_framebuffer();

  // drawing functions
  void redraw();
  void draw_pixels();
  void draw_zoom();

  // view transform functions
  void view_init();
  void set_view(float x, float y, float span);
  void move_view(float dx, float dy, float scale);

  // rasterize a point
  void rasterize_point( float x, float y, Color color );

  // rasterize a line
  void rasterize_line( float x0, float y0,
                       float x1, float y1,
                       Color color);

  // rasterize a triangle
  void rasterize_triangle( float x0, float y0,
                           float x1, float y1,
                           float x2, float y2,
                           Color color, Triangle *tri = NULL );

  //for part4额外添加一个计算重心坐标系数的函数
  bool bary_coord(float x, float y, float x0, float y0, float x1, float y1, float x2, float y2, float* params);


private:
  // Global state variables for SVGs, pixels, and view transforms
  // svgs应该是一堆图片的集合，渲染器用来渲染的目标图片集合
  std::vector<SVG*> svgs; size_t current_svg;
  std::vector<Matrix3x3> svg_to_ndc;
  float view_x, view_y, view_span;

  Matrix3x3 ndc_to_screen;

  std::vector<unsigned char> framebuffer;
  size_t width, height;

  // UI state info
  float cursor_x; float cursor_y;
  bool left_clicked;
  int show_zoom;
  int sample_rate;

  //纹理采样的方法
  PixelSampleMethod psm;
  LevelSampleMethod lsm;

  bool gl;

  //这个向量就是三通道RGB，size为3, 这里三通道的取值范围是0-255
  typedef std::vector<unsigned char> PixelColorStorage;

  // Intuitively, a sample buffer instance is a pixel,
  // or (samples_per_side x samples_per_side) sub-pixels.
  //一个 SampleBuffer 代表一个采样区域（一开始是一个pixel）,默认是正方形的区域
  struct SampleBuffer {
    //sub_pixel的集合，一个三维向量，本质上是一个二维区域，每个sub_pixel有三通道
    std::vector<std::vector<PixelColorStorage> > sub_pixels;
    //这个就是sub-pixel的边长
    size_t samples_per_side;

    SampleBuffer(size_t sps): samples_per_side(sps) {
      clear();
    }
    
    // Fill the subpixel at i,j with the Color c
    //用来填充sub-pixel的颜色
    void fill_color(int i, int j, Color c) {
      PixelColorStorage &p = sub_pixels[i][j];
      // Part 1: Overwrite PixelColorStorage p using Color c.
      //         Pay attention to different data types.
      if(p.size() != 3)
      {
        std::cout<<"PixelColorStorage没有初始化！"<<endl;
      }
      else
      {
        p[0] = (uint8_t)(c.r * 255);
        p[1] = (uint8_t)(c.g * 255);
        p[2] = (uint8_t)(c.b * 255);
      }
    }

    //填充每个sub-pixel
    void fill_pixel(Color c) {
      for (int i = 0; i < samples_per_side; ++i)
        for (int j = 0; j < samples_per_side; ++j)
          fill_color(i, j, c);
    }

    //part2需要用到，返回平均的颜色
    Color get_pixel_color() {
      //vector::data 是第一个元素的地址
      //return Color(sub_pixels[0][0].data());
      // Part 2: Implement get_pixel_color() for supersampling.
      // 需要返回平均颜色

      if(samples_per_side == 1)
          return Color(sub_pixels[0][0].data());

      Color arrangeColor = Color();
      for(int i = 0; i < samples_per_side; i++)
      {
        for(int j = 0; j < samples_per_side; j++)
        {
            try
            {
                PixelColorStorage &p = sub_pixels[i][j];
                //这里注意要转型
                arrangeColor += Color((float)p[0] / 255, (float)p[1] / 255, (float)p[2] / 255);
            }
            catch (int x)
            {
                std::cout<<"sub_pixels的长度与samples_per_side不相等！"<<std::endl;
            }

        }
      }
      //return Color(sub_pixels[0][0].data());
      //std::cout<<"平均前的颜色:"<<arrangeColor<<std::endl;
      arrangeColor.r /= samples_per_side * samples_per_side;
      arrangeColor.g /= samples_per_side * samples_per_side;
      arrangeColor.b /= samples_per_side * samples_per_side;
      //std::cout<<"平均后的颜色:"<<arrangeColor<<std::endl;
      return arrangeColor;
    }
    
    void clear() {
      //
      if (sub_pixels.size() == samples_per_side) {
        for (int i = 0; i < samples_per_side; ++i)
          for (int j = 0; j < samples_per_side; ++j)
          {
            //三维向量sub_pixels中的一维复制为大小为3，每个元素值为255的一个向量
            sub_pixels[i][j].assign(3, (unsigned char) 255);
          }
        return;
      }

      sub_pixels.clear();
      PixelColorStorage white = std::vector<unsigned char>(3, 255);
      std::vector<PixelColorStorage> row;
      row.reserve(samples_per_side);
      for (int i = 0; i < samples_per_side; ++i)
        row.push_back(white);
      sub_pixels.reserve(samples_per_side);
      for (int i = 0; i < samples_per_side; ++i)
        sub_pixels.push_back(row);
    }
  };

  //代表采样buffer的二维向量
  std::vector<std::vector<SampleBuffer> > samplebuffer;

  // This function takes the collected sub-pixel samples and
  // combines them together to fill in the framebuffer in preparation
  // for posting pixels to the screen.
  void resolve() {
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        //这里获取的是一个pixel的颜色，所以在Part2中我们需要平均这个pixel下面的sub-pixel
        //使得图形更加平滑
        Color col = samplebuffer[y][x].get_pixel_color();
        for (int k = 0; k < 3; ++k) {
          //framebuffer帧缓存是显示屏的缓存，最终用来显示到屏幕上
          framebuffer[3 * (y * width + x) + k] = (&col.r)[k] * 255;
        }
      }
    }
  }


};

} // namespace CGL

#endif // CGL_DRAWREND_H
