/*Aplicacion que permite realizarle a una imagen .bmp (de 24, 8 4 o 1 bits de profundidad) 1.Negativo, 
2.Rotar 90 grados CW, 3.Rotar 90 grados CCW, 4.Rotar 180 grados, 5.Rotar 270 grados CW, 6.Rotar 270 grados CCW, 
7.Espejo Horizontal, 8.Espejo Vertical. Dando como salida una imagen el el mismo formato de entrada 
(24, 8 4 o 1 bits de profundidad) llamada salida.bmp con la operacion escogida por el usuario*/

#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <math.h>  
using namespace std;

int Padding(int ancho, int Modo)
{
	/*Funcion que devuelve el numero de padding o relleno que tiene una imagen
	de 4 o de 1 bit (Siendo el Modo 8 si es de 1 bit y 4 si esta es de 4 bits)*/
	int ancho_nuevo, padding = 0;
	float aux_padding;

	if (ancho % 2 == 0)
	{
		if (Modo == 8)
			ancho_nuevo = ceil(float(ancho) / Modo);
		else
			ancho_nuevo = float(ancho) / Modo;
		
		aux_padding = (float(ancho_nuevo) / 4);
		aux_padding = float(aux_padding - floor(aux_padding));
		aux_padding *= 100;

		switch (int(aux_padding))
		{
		case 75:
			padding = 1;
			break;
		case 50:
			padding = 2;
			break;
		case 25:
			padding = 3;
			break;
		}
	}
	else
	{
		ancho_nuevo = (float(ancho / Modo) + 1);
		aux_padding = (float(ancho_nuevo) / 4);
		aux_padding = float(aux_padding - floor(aux_padding));
		aux_padding *= 100;

		switch (int(aux_padding))
		{
		case 75:
			padding = 1;
			break;
		case 50:
			padding = 2;
			break;
		case 25:
			padding = 3;
			break;
		}
	}
	return padding;
}

void Cambiar_Data(unsigned char *data_info)
{
	/*Metodo que cambia el alto por el ancho en la data de la cabecera, este es usado cuando se rota
	tanto 90 como 270 grados en cualquier direccion*/
	unsigned char aux1, aux2, aux3, aux4;

	aux1 = data_info[4];
	aux2 = data_info[5];
	aux3 = data_info[6];
	aux4 = data_info[7];

	data_info[4] = data_info[8];
	data_info[5] = data_info[9];
	data_info[6] = data_info[10];
	data_info[7] = data_info[11];

	data_info[8] = aux1;
	data_info[9] = aux2;
	data_info[10] = aux3;
	data_info[11] = aux4;
}

void Guardar_Salida(unsigned char *data_header, unsigned char *data_info, unsigned char *paleta, unsigned char *data, int tam_paleta, int longitud)
{
	/*Metodo que guarda la imagen resultante en el direcctorio imagenes bajo el nombre salida.bmp*/
	ofstream Archivo_Salida;

	Archivo_Salida.open("../imagenes/salida.bmp", ios::binary | ios::out);
	Archivo_Salida.write((char*)data_header, 14);
	Archivo_Salida.write((char*)data_info, 40);
	Archivo_Salida.write((char*)paleta, tam_paleta);
	Archivo_Salida.write((char*)data, longitud);
	Archivo_Salida.close();
}

void Guardar_Info(unsigned char *data_info, int &ancho, int &alto, int &tipo)
{
	/*Metodo que guarda el tipo de imagen que seria en este caso de cuantos bits de profundidad es,
	tambien guarda el ancho y el alto de la imagen que se desea modificar*/
	tipo = int(data_info[14]);

	ancho = data_info[4];
	ancho = (data_info[5] << 8) | ancho;
	ancho = (data_info[6] << 8) | ancho;
	ancho = (data_info[7] << 8) | ancho;

	alto = data_info[8];
	alto = (data_info[9] << 8) | alto;
	alto = (data_info[10] << 8) | alto;
	alto = (data_info[11] << 8) | alto;
}

void Negativo(unsigned char *datos, int tam)
{
	/*Metodo que modifica la paleta en caso que la imagen sea menor a 24 bits de profundidad
	y cuando esta es de 24 se modifica pixel a pixel el RGB de ca uno de estos para lograr
	el negativo de la imagen*/
	int i;
	for (i = 0; i < tam; ++i)
		datos[i] = 255 - datos[i];
}

