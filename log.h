#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <fstream>
#include <string.h>
 
//Singleton class

class Log
{

private:
    std::string filename;
    std::ofstream logHandler;
    std::string folder;
    
    Log()
    {
        this->folder = "Log/";  
        this->setFile("log.txt");
    
    }
    
    void openStream()
    {
        if(this->logHandler.is_open())
            this->closeStream();
            
        this->logHandler.open(this->filename.c_str());
    }
    void closeStream()
    {
        this->logHandler.close();     
    }
    
      //Setter / Getter
    void setFile(char* filename)
    {
        this->filename = this->folder + std::string(filename);
        this->openStream();
        
    }
    
    
    // Not defined, to prevent copying
    Log(const Log& );
    Log& operator =(const Log& other);

public:
    static Log& getInstance()
    {
        static Log instance;
        
        return instance;
    }
    
    template <typename T> 
    void debug(T text){this->logHandler << text << "\n"; };
 
    // Destructor
    ~Log()
    {
        this->closeStream();
        
    }
    
  
    
    

};



#endif