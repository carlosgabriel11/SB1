#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

//an struct to the EQU diretive
struct EQU{
    //the name of the equivalence
    string name;
    //the value of the equivalence
    int number;
};

//creating a new name...
typedef struct EQU EQU_Diretive;

//this class will have the function to preprocess the source file
class Preprocessor{
    //the pointer of the source file
    FILE* source;
    //the pointer to the new file created to contain
    //the source file after the preprocessing
    FILE* pre;

    //an structure to the EQU diretive
    vector<EQU_Diretive> equ;

    //check if the diretive EQU is present
     size_t findEQU(const string&);
     //check if any reserved word made by the EQU diretive
     size_t findWord(const string&, const string&);

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

//the method to find the position of the diretive EQU
size_t Preprocessor::findEQU(const string& str){
    //the variable to stock the position of EQU
    size_t pos;

    //try to find the position of EQU
    pos = str.find("EQU");

    //if it doesn't find
    if(pos == string::npos){
        return pos;
    }

    //if it find
    else{
        if(str[pos+2] == ' '){
            return pos;
        }
        else{
            return string::npos;
        }
    }
}

//the method to preprocess the source file
void Preprocessor::preprocess(){
    //an auxiliar char variable to get each character of the file
    char aux = 0;
    //an auxiliar string
    string aux_str;
    //an variable to stock the position of ;
    size_t pos;
    //an variavle to stock the position of EQU
    size_t posEQU;
    //a regular counter
    unsigned int counter;

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
            //check if there is the diretive EQU
            posEQU = findEQU(aux_str);

            //if find the diretive EQU
            if(posEQU != string::npos){
                //an auxiliar variable of position
                size_t posAux;
                //an auxiliar char variable to check the initial of the value
                char chAux;
                //an auxiliar struct to put on the vector
                EQU_Diretive equAux;
                //an auxiliar string to carry the value of the equivalence
                string valueStr;

                //find the beginning of the name of the equivalence
                for(counter = (posEQU + 2); ;counter++){
                    chAux = aux_str[counter];

                    if(chAux == ' '){
                        continue;
                    }
                    else{
                        posAux = counter;
                        break;
                    }
                }

                //get the name of the equivalence
                for(counter = posAux; ; counter++){
                    chAux = aux_str[counter];

                    //if the variable is a space
                    if(chAux == ' '){
                        posAux = counter;
                        break;
                    }

                    //put the letter on the auxiliar struct
                    equAux.name.push_back(chAux);
                }

                //find the beginning of the value of the equivalence
                for(counter = posAux; ;counter++){
                    chAux = aux_str[counter];

                    if(chAux == ' '){
                        continue;
                    }
                    else{
                        posAux = counter;
                        break;
                    }
                }

                //get the value of the equivalence
                for(counter = posAux; ; counter++){
                    chAux = aux_str[counter];

                    //if the variable is a space
                    if(chAux == ' '){
                        posAux = counter;
                        break;
                    }

                    //put the letter on the auxiliar struct
                    valueStr.push_back(chAux);
                }

                //put the value of the equivalence into the struct
                equAux.number = stoi(valueStr, nullptr);

                equ.push_back(equAux);
            }

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

            //if the comment in at anywhere
            else{
                //put each character of the string on the file until find the comment
                for(counter = 0; counter < pos; counter++){
                    fputc(aux_str[counter], pre);
                }

                //put the break line on the target file
                fputc('\n', pre);

                //clean the string
                aux_str.clear();

                continue;
            }
        }

    }
}
