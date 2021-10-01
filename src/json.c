#include "json.h"

int search_value(char *file_name, char *key, char *value)
{
    //Creo las variables con las que voy a trabajar
    FILE *archivo;
    char ch;
    char buffer[500];
    int cant_llaves=0, cant_comillas=0, cant_caracteres=0, i, j, tamanio_key, tamanio_lectura;

    //Abro el archivo provisto en el path_name
    archivo = fopen(file_name, "r");
    archivo = (FILE*) file_name;

    printf("\n%s\n", (char*)archivo);

    //Si hay algún problema abriendo el archivo, la función retorna -1
    if(!archivo)
        return -1;

    //Cuenta la cantidad de caracteres que tiene el archivo
    while((ch = fgetc(archivo)) != EOF)
    {
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

    //Cierro archivo y retorno '1' para indicar el éxito de la operación
    fclose(archivo);
    return 1;
}

int replace_value_file(char *file_name, char *key, char *value)
{
    //Creo las variables con las que voy a trabajar
    FILE *archivo;
    char ch;
    char *aux;
    char buffer[500];
    int cant_llaves=0, cant_comillas=0, cant_caracteres=0,i, j, tamanio_key, tamanio_lectura, delta, name_ocur=0, tamanio_copia;

    //Abro el archivo provisto en el path_name
    archivo = fopen(file_name, "r+");

    //Si hay algún problema abriendo el archivo, la función retorna -1
    if(!archivo)
        return -1;

    //Cuenta la cantidad de caracteres que tiene el archivo
    while((ch = fgetc(archivo)) != EOF)
    {
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

            //Escribo la palabra
            fprintf(archivo,"%s",value);

            //Coloco comillas y fin de linea
            fputc('\"', archivo);
            fputc('\n', archivo);

            //Escribo lo que resta del archivo
            fprintf(archivo,"%s",aux);              

        }
    }

    //Cierro archivo y retorno '1' para indicar el éxito de la operación
    fclose(archivo);
    return 1;
}

int replace_value(char *file_name, char *key, char *value)
{
    //Creo las variables con las que voy a trabajar
    FILE *archivo;
    char ch;
    char *aux;
    int cant_llaves=0, cant_comillas=0, cant_caracteres=0, i, j, k, tamanio_key, tamanio_lectura, delta, name_ocur=0, tamanio_copia;

    //Abro el archivo provisto en el path_name
    archivo = fopen(file_name, "r+");
    archivo = (FILE*) file_name;

    //Si hay algún problema abriendo el archivo, la función retorna -1
    if(!archivo)
        return -1;

    cant_caracteres= strlen(file_name);
    i=0;
    //Cuenta la cantidad de caracteres que tiene el archivo
    while(i < cant_caracteres)
    {
      //cant_caracteres++;
      if(file_name[i]=='{')
        cant_llaves++;       

      i++; 
    }
    
    printf("Cantidad de caracteres:%d \n", cant_caracteres);
    //Rebobina el cursor
    //rewind(archivo);

    //Si la cantidad de llaves es menor a dos, entonces el formato es incorrecto (retorna -2)
    if(cant_llaves<2)
        return -2;

    printf("Cantidad de llaves:%d \n", cant_llaves);
    //Determina el tamaño de la palabra clave
    tamanio_key = strlen(key);
    char *search_key= malloc(sizeof(char) * tamanio_key+ 1);

    printf("TAM DE KEY %s: %d\n", key, tamanio_key);

    //Se mueve el cursor a lo largo del archivo para buscar la palabra clave
    for(i=0;i<cant_caracteres;i++)
    {
        //Por cada pasada, pongo la cantidad de comillas enumeradas en 0
        cant_comillas=0;

        //Ubico el índice en función al loop
        //fseek(archivo,i,0);

        //Leo una string del tamaño de la palabra clave y la comparo con la misma
        //fgets(buffer,tamanio_key+1,archivo);
        for(j=0; j<tamanio_key; j++)
        {
            search_key[j]= file_name[i+j];
        }

        search_key[tamanio_key]= '\0';

        if(strcmp(search_key,key)==0)
        {
            printf("ENCONTRE ALGO!\n");
            if((strcmp(key,"name")==0) && name_ocur==0)
            {
                name_ocur=1;
                continue;
            }
            
            i += tamanio_key+1; 

            while(cant_comillas<2)
            {
                if(file_name[i]=='"')
                    cant_comillas++;
                
                i++;
            }

            //Me muevo tres posiciones para ubicarme en el comienzo de lo que quiero leer
            i++; //fgetc(archivo);
            i++; //fgetc(archivo);
            i++; //fgetc(archivo);

            j= i;
            
            while(file_name[j]!='\n')
            {
                j++;
            }

            char* buffer = malloc(j);
            
            for(k=0; k<j; k++)
            {
                buffer[k]= file_name[i];
                i++;
            }

            buffer[j]= '\0';
            printf("%s\n", buffer);

            
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

            //Escribo la palabra
            fprintf(archivo,"%s",value);
            for(j=0; value[j]!='\0'; j++)
            {
                file_name[i]= value[j];
                i++;
            }

            //Coloco comillas 
            file_name[i]='\"';
            i++;
            //Coloco fin de linea
            file_name[i]='\n';
            i++;

            //Escribo lo que resta del archivo
            fprintf(archivo,"%s",aux);              

        }
    }

    printf("LLEGUE AL FINAL!\n");
    //Cierro archivo y retorno '1' para indicar el éxito de la operación
    fclose(archivo);
    return 1;
}