# Mandelbrot Picture Generator
This simple program generates mandelbrot sets. The user can definine different arguments by passing them to the executable file.

## Generating executable
```
g++ -g -o mandelbrot.exe main.cpp src\mandelbrot.cpp
```

## Arguments
[executable] [picturename] [[option1] [arguments1] [option2] [arguments2] ...]<br>

mandelbrot.exe pic.bmp -u -2.0 1.75 -l 1.5 -1.75 -r 100 100 -c 255 255 255 1 0 0 0 0 255 255 255 60 -t 4
### Upper left point [-u]
The upper left point is a complex number with two arguments which have the datatype double. 
The first one is the real and the second one is the imaginary number. <br>
mandelbrot.exe -u -2.0 1.75 

### Lower right point [-l]
Same as the upper left point. <br>
mandelbrot.exe -l 2.0 -1.75 

### Resolution [-r]
The resolution has two arguments with unsigned numbers. 
The first one defines the height and the second one defines the width. <br>
mandelbrot.exe -r 100 100 

### Number of threads to start [-t]
The number of threads has only one unsigned argument. <br>
mandelbrot.exe -t 2

### Colour List [-c]
All of the numbers passed have to be unsigned integers.<br>
```
mandelbrot.exe -c [red] [green] [blue] [threshold]<br><br>
```
If i want to pass more than one colours I can do that like described below. <br>
```
mandelbrot.exe -c [colour1 red] [colour1 green] [colour1 blue] [colour1 threshold] [colour2 red] [colour2 green] [colour2 blue] [colour2 threshold] ... [colourN red] [colourN green] [colourN blue] [colourN threshold]
```

### Help [-?]

```mandelbrot.exe -?
Mandelbrot Picture Generator

examples:
mandelbrot.exe picture.bmp
mandelbrot.exe -u -1.5 1.0 -l 0.5 -1.0
mandelbrot.exe -t 4
mandelbrot.exe -c 255 255 255 1 0 0 0 0 255 255 255 100

                Picture file name. (default = "mandelbrot.bmp")
        -?      help\info
        -u      Upper left corner in  the graph, which is a complex number. (default = -2.0 + 1.25i)
        -l      Lower right corner in  the graph, which is a complex number. (default = 0.5 - 1.25i)
        -r      Resolution of the picture. (default = {h:1024 w:1024})
        -t      Number of threads started to generate the picture. (default = 8)
        -c      List of colours with their threshold value. The colours are 8 bit.
                (default = {{red=0, green=0, blue=0, threshold=0},{red=255, green=255, blue=255, threshold=1}{red=0, green=0, blue=0, threshold=60}})
```