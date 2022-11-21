#include <iostream>
#include <string.h>
#include <thread>
#include "inc\mandelbrot.hpp"

using namespace std;

#define RESOLUTION_WIDTH            (1024)
#define RESOLUTION_HEIGHT           (1024)
#define NUMBER_OF_THREADS           8
#define ARGLEN                      6

string info =   "Mandelbrot Picture Generator\n\n";

string argList[] = {
    "-?",
    "-u",
    "-l",
    "-r",
    "-t",
    "-c"
};

typedef struct{
    mandelbrot* mb;
    rgb_t * mandColours;
    resolution_t res;
    complex<double> upperLeft;
    complex<double> lowerRight;
}foo_t;

void foo(foo_t f){
    f.mb->createColorMatrix(f.mandColours, f.res, f.upperLeft, f.lowerRight);
}

bool startThreads(unsigned int numbOfThreads, mandelbrot* mb, rgb_t * colors, resolution_t * res, complex<double> upperLeft, complex<double> lowerRight){
    res->height = res->height / numbOfThreads;
    res->height = res->height * numbOfThreads;
    
    foo_t * f = (foo_t *)malloc(sizeof(foo_t)* numbOfThreads);

    if(f==NULL || mb == NULL || colors == NULL || numbOfThreads == 0){
        return 1;
    }
    
    for(unsigned int i = 0; i < numbOfThreads; i++){
        f[i] = {.mb = mb, 
                .mandColours = &colors[(res->width * (res->height / numbOfThreads) * i)], 
                .res = {res->height / numbOfThreads, res->width}, 
                .upperLeft  = complex<double>(real(upperLeft),  imag(upperLeft) - (imag(upperLeft) - imag(lowerRight)) / ((double)numbOfThreads) *  (double)i), 
                .lowerRight = complex<double>(real(lowerRight), imag(upperLeft) - (imag(upperLeft) - imag(lowerRight)) / ((double)numbOfThreads) *  ((double)(i + 1)))
                };
    };

    thread th[numbOfThreads];

    for(unsigned int i = 0; i < numbOfThreads; i++){
        th[i] = thread(foo, f[i]);
    }

    for(unsigned int i = 0; i < numbOfThreads; i++){
        th[i].join();
        th[i].~thread();
    }

    free(f); 

    return 0;
}

bool str2double(string str, double * val){
    const char * str_buf = str.c_str();
    unsigned int strlen = str.length();
    double val_buf = 1;
    double val_buf1 = 0;
    double val_buf2 = 0;

    bool point = false;
    unsigned int c = 0;
    unsigned int c_point = 0;

    if (str_buf[c] == '-'){
        c += 1;
        val_buf = -1;
    }

    for(; c < strlen; c++){
        if(((str_buf[c] < 48) || (str_buf[c] > 57)) && (str_buf[c] != '.')){
            return 1;
        }else if(str_buf[c] == '.'){
            point = true;
            c_point = c; // 235.123
        }else{
            if(!point){
                val_buf1 = val_buf1 * 10 + (double)(str_buf[c] - 48);
            } else{
                val_buf2 = val_buf2 * 0.1 + ((double)(str_buf[strlen - (c - c_point)] - 48))*0.1;
            }
        }
    }

    val_buf *= (val_buf1 + val_buf2);

    *val = val_buf;

    return 0;
}

bool str2uint(string str, unsigned int * val){
    const char * str_buf = str.c_str();
    unsigned int strlen = str.length();
    unsigned int val_buf = 0;

    for(unsigned int c = 0; c < strlen; c++){
        if(((str_buf[c] < 48) || (str_buf[c] > 57))){
            return 1;
        }else{
            val_buf = val_buf * 10 + (str_buf[c] - 48);
        }
    }

    *val = val_buf;

    return 0;
}

