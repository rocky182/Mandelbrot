#include "..\inc\mandelbrot.hpp"

typedef struct                       /**** BMP file header structure ****/
{
    unsigned int   bfSize;           /* Size of file */
    unsigned short bfReserved1;      /* Reserved */
    unsigned short bfReserved2;      /* ... */
    unsigned int   bfOffBits;        /* Offset to bitmap data */
} BITMAPFILEHEADER;

typedef struct                       /**** BMP file info structure ****/
{
    unsigned int   biSize;           /* Size of info header */
    int            biWidth;          /* Width of image */
    int            biHeight;         /* Height of image */
    unsigned short biPlanes;         /* Number of color planes */
    unsigned short biBitCount;       /* Number of bits per pixel */
    unsigned int   biCompression;    /* Type of compression to use */
    unsigned int   biSizeImage;      /* Size of image data */
    int            biXPelsPerMeter;  /* X pixels per meter */
    int            biYPelsPerMeter;  /* Y pixels per meter */
    unsigned int   biClrUsed;        /* Number of colors used */
    unsigned int   biClrImportant;   /* Number of important colors */
} BITMAPINFOHEADER;


mandelbrot::mandelbrot(mandelbrot_colour_t unstable, mandelbrot_colour_t stable, mandelbrot_colour_t max){
    colour_numb = 0;
    colours = NULL;
    addColour(unstable);
    addColour(stable);
    addColour(max);
}

mandelbrot::mandelbrot(mandelbrot_colour_t * colors, unsigned int numberOfColours){
    colour_numb = 1;
    colours = (mandelbrot_colour_t*) malloc(sizeof(mandelbrot_colour_t));
    colours[0] = colors[0];
    for(int i = 1; i < numberOfColours; i++){
        addColour(colors[i]);
    }
}

void mandelbrot::deleteColours(void){
    free(colours);
    colour_numb = 0;
    colours = NULL;
}

bool mandelbrot::addColour(mandelbrot_colour_t colour){
    mandelbrot_colour_t * colours_buf;
    if(colours == NULL){
        colours_buf = (mandelbrot_colour_t*) malloc(sizeof(mandelbrot_colour_t));
    }else{
        colours_buf = (mandelbrot_colour_t*) realloc(colours, (colour_numb + 1) * sizeof(mandelbrot_colour_t));
    }

    if (colours_buf == NULL){
        return 1;
    }
    colours = colours_buf;
    colours[colour_numb] = colour;
    colour_numb += 1;
    sortColors();
    return 0;
}

bool mandelbrot::createColorMatrix(rgb_t * mandColours, resolution_t res , complex<double> upperLeft , complex<double> lowerRight){
    if(mandColours==NULL){
        return 1;;
    }

    unsigned int i;
    int val;
    complex<double> pixVal = calcPixelValues(upperLeft, lowerRight, res);
    complex<double> c;
    
    for(unsigned int h = 0; h < res.height; h++){
        for(unsigned int w = 0; w < res.width; w++){
            c = complex<double>((real(upperLeft) + (double)w * real(pixVal)), (imag(upperLeft) - (double)h * imag(pixVal)));
            val = numb2Inf(c, colours[colour_numb-1].threshold);
            for(i = colour_numb-1; (i > 0) && (colours[i].threshold > val); i -= 1);
            
            mandColours[h*res.width + w] = colours[i].rgb;
        }
    }
    return 0;
} 

bool mandelbrot::createPicture(string fname, resolution_t res, complex<double> upperLeft, complex<double> lowerRight){
    rgb_t * mandColours = (rgb_t *) malloc(sizeof(rgb_t)*res.height*res.width);
    if(createColorMatrix(mandColours, res, upperLeft, lowerRight)){
        return 1;
    }

    if(createPicture(mandColours, fname, res)){
        return 1;
    }

    free(mandColours);
    return 0;
}

bool mandelbrot::createPicture(rgb_t* mandColours, string fname, resolution_t res){
    if(mandColours == NULL){
        return 1;
    }

    int len = fname.length();
    bool ppm = false;

    if(len > 3){
        if(fname.c_str()[len-4] == '.' && fname.c_str()[len-3] == 'p' && fname.c_str()[len-2] == 'p' && fname.c_str()[len-1] == 'm'){
            ppm = true;
        }else if (!(fname.c_str()[len-4] == '.' && fname.c_str()[len-3] == 'b' && fname.c_str()[len-2] == 'm' && fname.c_str()[len-1] == 'p')){
            fname.append(".bmp");
        }
    }else {
        fname.append(".bmp");
    }

    FILE* fp = fopen(fname.c_str(),"wb");
    bool ret = 0;
    
    if (createPicture(mandColours, fp, res, ppm)){
        ret = 1;
    }

    fclose(fp);
    return ret;
}

