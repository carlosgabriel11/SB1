#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>

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
    //the values of label
    vector<int> values;
    //the type of the label
    vector<string> type;

public:
    //set the attributes of this class
    void setSymbol(const string&, const int&, const bool&, const int&, const string&) throw(invalid_argument);
    //get the address
    int getAddress(const string&) throw(invalid_argument);
    //get the information if the symbol is extern
    bool getExtern(const string&) throw(invalid_argument);
    //get the value of the symbol
    int getValue(const string&) throw(invalid_argument);
    //get the type of the symbol
    string getType(const string&) throw(invalid_argument);
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
    //get the list of labels in the definition table
    vector<string> getLabel();
};

//this class will be the instruction table
class InstructionTable{
    //the add operation
    const static string ADD;
    //the sub operation
    const static string SUB;
    //the mult operation
    const static string MULT;
    //the div operation
    const static string DIV;
    //the jmp operation
    const static string JMP;
    //the jmpn operation
    const static string JMPN;
    //the jmpp operation
    const static string JMPP;
    //the jmpz operation
    const static string JMPZ;
    //the copy operation
    const static string COPY;
    //the load operation
    const static string LOAD;
    //the store operation
    const static string STORE;
    //the input operation
    const static string INPUT;
    //the output operation
    const static string OUTPUT;
    //the stop operation
    const static string STOP;

public:
    //check if the operation exists
    string checkOperation(const string&);
};

//this class will be the diretive table
class DiretiveTable{
    public:
    //the section directive
    const static string SECTION;
    //the space directive
    const static string SPACE;
    //the const directive
    const static string CONST;
    //the public directive
    const static string PUBLIC;
    //the extern directive
    const static string EXTERN;
    //ŧhe begin directive
    const static string BEGIN;
    //the end directive
    const static string END;
};

//this class will be the table of use
class UseTable{
    //the extern labels
    vector<string> externLabel;
    //the position of the extern labels
    vector<int> externPos;

public:
    //set an extern label
    void setExternLabel(const string&, const int&);
    //get the names of labels
    vector<string> getExternLabels();
    //get the position of the labels
    vector<int> getExternPos();

};

//this class will be the mounter
class Mounter{
    //the file after the preprocessing
    FILE* source;
    //the mounted file
    FILE* mounted;

    //an variable to check if the source file is a module or if it isn't
    bool MODULO = false;

    //the name of the mounted file
    string mountedName;

    //the tables
    SymbolTable symboltable;
    DefinitionTable definitiontable;
    InstructionTable instructiontable;
    DiretiveTable diretivetable;
    UseTable usetable;

    //get an line of the source file
    string getFileLine();

    //the first passage of the mounter
    void firstPassage();
    //the second passage of the mounter
    void secondPassage();
    //check if the word is present in the line
    size_t findWord(const string&, const string&);
    //analyse the labels
    void labelAnalysis(const string&) throw(invalid_argument);

public:
    //the constructor of the class
    Mounter(const string&);
    //the destructor of the class
    ~Mounter();
    //the action of mount
    void mount();
};


