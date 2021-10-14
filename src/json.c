#include "json.h"

int search_value(char *file_name, char *key, char *value)
{
    //Creo las variables con las que voy a trabajar
    FILE *archivo;
    char ch;
    char buffer[500];
    int cant_llaves=0, cant_comillas=0, cant_caracteres=0,i, tamanio_key, tamanio_lectura;

    //Abro el archivo provisto en el path_name
    archivo = fopen(file_name, "r");

    //Si hay algún problema abriendo el archivo, la función retorna -1
    if(!archivo)
        return -1;

    //Cuenta la cantidad de caracteres que tiene el archivo
    while(!feof(archivo))
    {
        ch = fgetc(archivo);
        cant_caracteres++;
        if(ch=='{')
            cant_llaves++;        
    }

    //Rebobina el cursor
    rewind(archivo);

    //Si la cantidad de llaves es menor a dos, entonces el formato es incorrecto (retorna -2)
    if(cant_llaves<2)
        return -2;

    //Determina el tamaño de la palabra clave
    tamanio_key = strlen(key);

    //Se mueve el cursor a lo largo del archivo para buscar la palabra clave
    for(i=0;i<cant_caracteres;i++)
    {
        //Por cada pasada, pongo la cantidad de comillas enumeradas en 0
        cant_comillas=0;

        //Ubico el índice en función al loop
        fseek(archivo,i,0);

        //Leo una string del tamaño de la palabra clave y la comparo con la misma
        fgets(buffer,tamanio_key+1,archivo);
        if(strcmp(buffer,key)==0)
        {
            //Si coinciden bajo una linea y elimino la etiqueta de tipo de valor
            fgets(buffer,9999,archivo);
            while(cant_comillas<2)
            {
                ch=fgetc(archivo);
                if(ch=='"')
                    cant_comillas++;
            }

            //Me muevo tres posiciones para ubicarme en el comienzo de lo que quiero leer
            fgetc(archivo);
            fgetc(archivo);
            fgetc(archivo);

            //Leo todo lo que viene después y veo que tamaño tiene
            fgets(buffer,9999,archivo);            
            tamanio_lectura = strlen(buffer);

            //Elimino la última comilla doble
            buffer[tamanio_lectura-2]='\0';  

            //Almaceno lo que me queda en el vector destino      
            strcpy(value, buffer);
        }
    }

    //Cierro archivo y retorno '0' para indicar el éxito de la operación
    fclose(archivo);
    return 0;
}

int replace_value(char *file_name, char *key, char *value)
{
    //Creo las variables con las que voy a trabajar
    FILE *archivo;
    char ch;
    char *aux;
    char buffer[500];
    int cant_llaves=0, cant_comillas=0, cant_caracteres=0,i, tamanio_key, tamanio_lectura, delta, name_ocur=0, tamanio_copia;

    //Abro el archivo provisto en el path_name
    archivo = fopen(file_name, "r+");

    //Si hay algún problema abriendo el archivo, la función retorna -1
    if(!archivo)
        return -1;

    //Cuenta la cantidad de caracteres que tiene el archivo
    while(!feof(archivo))
    {
        ch = fgetc(archivo);
        cant_caracteres++;
        if(ch=='{')
        cant_llaves++;  
    }

    //Rebobina el cursor
    rewind(archivo);

    //Si la cantidad de llaves es menor a dos, entonces el formato es incorrecto (retorna -2)
    if(cant_llaves<2)
        return -2;

    //Determina el tamaño de la palabra clave
    tamanio_key = strlen(key);

    //Se mueve el cursor a lo largo del archivo para buscar la palabra clave
    for(i=0;i<cant_caracteres;i++)
    {
        //Por cada pasada, pongo la cantidad de comillas enumeradas en 0
        cant_comillas=0;

        //Ubico el índice en función al loop
        fseek(archivo,i,0);

        //Leo una string del tamaño de la palabra clave y la comparo con la misma
        fgets(buffer,tamanio_key+1,archivo);
        if(strcmp(buffer,key)==0)
        {
            if((strcmp(key,"name")==0) && name_ocur==0)
            {
                name_ocur=1;
                continue;
            }
            //Si coinciden bajo una linea y elimino la etiqueta de tipo de valor
            fgets(buffer,9999,archivo);
            while(cant_comillas<2)
            {
                ch=fgetc(archivo);
                if(ch=='"')
                    cant_comillas++;
            }

            //Me muevo tres posiciones para ubicarme en el comienzo de lo que quiero leer
            fgetc(archivo);
            fgetc(archivo);
            fgetc(archivo);

            //Leo todo lo que viene después y veo que tamaño tiene
            fgets(buffer,9999,archivo);            

            tamanio_lectura = strlen(buffer);
            
            //Con delta me fijo cual es la diferencia entre palabras y si es negativa o positiva
            delta = tamanio_lectura-(strlen(value)-1);

            //Si es menor lo que escribo, el tamaño de lo que tengo que copiar del resto del archivo es menor
            if(delta<0)
            {
                tamanio_copia=cant_caracteres-ftell(archivo);          
            }
            else
            {
                tamanio_copia=cant_caracteres-ftell(archivo)-delta;
            }
            
            //Reservo espacio para la copia y la realizo
            aux = (char *) malloc(tamanio_copia+1);
            fread(aux,1,tamanio_copia,archivo);

            //Vuelvo el puntero hacia el comienzo de donde tengo que escribir
            fseek(archivo, -tamanio_lectura-tamanio_copia,SEEK_CUR);

            //Coloco comillas
            fputc('\"', archivo);   // VER BIEN HAY VECES QUE TENGO QUE PONERLA MANUAL Y HAY VECES QUE NO...
            
            //Escribo la palabra
            fprintf(archivo,"%s",value);

            //Coloco comillas y fin de linea
            fputc('\"', archivo);
            fputc('\n', archivo);

            //Escribo lo que resta del archivo
            printf("Lo que resta del archivo es:\n%s\n", aux);
            fprintf(archivo,"%s",aux);   
        }
    }

    //Cierro archivo y retorno '0' para indicar el éxito de la operación
    fclose(archivo);
    return 0;
}