unsigned char * Rotar_90CW(unsigned char *data, int &tam, int tipo, int &ancho, int &alto)
{
	/*Funcion donde generamos un nuevo arreglo de bytes donde vamos a tener la imagen rotada 90 grados a la derecha.
	El tamano de este arreglo va a variar segun sea el padding de la imagen anteriormente y el padding de la imagen rotada
	por ello debemos tener en cuanta hacer dichos calculos*/

	int i, l, j, k, w, padding_n, auxiliar, padding_a, nuevo_ancho, temp;
	unsigned char *data_2;
	unsigned int x;
	if (tipo < 24)
	{
		switch (tipo)
		{
		case 1:
			nuevo_ancho = ceil((float(ancho) / 8)); // Calculamos en ancho real en bytes
			i = 1, l = 0, j = 1, k = 0, w = 0, padding_n = Padding(alto, 8), padding_a = Padding(ancho, 8); //Calculamos el padding anterior y el nuevo
			x = ancho % 8, w = 0;

			if (x == 0) //Calculamos cuantos bits de padding hay en el ultimo byte de la fila
				x = 0;
			else
				x = 8 - x;

			if (alto % 8 == 0) // Calculamos el nuevo tamano del arreglo segun el alto de la imagen (nuevo ancho)
			{
				data_2 = new unsigned char[tam + (ancho * padding_n) - (alto * padding_a)];
				tam = tam + (ancho * padding_n) - (alto * padding_a);
			}
			else
			{
				data_2 = new unsigned char[tam + (ancho * padding_n) - (alto * padding_a) + ancho];
				tam = tam + (ancho * padding_n) - (alto * padding_a) + ancho;
			}

			while (k < tam)
			{
				if (i == alto + 1)
				{
					if (w >= 1) //Agregamos padding en el ultimo byte si es necesario
					{
						data_2[k] = data_2[k] << (8 - w);
						k++;
					}

					i = 1;
					l = 0;
					w = 0;

					if (x == 7)
					{
						x = 0;
						j++;
					}
					else
						x++;

					for (int w = k; w < k + padding_n; w++) //Agregamos padding al final de la fila
						data_2[k] = 0x00;

					k += padding_n;
				}

				if (w == 0)
				{
					data_2[k] = ((data[(nuevo_ancho * i) - j + (padding_a * l)]) & (1 << x)) >> x;
					w++;
				}
				else
				{
					data_2[k] = (data_2[k] << 1) | (((data[(nuevo_ancho * i) - j + (padding_a * l)]) & (1 << x)) >> x);
					w++;

					if (w == 8)
					{
						k++;
						w = 0;
					}
				}

				l++;
				i++;
			}
			auxiliar = ancho;
			ancho = alto;
			alto = auxiliar;
			return data_2;
			break;

		case 4:
			nuevo_ancho = round((float(ancho) / 2));// Calculamos en ancho real en bytes
			i = 1, l = 0, j = 1, k = 0, w = 0, padding_n = Padding(alto, 2), padding_a = Padding(ancho, 2); //Calculamos el padding anterior y el nuevo
			bool primero;

			if (ancho % 2 == 0) //Calculamos si hay padding en el ultimo byte de la fila
				primero = false;
			else
				primero = true;

			if (alto % 2 == 0) // Calculamos el nuevo tamano del arreglo segun el alto de la imagen (nuevo ancho)
			{
				if (!primero)
				{
					data_2 = new unsigned char[tam + (ancho * padding_n) - (alto * padding_a)];
					tam = tam + (ancho * padding_n) - (alto * padding_a);
				}
				else
				{
					data_2 = new unsigned char[tam + (ancho * padding_n) - (alto * padding_a) - (alto / 2)];
					tam = tam + (ancho * padding_n) - (alto * padding_a) - (alto / 2);
				}
			}
			else
			{
				if (primero)
				{
					data_2 = new unsigned char[tam + (ancho * padding_n) - (alto * padding_a) + (ancho / 2) - (alto / 2)];
					tam = tam + (ancho * padding_n) - (alto * padding_a) + (ancho / 2) - (alto / 2);
				}
				else
				{
					data_2 = new unsigned char[tam + (ancho * padding_n) - (alto * padding_a) + nuevo_ancho];
					tam = tam + (ancho * padding_n) - (alto * padding_a) + nuevo_ancho;
				}
			}

			while (k < tam)
			{
				if (i == alto + 1)
				{
					if (w == 1) //Agregamos padding en el ultimo byte si es necesario
					{
						data_2[k] = data_2[k] << 4;
						k++;
					}

					i = 1;
					l = 0;
					w = 0;

					if (primero)
					{
						primero = false;
						j++;
					}
					else
						primero = true;

					for (int w = k; w < k + padding_n; w++) //Agregamos padding al final de la fila
						data_2[k] = 0x00;

					k += padding_n;
				}

				if (primero) // Tomamos los primeros cuatro bits del byte
				{
					if (w == 0)
					{
						w = 1;
						data_2[k] = (data[(nuevo_ancho * i) - j + (padding_a * l)]) >> 4;
					}
					else
					{
						w = 0;
						data_2[k] = (data_2[k] << 4) | ((data[(nuevo_ancho * i) - j + (padding_a * l)]) >> 4);
						k++;
					}
				}
				else // Tomamos los ultimos cuatro bits del byte
				{
					if (w == 0)
					{
						w = 1;
						data_2[k] = ((data[(nuevo_ancho * i) - j + (padding_a * l)]) & 15);
					}
					else
					{
						w = 0;
						data_2[k] = (data_2[k] << 4) | ((data[(nuevo_ancho * i) - j + (padding_a * l)]) & 15);
						k++;
					}
				}
				l++;
				i++;
			}
			auxiliar = ancho;
			ancho = alto;
			alto = auxiliar;
			return data_2;
			break;

		case 8:
			i = 1, l = 0, j = 1, k = 0, padding_n = 4 - alto % 4, auxiliar, padding_a = 4 - ancho % 4; //Calculamos el padding anterior y el nuevo

			if (padding_n == 4)
				padding_n = 0;
			if (padding_a == 4)
				padding_a = 0;

			data_2 = new unsigned char[tam + (ancho * padding_n) - (alto * padding_a)]; // Calculamos el nuevo tamano del arreglo segun el alto de la imagen (nuevo ancho)
			tam = tam + (ancho * padding_n) - (alto * padding_a);

			while (k < tam)
			{
				if (i == alto + 1) {
					i = 1;
					l = 0;
					j++;
					for (int w = k; w < k + padding_n; w++) //Agregamos padding al final de la fila
						data_2[k] = 0x00;
					k += padding_n;
				}
				data_2[k] = data[(ancho * i) - j + (padding_a * l)];
				k++;
				l++;
				i++;
			}
			auxiliar = ancho;
			ancho = alto;
			alto = auxiliar;
			return data_2;
			break;
		}
	}
	else
	{
		i = 1, l = 0, j = 3, k = 0, padding_n = 4 - (alto * 3) % 4, auxiliar, padding_a = 4 - (ancho * 3) % 4, temp; //Calculamos el padding anterior y el nuevo

		if (padding_n == 4)
			padding_n = 0;
		if (padding_a == 4)
			padding_a = 0;

		data_2 = new unsigned char[tam + (ancho * padding_n) - (alto * padding_a)]; // Calculamos el nuevo tamano del arreglo segun el alto de la imagen (nuevo ancho)
		tam = tam + (ancho * padding_n) - (alto * padding_a);

		while (k < tam)
		{
			if (i == alto + 1) {
				i = 1;
				l = 0;
				j += 3;
				for (int x = k; x < k + padding_n; x++) //Agregamos padding al final de la fila
					data_2[k] = 0x00;
				k += padding_n;
			}
			temp = ((ancho * 3) * i) - j + (padding_a * l);
			data_2[k] = data[temp];
			data_2[k + 1] = data[temp + 1];
			data_2[k + 2] = data[temp + 2];
			k += 3;
			l++;
			i++;
		}
		auxiliar = ancho;
		ancho = alto;
		alto = auxiliar;
		return data_2;
	}
	return 0;
}

