#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

//this class will have the function to preprocess the source file
class Preprocessor{
    //the pointer of the source file
    FILE* source;
    //the pointer to the new file created to contain
    //the source file after the preprocessing
    FILE* pre;

public:
    //the constructor to open the file
    Preprocessor(const string&);
    //the destructor to close the file
    ~Preprocessor();
    //remove comments of the file
    void removeComments();

};

//start of the main function
int main(int argc, char** argv){
    Preprocessor *pre = new Preprocessor("teste.asm");

    return 0;
}

/*
DEFINITION OF THE METHODS OF THE CLASS Preprocessor
*/
//the method to open the source file in the read only and to create the preprocessed file
Preprocessor::Preprocessor(const string& name){
    //the name of the preprocessed file
    string preprocessName;
    //the position of . in the source file
    size_t pos;
    //a regular counter
    unsigned int counter;

    //get the position of the .
    pos = name.find('.');

    //create the name of the preprocessed file
    for(counter = 0; counter < pos; counter++){
        preprocessName.push_back(name[counter]);
    }

    preprocessName = preprocessName + ".pre";


    source = fopen(name.c_str(), "r");
    pre = fopen(preprocessName.c_str(), "w");
}
