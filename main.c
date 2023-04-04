//*****************************************************************
// PROYECTO 1 SISTEMAS OPERATIVOS (EN C)
// Grupo 5: María Andrea Mendez, Michael Joel Gonzalez y Santiago Castro
// Compilación: "gcc main.c -o imgconc"
// Ejecución: "./imgconc –i imagenIn –t imagenOut –o opcion –h nhilos"
// Observaciones "imagen.bmp" es un BMP de 24 bits

//*****************************************************************

//*****************************************************************
// LIBRERIAS INCLUIDAS
//*****************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

//*****************************************************************
// DECLARACION DE ESTRUCTURAS
//*****************************************************************
// Estructura para almacenar la cabecera de la imagen BMP y un apuntador a la
// matriz de pixeles
typedef struct BMP {
  char bm[2];    //(2 Bytes) BM (Tipo de archivo)
  int tamano;    //(4 Bytes) Tamaño del archivo en bytes
  int reservado; //(4 Bytes) Reservado
  int offset; //(4 Bytes) offset, distancia en bytes entre la img y los píxeles
  int tamanoMetadatos;    //(4 Bytes) Tamaño de Metadatos (tamaño de esta
                          // estructura = 40)
  int alto;               //(4 Bytes) Ancho (numero de píxeles horizontales)
  int ancho;              //(4 Bytes) Alto (numero de pixeles verticales)
  short int numeroPlanos; //(2 Bytes) Numero de planos de color
  short int profundidadColor; //(2 Bytes) Profundidad de color (debe ser 24 para
                              // nuestro caso)
  int tipoCompresion; //(4 Bytes) Tipo de compresión (Vale 0, ya que el bmp es
                      // descomprimido)
  int tamanoEstructura;   //(4 Bytes) Tamaño de la estructura Imagen (Paleta)
  int pxmh;               //(4 Bytes) Píxeles por metro horizontal
  int pxmv;               //(4 Bytes) Píxeles por metro vertical
  int coloresUsados;      //(4 Bytes) Cantidad de colores usados
  int coloresImportantes; //(4 Bytes) Cantidad de colores importantes
  unsigned char ***pixel; // Puntero a una tabla dinamica de caracteres de 3
                          // dimensiones para almacenar los pixeles
} BMP;

//*****************************************************************
// DECLARACIÓN DE FUNCIONES
//*****************************************************************
void validar_extension(char *nombre_archivo,
                       char *extension); // Función que verifica la extención
void validar_args(int argc, char *argv[], char *imagenIn, char *imagenOut,
                  int *opcion, int *nHilos);
void abrir_imagen(BMP *imagen, char ruta[]); // Función para abrir la imagen BMP
void crear_imagen(BMP *imagen, char ruta[]); // Función para crear una imagen
                                             // BMP
void convertir1(BMP *imagen, int nhilos);
void convertir2(BMP *imagen, int nhilos);
void convertir3(BMP *imagen, int nhilos);

//*****************************************************************
// PROGRAMA PRINCIPAL
//*****************************************************************
int main(int argc, char *argv[]) {
  BMP img; // Estructura de tipo imágen
  char IMAGEN[45], IMAGEN_TRATADA[45];
  int opcion, numHilos;

  validar_args(argc, argv, IMAGEN, IMAGEN_TRATADA, &opcion, &numHilos);

  abrir_imagen(&img, IMAGEN);
  printf("\n*****************************************************************");
  printf("\nIMAGEN: %s", IMAGEN);
  printf("\n*****************************************************************");
  printf("\nDimensiones de la imágen:\tAlto=%d\tAncho=%d\n", img.alto,
         img.ancho);

  switch (opcion) {
  case 1:
    convertir1(&img, numHilos);
    break;
  case 2:
    convertir2(&img, numHilos);
    break;
  case 3:
    convertir3(&img, numHilos);
    break;
  default:
    perror("\n\nLa opción ingresada no es válida (debe ser del 1 al 3)\n\n");
    exit(1);
    break;
  }

  crear_imagen(&img, IMAGEN_TRATADA);
  printf("\nImágen BMP tratada en el archivo: %s\n", IMAGEN_TRATADA);

  return 0;
}

//************************************************************************
// FUNCIONES
//************************************************************************

