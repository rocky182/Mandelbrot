
#ifndef __MANDELBROT__
#define __MANDELBROT__

#include <cstdint>
#include <fstream>
#include <stdio.h>
#include <complex>


using namespace std;

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} rgb_t;

typedef struct{
    rgb_t rgb;
    int threshold;
}mandelbrot_colour_t;

typedef struct{
    unsigned int height;
    unsigned int width;
}resolution_t;

class mandelbrot{
    public:
        mandelbrot(mandelbrot_colour_t unstable = {{255, 255, 255}, 1}, mandelbrot_colour_t stable = {{0, 0, 0}, 0}, mandelbrot_colour_t max = {{0 ,0, 0}, 60});
        mandelbrot(mandelbrot_colour_t * colors, unsigned int numberOfColours);
        void deleteColours(void);
        bool addColour(mandelbrot_colour_t colour);
        bool createColorMatrix( rgb_t * mandColours, 
                                resolution_t res = {1024, 1024}, 
                                complex<double> upperLeft = complex<double>(-2.5 + 2.0i), 
                                complex<double> lowerRight = complex<double>(1.5 - 2.0i));
        bool createPicture( string fname = "mandelbrot.bmp", 
                            resolution_t res = {1024, 1024}, 
                            complex<double> upperLeft = complex<double>(-2.5 + 2.0i), 
                            complex<double> lowerRight = complex<double>(1.5 - 2.0i));
        bool createPicture( rgb_t* colours,
                            string fname = "mandelbrot.bmp",
                            resolution_t res = {1024, 1024}
                            );
        bool createPicture( FILE* fp, 
                            resolution_t res = {1024, 1024}, 
                            complex<double> upperLeft = complex<double>(-2.0 + 2.0i), 
                            complex<double> lowerRight = complex<double>(2.0 - 2.0i),
                            bool ppm = false
                            );
        bool createPicture( rgb_t* colours,
                            FILE* fp,
                            resolution_t res = {1024, 1024},
                            bool ppm = false
                            );
        ~mandelbrot();

    private:
        mandelbrot_colour_t * colours;
        unsigned int colour_numb;

        complex<double> calcPixelValues(complex<double> upperLeftCorner, complex<double> lowerRightCorner, resolution_t resolution);
        void sortColors();
        unsigned int numb2Inf(complex<double> c, unsigned long max);
};

#endif // __MANDELBROT__
