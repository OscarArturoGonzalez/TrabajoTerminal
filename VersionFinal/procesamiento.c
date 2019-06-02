#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "imagen.h"
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define PUERTO 			5000	//Número de puerto asignado al servidor
#define COLA_CLIENTES 	5 		//Tamaño de la cola de espera para clientes
#define TAM_BUFFER 		30

#define DIMASK 3
#define NUM_HILOS 4

#define NumInput 7
#define NumHidden 5
#define NumOutput 1

unsigned char * RGBToGray(unsigned char *imagenRGB, uint32_t width, uint32_t height);
void guardarBMP(char *filename, bmpInfoHeader *info, unsigned char *imgdata);
void GrayToRGB(unsigned char *imagenRGB, unsigned char *imagenGray,  uint32_t width, uint32_t height);
void brilloImagen(unsigned char* imagenGray, uint32_t width, uint32_t height);
unsigned char * reservarMemoria(uint32_t width, uint32_t height);
unsigned char * sustracImagen(unsigned char* imagenA, unsigned char* imagenB, uint32_t width, uint32_t height);
void * filtroGaussiano(void *idh);

int thotsu(unsigned char * imagen, uint32_t width, uint32_t height );
unsigned char * binarizar(unsigned char * imagen, int umbral, uint32_t width, uint32_t height);
void* subImagen(void *arg);
void* sauvola(void *arg);
void * dilata(void *arg);

void * momentosGeometricos(void *idh);
void momentosHu(void);
long double normalizaPhi(long double phi,int moment);
int clasifica(void);

FILE *archivo, *archivo2;

bmpInfoHeader info;
unsigned char *imagenRGB, *imagenGray, *imagenFiltroA, *imagenFiltroB, *imagenResultado, *imagenAux, *imgBin, *otsu, *imgSau;

char str[25];//25
char str2[25];
int valor, x=1;
char str3[100];
char comando[]="raspistill -n -t 500 -e bmp -w 640 -h 480 -o ";


long double m00=0, m10=0, m01=0, m11=0, m20=0, m02=0, m30=0, m03=0, m12=0, m21=0;
long double *phi;
// unsigned char* cropROI(unsigned char* imagen, uint32_t width, uint32_t height);
// int Xmay=0, Ymay=0, Xmin=0, Ymin=0, ancho=0, alto=0;
pthread_mutex_t bloqueo;

double Output[NumOutput+1];
double SumH[NumHidden+1];
double SumO[NumHidden+1];
double Hidden[NumHidden+1];

double WeightIH[NumInput+1][NumHidden+1]={{0,-3.637177,2.204362,7.657526,-2.040084,1.599772},
										  {0,-1.735487,-10.055460,20.850000,-6.787068,1.648733},
										  {0,-1.472776,0.307812,2.479223,1.941254,9.358409},
										  {0,4.042807,3.552047,18.937120,5.880976,4.420268},
										  {0,-0.141333,-11.287840,-34.392500,-11.659490,-6.127501},
										  {0,2.239746,-0.881157,3.796947,-0.205593,1.078233},
										  {0,0.507813,-0.853734,-3.925476,0.475100,0.120820},
										  {0,-1.520642,-1.079569,-10.190050,0.447602,-3.154178}};

double WeightHO[NumHidden+1][NumOutput+1]={{0,-7.203300},             
										   {0,8.291727},
										   {0,14.234720},	        
										   {0,21.333350},
										   {0,-13.898290},
										   {0,-16.146340}}; 										/*
										 {{0	,bias-1	,bias-2	},
										  {0	,1-1	,1-2	}, <<---(input 1 con neurona oculta 1, input 1 con neurona oculta 2, ... )
										  {0	,2-1	,2-2	}};*/
                    
                                                        




