Compile with g++ .\main.cpp .\image.cpp ocr.cpp -lgdi32 -w -std=c++11

-lgdi32: 	Links gdi32 library
-w:	 	Ignores warnings
-std=c++11:	For unordered map library



main is compiled and executed -> on button click -> instantiate image(image.h) class -> take screenshot (image.h), save to BMP image (image.h), read pixel data(image.h) -> call OCR (ocr.h) algorithm to parse the data -> ??? -> profit!!
