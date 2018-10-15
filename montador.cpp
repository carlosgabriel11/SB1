#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

//an struct to the EQU diretive
struct EQU{
    //the name of the equivalence
    string name;
    //the value of the equivalence
    string number;
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

    //the string that contains the name of the preprocessing file name
    string preName;

    //an structure to the EQU diretive
    vector<EQU_Diretive> equ;

    //check if the word is present in the line
    size_t findWord(const string&, const string&);
    //store the informations of the EQU into the struct
    void storeEQUInfo(const size_t&, const string&);
    //substitute a name that exists in the vector by its value
    string substituteEQU(const string&);
    //check the value of the IF directive
    bool checkIF(const string&, const size_t&);

public:
    //the constructor to open the file
    Preprocessor(const string&);
    //the destructor to close the file
    ~Preprocessor();
    //get the preprocessing file name
    string getPreprocessingName();
    //the method to preprocess the source file
    void preprocess();

};

//this class will be the symbol table of the first passage of the mounter
class SymbolTable{
    //the label
    vector<string> label;
    //the address
    vector<int> address;
    //if the label is extern
    vector<bool> isExtern;

public:
    //set the attributes of this class
    void setSymbol(const string&, const int&, const bool&) throw(invalid_argument);
    //get the address
    int getAddress(const string&) throw(invalid_argument);
    //get the information if the symbol is extern
    bool getExtern(const string&) throw(invalid_argument);
};


//this class will be the definition table of the first passage of the mounter
class DefinitionTable{
    //the public labels
    vector<string> label;
    //the address of the public labels
    vector<int> address;

public:
    //set the label name
    void setLabel(const string&) throw(invalid_argument);
    //set the address of the label
    void setAddress(const string&, const int&);
    //get the address of the label
    int getAddress(const string&) throw(invalid_argument);
};

//this class will be the instruction table

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

    //get the preprocessing name
    preName = preprocessName;

    source = fopen(name.c_str(), "r");
    pre = fopen(preprocessName.c_str(), "w");
}

//the destructor that have the function to close the files
Preprocessor::~Preprocessor(){
    fclose(source);
    fclose(pre);
}

//the method to check if there is any word in the string
size_t Preprocessor::findWord(const string& line, const string& word){
    //the variable to receive the position of the word in the line
    size_t pos;

    //check if the word in the line
    pos = line.find(word);

    //return npos if the word isn't in the string
    if(pos == string::npos){
        return pos;
    }
    //check if there is space after the word
    else{
        if((line[pos + word.size()] == ' ') || (line[pos + word.size()] == '\n')){
            return pos;
        }
        else{
            return string::npos;
        }
    }

}

//the method to store the informations into the struct
void Preprocessor::storeEQUInfo(const size_t& pos, const string& line){
    //the string to receive the name of the equivalence
    string auxName;
    //the value of the equivalence
    string auxValue;
    //a regular counter
    unsigned int counter;
    //the position of the name
    size_t posName;
    //create an auxiliar struct to put into the vector
    EQU_Diretive equAux;

    //a loop to get the position of the name
    for(counter = pos - 3; ; counter--){
        //leave the loop if the character in the line is a space
        if(line[counter] == ' ' || line[counter] == '\t'){
            posName = counter + 1;
            break;
        }
        //leave the loop if the value of counter is 0
        if(counter == 0){
            posName = 0;
            break;
        }
    }

    //a loop to get the name
    for(counter = posName; ; counter++){
        //leave the loop when find a :
        if(line[counter] == ':'){
            break;
        }

        //put each character in the string of the name
        auxName.push_back(line[counter]);
    }

    //a loop to get the value
    for(counter = pos + 4; counter < line.size() - 1; counter++){
        auxValue.push_back(line[counter]);
    }

    //put the name and the value into the auxiliar struct
    equAux.name = auxName;
    equAux.number = auxValue;

    //put the auxiliar string into the vector
    equ.push_back(equAux);
}

//a method to substitute the name in the line by its value
string Preprocessor::substituteEQU(const string& line){
    //an auxiliar string
    string auxStr = line;
    //a variable that will be returned with the name substituted
    string substitute;
    //regular counters
    unsigned int counter1, counter2;
    //an auxiliar position variable
    size_t auxPos;

    //a loop to check each value in the struct
    for(counter1 = 0; counter1 < equ.size(); counter1++){
        auxPos = findWord(line, equ[counter1].name);

        //if the word isn't in the vector
        if(auxPos == string::npos){
            continue;
        }

        //creating a new string
        for(counter2 = 0; counter2 < auxPos; counter2++){
            substitute.push_back(auxStr[counter2]);
        }

        for(counter2 = 0; counter2 < equ[counter1].number.size(); counter2++){
            substitute.push_back(equ[counter1].number[counter2]);
        }

        for(counter2 = auxPos + equ[counter1].name.size(); counter2 < auxStr.size(); counter2++){
            substitute.push_back(auxStr[counter2]);
        }

        auxStr = substitute;
    }

    return auxStr;
}