int main(void)
{
	register int nh;
	int resultado=0;
    int *hilo, nhs[NUM_HILOS], i, k;//nhs arreglo de identificadores para hilos, para cada hilo (correctamente)
	pthread_t tids[NUM_HILOS]; //arreglo de identificadores 

	archivo = fopen("momentosHu.txt", "w+");
	archivo2 = fopen("clasificacion.txt", "w+");

	char carpeta[]="Cam/";

	sprintf(str3,"%s%sbase.bmp",comando,carpeta);
	system(str3);

	sprintf(str,"%sbase.bmp",carpeta);
	imagenRGB = abrirBMP(str, &info);	
	displayInfo(&info);//informacion de la imagen 

	imagenGray=RGBToGray(imagenRGB, info.width, info.height);
	//GrayToRGB(imagenRGB, imagenGray, info.width, info.height);
	//guardarBMP("Resultados/EscalaGris1.bmp", &info, imagenRGB);
	
	/*Filtro*/
	imagenAux=reservarMemoria(info.width,info.height);

	for(nh=0; nh < NUM_HILOS; nh++)
    {
         nhs[nh]=nh;
         pthread_create(&tids[nh], NULL, filtroGaussiano, (void *)&nhs[nh]); //el ultimo parametro envia dir del ultimo hilo(indice)
    }
	
	for( nh=0; nh < NUM_HILOS; nh++ )
    {
         pthread_join(tids[nh], (void **)&hilo);
         printf("\nHilo %d terminado\n",*hilo );
    }
    
    /***********************************************/
    
	imagenFiltroA = imagenAux;

	//GrayToRGB(imagenRGB, imagenAux, info.width, info.height);
	//guardarBMP("filtroA.bmp", &info, imagenRGB);
	
	/*Inicializar lectura del sensor*/
	wiringPiSetup();
	pinMode(0, INPUT);

	/************Configuración de Red*************/


	int sockfd, cliente_sockfd;
   	struct sockaddr_in direccion_servidor; //Estructura de la familia AF_INET, que almacena direccion
   	char leer_mensaje[TAM_BUFFER];
/*	
 *	AF_INET - Protocolo de internet IPV4
 *  htons - El ordenamiento de bytes en la red es siempre big-endian, por lo que
 *  en arquitecturas little-endian se deben revertir los bytes
 *  INADDR_ANY - Se elige cualquier interfaz de entrada
 */	
   	memset (&direccion_servidor, 0, sizeof (direccion_servidor));	//se limpia la estructura con ceros
   	direccion_servidor.sin_family 		= AF_INET;
   	direccion_servidor.sin_port 		= htons(PUERTO); /*htons::little endian(usada por la computadora) o big endian(el ultimo bit el más grande)*/
   	direccion_servidor.sin_addr.s_addr 	= INADDR_ANY;  

/*
 *	Creacion de las estructuras necesarias para el manejo de un socket
 *  SOCK_STREAM - Protocolo orientado a conexión
 */
   	printf("Creando Socket ....\n");
   	if( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Ocurrio un problema en la creacion del socket");
		exit(1);
   	}
/*
 *  bind - Se utiliza para unir un socket con una dirección de red determinada
 */
   	printf("Configurando socket ...\n");
   	if( bind(sockfd, (struct sockaddr *) &direccion_servidor, sizeof(direccion_servidor)) < 0 )
	{
		perror ("Ocurrio un problema al configurar el socket");
		exit(1);
   	}
/*
 *  listen - Marca al socket indicado por sockfd como un socket pasivo, esto es, como un socket
 *  que será usado para aceptar solicitudes de conexiones de entrada usando "accept"
 *  Habilita una cola asociada la socket para alojar peticiones de conector procedentes
 *  de los procesos clientes
 */
   	printf ("Estableciendo la aceptacion de clientes...\n");
	if( listen(sockfd, COLA_CLIENTES) < 0 )//descriptor y numero de conexiones
	{
		perror("Ocurrio un problema al crear la cola de aceptar peticiones de los clientes");
		exit(1);
   	}
/*
 *  accept - Aceptación de una conexión
 *  Selecciona un cliente de la cola de conexiones establecidas
 *  se crea un nuevo descriptor de socket para el manejo
 *  de la nueva conexion. Apartir de este punto se debe
 *  utilizar el nuevo descriptor de socket
 *  accept - ES BLOQUEANTE 
 */
   	printf ("En espera de peticiones de conexión ...\n");
   	if( (cliente_sockfd = accept(sockfd, NULL, NULL)) < 0 )//descriptor:::puede crearse un fork para cada conexion con un fork
	{
		perror("Ocurrio algun problema al atender a un cliente");
		exit(1);
   	}

/*
 *	Inicia la transferencia de datos entre cliente y servidor
 */
  

   	printf("Se aceptó un cliente, atendiendolo \n");
   	if( read (cliente_sockfd, &leer_mensaje, TAM_BUFFER) < 0 )
	{
		perror ("Ocurrio algun problema al recibir datos del cliente");
		exit(1);
   	}
   	printf ("El cliente nos envio el siguiente mensaje: \n %s \n", leer_mensaje);

   	/*********Procesamiento*********/

	while(1)
	{
		valor = digitalRead(0);
		if(valor==1){
			sprintf(str3,"%s%sprueba%d.bmp",comando,carpeta,x);
			system(str3);

			sprintf(str,"%sprueba%d.bmp",carpeta,x);
			printf("Prueba actual: %s\n",str);
			imagenRGB = abrirBMP(str, &info);
			displayInfo(&info);//informacion de la imagen 
			imagenGray=RGBToGray(imagenRGB, info.width, info.height);
			//GrayToRGB(imagenRGB, imagenGray, info.width, info.height);
			//guardarBMP("EscalaGris2.bmp", &info, imagenRGB);

			/*Filtro*/

			imagenAux=reservarMemoria(info.width,info.height);

			for(nh=0; nh < NUM_HILOS; nh++)
		    {
		         nhs[nh]=nh;
		         pthread_create(&tids[nh], NULL, filtroGaussiano, (void *)&nhs[nh]); //el ultimo parametro envia dir del ultimo hilo(indice)
		    }
			
			for( nh=0; nh < NUM_HILOS; nh++ )
		    {
		         pthread_join(tids[nh], (void **)&hilo);
		         printf("\nHilo %d terminado\n",*hilo );
		    }
			
			imagenFiltroB = imagenAux;
			//GrayToRGB(imagenRGB, imagenAux, info.width, info.height);
			//guardarBMP("filtroB.bmp", &info, imagenRGB);
			
			imagenAux=reservarMemoria(info.width,info.height);

			imagenResultado=sustracImagen(imagenFiltroA, imagenFiltroB, info.width, info.height);
			
			GrayToRGB(imagenRGB, imagenResultado, info.width, info.height); //revisar este detalle B es la imagen de menor tamaño
			sprintf(str2,"Resultados/sustraccion%d.bmp",x);
			guardarBMP(str2, &info, imagenRGB);
			
			imgBin = reservarMemoria(info.width,info.height);
		        otsu=reservarMemoria(info.width,info.height);
		    	imgSau=reservarMemoria(info.width,info.height);
		    
		    	int umbral=thotsu(imagenResultado,info.width,info.height);
		    	otsu=binarizar(imagenResultado,umbral,info.width,info.height);               //la binarizacion de la imagen depende no exactamente Ostu*/

		    	for(i=0;i<NUM_HILOS;i++){
		        	nhs[i]=i;                                                            //creacion de hilos
		        	pthread_create(&tids[i],NULL,subImagen,(void *)&nhs[i]);               //crea varios hilos
		    	}
		    	for(k=0;k<NUM_HILOS;k++){
		         	pthread_join(tids[k],(void**)&hilo);                                //espera a que termine la ejecucion del hilo
		         	printf("\nHilo %d terminado\n",*hilo);
		    	}
		    	for(i=0;i<NUM_HILOS;i++){
		        	nhs[i]=i;                                                            //creacion de hilos
		        	pthread_create(&tids[i],NULL,sauvola,(void *)&nhs[i]);               //crea varios hilos
		    	}
		    	for(k=0;k<NUM_HILOS;k++){
		         	pthread_join(tids[k],(void**)&hilo);                                //espera a que termine la ejecucion del hilo
		         	printf("\nHilo %d terminado\n",*hilo);
		    	}


		    	//GrayToRGB(imagenRGB, imgBin, info.width, info.height);
		    	//sprintf(str2,"Resultados/imgBin%d.bmp",cont);
		    	//guardarBMP(str2, &info, imagenRGB);
		    	//GrayToRGB(imagenRGB, otsu, info.width, info.height);
		    	//guardarBMP("otsu.bmp", &info, imagenRGB);
		    	GrayToRGB(imagenRGB, imgSau, info.width, info.height);
		    	sprintf(str2,"Resultados/sauvola%d.bmp",x);
		    	guardarBMP(str2, &info, imagenRGB);

		    	// crop=cropROI(imgSau, info.width, info.height);

		    	// GrayToRGB(imagenRGB, crop, info.width, info.height);
		    	// //info.width=ancho;
		    	// //info.height=alto;
		    	// guardarBMP("crop.bmp", &info, imagenRGB);

		    	m00=0; m10=0; m01=0; m11=0; m20=0; m02=0; m30=0; m03=0; m12=0; m21=0;

		   	pthread_mutex_init(&bloqueo, NULL);

		   	for(nh=0; nh < NUM_HILOS; nh++)
		    	{
		         	nhs[nh]=nh;
		         	pthread_create(&tids[nh], NULL, momentosGeometricos, (void *)&nhs[nh]); //el ultimo parametro envia dir del ultimo hilo(indice)
		    	}
			
			for( nh=0; nh < NUM_HILOS; nh++ )
		    	{
		         	pthread_join(tids[nh], (void **)&hilo);
		         	printf("\nHilo %d terminado\n",*hilo );
		    	}

		    	pthread_mutex_destroy(&bloqueo);

		    	momentosHu();

		    	for (i = 1; i <=7 ; i++)
		    	{
		    		fprintf(archivo, "%Lf ", phi[i]);	
		    	}

		    	fprintf(archivo, "\n");

		    	resultado = clasifica();

		    	if(resultado == 1){
			   		if( write (cliente_sockfd, "Alerta!!!!! Persona Detectada", 30) < 0 )
					{
			   			perror("Ocurrio un problema en el envio de un mensaje al cliente");
			   			exit(1);
			   		}

			    }else{
			   		if( write (cliente_sockfd, "No hay persona", 30) < 0 )
					{
			   			perror("Ocurrio un problema en el envio de un mensaje al cliente");
			   			exit(1);
			   		}
			    }

		    	x++;
		}else{
			printf("%d\n", valor);
		}
		sleep(1);
		//fclose (archivo);
		//fclose (archivo2);
	}

	close (sockfd);
   	close (cliente_sockfd);
   	
	free(imagenRGB);
	free(imagenGray);
	free(imagenFiltroA);
	free(imagenFiltroB);
	free(imagenResultado);
	free(imagenAux);
	free(imgBin);
	free(otsu);
	free(imgSau);
	free(phi);
}

