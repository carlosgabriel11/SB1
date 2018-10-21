// Updated 10/20/2018 9:30 PM
#include <iostream>
#include <map>
#include <bits/stdc++.h>
#include <fstream>

using namespace std;

string outname;

void printIt(std::map<string,int> m) {      // Funcao para print de std::map para debug visual
    cout << "tabela print: " << endl;
    for(std::map<string,int>::iterator it=m.begin();it!=m.end();++it)
        std::cout << it->first<<":"<<it->second<<" ";
    std::cout << "\n";
}

void PrintIt(auto m){                       // Funcao para print de std::map TU para debug visual
  cout << "TU: " << endl;
  for(std::map<int, string>::iterator it=m.begin();it!=m.end();++it)
      std::cout << it->first<<":"<<it->second<<" ";
  std::cout << "\n";
}

void get_data(auto relatives, auto FileName, auto TD, auto TU, auto Fator_correcao, int i){ // Funcao para pegar os dados das tabelas, fator de correcao, e enderecos relativos

  string full_str, str;
  char aux[50];
  int num;

  FILE *ptr;
  ptr = fopen(FileName.c_str(),"r");

  fscanf(ptr,"%s", aux);
  fscanf(ptr,"%s", aux);
  while(strcmp(aux,"TABLE")!=0){  // Aquisicao dos dados da tabela de Uso
    fscanf(ptr,"%s", aux);
    if(strcmp(aux,"TABLE")!=0){
      fscanf(ptr, "%d", &num);
      str = aux;
      TU[i].insert(make_pair(num,str));
    }
  }
  fscanf(ptr,"%s", aux);
  while(strcmp(aux,"RELATIVE")!=0){  // Aquisicao dos dados da tabela de definicoes
    fscanf(ptr,"%s", aux);
    if(strcmp(aux,"RELATIVE")!=0){
      fscanf(ptr, "%d", &num);
      str = aux;
      TD[i].insert(make_pair(str,num));
    }
  }
  while(strcmp(aux,"CODE")!=0){       // Aquisicao dos dados de enderecos relativos
    fscanf(ptr, "%s", aux);
    if(strcmp(aux,"CODE")!=0){
      relatives[i].push_back(stoi(aux));
    }
  }

  Fator_correcao[i] = 0;
  while(fscanf(ptr,"%d", &num)!=EOF){
    Fator_correcao[i]++;
  }
  if(i!=0) Fator_correcao[i] += Fator_correcao[i-1]; // Fator de correcao agora leva em conta todos os modulos anteriores

  fclose(ptr);

}

void gera_binario(auto TDglobal, auto TU, auto Fator_correcao, auto FileNames, auto N_arquivos, auto relatives){
    FILE *outptr;
    outptr = fopen(outname.c_str(), "w"); // MODIFICAR AQUI O NOME DO ARQUIVO DE SAIDA
    std::map<int, string>::iterator it;


    FILE  *inptr;
    for(int i=0;i<N_arquivos;i++){

      inptr = fopen(FileNames[i].c_str(), "r");

      char aux[50]= "NAOCONSISTENTE";
      while(strcmp(aux, "CODE")!=0){
        fscanf(inptr, "%s", aux);
      }
      int counter = 0, tempREL=0, cod; // contador de tokens e variaveis temporarias de relatives
      while(fscanf(inptr, "%d", &cod)!=EOF){

        it = TU[i].find(counter);
        if(it!=TU[i].end()){
          cod = TDglobal[it->second]; // Se a posicao atual esta na tabela de uso, subsituir pelo valor da TDglobal(tabela de definicoes global)

        }
        else if(tempREL<relatives[i].size()){
          if(counter==relatives[i][tempREL]){ // Se posicao atual esta na lista de enderecos relativos, soma offset do fator de correcao

            if(i!=0)cod+=Fator_correcao[i-1];
            tempREL++;
          }
        }
        counter++;
        fprintf(outptr, "%d ", cod);
      }
      fclose(inptr);
    }
    fclose(outptr);
}

int main(int argc,char **argv){
    // Pega o numero de arquivos de entrada
    int N_arquivos, t;
    string str;
    N_arquivos = argc-1;

    // Pega o nome dos arquivos
    string FileNames[N_arquivos];
    for(int i=0;i<N_arquivos;i++){
      FileNames[i] = argv[i+1];
      str = FileNames[i];

      t = str.size();
      if((str[t-1]!='j')||(str[t-2]!='b')||(str[t-3]!='o')||(str[t-4]!='.')){ // Arruma o nome dos arquivos de entrada com extensao .obj
        str += ".obj";
      }
      FileNames[i] = str;
    }
    outname = FileNames[0];
    t = outname.size();     // Arruma o nome do arquivo de saida com extensao .e
    outname[t-3] = 'e';
    outname[t-2] = '\0';
    outname[t-1] = '\0';

    if(argc==2){  // Caso haja somente um modulo
      FILE *outptr, *inptr;
      int num;
      outptr = fopen(outname.c_str(), "w"); // MODIFICAR AQUI O NOME DO ARQUIVO DE SAIDA
      inptr = fopen(FileNames[0].c_str(), "r");
      while(fscanf(inptr, "%d", &num)!=EOF){
        fprintf(outptr, "%d ", num);
      }
      fclose(inptr);
      fclose(outptr);
    }
    else{ // Caso haja mais de um modulo para ligar
      //Obtencao das tabelas de Uso e de definicoes, assim como tamanho dos arquivos objeto e lista de enderecos relativos
      int Fator_correcao[N_arquivos];
      vector<int> relatives[N_arquivos];
      map<string,int> TD[N_arquivos], TDglobal;
      map<int,string> TU[N_arquivos];
      for(int i=0;i<N_arquivos;i++){
        get_data(relatives, FileNames[i], TD, TU, Fator_correcao, i);
      }
      //Gerando tabela de definicoes global
      for(int i=0;i<N_arquivos;i++){
          for(auto it=TD[i].begin();it!=TD[i].end();++it){
              if(i!=0) TDglobal[it->first] = it->second + Fator_correcao[i-1];
              else     TDglobal[it->first] = it->second;
          }
      }

      // Inicio da escrita no arquivo de saida
      gera_binario(TDglobal, TU, Fator_correcao, FileNames, N_arquivos, relatives);
    }
    return 0;
}