//start of the main function
int main(int argc, char** argv){
    if(argc == 1){
        //no source file
        cout << "SEM ARQUIVO" << endl;
        //leave the program
        exit(1);
    }

    Preprocessor *pre = new Preprocessor(argv[1]);
    Mounter *mounter = new Mounter(pre->getPreprocessingName());

    pre->preprocess();

    delete pre;

    mounter->mount();

    delete mounter;

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
void SymbolTable::setSymbol(const string& str, const int& position, const bool& status, const int& number, const string& str2) throw(invalid_argument){
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
    values.push_back(number);
    type.push_back(str2);
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

//the method to get the value of the label
int SymbolTable::getValue(const string& str) throw(invalid_argument){
    //a regular counter
    unsigned int counter;

    //a loop to search a label into the vector
    for(counter = 0; counter < label.size(); counter++){
        if(label[counter] == str){
            return values[counter];
        }
    }

    //if the label isn't in the vector
    throw invalid_argument("ROTULO INEXISTENTE");
}

//the method to get the type of the label
string SymbolTable::getType(const string& str) throw(invalid_argument){
    //a regular counter
    unsigned int counter;

    //a loop to search a label into the vector
    for(counter = 0; counter < label.size(); counter++){
        if(label[counter] == str){
            return type[counter];
        }
    }

    //if the label isn't in the vector
    throw invalid_argument("ROTULO INEXISTENTE");
}

/*
DEFINITION OF THE METHODS OF THE CLASS DefinitionTable
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

//the method to get the list of labels in the definition table
vector<string> DefinitionTable::getLabel(){
    return label;
}

/*
DEFINITION OF THE METHODS AND ATTRIBUTES OF THE CLASS InstructionTable
*/
//definition of the add operation
const string InstructionTable::ADD = "ADD";
//definition of the sub operation
const string InstructionTable::SUB = "SUB";
//definition of the mult operation
const string InstructionTable::MULT = "MULT";
//definition of the div operation
const string InstructionTable::DIV = "DIV";
//definition of the jmp operation
const string InstructionTable::JMP = "JMP";
//definition of the jmpn operation
const string InstructionTable::JMPN = "JMPN";
//definition of the jmpp operation
const string InstructionTable::JMPP = "JMPP";
//definition of the jmpz operation
const string InstructionTable::JMPZ = "JMPZ";
//definition of the copy operation
const string InstructionTable::COPY = "COPY";
//definition of the load operation
const string InstructionTable::LOAD = "LOAD";
//definition of the store operation
const string InstructionTable::STORE = "STORE";
//definition of the input operation
const string InstructionTable::INPUT = "INPUT";
//definition of the output operation
const string InstructionTable::OUTPUT = "OUTPUT";
//definition of the stop operation
const string InstructionTable::STOP = "STOP";

//the method to check the opcode of the operations
string InstructionTable::checkOperation(const string& str){
    if(str == ADD){
        return "01";
    }
    else if(str == SUB){
        return "02";
    }
    else if(str == MULT){
        return "03";
    }
    else if(str == DIV){
        return "04";
    }
    else if(str == JMP){
        return "05";
    }
    else if(str == JMPN){
        return "06";
    }
    else if(str == JMPP){
        return "07";
    }
    else if(str == JMPZ){
        return "08";
    }
    else if(str == COPY){
        return "09";
    }
    else if(str == LOAD){
        return "10";
    }
    else if(str == STORE){
        return "11";
    }
    else if(str == INPUT){
        return "12";
    }
    else if(str == OUTPUT){
        return "13";
    }
    else if(str == STOP){
        return "14";
    }
    else{
        return "-1";
    }
}

/*
DEFINITION OF THE ATTRIBUTES OF THE CLASS DirectiveTable
*/
//definition of the attribute section
const string DiretiveTable::SECTION = "SECTION";
//definition of the attribute space
const string DiretiveTable::SPACE = "SPACE";
//definition of the attribute const
const string DiretiveTable::CONST = "CONST";
//definition of the attribute public
const string DiretiveTable::PUBLIC = "PUBLIC";
//definition of the attribute extern
const string DiretiveTable::EXTERN = "EXTERN";
//definition of the attribute begin
const string DiretiveTable::BEGIN = "BEGIN";
//definition of the attibute end
const string DiretiveTable::END = "END";

/*
DEFINITION OF THE METHODS OF THE CLASS UseTable
*/
//definition of the method to set an external label in the use table
void UseTable::setExternLabel(const string& str, const int& pos){
    externLabel.push_back(str);
    externPos.push_back(pos);
}

//definition of the method to get the vector of the extern labels
vector<string> UseTable::getExternLabels(){
    return externLabel;
}

//definition of the method to gt the vector of the position of the extern labels
vector<int> UseTable::getExternPos(){
    return externPos;
}

/*
DEFINITION OF THE METHODS OF THE CLASS Mounter
*/
//this constructor will open the files
Mounter::Mounter(const string& fileName){
    //a regular counter
    unsigned int counter;
    //an auxiliar variable of the position
    size_t auxPos;

    //open the file
    source = fopen(fileName.c_str(), "r");

    //get the position of the .
    auxPos = fileName.find('.');

    //get the name of the mounted file
    for(counter = 0; counter < auxPos; counter++){
        mountedName.push_back(fileName[counter]);
    }

    mountedName = mountedName + ".obj";
}

//this destructor will close the files
Mounter::~Mounter(){
    //closing the files
    fclose(source);
    fclose(mounted);
}

//the method to check if there is any word in the string
size_t Mounter::findWord(const string& line, const string& word){
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
        if((line[pos + word.size()] == ' ') || (line[pos + word.size()] == '\n') || (line[pos + word.size()] == '\r') || (line[pos + word.size()] == '\t')){
            return pos;
        }
        else{
            return string::npos;
        }
    }

}

//this method will get a file line
string Mounter::getFileLine(){
    //an auxiliar char
    char chAux;
    //the line of the file
    string line;

    while(chAux != EOF){
        //get a character from the file
        chAux = fgetc(source);
        //put the character in the string
        line.push_back(chAux);

        //check if it is in the end of the file
        if((chAux == EOF) || (chAux == '\0')){
            break;
        }

        //if the character is a \n, return
        if(chAux == '\n'){
            if(line == "\n"){
                return "";
            }

            return line;
        }
    }

    return "";
}

//the method to analyse the label
void Mounter::labelAnalysis(const string& str) throw(invalid_argument){
    if((str[0] >= '0') && (str[0] <= '9')){
        throw invalid_argument("TOKEN INVALIDO");
    }
}

//this method will do the first passage of the mounter
void Mounter::firstPassage(){
    //the line of the file
    string line;
    //the number of the line of the file
    unsigned int lineNumber = 1;
    //the position counter
    unsigned int posNumber = 0;
    //a regular counter
    unsigned int counter;
    //an auxiliar position variable
    size_t auxPos;
    //an variable to check if it is in the section text
    bool text = false;
    //an variable to check if it is in the section data
    bool data = false;
    //an variable to check if it is in the section bss
    bool bss = false;
    //an variable to contain the label
    string label;
    //an variable to contain the operations
    string operation = "";
    //the opcode of the instruction
    string opcode;

    //the loop of the first passage
    do{
        //get a line from the file
        line = getFileLine();

        if(line == ""){
            break;
        }

        //search for :
        auxPos = line.find(':');

        //if there isn't a :
        if(auxPos == string::npos){
            label = "";

            try{
                //get the position of the operation
                for(counter = 0; ; counter++){
                    if((line[counter] != ' ') && (line[counter] != '\t') && (line[counter] != '\r')){
                        auxPos = counter;
                        break;
                    }

                    //if reached in the end of the line
                    if(line[counter] == '\n'){
                        throw invalid_argument("SEM OPERACAO");
                    }
                }
            }

            //if there isn't any operation
            catch(invalid_argument& e){
                cerr << "ERRO NA LINHA " << lineNumber << endl;
                cerr << e.what() << endl;
                //close the file
                fclose(source);
                //exit the program
                exit(1);
            }

            //get the operation
            for(counter = auxPos; ; counter++){
                if((line[counter] == ' ')|| (line[counter] == '\t') || (line[counter] == '\n') || (line[counter] == '\r')){
                    break;
                }

                //put the character in the operation string
                operation.push_back(line[counter]);
            }

            //get the opcode of the operation
            opcode = instructiontable.checkOperation(operation);

            //if the operation doesn't exist
            if((opcode == "-1") && (operation != "")){
                //check if it is a directive
                try{
                    //the diretive section
                    if(operation == diretivetable.SECTION){
                        //if it is the section text
                        if(findWord(line, "TEXT") != string::npos){
                            //find the diretive text
                            text = true;
                            data = false;
                            bss = false;

                            lineNumber++;
                            label.clear();
                            operation.clear();
                            continue;
                        }
                        else if(findWord(line, "DATA") != string::npos){
                            //find the diretive data
                            data = true;
                            text = false;
                            bss = false;

                            lineNumber++;
                            label.clear();
                            operation.clear();
                            continue;
                        }
                        else if(findWord(line, "BSS") != string::npos){
                            //find the diretive bss
                            bss = true;
                            text = false;
                            data = false;

                            lineNumber++;
                            label.clear();
                            operation.clear();
                            continue;
                        }
                        //if this is a section invalid
                        else{
                            throw invalid_argument("SECAO INVALIDA");
                        }
                    }

                    //the diretive space
                    else if(operation == diretivetable.SPACE){
                        throw invalid_argument("DIRETIVA ERRADA");
                    }

                    //the diretive const
                    else if(operation == diretivetable.CONST){
                        throw invalid_argument("DIRETIVA ERRADA");
                    }

                    //the diretive public
                    else if(operation == diretivetable.PUBLIC){
                        //the public label
                        string operand;

                        //get the position of the diretive
                        auxPos = findWord(line, "PUBLIC");

                        //get the position of the operator
                        for(counter = auxPos + 6; ; counter++){
                            if((line[counter] != ' ') && (line[counter] != '\t') && (line[counter] != '\n') && (line[counter] != '\r')){
                                auxPos = counter;
                                break;
                            }

                            if(line[counter] == '\n'){
                                throw invalid_argument("DIRETIVA PUBLIC SEM OPERANDO");
                            }
                        }

                        //get the operand
                        for(counter = auxPos; ; counter++){
                            if((line[counter] == ' ') || (line[counter] == '\t') || (line[counter] == '\n') || (line[counter] == '\r')){
                                break;
                            }

                            operand.push_back(line[counter]);
                        }

                        //put the operand in the definition table
                        definitiontable.setLabel(operand);

                        lineNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }

                    //the diretive extern
                    else if(operation == diretivetable.EXTERN){
                        throw invalid_argument("DIRETIVA EXTERN SEM ROTULO");
                    }

                    //the diretive begin
                    else if(operation == diretivetable.BEGIN){
                        if(MODULO){
                            throw invalid_argument("DIRETIVA BEGIN REPETIDA");
                        }

                        MODULO = true;
                        lineNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }

                    //the diretive end
                    else if(operation == diretivetable.END){
                        if(!MODULO){
                            throw invalid_argument("SECAO TEXTO FALTANTE");
                        }

                        lineNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }

                    //if it doesn't find the diretive
                    else{
                        throw invalid_argument("OPERACAO INEXISTENTE");
                    }
                }

                //the diretive doesn't exist
                catch(invalid_argument& e){
                    //the errors
                    cerr << "ERRO NA LINHA " << lineNumber << endl;
                    cerr << e.what() << endl;
                    //close the file
                    fclose(source);
                    //leave the program
                    exit(1);

                }
            }

            //if the opcode exists
            else{
                try{

                    //if the operations isn't in the section text
                    if(!text){
                        throw invalid_argument("SECAO TEXTO FALTANTE");
                    }

                    //the operation of size 1
                    if(opcode == "14"){
                        posNumber++;
                        lineNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }

                    //the operation of size 3
                    else if(opcode == "09"){
                        posNumber += 3;
                        lineNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }

                    //the operation of size 2
                    else{
                        posNumber += 2;
                        lineNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }
                }
                catch(invalid_argument& e){
                    cerr << e.what() << endl;
                    cerr << "ERRO NA LINHA " << lineNumber << endl;
                    //close the file
                    fclose(source);
                    //leave the program
                    exit(1);
                }
            }

        }
        else{
            //get the label
            for(counter = 0; counter < auxPos; counter++){
                //the label doesn't have spaces
                if((line[counter] == ' ') || (line[counter] == '\t')){
                    continue;
                }
                label.push_back(line[counter]);
            }

            //get the position of the operation
            try{
                //analyse the label
                labelAnalysis(label);

                for(counter = (auxPos + 1); ; counter++){
                    if((line[counter] != ' ') && (line[counter] != '\n') && (line[counter] != '\t')){
                        auxPos = counter;
                        break;
                    }
                    //check if it has reached the end of the file
                    if(line[counter] == '\n'){
                        throw invalid_argument("FALTA OPERACAO");
                    }
                }
            }
            catch(invalid_argument& e){
                cerr << "Erro na linha " << lineNumber << endl;
                cerr << e.what() << endl;
                //close the file
                fclose(source);
                //leave the program
                exit(1);
            }

            //get the operation
            for(counter = auxPos; ; counter++){
                if((line[counter] == ' ') || (line[counter] == '\n') || (line[counter] == '\t') || (line[counter] == '\r')){
                    break;
                }

                operation.push_back(line[counter]);
            }

            //get the opcode of the operation
            opcode = instructiontable.checkOperation(operation);

            //if the instruction doesn't exist
            if((opcode == "-1") && (operation != "")){
                //check if it is a directive
                try{
                    //the diretive section
                    if(operation == diretivetable.SECTION){
                        //if it is the section text
                        if(findWord(line, "TEXT") != string::npos){
                            //find the diretive text
                            text = true;
                            data = false;
                            bss = false;

                            //put the informations into the symbol table
                            symboltable.setSymbol(label, posNumber, false, 0, "SECTION");

                            lineNumber++;
                            label.clear();
                            operation.clear();
                            continue;
                        }
                        else if(findWord(line, "DATA") != string::npos){
                            //find the diretive data
                            data = true;
                            text = false;
                            bss = false;

                            //put the informations into the symbol table
                            symboltable.setSymbol(label, posNumber, false, 0, "SECTION");

                            lineNumber++;
                            label.clear();
                            operation.clear();
                            continue;
                        }
                        else if(findWord(line, "BSS") != string::npos){
                            //find the diretive bss
                            bss = true;
                            text = false;
                            data = false;

                            //put the informations into the symbol table
                            symboltable.setSymbol(label, posNumber, false, 0, "SECTION");

                            lineNumber++;
                            label.clear();
                            operation.clear();
                            continue;
                        }
                        //if this is a section invalid
                        else{
                            throw invalid_argument("SECAO INVALIDA");
                        }
                    }

                    //the diretive space
                    else if(operation == diretivetable.SPACE){
                        if(data || text || !bss){
                            throw invalid_argument("DIRETIVA NA SECAO ERRADA");
                        }
                        //the total of allocated memory because of the space
                        string memoryAllocated;

                        //find the position of the diretive space
                        auxPos = findWord(line, "SPACE");

                        //get the position of the space
                        for(counter = auxPos + 5; ; counter++){
                            if(line[counter] == '\n'){
                                auxPos = string::npos;
                                break;
                            }

                            if((line[counter] != ' ') && (line[counter] != '\t') && (line[counter] != '\r')){
                                auxPos = counter;
                                break;
                            }
                        }

                        //if the length of the space isn't specified, the size is 1
                        if(auxPos == string::npos){
                            symboltable.setSymbol(label, posNumber, false, 0, "SPACE");

                            lineNumber++;
                            posNumber++;
                            label.clear();
                            operation.clear();
                            continue;
                        }

                        else{
                            for(counter = auxPos; ; counter++){
                                if((line[counter] == ' ') || (line[counter] == '\t') || (line[counter] == '\n') || (line[counter] == '\r')){
                                    break;
                                }

                                memoryAllocated.push_back(line[counter]);
                            }

                            symboltable.setSymbol(label, posNumber, false, 0, "SPACE");

                            lineNumber++;
                            posNumber += stoi(memoryAllocated);
                            label.clear();
                            operation.clear();
                            continue;
                        }
                    }

                    //the diretive const
                    else if(operation == diretivetable.CONST){
                        if(!data || text || bss){
                            throw invalid_argument("DIRETIVA NA SECAO ERRADA");
                        }

                        //the value of the constant
                        string value;

                        //get the position of the diretive const
                        auxPos = findWord(line, "CONST");

                        //get the position of the const value
                        for(counter = auxPos + 5; ; counter++){
                            if((line[counter] != ' ') && (line[counter] != '\t')){
                                auxPos = counter;
                                break;
                            }

                            if(line[counter] == '\n'){
                                throw invalid_argument("DIRETIVA CONST SEM VALOR");
                            }
                        }

                        //get the value of the const
                        for(counter = auxPos; ; counter++){
                            if((line[counter] == ' ') || (line[counter] == '\t') || (line[counter] == '\n')){
                                break;
                            }

                            value.push_back(line[counter]);
                        }

                        symboltable.setSymbol(label, posNumber, false, stoi(value), "CONST");

                        lineNumber++;
                        posNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }
                    //the diretive public
                    else if(operation == diretivetable.PUBLIC){
                        //the public label
                        string operand;

                        //get the position of the diretive
                        auxPos = findWord(line, "PUBLIC");

                        //get the position of the operator
                        for(counter = auxPos + 6; ; counter++){
                            if((line[counter] != ' ') && (line[counter] != '\t') && (line[counter] != '\n') && (line[counter] != '\r')){
                                auxPos = counter;
                                break;
                            }

                            if(line[counter] == '\n'){
                                throw invalid_argument("DIRETIVA PUBLIC SEM OPERANDO");
                            }
                        }

                        //get the operand
                        for(counter = auxPos; ; counter++){
                            if((line[counter] == ' ') || (line[counter] == '\t') || (line[counter] == '\n') || (line[counter] == '\r')){
                                break;
                            }

                            operand.push_back(line[counter]);
                        }

                        //put the operand in the definition table
                        definitiontable.setLabel(operand);

                        symboltable.setSymbol(label, auxPos, false, 0, "PUBLIC");

                        lineNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }

                    //the diretive extern
                    else if(operation == diretivetable.EXTERN){
                        symboltable.setSymbol(label, 0, true, 0, "EXTERN");

                        lineNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }

                    //the diretive begin
                    else if(operation == diretivetable.BEGIN){
                        //put in the symbol table
                        symboltable.setSymbol(label, posNumber, false, 0, "BEGIN");

                        MODULO = true;
                        lineNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }

                    //the diretive end
                    else if(operation == diretivetable.END){
                        if(!MODULO){
                            throw invalid_argument("DIRETIVA ERRADA");
                        }

                        //put in the symbol table
                        symboltable.setSymbol(label, posNumber, false, 0, "END");

                        lineNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }

                    //if it doesn't find the diretive
                    else{
                        throw invalid_argument("OPERACAO INEXISTENTE");
                    }
                }

                //the diretive doesn't exist
                catch(invalid_argument& e){
                    //the errors
                    cerr << "ERRO NA LINHA " << lineNumber << endl;
                    cerr << e.what() << endl;
                    //close the file
                    fclose(source);
                    //leave the program
                    exit(1);

                }
            }

            //if the operation exists
            else{
                try{
                    //if it is not in the section text
                    if(!text){
                        throw invalid_argument("OPERACAO NA SECAO ERRADA");
                    }

                    //put the label in the symbol table
                    symboltable.setSymbol(label, posNumber, false, 0, "");

                    //the operation of size 1
                    if(opcode == "14"){
                        posNumber++;
                        lineNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }

                    //the operation of size 3
                    else if(opcode == "09"){
                        posNumber += 3;
                        lineNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }

                    //the operation of size 2
                    else{
                        posNumber += 2;
                        lineNumber++;
                        label.clear();
                        operation.clear();
                        continue;
                    }
                }
                catch(invalid_argument& e){
                    cerr << e.what() << endl;
                    cerr << "ERRO NA LINHA " << lineNumber << endl;
                    fclose(source);
                    exit(1);
                }

            }
        }

        label.clear();
        operation.clear();
    }while(line != "");

    //put the addressses of the definition table
    try{
        //all public labels
        vector<string> definitions;

        //get the public labels
        definitions = definitiontable.getLabel();

        //get the addresses of the label
        for(counter = 0; counter < definitions.size(); counter++){
            definitiontable.setAddress(definitions[counter], symboltable.getAddress(definitions[counter]));
        }

    }
    //if doesn't encounter the label in the symbol table
    catch(invalid_argument& e){
        cerr << e.what() << endl;
        //close the file
        fclose(source);
        //leave the program
        exit(1);
    }

    //cout << symboltable.getAddress("OLD_DATA") << endl;
}

//definition of the method of the second passage
void Mounter::secondPassage(){
    //the line counter
    unsigned int lineNumber = 1;
    //the position counter
    unsigned int posNumber = 0;
    //the line of the file
    string line;
    //the operation
    string operation;
    //the operand
    string operand;
    //the opcode of the operation
    string opcode;
    //the output code
    string code = "";
    //the relative position
    vector<int> relative;
    //an auxiliar position variable
    size_t auxPos;
    //a regular counter
    unsigned int counter;

    //put the cursor in the beginning of the file
    fseek(source, 0, SEEK_SET);

    do{
        //get the line from the file
        line = getFileLine();

        if(line == ""){
            break;
        }

        //get the position of the :
        auxPos = line.find(':');

        //if doesn't have the :
        if(auxPos == string::npos){
            counter = 0;
        }
        else{
            counter = auxPos + 1;
        }

        //get the position of the operation
        while(true){
            if((line[counter] != ' ') && (line[counter] != '\t') && (line[counter] != '\r')){
                auxPos = counter;
                break;
            }

            counter++;
        }

        //get the operation
        for(counter = auxPos; ; counter++){
            if((line[counter] == ' ') || (line[counter] == '\t') || (line[counter] == '\r') || (line[counter] == '\n')){
                break;
            }

            //put the character in the string
            operation.push_back(line[counter]);
        }

        //get the opcode of the operation
        opcode = instructiontable.checkOperation(operation);

        //if the instruction doesnt exists
        if((opcode == "-1") && (operation != "")){
            //the diretives
            if(operation == diretivetable.BEGIN){
                lineNumber++;
                operation.clear();
                continue;
            }

            else if(operation == diretivetable.CONST){
                auxPos = line.find("CONST");

                //get the position of the operand
                for(counter = auxPos + 5; ; counter++){
                    if((line[counter] != ' ') && (line[counter] != '\t') && (line[counter] != '\r')){
                        auxPos = counter;
                        break;
                    }
                }

                //get the operand
                for(counter = auxPos; ; counter++){
                    if((line[counter] == ' ') || (line[counter] == '\t') || (line[counter] == '\r') || (line[counter] == '\n')){
                        break;
                    }

                    operand.push_back(line[counter]);
                }

                if(stoi(operand) < 10){
                    code += "0";
                }

                //add in the code
                code += operand;
                code += " ";

                //add the position
                posNumber++;
                lineNumber++;

                operation.clear();
                operand.clear();

                continue;
            }

            else if(operation == diretivetable.END){
                lineNumber++;
                operation.clear();
                continue;
            }

            else if(operation == diretivetable.EXTERN){
                lineNumber++;
                operation.clear();
                continue;
            }

            else if(operation == diretivetable.PUBLIC){
                lineNumber++;
                operation.clear();
                continue;
            }

            else if(operation == diretivetable.SECTION){
                lineNumber++;
                operation.clear();
                continue;
            }

            else if(operation == diretivetable.SPACE){
                auxPos = line.find("SPACE");

                //a variable to check if there is a operand
                bool state = false;

                //get the position of the operand
                for(counter = auxPos + 5; ; counter++){
                    if(line[counter] == '\n'){
                        state = true;
                    }

                    if((line[counter] != ' ') && (line[counter] != '\t') && (line[counter] != '\r')){
                        auxPos = counter;
                        break;
                    }
                }

                if(state){
                    code += "00 ";
                    lineNumber++;
                    posNumber++;

                    operation.clear();

                    continue;
                }

                //get the operand
                for(counter = auxPos; ; counter++){
                    if((line[counter] == ' ') || (line[counter] == '\t') || (line[counter] == '\r')){
                        break;
                    }

                    operand.push_back(line[counter]);
                }

                //add in the code
                for(counter = 0; counter < (unsigned int)stoi(operand); counter++){
                    code += "00 ";
                    posNumber++;
                }

                //add the line
                lineNumber++;

                operation.clear();
                operand.clear();

                continue;
            }
        }

        else{
            try{

                //all the operations except copy and stop
                if((opcode != "09") && (opcode != "14")){
                    //get the position of the operand
                    auxPos = line.find(operation);

                    for(counter = auxPos + operation.size(); ; counter++){
                        if((line[counter] != ' ') && (line[counter] != '\t') && (line[counter] != '\r')){
                            auxPos = counter;
                            break;
                        }
                    }

                    //get the operand
                    for(counter = auxPos; ; counter++){
                        if((line[counter] == ' ') || (line[counter] == '\t') || (line[counter] == '\r') || (line[counter] == '\n')){
                            break;
                        }

                        operand.push_back(line[counter]);
                    }

                    //for division by zero
                    if(opcode == "04"){
                        //if it is a const label
                        if(symboltable.getType(operand) == "CONST"){
                            if(symboltable.getValue(operand) == 0){
                                throw invalid_argument("DIVISAO POR 0");
                            }
                        }
                    }

                    //for the jumps, search if it a valid label
                    if((opcode == "05") || (opcode == "06") || (opcode == "07") || (opcode == "08")){
                        if((symboltable.getType(operand) == "CONST") || (symboltable.getType(operand) == "SPACE")){
                            throw invalid_argument("PULO PARA SECAO ERRADA");
                        }
                    }

                    //for the store and the input, check if it is modifying a constant value
                    else if((opcode == "11") || (opcode == "12")){
                        if(symboltable.getType(operand) == "CONST"){
                            throw invalid_argument("MODIFICACAO DE VALOR CONSTANTE");
                        }

                        //check if the operand is valid
                        if((symboltable.getType(operand) != "SPACE") && (symboltable.getType(operand) != "EXTERN")){
                            throw invalid_argument("ROTULO INVALIDO");
                        }
                    }

                    else{
                        //check if the operand is valid
                        if((symboltable.getType(operand) != "CONST") && (symboltable.getType(operand) != "SPACE") && (symboltable.getType(operand) != "EXTERN")){
                            throw invalid_argument("ROTULO INVALIDO");
                        }
                    }

                    //put the informations in the table of use
                    if(symboltable.getExtern(operand)){
                        usetable.setExternLabel(operand, posNumber + 1);
                    }
                    else{
                        relative.push_back(posNumber+1);
                    }

                    //put the informations into the code
                    code += opcode;
                    code += " ";

                    if(symboltable.getAddress(operand) < 10){
                        code += "0";
                    }

                    code += to_string(symboltable.getAddress(operand));
                    code += " ";

                    posNumber += 2;
                    lineNumber++;

                    operation.clear();
                    operand.clear();
                    continue;
                }

                //the copy operation
                else if(opcode == "09"){
                    //the copy have two operands
                    string secondOperand;

                    //get the position of the operand
                    auxPos = line.find(operation);

                    for(counter = auxPos + operation.size(); ; counter++){
                        if((line[counter] != ' ') && (line[counter] != '\t') && (line[counter] != '\r')){
                            auxPos = counter;
                            break;
                        }
                    }

                    //get the operand
                    for(counter = auxPos; ; counter++){
                        if((line[counter] == ' ') || (line[counter] == '\t') || (line[counter] == '\r') || (line[counter] == ',')){
                            break;
                        }

                        operand.push_back(line[counter]);
                    }

                    //get the position of the second operand
                    for(counter = auxPos + operand.size() + 1; ; counter++){
                        if((line[counter] != ' ') && (line[counter] != '\t') && (line[counter] != '\r') && (line[counter] != ',')){
                            auxPos = counter;
                            break;
                        }
                    }

                    //get the second operand
                    for(counter = auxPos; ; counter++){
                        if((line[counter] == ' ') || (line[counter] == '\t') || (line[counter] == '\r') || (line[counter] == '\n')){
                            break;
                        }

                        secondOperand.push_back(line[counter]);
                    }

                    //check if the second operand is the type const
                    if(symboltable.getType(secondOperand) == "CONST"){
                        throw invalid_argument("MODIFICACAO DE VALOR CONSTANTE");
                    }

                    //check if the first operand is valid
                    if((symboltable.getType(operand) != "CONST") && (symboltable.getType(operand) != "SPACE") && (symboltable.getType(operand) != "EXTERN")){
                        throw invalid_argument("ROTULO INVALIDO");
                    }

                    //check if the second operand is valid
                    if((symboltable.getType(secondOperand) != "SPACE") && (symboltable.getType(secondOperand) != "EXTERN")){
                        throw invalid_argument("ROTULO INVALIDO");
                    }

                    //put the informations in the table of use
                    if(symboltable.getExtern(operand)){
                        usetable.setExternLabel(operand, posNumber + 1);
                    }
                    else{
                        relative.push_back(posNumber + 1);
                    }

                    if(symboltable.getExtern(secondOperand)){
                        usetable.setExternLabel(operand, posNumber + 2);
                    }
                    else{
                        relative.push_back(posNumber + 1);
                    }

                    //put the informations in the code
                    code += opcode;
                    code += " ";

                    if(symboltable.getAddress(operand) < 10){
                        code += "0";
                    }

                    code += to_string(symboltable.getAddress(operand));
                    code += " ";

                    if(symboltable.getAddress(secondOperand) < 10){
                        code += "0";
                    }

                    code += to_string(symboltable.getAddress(secondOperand));
                    code += " ";

                    //add the address and the line number
                    posNumber += 3;
                    lineNumber++;

                    operation.clear();
                    operand.clear();
                    continue;
                }

                //the stop operation
                else if(opcode == "14"){

                //put the opcode in the code
                code += opcode;
                code += " ";

                //add the address and the line number
                posNumber++;
                lineNumber++;

                operation.clear();
                operand.clear();
                continue;
                }
            }

            //if there is an error
            catch(invalid_argument& e){
                cerr << e.what() << endl;
                cerr << "ERRO NA LINHA " << lineNumber << endl;
                //close the file
                fclose(source);
                //leave the program
                exit(1);
            }
        }
    }while(line != "");

    //put the informations on the file
    mounted = fopen(mountedName.c_str(), "w");

    //if it is a module
    if(MODULO){
        //the informations in the use table
        vector<string> use = usetable.getExternLabels();
        vector<int> usePos = usetable.getExternPos();
        //the informations in the definition table
        vector<string> definition = definitiontable.getLabel();

        //put the informations in the file
        //put the informations of the use table in the file
        fputs("TABLE USE\n", mounted);
        for(counter = 0; counter < use.size(); counter++){
            fprintf(mounted, "%s ", use[counter].c_str());
            fprintf(mounted, "%d\n", usePos[counter]);
        }

        //put the informations of the definition table
        fprintf(mounted, "\nTABLE DEFINITION\n");
        for(counter = 0; counter < definition.size(); counter++){
            fprintf(mounted, "%s ", definition[counter].c_str());
            fprintf(mounted, "%d\n", definitiontable.getAddress(definition[counter]));
        }

        //put the informations of the relatives
        fprintf(mounted, "\nRELATIVE\n");
        for(counter = 0; counter < relative.size(); counter++){
            fprintf(mounted, "%d ", relative[counter]);
        }

        fprintf(mounted, "\n\nCODE\n");

        //put the code
        fputs(code.c_str(), mounted);
    }
    else{
        fprintf(mounted,"CODE\n");
        fputs(code.c_str(), mounted);
    }
}

//this method will mount the source file
void Mounter::mount(){
    firstPassage();
    secondPassage();

    cout << "MONTAGEM BEM SUCEDIDA" << endl;
}