// unsigned char* cropROI(unsigned char* imagen, uint32_t width, uint32_t height)
// {
// 	register int x,y;
// 	int indice=0, indice2=0, i=0, j=0;
// 	int coordX[width*height];
// 	int coordY[width*height]; 

// 	unsigned char* crop;

// 	for (i = 0; i < width*height; i++)
// 	{
// 		coordX[i]=-1;
// 		coordY[i]=-1; 
// 	}
	
// 	i=0;

// 	for(y=0;y<height;y++)
// 	{
// 		for(x=0;x<width;x++)
// 		{
// 			indice=(y*width+x);
// 			if(imagen[indice]==255){ //255 para el objeto y 0 para el fondo. Delimita región de interes 
// 				//imagen[indice]=imagen[indice]/255;
// 				coordX[i]=x;
// 				coordY[i]=y;
// 				i++;
// 				//printf("%d\n", i);
// 			}
// 		}
// 	}

// 	Xmay = coordX[0];
// 	Xmin = coordX[0];

// 	for (i = 0; i < width*height; i++)
// 	{
// 		if(coordX[i]>Xmay){
//         	Xmay = coordX[i];
//     	}
//     	if ((coordX[i] < Xmin)&& coordX[i]!=-1){
//     		Xmin = coordX[i];
//     	}
// 	}