void validar_extension(char *nombre_archivo, char *extension) {
  char delimitador[] = ".";
  char tmp[45], *token, token_anterior[10];

  strcpy(tmp, nombre_archivo);
  token = strtok(tmp, delimitador);

  if (token != NULL) {
    while (token != NULL) {

      // Sólo en la primera pasamos la cadena; en las siguientes pasamos NULL
      strcpy(token_anterior, token);
      token = strtok(NULL, delimitador);
    }
  }
  if (strcmp(token_anterior, "bmp") != 0) {
    printf("\nEl archivo '%s' no tiene extension .%s\n\n", nombre_archivo,
           extension);
    exit(1);
  }
}

void validar_args(int argc, char *argv[], char *imagenIn, char *imagenOut,
                  int *opcion, int *nHilos) {
  if (argc != 9) {
    printf("\nIndique el nombre de la imagen, el nombre de la imagen de "
           "salida, la opción (1 a 3) y el número de hilos (> 0) - Ejemplo:\n\n"
           "~/proyecto1-sisop$ %s -i imagen.bmp -t imagen-tratada.bmp -o 2 -h "
           "5\n\n",
           argv[0]);
    exit(1);
  } else {
    for (int i = 1; i < 9; i += 2) {
      if (strcmp(argv[i], "-i") == 0) {
        validar_extension(argv[i + 1], "bmp");

        strcpy(imagenIn, argv[i + 1]);

      } else if (strcmp(argv[i], "-t") == 0) {
        validar_extension(argv[i + 1], "bmp");

        strcpy(imagenOut, argv[i + 1]);

      } else if (strcmp(argv[i], "-o") == 0) {

        *opcion = atoi(argv[i + 1]);

      } else if (strcmp(argv[i], "-h") == 0) {

        *nHilos = atoi(argv[i + 1]);

      } else {
        printf("\nLa flag %s no es válida\n\n", argv[i]);
        exit(1);
      }
    }
  }
}

void abrir_imagen(BMP *imagen, char *ruta) {
  FILE *archivo; // Puntero FILE para el archivo de imágen a abrir
  int i, j, k;
  unsigned char P[3];

  // Abrir el archivo de imágen
  archivo = fopen(ruta, "rb+");
  if (!archivo) {
    // Si la imágen no se encuentra en la ruta dada
    printf("La imágen %s no se encontro\n", ruta);
    exit(1);
  }

  // Leer la cabecera de la imagen y almacenarla en la estructura a la que
  // apunta imagen
  fseek(archivo, 0, SEEK_SET);
  fread(&imagen->bm, sizeof(char), 2, archivo);
  fread(&imagen->tamano, sizeof(int), 1, archivo);
  fread(&imagen->reservado, sizeof(int), 1, archivo);
  fread(&imagen->offset, sizeof(int), 1, archivo);
  fread(&imagen->tamanoMetadatos, sizeof(int), 1, archivo);
  fread(&imagen->alto, sizeof(int), 1, archivo);
  fread(&imagen->ancho, sizeof(int), 1, archivo);
  fread(&imagen->numeroPlanos, sizeof(short int), 1, archivo);
  fread(&imagen->profundidadColor, sizeof(short int), 1, archivo);
  fread(&imagen->tipoCompresion, sizeof(int), 1, archivo);
  fread(&imagen->tamanoEstructura, sizeof(int), 1, archivo);
  fread(&imagen->pxmh, sizeof(int), 1, archivo);
  fread(&imagen->pxmv, sizeof(int), 1, archivo);
  fread(&imagen->coloresUsados, sizeof(int), 1, archivo);
  fread(&imagen->coloresImportantes, sizeof(int), 1, archivo);

  // Validar ciertos datos de la cabecera de la imágen
  if (imagen->bm[0] != 'B' || imagen->bm[1] != 'M') {
    printf("La imagen debe ser un bitmap.\n");
    exit(1);
  }
  if (imagen->profundidadColor != 24) {
    printf("La imagen debe ser de 24 bits.\n");
    exit(1);
  }

  // Reservar memoria para la matriz de pixels

  imagen->pixel = malloc(imagen->alto * sizeof(char *));
  for (i = 0; i < imagen->alto; i++) {
    imagen->pixel[i] = malloc(imagen->ancho * sizeof(char *));
  }

  for (i = 0; i < imagen->alto; i++) {
    for (j = 0; j < imagen->ancho; j++)
      imagen->pixel[i][j] = malloc(3 * sizeof(char));
  }

  // Pasar la imágen a el arreglo reservado en escala de grises
  // unsigned char R,B,G;

  for (i = 0; i < imagen->alto; i++) {
    for (j = 0; j < imagen->ancho; j++) {
      for (k = 0; k < 3; k++) {
        fread(&P[k], sizeof(char), 1, archivo);       // Byte Blue del pixel
        imagen->pixel[i][j][k] = (unsigned char)P[k]; // Formula correcta
      }
    }
  }

  // Cerrrar el archivo
  fclose(archivo);
}