void EspejoV(unsigned char *data, int tam, int tipo, int ancho, int alto)
{
	/*Accion donde cambiamos uno a uno los bytes de la imagen de tal forma que a traves 
	de esto se logre el espejo vertical de dicha imagen (cabe destacar que nos saltamos el 
	padding, ya que deveriamos ignorarlo)*/

	int limite = tam / 2, i, j, k, width, padding_a; //Calculamos el limite donde vamos a trabajar, ya que el cambio es uno a uno el limite es la mitad del tamano
	unsigned char auxiliar;

	if (tipo < 24)
	{
		switch (tipo)
		{
		case 1:
			padding_a = Padding(ancho, 8);   //Calculamos el padding 
			width = ceil(float(ancho) / 8);  //Calculamos el ancho real de la imagen en bytes(sin tomar en cuenta el padding)
			i = alto - 1, j = 0, k = 0;     
			while (k < limite)
			{
				if (j == width) { //Nos cambiamos de fila al ya modificar los pixeles de la fila en donde estabamos 
					j = 0;
					i--;
					k += padding_a; //Nos saltamos el padding
				}
				auxiliar = data[(width * i) + (padding_a * i) + j];
				data[(width * i) + (padding_a * i) + j] = data[k];
				data[k] = auxiliar;
				j++;
				k++;
			}
			break;
		case 4:
			padding_a = Padding(ancho, 2); //Calculamos el padding 
			width = round(float(ancho) / 2); //Calculamos el ancho real de la imagen en bytes(sin tomar en cuenta el padding)
			i = alto - 1, j = 0, k = 0;
			while (k < limite)
			{
				if (j == width) { //Nos cambiamos de fila al ya modificar los pixeles de la fila en donde estabamos 
					j = 0;
					i--;
					k += padding_a; //Nos saltamos el padding
				}
				auxiliar = data[(width * i) + (padding_a * i) + j];
				data[(width * i) + (padding_a * i) + j] = data[k];
				data[k] = auxiliar;
				j++;
				k++;
			}
			break;
		case 8:
			padding_a = 4 - ancho % 4; //Calculamos el padding 

			if (padding_a == 4) //Calculamos el padding 
				padding_a = 0;

			i = alto - 1, j = 0, k = 0;
			while (k < limite)
			{
				if (j == ancho) { //Nos cambiamos de fila al ya modificar los pixeles de la fila en donde estabamos 
					j = 0;
					i--;
					k += padding_a; //Nos saltamos el padding
				}
				auxiliar = data[(ancho * i) + (padding_a * i) + j];
				data[(ancho * i) + (padding_a * i) + j] = data[k];
				data[k] = auxiliar;
				j++;
				k++;
			}
			break;
		}
	}
	else
	{
		
		padding_a = 4 - (ancho * 3) % 4; //Calculamos el padding 

		if (padding_a == 4) //Calculamos el padding 
			padding_a = 0;

		width = ancho * 3; //Calculamos el ancho real de la imagen en bytes(sin tomar en cuenta el padding)
		i = alto - 1, j = 0, k = 0;
		while (k < limite)
		{
			if (j == width) { //Nos cambiamos de fila al ya modificar los pixeles de la fila en donde estabamos 
				j = 0;
				i--;
				k += padding_a; //Nos saltamos el padding
			}
			auxiliar = data[(width * i) + (padding_a * i) + j];
			data[(width * i) + (padding_a * i) + j] = data[k];
			data[k] = auxiliar;
			j++;
			k++;
		}
	}
}