// 	printf("Xmay=%d\n", Xmay);
// 	printf("Xmin=%d\n", Xmin);
	
// 	Ymay = coordY[0];
// 	Ymin = coordY[0];

// 	for (i = 0; i <  width*height; i++)
// 	{
// 		if(coordY[i]>Ymay){
//         	Ymay = coordY[i];
//     	}
//     	if ((coordY[i] < Ymin)&& coordY[i]!=-1){
//     		Ymin = coordY[i];
//     	}
// 	}

// 	printf("Ymay=%d\n", Ymay);
// 	printf("Ymin=%d\n", Ymin);

// 	ancho=Xmay-Xmin;
// 	alto=Ymay-Ymin;

// 	printf("ancho=%d\n", ancho);
// 	printf("alto=%d\n", alto);

// 	crop=(unsigned char *)malloc(width*height*sizeof(unsigned char));
// 	i=0;

// 	for(y=Ymin;y<Ymay;y++)
// 	{
// 		for(x=Xmin;x<Xmay;x++)
// 		{
// 			// for(j=0;j<alto;j++)
// 			// {
// 			// 	for(i=0;i<ancho;i++)
// 			// 	{
// 					indice=(y*width+x);
// 					//indice2=(j*ancho+i); 
// 					//indice2=(j+i); 
// 					// indice=(y*ancho+x); 
// 					//indice2=(j*ancho+i);
// 					// if(indice2%2==0){ 
// 					// 	crop[indice2]=0;
// 					// }else{
// 					// 	crop[indice2]=255;
// 					// }
// 					crop[indice]=imagen[indice];
// 					//printf("%d\n",j);
// 					i++;
// 			// 	}
// 			// }
// 		}
// 	}

// 	return crop;
// }

void * momentosGeometricos(void * idh)
{
    int nh=*(int *)idh;
    int tamBloque=info.height/NUM_HILOS;
    int iniBloque=nh*tamBloque;
    int finBloque=iniBloque+tamBloque; 
	register int x,y;
	int indice=0;
	for(y=iniBloque*nh;y<finBloque;y++)
	{
		for(x=0;x<info.width;x++)
		{
			indice=(y*info.width+x);
			if(imgSau[indice]==255){ //255 para el objeto y 0 para el fondo. Delimita región de interes 
				//imagen[indice]=imagen[indice]/255;
				pthread_mutex_lock(&bloqueo);
				m00+=imgSau[indice];
				m10=m10+(x*imgSau[indice]); 
				m01=m01+(y*imgSau[indice]);
				m11=m11+(x*y*imgSau[indice]);
				m20=m20+(pow(x,2)*imgSau[indice]);
				m02=m02+(pow(y,2)*imgSau[indice]);
				m30=m30+(pow(x,3)*imgSau[indice]);
				m03=m03+(pow(y,3)*imgSau[indice]);
				m12=m12+(x*pow(y,2)*imgSau[indice]);
				m21=m21+(pow(x,2)*y*imgSau[indice]); 
				//printf("%d ",imagen[indice]);
				pthread_mutex_unlock(&bloqueo);
			}
		}
		//printf("\n");
	}
	pthread_exit(idh);
}