//the method to check the value of the IF directive
bool Preprocessor::checkIF(const string& line, const size_t& position){
    //a regular counter
    unsigned int counter;
    //get the position of the values
    size_t valuePos;
    //the string of the values
    string value;

    //get the position of the value
    for(counter = position + 2; ; counter++){
        if(line[counter] != ' '){
            valuePos = counter;
            break;
        }
    }

    //get the value
    for(counter = valuePos; ; counter++){
        if((line[counter] == ' ') || (line[counter] == '\n')){
            break;
        }

        value.push_back(line[counter]);
    }

    //if the value is 1, return true
    if(stoi(value) == 1){

        return true;
    }
    else{
        return false;
    }
}

//the method to return the name of the preprocessing file
string Preprocessor::getPreprocessingName(){
    return preName;
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
    //an variable to stock the postion of the IF
    size_t posIF;
    //a regular counter
    unsigned int counter;
    //an state to check if the line is going to be written in the file
    bool state = true;
    //an state of the IF directive
    bool stateIF = true;

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
            //check if there is the EQU diretive
            posEQU = findWord(aux_str, "EQU");

            //store the name and value into the EQU vector
            if(posEQU != string::npos){
                storeEQUInfo(posEQU, aux_str);
                state = false;
            }

            if(posEQU == string::npos){
                aux_str = substituteEQU(aux_str);
            }

            //check if there is an IF directive
            posIF = findWord(aux_str, "IF");

            if(posIF != string::npos){
                state = false;
                stateIF = checkIF(aux_str, posIF);
                //if the stateIF is false, the next line can't be on the preprocessing
                //so, when counter is 0 and stateIF is false, the next line is cleaned
                counter = 0;
            }

            if(!stateIF){
                if(counter == 2){
                    stateIF = true;
                }
                else{
                    counter++;
                }
            }

            if(state && stateIF){
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
            aux_str.clear();
            state = true;
        }
    }
}

/*
DEFINITION OF THE METHODS OF THE CLASS SymbolTable
*/
//the method to set the symbols in the symbol table
void SymbolTable::setSymbol(const string& str, const int& position, const bool& status) throw(invalid_argument){
    //a regular counter
    unsigned int counter;

    //©heck if the symbol is already on the table
    for(counter = 0; counter < label.size(); counter++){
        if(label[counter] == str){
            throw invalid_argument("ROTULO REPETIDO");
        }
    }

    label.push_back(str);
    address.push_back(position);
    isExtern.push_back(status);
}

//the method to get the address of any label
int SymbolTable::getAddress(const string& str) throw(invalid_argument){
    //a regular counter
    unsigned int counter;

    //a loop to search the label into the vector
    for(counter = 0; counter < label.size(); counter++){
        if(label[counter] == str){
            return address[counter];
        }
    }

    throw invalid_argument("ROTULO INEXISTENTE");
}

//the method to get if the label is extern or not
bool SymbolTable::getExtern(const string& str) throw(invalid_argument){
    //a regular counter
    unsigned int counter;

    //a loop to search a label into the vector
    for(counter = 0; counter < label.size(); counter++){
        if(label[counter] == str){
            return isExtern[counter];
        }
    }

    //if the label isn't in the vector
    throw invalid_argument("ROTULO INEXISTENTE");
}

/*
DEFINITION AND METHODS OF THE CLASS DefinitionTable
*/
//the method to put the label into the definition table
void DefinitionTable::setLabel(const string& str) throw(invalid_argument){
    //a regular counter
    unsigned int counter;

    //check if this label is already on the table
    for(counter = 0; counter < label.size(); counter++){
        if(label[counter] == str){
            throw invalid_argument("REPETICAO DE DIRETIVA PUBLICA PARA O MESMO ROTULO");
        }
    }

    //put on the vector
    label.push_back(str);
    address.push_back(0);
}

//the method to put the address into the definition table
void DefinitionTable::setAddress(const string& str, const int& number){
    //a regular counter
    unsigned int counter;

    //get the position of the label
    for(counter = 0; counter < label.size(); counter++){
        if(label[counter] == str){
            address[counter] = number;
            return;
        }
    }
}

//the method to get the address of the label in the definition table
int DefinitionTable::getAddress(const string& str) throw(invalid_argument){
    //a regular counter
    unsigned int counter;

    //get the position of the label
    for(counter = 0; counter < label.size(); counter++){
        if(label[counter] == str){
            return address[counter];
        }
    }

    throw invalid_argument("ROTULO NAO ENCONTRADO NA TABELA DE DEFINICAO");
}