int main()
{
	unsigned char *Data, *Data_header, *Data_info, *Paleta = NULL, *Rotada90, *Rotada90_2, *Rotada90_3;
	int Operacion, Longitud, Ancho, Alto, Tipo, Tam_paleta;
	string NombreArchivo, Ruta;
	const char * Ruta_Valida;
	ifstream Imagen;
	bool Parada;
	
	Parada = false, Tam_paleta = 0;
	
	while (!Parada)
	{
		cout << "Introduzca el nombre del Archivo .bmp" << endl;
		cin >> NombreArchivo;
		
		Tam_paleta = 0, Paleta = NULL;
		Ruta = "../imagenes/" + NombreArchivo;
		Ruta_Valida = Ruta.c_str();

		Imagen.open(Ruta_Valida, ios::binary | ios::in);
			Imagen.seekg(0, Imagen.end);
			Longitud = Imagen.tellg();
		if (Longitud > 0)
		{
			Imagen.seekg(0, Imagen.beg);
			Longitud -= 54;
			Data_header = new unsigned char[14];
			Data_info = new unsigned char[40];
			Data = new unsigned char[Longitud];

			Imagen.read((char*)Data_header, 14);
			Imagen.read((char*)Data_info, 40);
			Guardar_Info(Data_info, Ancho, Alto, Tipo);

			if (Tipo < 24)
			{
				switch (Tipo)
				{
				case 1:
					Paleta = new unsigned char[8];
					Tam_paleta = 8;
					break;
				case 4:
					Paleta = new unsigned char[64];
					Tam_paleta = 64;
					break;
				case 8:
					Paleta = new unsigned char[1024];
					Tam_paleta = 1024;
					break;
				}
				Imagen.read((char*)Paleta, Tam_paleta);
				Longitud -= Tam_paleta;
				Imagen.read((char*)Data, Longitud);
			}
			else
				Imagen.read((char*)Data, Longitud);
			Imagen.close();
			
			cout << endl;
			cout << "Introduzca el numero de la operacion a realizar" << endl;
			cout << "1.Negativo" << endl;
			cout << "2.Rotar 90 grados CW" << endl;
			cout << "3.Rotar 90 grados CCW" << endl;
			cout << "4.Rotar 180 grados" << endl;
			cout << "5.Rotar 270 grados CW" << endl;
			cout << "6.Rotar 270 grados CCW" << endl;
			cout << "7.Espejo Horizontal" << endl;
			cout << "8.Espejo Vertical" << endl;
			cout << "9.Salir" << endl;
			cin >> Operacion;

			switch (Operacion)
			{
			case 1:
				if (Tipo < 24)
					Negativo(Paleta, Tam_paleta);
				else
					Negativo(Data, Longitud);
				Guardar_Salida(Data_header, Data_info, Paleta, Data, Tam_paleta, Longitud);
				break;
			case 2:
				Rotada90 = Rotar_90CW(Data, Longitud, Tipo, Ancho, Alto);
				Cambiar_Data(Data_info);
				Guardar_Salida(Data_header, Data_info, Paleta, Rotada90, Tam_paleta, Longitud);
				break;
			case 3:
				Rotada90 = Rotar_90CW(Data, Longitud, Tipo, Ancho, Alto);
				Rotada90_2 = Rotar_90CW(Rotada90, Longitud, Tipo, Ancho, Alto);
				Rotada90_3 = Rotar_90CW(Rotada90_2, Longitud, Tipo, Ancho, Alto);
				Cambiar_Data(Data_info);
				Guardar_Salida(Data_header, Data_info, Paleta, Rotada90_3, Tam_paleta, Longitud);
				break;
			case 4:
				Rotada90 = Rotar_90CW(Data, Longitud, Tipo, Ancho, Alto);
				Rotada90_2 = Rotar_90CW(Rotada90, Longitud, Tipo, Ancho, Alto);
				Guardar_Salida(Data_header, Data_info, Paleta, Rotada90_2, Tam_paleta, Longitud);
				break;
			case 5:
				Rotada90 = Rotar_90CW(Data, Longitud, Tipo, Ancho, Alto);
				Rotada90_2 = Rotar_90CW(Rotada90, Longitud, Tipo, Ancho, Alto);
				Rotada90_3 = Rotar_90CW(Rotada90_2, Longitud, Tipo, Ancho, Alto);
				Cambiar_Data(Data_info);
				Guardar_Salida(Data_header, Data_info, Paleta, Rotada90_3, Tam_paleta, Longitud);
				break;
			case 6:
				Rotada90 = Rotar_90CW(Data, Longitud, Tipo, Ancho, Alto);
				Cambiar_Data(Data_info);
				Guardar_Salida(Data_header, Data_info, Paleta, Rotada90, Tam_paleta, Longitud);
				break;
			case 7:
				EspejoV(Data, Longitud, Tipo, Ancho, Alto);
				Rotada90 = Rotar_90CW(Data, Longitud, Tipo, Ancho, Alto);
				Rotada90_2 = Rotar_90CW(Rotada90, Longitud, Tipo, Ancho, Alto);
				Guardar_Salida(Data_header, Data_info, Paleta, Rotada90_2, Tam_paleta, Longitud);
				break;
			case 8:
				EspejoV(Data, Longitud, Tipo, Ancho, Alto);
				Guardar_Salida(Data_header, Data_info, Paleta, Data, Tam_paleta, Longitud);
				break;
			case 9:
				Parada = true;
				break;
			}
			cout << endl;

			delete[] Data_header;
			delete[] Data_info;
			delete[] Data;
			delete[] Paleta;
		}
		else
		{
			Imagen.close();
			cout << "Imagen no encontrada...Por favor introduzca un nombre que este dentro de la carpeta imagenes" << endl;
		}
	}
	return 0;
}