void momentosHu(){

	long double xbar=0, ybar=0, n10=0, n01=0, n11=0, n20=0, n02=0, n30=0, n03=0, n21=0, n12=0;

	phi=(long double *)malloc(8*sizeof(long double));

	printf("m00= %Lf\n",m00); //%ld
	printf("m10= %Lf\n",m10);
	printf("m01= %Lf\n",m01);
	printf("m11= %Lf\n",m11);
	printf("m20= %Lf\n",m20);
	printf("m02= %Lf\n",m02);
	printf("m30= %Lf\n",m30);
	printf("m03= %Lf\n",m03);
	printf("m12= %Lf\n",m12);
	printf("m21= %Lf\n",m21);

	xbar = m10/m00;
	ybar = m01/m00; /*Centroides*/

	printf("xbar= %Lf\n",xbar);
	printf("ybar= %Lf\n",ybar);
	
	n10 = 0;
	n01 = 0;							/*Momentos Centrales Normalizados*/
	n11 = (m11-ybar*m10)/pow(m00,2);
	n20 = (m20-xbar*m10)/pow(m00,2);
	n02 = (m02-ybar*m01)/pow(m00,2);
	n30 = (m30-3*xbar*m20+2*pow(xbar,2)*m10)/pow(m00,2.5);
	n03 = (m03-3*ybar*m02+2*pow(ybar,2)*m01)/pow(m00,2.5);
	n21 = (m21-2*xbar*m11-ybar*m20+2*pow(xbar,2)*m01)/pow(m00,2.5);
	n12 = (m12-2*ybar*m11-xbar*m02+2*pow(ybar,2)*m10)/pow(m00,2.5);

	printf("n11= %Lf\n",n11);
	printf("n20= %Lf\n",n20);
	printf("n02= %Lf\n",n02);
	printf("n30= %Lf\n",n30);
	printf("n03= %Lf\n",n03);
	printf("n12= %Lf\n",n12);
	printf("n21= %Lf\n",n21);

	phi[0] = 0;
	phi[1] = n20 + n02;					/*Momentos de Hu*/
	phi[2] = pow((n20 - n02),2) + 4*pow(n11,2);
	phi[3] = pow((n30 - 3*n12),2) + pow((3*n21 - n03),2);
	phi[4] = pow((n30 + n12),2) + pow((n21 + n03),2);
	phi[5] = (n30 - 3*n12) * (n30 + n12) * ( pow((n30 + n12),2) - 3*pow((n21 + n03),2) ) + (3*n21 - n03) * (n21 + n03) * ( 3*pow((n30 + n12),2) - pow((n21 + n03),2) );
	phi[6] = (n20 - n02) * ( pow((n30 + n12),2) - pow((n21 + n03),2) ) + 4 * n11 * (n30 + n12) * (n21 + n03);
	phi[7] = (3*n21 - n03) * (n30 + n12) * ( pow((n30 + n12),2) - 3*pow((n21 + n03),2) ) + (3*n12 - n30) * (n21 + n03) * ( 3*pow((n30 + n12),2) - pow((n21 + n03),2) );


	printf("phi1= %Lf\n",phi[1]);
	printf("phi2= %Lf\n",phi[2]*100000000);
	printf("phi3= %Lf\n",phi[3]*100000000);
	printf("phi4= %Lf\n",phi[4]*100000000);
	printf("phi5= %Lf\n",phi[5]*100000000);
	printf("phi6= %Lf\n",phi[6]*100000000);
	printf("phi7= %Lf\n",phi[7]*100000000);

	/*Normalizacion de Momentos de Hu en el rango de -1 a +1 para NN*/

	// Xrec2 =(-sign(invmoments2(I1)).*(log10(abs(invmoments2(I1)))));
	//(-1*((phi1 > 0) ? 1 : ((phi1 < 0) ? -1 : 0)))*(log10(abs(phi1)));
	// funcion sign:
	// 	1 if the corresponding element of x is greater than 0.
	// 	0 if the corresponding element of x equals 0.
	// 	-1 if the corresponding element of x is less than 0.

	//Otra opcion:
	//trainingData[w][i] = hu[i] < 0 ? (-1)*log10(abs(hu[i])) : log10(abs(hu[i]));

	
	phi[1]=normalizaPhi(phi[1],1);
	phi[2]=normalizaPhi(phi[2],2);
	phi[3]=normalizaPhi(phi[3],3);
	phi[4]=normalizaPhi(phi[4],4);
	phi[5]=normalizaPhi(phi[5],5);
	phi[6]=normalizaPhi(phi[6],6);
	phi[7]=normalizaPhi(phi[7],7);

	printf("phi1N= %Lf\n",phi[1]);
	printf("phi2N= %Lf\n",phi[2]);
	printf("phi3N= %Lf\n",phi[3]);
	printf("phi4N= %Lf\n",phi[4]);
	printf("phi5N= %Lf\n",phi[5]);
	printf("phi6N= %Lf\n",phi[6]);
	printf("phi7N= %Lf\n",phi[7]);

}

long double normalizaPhi(long double phi, int moment){
	long double norm1, E;
	//long double DE[7]={0.1302,0.7378,1.1975,1.6583,20.3833,14.0535,23.1117};
	//long double MED[7]={2.9834,6.4931,10.5274,11.4782,10.4421,5.4356,-2.8186};
	
	long double DE[7]={0.1374,0.7178,0.9958,1.3692,17.9311,12.9836,22.4038};
	long double MED[7]={2.9735,6.4760,10.2437,11.0360,12.8087,6.5911,1.4749};

	moment=moment-1;

	norm1=(-1*((phi > 0) ? 1 : ((phi < 0) ? -1 : 0))) * (log10(((phi > 0) ? 1*phi : ((phi < 0) ? -1*phi : 0)))); //31 ultimo valor

	E = exp(-((norm1-MED[moment])/DE[moment]));
	
	return ( 1 - E ) / ( 1 + E );	

}