bool mandelbrot::createPicture(FILE* fp, resolution_t res, complex<double> upperLeft, complex<double> lowerRight, bool ppm){
    rgb_t * mandColours = (rgb_t *) malloc(sizeof(rgb_t)*res.height*res.width);
    if(createColorMatrix(mandColours, res, upperLeft, lowerRight)){
        return 1;
    }

    if(createPicture(mandColours, fp, res, ppm)){
        return 1;
    }

    free(mandColours);
    return 0;
}

bool mandelbrot::createPicture(rgb_t* mandColours, FILE* fp, resolution_t res, bool ppm){
    if(mandColours == NULL || fp == NULL){
        return 1;
    }
    
    if(ppm){
        fprintf(fp,"P6\n");               // P6 filetype
        fprintf(fp, "%d %d\n", res.width, res.height);   // dimensions
        fprintf(fp,"255\n");              // Max pixel
    }else{
        unsigned short bmpType=0x4d42;
        BITMAPFILEHEADER bmpHeader;
        BITMAPINFOHEADER bmpInfoHeader;

        bmpHeader.bfReserved1 = 0;
        bmpHeader.bfReserved2 = 0;
        bmpHeader.bfSize = 2 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + res.width * res.height * 3;
        bmpHeader.bfOffBits = 0x36;

        bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfoHeader.biWidth = res.width;
        bmpInfoHeader.biHeight = res.height;
        bmpInfoHeader.biPlanes = 1;
        bmpInfoHeader.biBitCount = 24;
        bmpInfoHeader.biCompression = 0;
        bmpInfoHeader.biSizeImage = 0;
        bmpInfoHeader.biXPelsPerMeter = 5000;
        bmpInfoHeader.biYPelsPerMeter = 5000;
        bmpInfoHeader.biClrUsed = 0;
        bmpInfoHeader.biClrImportant = 0;

        
        fwrite(&bmpType,1,sizeof(bmpType),fp);
        fwrite(&bmpHeader, 1, sizeof(bmpHeader), fp);
        fwrite(&bmpInfoHeader, 1, sizeof(bmpInfoHeader), fp);
    }

    if(ppm){
        fwrite(mandColours, res.width*res.height, 3, fp);
    }else{
        unsigned char padding[] = {0, 0, 0};

        for(int i = 0; i < res.height; i++){
            for(int c = 0; c < res.width; c++){
                fwrite(&mandColours[i*res.width + c].blue, 1, 1, fp);
                fwrite(&mandColours[i*res.width + c].green, 1, 1, fp);
                fwrite(&mandColours[i*res.width + c].red, 1, 1, fp);
            }
            // fwrite(&padding, sizeof(padding), 1, fp);
        }
    }
    return 0;
}

mandelbrot::~mandelbrot(){
    deleteColours();
}

complex<double> mandelbrot::calcPixelValues(complex<double> upperLeftCorner, complex<double> lowerRightCorner, resolution_t resolution){
        double height = (imag(upperLeftCorner) - imag(lowerRightCorner)) / resolution.height;
        double width  = (real(lowerRightCorner) - real(upperLeftCorner)) / resolution.width;
        return {width, height};
}

void mandelbrot::sortColors(){
    mandelbrot_colour_t colour ;
    bool changed;
    do{
        changed = 0;
        for(unsigned int i = 1; i < colour_numb; i += 1){
            if(colours[i-1].threshold > colours[i].threshold){
                changed = 1;
                colour = colours[i-1];
                colours[i-1] = colours[i];
                colours[i] = colour;
            }
        }
    }while(changed);
}

unsigned int mandelbrot::numb2Inf(complex<double> c, unsigned long max){
    unsigned int cnt = 0;
    complex<double> z = complex<double>(0.0 + 0.0i) + c;
    double zabs = abs(z);

    for(; (cnt < max) && (zabs != INFINITY) && (zabs > 0.0001); zabs = abs(z), cnt++){
        z = z * z + c;
    }

    return cnt;
}
