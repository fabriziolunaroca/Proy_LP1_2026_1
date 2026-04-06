#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include <stdbool.h>

int main(){
    srand(time(0));
    float money;
    bool validar = true;
    int tipo;

    while(validar){
        printf("Ingrese el monto a jugar: ");
        scanf("%f", &money);
        printf("Ingrese el tipo de apuesta; x6(1), x2(2): ");
        scanf("%i", &tipo);

        int num1, num2;

        if(tipo == 1){
            printf("Ingrese el num a jugar(del 1 al 10): ");
            scanf("%i", &num1);
        }else{
            printf("Ingrese el num 1 a jugar(del 1 al 10): ");
            scanf("%i", &num1);
            printf("Ingrese el num 2 a jugar(del 1 al 10): ");
            scanf("%i", &num2);
        }

        int array[5] = {};
        int n = 5;

        for(int i = 0; i < n; i++){
            int var = rand() % 10 + 1;
            for(int j = 0; j < 5; j++){
                if(var == array[j]){
                    i--;
                }else{
                    array[i] = var;
                    break;
                }
            }
        }

        printf("----Numeros ganadores----\n");
        for(int i = 0; i < 5; i++){
            printf("%i\t", array[i]);
        }
        printf("\n");

        if(tipo == 1){
            float montito = 0;
            for(int i = 0; i < 5; i++){
                if(array[i] == num1){
                    montito += money*6;
                    printf("GANASTE %f\n", montito);
                    break;
                }
            }
            if(montito == 0){
                printf("PERDISTE\n");
            }else{
                printf("GANASTE %f", montito);
            }
        }

        if(tipo == 2){
            float montito = 0;
            for(int i = 0; i < 5; i++){
                if(array[i] == num1){
                    montito += money*2;
                }
                if(array[i] == num2){
                    montito += money*2;
                }
            }
            if(montito == 0){
                printf("PERDISTE\n");
            }else{
                printf("GANASTE %f", montito);
            }
        }

        char l;
        printf("Quieres volver a jugar?(s/n): ");
        scanf(" %c", &l);

        if(l == 's'){
            validar = true;
        }else{
            validar = false;
        }
    }
}