int clasifica(void){

	register int i, j, k;

	for( j = 1 ; j <= NumHidden ; j++ ) {         	/* j es el indice de las capas ocultas:: i es el indice de las entradas*/
		SumH[j] = WeightIH[0][j]; 					//sumar el bias
		for( i = 1 ; i <= NumInput ; i++ ) {		//Hasta el numero de entradas
			SumH[j] += phi[i] * WeightIH[i][j];
		}
		Hidden[j] = ((exp(SumH[j])) - (exp(-SumH[j])))/((exp(SumH[j])) + (exp(-SumH[j])));		//Activacion con Tanh
		printf("Hidden= %lf \n", Hidden[j]); //%lf
	}

	for( k = 1 ; k <= NumOutput ; k++ ) {         	/* k es el  indice de las salidas */
		SumO[k] = WeightHO[0][k];
		for( j = 1 ; j <= NumHidden ; j++ ) {
			SumO[k] += Hidden[j] * WeightHO[j][k];
		}
		Output[k] = 1.0/(1.0 + exp(-SumO[k]));  	//Activacion con sigmoide
		printf("Output= %lf \n", Output[k]); //%lf
	}

	if(Output[1]>0.6){
		printf("Persona detectada!\n");
		fprintf(archivo2, "%s   Persona detectada!", str);
		fprintf(archivo2, "\n");
		return 1;
	}else{
		printf("No hay persona\n");
		fprintf(archivo2, "%s   No hay persona", str);
		fprintf(archivo2, "\n");
		return 0;
	}


	

}

unsigned char * sustracImagen(unsigned char* imagenA, unsigned char* imagenB, uint32_t width, uint32_t height)
{
	register int x,y;
	int indice=0, sustraccion=0;
	unsigned char *imagenRes;

	imagenRes=reservarMemoria(width,height);

	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			indice=(y*width+x);
			sustraccion=imagenA[indice]-imagenB[indice]; 
			imagenRes[indice]=(sustraccion<0)?0:sustraccion;
			
		}
	}
	return imagenRes;
}


// if(sustraccion>=thres){
// 				//imagenRes[indice]=(sustraccion<0)?0:sustraccion;
// 				imagenRes[indice]=255;
// 			}else{
// 				imagenRes[indice]=0;
// 			}
void brilloImagen(unsigned char* imagenGray, uint32_t width, uint32_t height)
{
	register int x,y;
	int indice, brillo;

	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			indice=(y*width+x); 
			brillo=imagenGray[indice]+40; //aumentar el brillo 
			imagenGray[indice]=(brillo>255)?255:brillo;
		}
	}
}

void * filtroGaussiano(void * idh)
{
	int nh=*(int *)idh;
    int tamBloque=info.height/NUM_HILOS; //tamaño entre el numero de hilos
    int iniBloque=nh*tamBloque;//numero de hilo multiplicado por el bloque a alcanzar
    int finBloque=iniBloque+tamBloque; 
    //register int i;
    register int y, x, ym, xm;
	int conv, indiceI, indiceM;
	unsigned char mascara[DIMASK*DIMASK]={
		1	,2	,1,
		2	,4	,2,
		1	,2	,1 };
		// 0.0751, 0.1238, 0.0751,
		// 0.1238, 0.2042, 0.1238,
		// 0.0751, 0.1238, 0.0751 };

	for(y=iniBloque-(nh*DIMASK);y<finBloque-DIMASK;y++) //llega hasta el tamaño-(DIMASK)::y=inibloq-numerodehilo*tamanio de mascara::
		for(x=0;x<info.width-DIMASK;x++)				//para no brincarse pixeles
			{
				conv=0;
				for(ym=y;ym<y+DIMASK;ym++) //RECORRER EL CUADRITO DE LA MASCARA:: hasta dimask, solo se delimita por el cuadro
					for(xm=x;xm<x+DIMASK;xm++)//RECORRER EL CUADRITO DE LA MASCARA:: hasta dimask, solo se delimita por el cuadro
					{
						indiceI=ym*info.width+xm; //puede sustituirse con un contador de 0-9, para este caso.//fila+ancho+columna
						indiceM=(ym-y)*DIMASK+(xm-x);//restarle el desplazamiento horizontal y vertical 
						conv += imagenGray[indiceI]*mascara[indiceM];
					}
					conv/=16;//al finalizar multiplica por el factor 1/16
					indiceI=(y+1)*info.width+(x+1);//+1 para llegar al centro, en este caso de 3*3
					imagenAux[indiceI]=conv; //sustituir el valor en la imagen de resultado imagenP
			}
	pthread_exit(idh);
}

