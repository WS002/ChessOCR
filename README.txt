Compile with g++ .\main.cpp .\image.cpp ocr.cpp -lgdi32 -std=c++0x -w

-lgdi32: 	Links gdi32 library
-w:	 	Ignores warnings
-std=c++0x:	Needed for unordered map


main is compiled and executed -> on button click -> instantiate image(image.h) class -> take screenshot (image.h), save to BMP image (image.h), read pixel data(image.h) -> call OCR (ocr.h) algorithm to parse the data -> ??? -> profit!!