int main(int argc, char ** argv){
    info += "examples:\n";
    info += "mandelbrot.exe picture.bmp\n";
    info += "mandelbrot.exe -u -1.5 1.0 -l 0.5 -1.0\n";
    info += "mandelbrot.exe -r 600 800\n";
    info += "mandelbrot.exe -t 4\n";
    info += "mandelbrot.exe -c 255 255 255 1 0 0 0 0 255 255 255 100\n\n";
    info += "\t\tPicture file name. (default = \"mandelbrot.bmp\")\n";
    info += "\t-?\thelp\\info\n";
    info += "\t-u\tUpper left corner in  the graph, which is a complex number. (default = -2.0 + 1.25i)\n";
    info += "\t-l\tLower right corner in  the graph, which is a complex number. (default = 0.5 - 1.25i)\n";
    info += "\t-r\tResolution of the picture. (default = {h:1024 w:1024})\n";
    info += "\t-t\tNumber of threads started to generate the picture. (default = 8)\n";
    info += "\t-c\tList of colours with their threshold value. The colours are 8 bit.\n\t\t(default = {{red=0, green=0, blue=0, threshold=0},{red=255, green=255, blue=255, threshold=1}{red=0, green=0, blue=0, threshold=60}})\n\n";

    typedef struct {
        mandelbrot mb;
        string fname = "mandelbrot";
        complex<double> upperLeft = (-2.0 + 1.25i);
        complex<double> lowerRight = (0.5 - 1.25i);
        resolution_t res = {RESOLUTION_HEIGHT, RESOLUTION_WIDTH};
        unsigned numbOfThreads = NUMBER_OF_THREADS;
        rgb_t * colors;
    }mandelbrot_config_t;

    mandelbrot_config_t mbConf;

    if(argc > 1){
        int c = 0;
        double imag = 0;
        double real = 0;

        for(int i = 1; i < argc; i++, c = -2){
            if(argv[i][0] == '-'){
                c = 0;
                for(; (c < ARGLEN) && (argv[i][1] != argList[c][1]); c++);

                if((argv[i][1] != argList[c][1])){
                    c++;
                }

            }else if (i == 1){
                c = -1;
            }
            
            switch (c)
            {
            case -1:
                mbConf.fname = argv[i];
                break;

            case 0:
                cout << info.c_str();
                return 0;
                
            case 1:
                i++;
                if (i >= argc){
                    cout << "Too few arguments in \"-u\" (#1).\n";
                    return 1;
                }


                if(str2double((const char *)argv[i], &real)){
                    cout << "Wrong first argument in \"-u\".\n";
                    return 1;
                }

                i++;

                if (i >= argc){
                    cout << "Too few arguments in \"-u\" (#2).\n";
                    return 1;
                }

                if(str2double((const char *)argv[i], &imag)){
                    cout << "Wrong second argument in \"-u\".\n";
                    return 1;
                }

                mbConf.upperLeft = complex<double>(real, imag);
                break;
                
            case 2:
                i++;
                if (i >= argc){
                    cout << "Too few arguments in \"-l\".\n";
                    return 1;
                }

                if(str2double((const char *)argv[i], &real)){
                    cout << "Wrong first argument in \"-l\".\n";
                    return 1;
                }

                i++;

                if (i >= argc){
                    cout << "Too few arguments in \"-l\".\n";
                    return 1;
                }

                if(str2double((const char *)argv[i], &imag)){
                    cout << "Wrong second argument in \"-l\".\n";
                    return 1;
                }

                mbConf.lowerRight = complex<double>(real, imag);
                break;
                
            case 3:
                i++;
                if (i >= argc){
                    cout << "Too few arguments in \"-r\".\n";
                    return 1;
                }

                if(str2uint((const char *)argv[i], &mbConf.res.height)){
                    cout << "Wrong first argument in \"-r\".\n";
                    return 1;
                }

                i++;

                if (i >= argc){
                    cout << "Too few arguments in \"-r\".\n";
                    return 1;
                }

                if(str2uint((const char *)argv[i], &mbConf.res.width)){
                    cout << "Wrong second argument in \"-r\".\n";
                    return 1;
                }
                
                break;
                
            case 4:
                i++;

                if (i >= argc){
                    cout << "Too few arguments in \"-t\".\n";
                    return 1;
                }

                if(str2uint((const char *)argv[i], &mbConf.numbOfThreads)){
                    cout << "Wrong argument in \"-t\".\n";
                    return 1;
                }
                break;
                
            case 5:
                mandelbrot_colour_t color_buf;
                i += 1;

                mbConf.mb.deleteColours();

                while((i < (argc - 3)) && (argv[i][0] > 47 && argv[i][0] < 58)){
                    if(str2uint((const char *)argv[i++], (unsigned int *) &(color_buf.rgb.red))){
                        cout << "Wrong argument in \"-c\".\n";
                        return 1;
                    }
                    if(str2uint((const char *)argv[i++], (unsigned int *) &(color_buf.rgb.green))){
                        cout << "Wrong argument in \"-c\".\n";
                        return 1;
                    }
                    if(str2uint((const char *)argv[i++], (unsigned int *) &(color_buf.rgb.blue))){
                        cout << "Wrong argument in \"-c\".\n";
                        return 1;
                    }
                    if(str2uint((const char *)argv[i++], (unsigned int *) &color_buf.threshold)){
                        cout << "Wrong argument in \"-c\".\n";
                        return 1;
                    }
                    if(mbConf.mb.addColour(color_buf)){
                        cout << "Wrong argument in \"-c\".\n";
                        return 1;
                    }
                }

                i--;

                break;
            
            default:
                cout << "Wrong input!\n\n";
                cout << info;
                return 1;
            }
        }
    }
    
    mbConf.colors = (rgb_t *) malloc(sizeof(rgb_t) * mbConf.res.height * mbConf.res.width);

    if(mbConf.colors == NULL){
        cout << "Couldn't initialize the color matrix!\n";
        return 2;
    }
    cout << "Colour matrix is allocated!\n";
    cout << "Starting Threads!\n";

    if(startThreads(mbConf.numbOfThreads, &mbConf.mb, mbConf.colors, &mbConf.res, mbConf.upperLeft, mbConf.lowerRight)){
        cout << "Couldn't start multithreading!\n";
        free(mbConf.colors);
        return 3;
    }

    cout << "Creating Picture!\n";
    mbConf.mb.createPicture(mbConf.colors, mbConf.fname, mbConf.res);

    cout << "Picture created!\n";
    free(mbConf.colors);
    return 0;
}