void GrayToRGB(unsigned char *imagenRGB, unsigned char *imagenGray,  uint32_t width, uint32_t height)
{
	register int x,y;
	int indiceRGB, indiceGray;
	unsigned char nivelGris;

	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			indiceGray=(y*width+x); //desplazamiento a un pixel, para desplazamiento lineal. *3 --saltar componenetes rgb
			indiceRGB=indiceGray*3;
			//te posiciona en el primer byte del pixel, en el bit de B.
			//nivelGris=(imagen[indice]+imagen[indice+1]+imagen[indice+2])/3; //(R+G+B)/3 Archivo NG2
			//0.11* se va a la unidad de punto flotante y es menor el rendimiento.
			nivelGris=imagenGray[indiceGray];
			imagenRGB[indiceRGB]   = nivelGris; //copiar valor de gris a cada pixel
			imagenRGB[indiceRGB+1] = nivelGris;
			imagenRGB[indiceRGB+2] = nivelGris;
		}
	}
}


unsigned char * RGBToGray(unsigned char *imagenRGB, uint32_t width, uint32_t height)
{
	register int x,y;
	int indiceRGB, indiceGray;
	unsigned char nivelGris;
	unsigned char *imagenGray;

	imagenGray=(unsigned char *)malloc(width*height*sizeof(unsigned char));
	if(imagenGray==NULL)
	{
		perror("Error al asignar memoria ");
		exit(EXIT_FAILURE);

	}

	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			indiceGray=(y*width+x); //desplazamiento a un pixel, para desplazamiento lineal. *3 --saltar componenetes rgb
			indiceRGB=indiceGray*3;
			//te posiciona en el primer byte del pixel, en el bit de B.
			//nivelGris=(imagen[indice]+imagen[indice+1]+imagen[indice+2])/3; //(R+G+B)/3 Archivo NG2
			//0.11* se va a la unidad de punto flotante y es menor el rendimiento.
			nivelGris=((imagenRGB[indiceRGB]*11+imagenRGB[indiceRGB+1]*59+imagenRGB[indiceRGB+2]*30)/100);
			//imagen[indice] 	 = nivelGris; //copiar valor de gris a cada pixel
			//imagen[indice+1] = nivelGris;
			//imagen[indice+2] = nivelGris;
			imagenGray[indiceGray]= nivelGris;
		}
		
	}
	return imagenGray;
}

unsigned char * reservarMemoria(uint32_t width, uint32_t height)
{
	unsigned char *imagen;
	imagen=(unsigned char *)malloc(width*height*sizeof(unsigned char));
	if(imagen==NULL)
	{
		perror("Error al asignar memoria ");
		exit(EXIT_FAILURE);

	}
	return imagen;
}

void * dilata(void *arg){
    register int y,x,ym,xm;
    int bloque=200;
    int b2=80;
    int conv,indiceI,indiceM,aux;
    int nh=*(int*)arg;
    //int temp[bloque*bloque];
    int tamBloque=info.height/NUM_HILOS;                                    //bloques por hilo
    int iniBloque=nh*tamBloque;                                             //inicio del bloque
    int finBloque=iniBloque+tamBloque;                                      //fin del bloque
  
        for(y=iniBloque-(nh*b2); y<finBloque-b2;y++){               //recorrer imagen
            for(x=0; x<info.width-bloque; x++){                             
                conv=0;
                for(ym=y; ym<y+b2; ym++){                               //Recorre vecindad
                    for(xm=x; xm<x+bloque; xm++){                           
                        indiceI=(ym)*info.width+xm;                           
                        aux=imgSau[indiceI];
                        if(aux==255)
                            conv++;
                    }                                                
                }
            if(conv<=20){
                for(ym=y; ym<y+b2; ym++){                              
                    for(xm=x; xm<x+bloque; xm++){                          
                        indiceI=(ym)*info.width+xm;                           
                        aux=imgSau[indiceI]*0;                       
                        imgSau[indiceI]=0;
                    }                                                
                }
            }
            }
        }
    pthread_exit(arg);
}




/*UMBRALIZACION LOCAL METODO2
Souvola ayuda a rellenar regiones dentro de obtejos o engrosarlos en imagenes binarias
por lo que se calcula la media de pixeles y la desviacion estandar
Se utiliza la media de la imagen umbralizada localmente con la mediana (imgBin) para engorsar la zona deseada
tomando como base la imagen sustraccion
https://www.tamps.cinvestav.mx/~wgomez/diapositivas/AID/Clase19.pdf <- Metodo Souvola
*/
void* sauvola(void *arg){
register int y,x,ym,xm;
    int bloque=21;
    float media,desv;
    int a,k=0.2;                                                            //Los valores de k 0.2 o 0.5
    double umbral;
    int indiceI;
    int nh=*(int*)arg;
    int tamBloque=info.height/NUM_HILOS;                                    //bloques por hilo
    int iniBloque=nh*tamBloque;                                             //inicio del bloque
    int finBloque=iniBloque+tamBloque;   
  
        for(y=iniBloque-(nh*bloque); y<finBloque-bloque;y++){               //recorrer imagen
            for(x=0; x<info.width-bloque; x++){                             
                media=0.0;
                desv=0.0;
                umbral=0;
                a=0;
                for(ym=y; ym<y+bloque; ym++){                               //recorrer vecindad segun bloque
                    for(xm=x; xm<x+bloque; xm++){                           
                        indiceI=(ym)*info.width+xm;                         
                        media=media+imgBin[indiceI];                        //calculo de media
                        a=a+pow(imagenResultado[indiceI],2);                  //calculo de desviacion
                    }                                                
                }
            //CALCULO DE MEDIA Y DESV
            media=media/(bloque*bloque);
            desv=sqrt(a/(bloque*bloque)-pow(media,2));

            umbral=media*(1+(k*(desv/128)-1));                              //Calculo de umbral segun Souvola

            indiceI=(y+1)*info.width+(x+1);
            imgSau[indiceI]=((imagenResultado[indiceI])>=(umbral))?0:255;
            }
        }
     pthread_exit(arg); 
}


