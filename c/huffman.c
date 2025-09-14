#include <string.h>
#include "heap.h"
#include "buffers.h"
#include <stdlib.h>

arbreCompression *creationArbre(int f, int v){    
    arbreCompression *arbre = malloc(sizeof(arbreCompression));
    arbre->frequency = f;
    arbre->value = v;
    arbre->fg = NULL;
    arbre->fd = NULL;
    return arbre;
}

arbreCompression *fusion(arbreCompression *a1, arbreCompression *a2){
    arbreCompression *arbre = malloc(sizeof(arbreCompression));
    arbre->frequency = (*a1).frequency+(*a2).frequency;
    arbre->value = -1;
    arbre->fg = a1;
    arbre->fd = a2;    
    return arbre;
}

int *countFrequence(FILE *input_stream){
    int *res = malloc(sizeof(int)*256);
    for(int i=0;i<256;i++){
        res[i] = 0;
    }
    int charactere;
    while(charactere != EOF){
        charactere = fgetc(input_stream);
        res[charactere]++;
    }    
    return res;
}

arbreCompression *huffman_tree_huffman(int *frequency){
    unsigned int compteur = 0;
    for(int i=0;i<256;i++){
        if(frequency[i] != 0){
            compteur++;
        }
    }
    arbreCompression **treesList = malloc(compteur*sizeof(couple));
    compteur = 0;
    for(int i=0;i<256;i++){
        if(frequency[i] != 0){
            treesList[compteur] = creationArbre(frequency[i],i);            
            compteur++;
        }
    }    
    tas *heap = NULL;
    ajoutListe(&heap,treesList,compteur);
    arbreCompression *min1;
    arbreCompression *min2;
    for(unsigned int i=0;i<compteur-1;i++){
        min1 = popMinElement(&heap);        
        min2 = popMinElement(&heap);
        ajout(&heap,fusion(min1,min2));
    }
    return popMinElement(&heap);
}

void fill_up(char *temp,arbreCompression *arb,char **tab){    
    if((*arb).fg == NULL && (*arb).fd == NULL){        
        tab[(*arb).value] = malloc(strlen(temp));        
        strcpy(tab[(*arb).value],temp);
    }
    else{
        int taille = strlen(temp);
        char *temp1 = malloc(taille+1);
        char *temp2 = malloc(taille+1);        
        strcpy(temp1,temp);
        strcpy(temp2,temp);
        temp1[taille] = '0';
        temp1[taille+1] = '\0';
        temp2[taille] = '1';
        temp2[taille+1] = '\0';    
        fill_up(temp1,(*arb).fg,tab);
        fill_up(temp2,(*arb).fd,tab);
    }
}

char **code_compression(arbreCompression *arbreHuffman){
    char **code_table = malloc(sizeof(char*)*256);
    for(int i=0;i<256;i++){
        code_table[i] = malloc(sizeof(char));        
        code_table[i][0] = '\0';
    }
    char *temp = malloc(sizeof(char));    
    temp[0] = '\0';    
    fill_up(temp,arbreHuffman,code_table);
    return code_table;
}

void huffman_tree_to_compress_file(OutputBuffer *output_buffer, arbreCompression *arbreHuffman){
    if((*arbreHuffman).fg == NULL && (*arbreHuffman).fd == NULL){
        write_bit(output_buffer,0);
        write_byte(output_buffer,(*arbreHuffman).value);
    }
    else{
        write_bit(output_buffer,1);
        huffman_tree_to_compress_file(output_buffer,(*arbreHuffman).fg);
        huffman_tree_to_compress_file(output_buffer,(*arbreHuffman).fd);
    }
}

void compression(FILE *input_stream, OutputBuffer *output_buffer, char **code_table){
    while(1){
        int character = fgetc(input_stream);
        if(character == EOF){
            break;
        }        
        int i=0;
        while(code_table[character][i] != '\0'){             
            write_bit(output_buffer,code_table[character][i]-'0');
            i++;
        }
    }
    finish(output_buffer);
}

void file_compression_wrapper(char *input_file_name, char *output_file_name){
    FILE *input_stream = NULL;
    input_stream = fopen(input_file_name,"r");
    FILE *output_value = NULL;
    output_value = fopen(output_file_name,"wb");
    OutputBuffer output_buffer ;
    init_OutputBuffer(&output_buffer,output_value);
    int *tab = countFrequence(input_stream);
    arbreCompression *huffman_tree = huffman_tree_huffman(tab);
    char **tab_code = code_compression(huffman_tree);
    fseek(input_stream,0,SEEK_SET);
    huffman_tree_to_compress_file(&output_buffer,huffman_tree);    
    compression(input_stream,&output_buffer,tab_code);
    fclose(input_stream);
    fclose(output_value);
}

//Décompression

arbreCompression *building_back_huffman_tree(InputBuffer *input_buffer){
    arbreCompression *res = malloc(sizeof(arbreCompression));
    int character = read_bit(input_buffer);
    if(character==0){
        int character2 = read_byte(input_buffer);        
        res = creationArbre(0,character2);
        return res;
    }
    arbreCompression *left = malloc(sizeof(arbreCompression));
    arbreCompression *right = malloc(sizeof(arbreCompression));
    left = building_back_huffman_tree(input_buffer);
    right = building_back_huffman_tree(input_buffer);
    res = fusion(left,right);
    return res;
}

void decompression(InputBuffer *input_buffer,OutputBuffer *output_buffer, arbreCompression *huffman_tree){
    arbreCompression *node = huffman_tree;
    int nb_bits = 10;    
    int current_bit;
    while(nb_bits>0){                
        if((*node).fg == NULL && (*node).fd == NULL){              
            write_byte(output_buffer,(*node).value);
            node = huffman_tree;            
        }
        else{
            if((*input_buffer).file_end && nb_bits == 10){
                current_bit = read_bit(input_buffer);                
                nb_bits = (*input_buffer).bits_number + 1;
            }
            else{
                current_bit = read_bit(input_buffer);                
                if((*input_buffer).file_end && nb_bits == 10){
                    nb_bits = (*input_buffer).bits_number + 1;
                }    
            }
            if(current_bit == 0){
                node = (*node).fg;
            }
            else{
                if(current_bit == 1){
                    node = (*node).fd;
                }
            }
            if(nb_bits<10){
                nb_bits--;
            }
        }
    }    
    if((*node).fg == NULL && (*node).fd == NULL){        
        write_byte(output_buffer,(*node).value);
    }
}

void file_decompression_wrapper(char *input_file_name, char *output_file_name){
    FILE *input_stream = NULL;
    input_stream = fopen(input_file_name,"rb");
    FILE *output_value = NULL;
    output_value = fopen(output_file_name,"w");
    InputBuffer input_buffer ;
    init_InputBuffer(&input_buffer,input_stream);
    OutputBuffer output_buffer ;
    init_OutputBuffer(&output_buffer,output_value);
    arbreCompression *res = building_back_huffman_tree(&input_buffer);
    decompression(&input_buffer,&output_buffer,res);
    fclose(input_stream);
    fclose(output_value);
}

int main(int argc, char *argv[]){    
    if(argv[1][0] == 'c'){
        file_compression_wrapper(argv[2],argv[3]);
    }else{
        if(argv[1][0] == 'd'){
            file_decompression_wrapper(argv[2],argv[3]);
        }
    }
    return 0;
}
//Pour compresser : ./nom_du_fichier.exe c "entree" "sortie"
//Pour décompresser : ./nom_du_fichier.exe d "entree" "sortie"