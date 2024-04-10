# Project: unfavbs
# Makefile created by Embarcadero Dev-C++ 6.3

CPP      = g++.exe
CC       = gcc.exe
WINDRES  = windres.exe
OBJ      = unfavbs.o
LINKOBJ  = unfavbs.o
LIBS     = -L"C:/env/mingw/lib" -L"C:/env/mingw/x86_64-w64-mingw32/lib" -static-libgcc -L"C:/env/mingw/OpenCV/x64/mingw/lib" -L"C:/env/mingw/zlib/build" C:/env/mingw/zlib/build/libzlib.dll.a C:/env/mingw/zlib/build/libzlibstatic.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_calib3d452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_core452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_dnn452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_features2d452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_flann452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_gapi452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_highgui452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_imgcodecs452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_imgproc452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_ml452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_objdetect452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_photo452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_stitching452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_video452.dll.a C:/env/mingw/OpenCV/x64/mingw/lib/libopencv_videoio452.dll.a
INCS     = -I"C:/env/mingw/include" -I"C:/env/mingw/x86_64-w64-mingw32/include" -I"C:/env/mingw/lib/gcc/x86_64-w64-mingw32/8.1.0/include" -I"C:/env/mingw/OpenCV/include" -I"C:/env/mingw/zlib/build" -I"C:/env/mingw/zlib"
CXXINCS  = -I"C:/env/mingw/include" -I"C:/env/mingw/x86_64-w64-mingw32/include" -I"C:/env/mingw/lib/gcc/x86_64-w64-mingw32/8.1.0/include" -I"C:/env/mingw/lib/gcc/x86_64-w64-mingw32/8.1.0/include/c++" -I"C:/env/mingw/OpenCV/include" -I"C:/env/mingw/zlib/build" -I"C:/env/mingw/zlib"
BIN      = unfavbs.exe
CXXFLAGS = $(CXXINCS) 
CFLAGS   = $(INCS) 
DEL      = C:\Program Files (x86)\Dev-Cpp\DevCpp.exe INTERNAL_DEL

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${DEL} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

unfavbs.o: unfavbs.cpp
	$(CPP) -c unfavbs.cpp -o unfavbs.o $(CXXFLAGS)