/*UMBRALIZACION LOCAL
Se obtiene una subimagen de tamaño bloque para umbralizar la imagen de forma local y rellenar el pixel según su vencindad
para obtener un mejor resultado se utilizan los pixeles para la media de la imagen umbralizada general con otsu
así se obtiene la media de los pixeles en 0 y 1 para obtener una imagen dilatada.

*/
void* subImagen(void *arg){
    register int y,x,ym,xm,i,j;
    int bloque=5;
    int indiceI,temp[bloque*bloque],cont,aux;
    int nh=*(int*)arg;
    int tamBloque=info.height/NUM_HILOS;                                    //bloques por hilo
    int iniBloque=nh*tamBloque;                                             //inicio del bloque
    int finBloque=iniBloque+tamBloque;   

  
        for(y=iniBloque-(nh*bloque); y<finBloque-bloque;y++){               //recorrer imagen
            for(x=0; x<info.width-bloque; x++){                             
                cont=0;
                for(ym=y; ym<y+bloque; ym++){                               //recorrer subimagen
                    for(xm=x; xm<x+bloque; xm++){                           
                        indiceI=(ym)*info.width+xm;                         //indice de la subimagen
                        temp[cont]=otsu[indiceI];                           //Obtiene los valores de los pixeles en la subimagen
                        cont++;
                    }                                                
                }
            //ordenamiento de valores
            for (i=1;i<(bloque*bloque);i++)
                for (j=0;j<(bloque*bloque)-1;j++)
                    if (temp[j]>temp[j+1]){
                        aux=temp[j];
                        temp[j]=temp[j+1];
                        temp[j+1]=aux;
                    }
            indiceI=(y+1)*info.width+(x+1);
            imgBin[indiceI]=((imagenResultado[indiceI])>=(temp[8]))?0:255;     //Se obtiene el valor de la media de los pixeles ordenados
            }
        }
    
     pthread_exit(arg);   
}

/* METODO DE UMBRALIZACION DE OTSU */
int thotsu(unsigned char* imagen, uint32_t width, uint32_t height){
    int i;
    int histograma[255];                                                       //Arreglo guarda los valores del histograma
    register int x,y;
    int ValorMax = 0;
    int indice,h=0,sum=0;
    float sumB = 0;
	int wB = 0,wF = 0;
	float varMax = 0;
	int threshold = 0;

    for(i=0; i<256; i++){                                                     //Valores de histograma en cero
        histograma[i]=0;
    }
    
    for(y=0;y<height;y++){                                                    //Creacion del histograma por clases de pixeles
        for(x=0;x<width;x++){
            indice=(y*width+x);
            h=imagen[indice];
            histograma[h]+=1;
            if (histograma[h] > ValorMax)                                     //Valor maximo de pixeles contabilizados 
                ValorMax = histograma[h];
        }
    }

    for(int t=0;t<256;t++){ 
        sum+=t*histograma[t];
    }
	for(int t=0;t<256;t++){
		wB += histograma[t];					                              // Clase 1
		if (wB == 0) 
            continue;
		wF = (height*width) - wB;						                      // Clase 2
		if (wF == 0) 
            break;
		sumB += (float) (t * histograma[t]);
		float mB = sumB / wB;				                                  // Media Clase 1
		float mF = (sum - sumB) / wF;		                                  // Media Clase 2
		float varBetween = (float)wB * (float)wF * (mB - mF) * (mB - mF);	  // Calculo de varianza

		if (varBetween > varMax){                                             // Umbral maximizado
			varMax = varBetween;
			threshold = t;   
		}
	}
    return threshold;   
}

/*METODO DE UMBRALIZACION POR SEGMENTOS DE IMAGEN*/
unsigned char * binarizar(unsigned char * imagen, int umbral, uint32_t width, uint32_t height){     
    register int x,y;
    int indice;
    unsigned char * bina;
    bina=reservarMemoria(width,height);
    for(y=0;y<height;y++){
        for(x=0;x<width;x++){
            indice=(y*width+x);
            bina[indice]=((imagen[indice])>=umbral)?255:0;                      //Binarizacion de imagen

        }
    }
    return bina;
}