void crear_imagen(BMP *imagen, char *ruta) {
  FILE *archivo; // Puntero FILE para el archivo de imágen a abrir

  int i, j, k;

  // Abrir el archivo de imágen
  archivo = fopen(ruta, "wb+");
  if (!archivo) {
    // Si la imágen no se encuentra en la ruta dada
    printf("La imágen %s no se pudo crear\n", ruta);
    exit(1);
  }

  // Escribir la cabecera de la imagen en el archivo
  fseek(archivo, 0, SEEK_SET);
  fwrite(&imagen->bm, sizeof(char), 2, archivo);
  fwrite(&imagen->tamano, sizeof(int), 1, archivo);
  fwrite(&imagen->reservado, sizeof(int), 1, archivo);
  fwrite(&imagen->offset, sizeof(int), 1, archivo);
  fwrite(&imagen->tamanoMetadatos, sizeof(int), 1, archivo);
  fwrite(&imagen->alto, sizeof(int), 1, archivo);
  fwrite(&imagen->ancho, sizeof(int), 1, archivo);
  fwrite(&imagen->numeroPlanos, sizeof(short int), 1, archivo);
  fwrite(&imagen->profundidadColor, sizeof(short int), 1, archivo);
  fwrite(&imagen->tipoCompresion, sizeof(int), 1, archivo);
  fwrite(&imagen->tamanoEstructura, sizeof(int), 1, archivo);
  fwrite(&imagen->pxmh, sizeof(int), 1, archivo);
  fwrite(&imagen->pxmv, sizeof(int), 1, archivo);
  fwrite(&imagen->coloresUsados, sizeof(int), 1, archivo);
  fwrite(&imagen->coloresImportantes, sizeof(int), 1, archivo);

  // Pasar la imágen del arreglo reservado en escala de grises a el archivo
  // (Deben escribirse los valores BGR)
  for (i = 0; i < imagen->alto; i++) {
    for (j = 0; j < imagen->ancho; j++) {

      for (k = 0; k < 3; k++)
        fwrite(&imagen->pixel[i][j][k], sizeof(char), 1,
               archivo); // Escribir el Byte Blue del pixel
    }
  }
  // Cerrrar el archivo
  fclose(archivo);
}

void convertir1(BMP *imagen, int nhilos) {
  int i, j, k;

  unsigned char temp;

  for (i = 0; i < imagen->alto; i++) {
    for (j = 0; j < imagen->ancho; j++) {
      temp = (unsigned char)((imagen->pixel[i][j][2] * 0.3) +
                             (imagen->pixel[i][j][1] * 0.59) +
                             (imagen->pixel[i][j][0] * 0.11));
      // temp = (unsigned char)((imagen->pixel[i][j][2] + imagen->pixel[i][j][1]
      // + imagen->pixel[i][j][0]) / 3);

      for (k = 0; k < 3; k++)
        imagen->pixel[i][j][k] = (unsigned char)temp; // Formula correcta
    }
  }
}

void convertir2(BMP *imagen, int nhilos) {
  int i, j, k;

  unsigned char temp;

  for (i = 0; i < imagen->alto; i++) {
    for (j = 0; j < imagen->ancho; j++) {
      // temp = (unsigned
      // char)((imagen->pixel[i][j][2]*0.3)+(imagen->pixel[i][j][1]*0.59)+
      // (imagen->pixel[i][j][0]*0.11));
      temp = (unsigned char)((imagen->pixel[i][j][2] + imagen->pixel[i][j][1] +
                              imagen->pixel[i][j][0]) /
                             3);

      for (k = 0; k < 3; k++)
        imagen->pixel[i][j][k] = (unsigned char)temp; // Formula correcta
    }
  }
}

void convertir3(BMP *imagen, int nhilos) {
  int i, j, k;
  
  for (i = 0; i < imagen->alto; i++) {
    for (j = 0; j < imagen->ancho; j++) {
      //poner transformación aqui
      imagen->pixel[i][j][1] = (unsigned char)imagen->pixel[i][j][1]*-1;//green 0.7
      imagen->pixel[i][j][2] = (unsigned char)imagen->pixel[i][j][2]*1;//red 0.8
      imagen->pixel[i][j][3] = (unsigned char)imagen->pixel[i][j][3]*-1;//blue 0.5
    }
  }
}