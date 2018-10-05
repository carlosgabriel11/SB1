#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

//this class will have the function to preprocess the target file
class Preprocessor{
    //the pointer of the target file
    FILE* target;
    //the pointer to the new file created to contain
    //the target after the preprocessing
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

    return 0;
}
