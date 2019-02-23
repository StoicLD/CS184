#include "image.h"
#include "lodepng.h"


Image::Image() {
	width = height = 0;
}

Image::Image(const std::string& filename) {
	read(filename);
}

Image::Image(const unsigned& w, const unsigned& h)
: width(w), height(h), data(4 * w * h) {

}

void Image::read(std::string filename) {
	// FIXME
	data.clear();
	lodepng::decode(data, width, height, filename);
}

void Image::write(std::string filename) const {
	// FIXME
	//最后两个变量都是有默认参数的，所以只要传入四个参数就可以了
	lodepng::encode(filename, data, width, height);
}

uint8_t* Image::at(int x, int y) {
	// FIXME
	if(x < 0 || y < 0 || 4 * (y * width + x) >= data.size())
    {
	    std::cout<<"参数规格越界"<<std::endl;
        return 0;
    }
    //return &data[4 * (y * width + x)];
    return &data[0] + 4 * (y * width + x);
}

Image Image::operator*(const Filter& filter) {
	// FIXME
    Image tmp(width, height);
    //一种解法，相对中心位置处于filter的左上角也就是第一个坐标
    int image_size = tmp.height * tmp.width * 4;
    for(int image_y = 0; image_y < tmp.height; image_y++)
    {
        for(int image_x = 0; image_x < tmp.width; image_x++)
        {
            //四通道注意！！！
            float sums[4] = {0, 0, 0, 0};
            int current_axes = *at(image_x, image_y);
            for(int box_y = 0; box_y < filter.height; box_y++)
            {
                for(int box_x = 0; box_x < filter.width; box_x++)
                {
                    //改进之后，以当前旧图片坐标为中心点，filter大小的盒子向四周累加
                    int xi = image_x + box_x - filter.width/2;
                    int yi = image_y + box_y - filter.height/2;
                    if(xi < 0 || xi > tmp.width || yi < 0 || yi > tmp.height)
                        continue;
                    int current_box_axes = *tmp.at(xi, yi);
                    for(int i = 0; i < 4; i++)
                    {
                        if (current_box_axes + i < image_size)
                        {
                            sums[i] += filter.at(box_x, box_y) * (*at(xi, yi));
                        }
                    }
                }
            }
            for(int j = 0; j < 4; j++)
            {
                if(current_axes + j < image_size)
                    tmp.data[current_axes + j] = (unsigned char)sums[j];
            }

        }
    }
    return tmp;
}