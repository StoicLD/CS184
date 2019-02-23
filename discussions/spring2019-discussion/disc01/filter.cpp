#include "filter.h"


Filter::Filter() {
	width = height = 1;
	kernel = {1.f};
}

Filter::Filter(const std::string& filename) {
	read(filename);
}

void Filter::read(std::string filename) {
	// FIXME
    std::ifstream ifs(filename);
    if(ifs.is_open()){
        ifs>>width>>height;
        try {
            int ele;
            kernel.clear();
            for(int i = 0; i < width * height; i++)
            {
                ifs>>ele;
                kernel.insert(kernel.begin(), ele);
            }
        }
        catch(int er)
        {
            std::cout<<"inconsistant of width and height"<<std::endl;
        }
    }
    else{
        std::cout<<"failed to open the file"<<filename<<std::endl;
    }
    normalize();
    ifs.close();
}

void Filter::normalize() {
	// FIXME
    auto iter = kernel.begin();
    float sum = 0;
    while(iter != kernel.end())
    {
        sum += *iter;
        iter++;
    }
    for(int i = 0; i < kernel.size(); i++)
    {
        kernel[i] /= sum;
    }
}

const float& Filter::at(int x, int y) const {
	// FIXME
    if(kernel.size() <= y*width + x || (x < 0) || (y<0))
    {
        std::cout<<"x或者y的输入超过了kernel范围"<<std::endl;
    }
	return kernel[y*width + x];
}

