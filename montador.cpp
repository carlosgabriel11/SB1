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
    //the method to preprocess the source file
    void preprocess();

};

//start of the main function
int main(int argc, char** argv){
    Preprocessor *pre = new Preprocessor("triangulo.asm");

    pre->preprocess();

    delete pre;

    return 0;
}

/*
DEFINITION OF THE METHODS OF THE CLASS Preprocessor
*/
//the constructor that opens the source file in the read only and to create the preprocessed file
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

//the destructor that have the function to close the files
Preprocessor::~Preprocessor(){
    fclose(source);
    fclose(pre);
}

//the method to preprocess the source file
void Preprocessor::preprocess(){
    //an auxiliar char variable to get each character of the file
    char aux = 0;
    //an auxiliar string
    string aux_str;
    //an variable to stock the position of ;
    size_t pos;

    //read each character of the file
    while(aux != EOF){
        //read each character
        aux = fgetc(source);
        aux_str.push_back(aux);

        //if it is in the end of the file, leave the loop
        if(aux == EOF){
            break;
        }

        if(aux == '\n'){

            //check the position of the comments
            pos = aux_str.find(';');

            //if it doesn't have comments
            if(pos == string::npos){
                //put the string in the preprocessed file
                fputs(aux_str.c_str(), pre);
                //clean the string
                aux_str.clear();

                continue;
            }

            //if the comment is in the beginning of the line
            else if(pos == 0){
                //clean the string
                aux_str.clear();

                continue;
            }
        }

    }
}
