#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "imagen.h"
#include <string.h>

#define DIMASK 3
#define NUM_HILOS 4

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
long double normalizaPhi(long double phi);

bmpInfoHeader info;
unsigned char *imagenRGB, *imagenGray, *imagenFiltroA, *imagenFiltroB, *imagenResultado, *imagenAux, *imgBin, *otsu, *imgSau;

long double m00=0, m10=0, m01=0, m11=0, m20=0, m02=0, m30=0, m03=0, m12=0, m21=0;
long double *phi;
// unsigned char* cropROI(unsigned char* imagen, uint32_t width, uint32_t height);
// int Xmay=0, Ymay=0, Xmin=0, Ymin=0, ancho=0, alto=0;

pthread_mutex_t bloqueo;

int main(void)
{
	register int nh;

    int *hilo, nhs[NUM_HILOS], i, k;//nhs arreglo de identificadores para hilos, para cada hilo (correctamente)
	pthread_t tids[NUM_HILOS]; //arreglo de identificadores 

	FILE *archivo;
	archivo = fopen("momentosHu.txt", "w+");

	// imagenRGB = abrirBMP("ImagenesRandom/base.bmp", &info);	
	// displayInfo(&info);//informacion de la imagen 

	// imagenGray=RGBToGray(imagenRGB, info.width, info.height);
	// GrayToRGB(imagenRGB, imagenGray, info.width, info.height);
	// guardarBMP("EscalaGris1.bmp", &info, imagenRGB);
	
	// /*Filtro*/
	// imagenAux=reservarMemoria(info.width,info.height);

	// for(nh=0; nh < NUM_HILOS; nh++)
 //    {
 //         nhs[nh]=nh;
 //         pthread_create(&tids[nh], NULL, filtroGaussiano, (void *)&nhs[nh]); //el ultimo parametro envia dir del ultimo hilo(indice)
 //    }
	
	// for( nh=0; nh < NUM_HILOS; nh++ )
 //    {
 //         pthread_join(tids[nh], (void **)&hilo);
 //         printf("\nHilo %d terminado\n",*hilo );
 //    }
    
 //    /***********************************************/
    
	// imagenFiltroA = imagenAux;

	// GrayToRGB(imagenRGB, imagenAux, info.width, info.height);
	// guardarBMP("filtroA.bmp", &info, imagenRGB);

	char str[25];//25
	char str2[25];

	int cont=0;

	for (cont = 1; cont <= 12; cont++)
	{

	//sprintf(str, "%d", cont);

	sprintf(str,"Tornillo2/otsu_%d.bmp",cont);
	printf("Prueba actual: %s\n",str);

	imagenRGB = abrirBMP(str, &info);
	displayInfo(&info);//informacion de la imagen 
	//imagenGray=RGBToGray(imagenRGB, info.width, info.height);
	//GrayToRGB(imagenRGB, imagenGray, info.width, info.height);
	sprintf(str2,"Resultados/res%d.bmp",cont);
	guardarBMP(str2, &info, imagenRGB);

	//brilloImagen(imagenGray, info.width, info.height);

	/*Filtro*/

	// imagenAux=reservarMemoria(info.width,info.height);

	// for(nh=0; nh < NUM_HILOS; nh++)
 //    {
 //         nhs[nh]=nh;
 //         pthread_create(&tids[nh], NULL, filtroGaussiano, (void *)&nhs[nh]); //el ultimo parametro envia dir del ultimo hilo(indice)
 //    }
	
	// for( nh=0; nh < NUM_HILOS; nh++ )
 //    {
 //         pthread_join(tids[nh], (void **)&hilo);
 //         printf("\nHilo %d terminado\n",*hilo );
 //    }
	
	// imagenFiltroB = imagenAux;

	// GrayToRGB(imagenRGB, imagenAux, info.width, info.height);
	// guardarBMP("filtroB.bmp", &info, imagenRGB);
	
	// imagenAux=reservarMemoria(info.width,info.height);

	// imagenResultado=sustracImagen(imagenFiltroA, imagenFiltroB, info.width, info.height);
	// //brilloImagen(imagenGray, info.width, info.height);
	
	// GrayToRGB(imagenRGB, imagenResultado, info.width, info.height); //revisar este detalle B es la imagen de menor tamaño
	// sprintf(str2,"Resultados/sustraccion%d.bmp",cont);
	// guardarBMP(str2, &info, imagenRGB);
	// //system("mkdir Raspberry");

	// imgBin = reservarMemoria(info.width,info.height);
 //    otsu=reservarMemoria(info.width,info.height);
 //    imgSau=reservarMemoria(info.width,info.height);
    
 //    int umbral=thotsu(imagenResultado,info.width,info.height);
 //    otsu=binarizar(imagenResultado,umbral,info.width,info.height);               //la binarizacion de la imagen depende no exactamente Ostu*/

 //    for(i=0;i<NUM_HILOS;i++){
 //        nhs[i]=i;                                                            //creacion de hilos
 //        pthread_create(&tids[i],NULL,subImagen,(void *)&nhs[i]);               //crea varios hilos
 //    }
 //    for(k=0;k<NUM_HILOS;k++){
 //         pthread_join(tids[k],(void**)&hilo);                                //espera a que termine la ejecucion del hilo
 //         printf("\nHilo %d terminado\n",*hilo);
 //    }
 //    for(i=0;i<NUM_HILOS;i++){
 //        nhs[i]=i;                                                            //creacion de hilos
 //        pthread_create(&tids[i],NULL,sauvola,(void *)&nhs[i]);               //crea varios hilos
 //    }
 //    for(k=0;k<NUM_HILOS;k++){
 //         pthread_join(tids[k],(void**)&hilo);                                //espera a que termine la ejecucion del hilo
 //         printf("\nHilo %d terminado\n",*hilo);
 //    }

 //    GrayToRGB(imagenRGB, imgBin, info.width, info.height);
 //    guardarBMP("imgBin.bmp", &info, imagenRGB);
 //    GrayToRGB(imagenRGB, otsu, info.width, info.height);
 //    guardarBMP("otsu.bmp", &info, imagenRGB);
 //    GrayToRGB(imagenRGB, imgSau, info.width, info.height);

 //    sprintf(str2,"Resultados/sauvola%d.bmp",cont);
 //    guardarBMP(str2, &info, imagenRGB);
	
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

    for (i = 0; i < 7; i++)
    {
    	fprintf(archivo, "%Lf ", phi[i]);	
    }

    fprintf(archivo, "\n");

	} 

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
			//if(imagenRGB[indice]==255){ //255 para el objeto y 0 para el fondo. Delimita región de interes 
				//imagen[indice]=imagen[indice]/255;
				pthread_mutex_lock(&bloqueo);
				m00+=imagenRGB[indice];
				m10=m10+(x*imagenRGB[indice]); 
				m01=m01+(y*imagenRGB[indice]);
				m11=m11+(x*y*imagenRGB[indice]);
				m20=m20+(pow(x,2)*imagenRGB[indice]);
				m02=m02+(pow(y,2)*imagenRGB[indice]);
				m30=m30+(pow(x,3)*imagenRGB[indice]);
				m03=m03+(pow(y,3)*imagenRGB[indice]);
				m12=m12+(x*pow(y,2)*imagenRGB[indice]);
				m21=m21+(pow(x,2)*y*imagenRGB[indice]); 
				//printf("%d ",imagen[indice]);
				pthread_mutex_unlock(&bloqueo);
			//}
		}
		//printf("\n");
	}
	pthread_exit(idh);
}

void momentosHu(){

	long double xbar=0, ybar=0, n10=0, n01=0, n11=0, n20=0, n02=0, n30=0, n03=0, n21=0, n12=0;

	phi=(long double *)malloc(7*sizeof(long double));

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


	phi[0] = n20 + n02;					/*Momentos de Hu*/
	phi[1] = pow((n20 - n02),2) + 4*pow(n11,2);
	phi[2] = pow((n30 - 3*n12),2) + pow((3*n21 - n03),2);
	phi[3] = pow((n30 + n12),2) + pow((n21 + n03),2);
	phi[4] = (n30 - 3*n12) * (n30 + n12) * ( pow((n30 + n12),2) - 3*pow((n21 + n03),2) ) + (3*n21 - n03) * (n21 + n03) * ( 3*pow((n30 + n12),2) - pow((n21 + n03),2) );
	phi[5] = (n20 - n02) * ( pow((n30 + n12),2) - pow((n21 + n03),2) ) + 4 * n11 * (n30 + n12) * (n21 + n03);
	phi[6] = (3*n21 - n03) * (n30 + n12) * ( pow((n30 + n12),2) - 3*pow((n21 + n03),2) ) + (3*n12 - n30) * (n21 + n03) * ( 3*pow((n30 + n12),2) - pow((n21 + n03),2) );


	printf("phi1= %Lf\n",phi[0]);
	printf("phi2= %Lf\n",phi[1]*1);
	printf("phi3= %Lf\n",phi[2]*1);
	printf("phi4= %Lf\n",phi[3]*1);
	printf("phi5= %Lf\n",phi[4]*1);
	printf("phi6= %Lf\n",phi[5]*1);
	printf("phi7= %Lf\n",phi[6]*1);

	/*Normalizacion de Momentos de Hu en el rango de -1 a +1 para NN*/

	// Xrec2 =(-sign(invmoments2(I1)).*(log10(abs(invmoments2(I1)))));
	//(-1*((phi1 > 0) ? 1 : ((phi1 < 0) ? -1 : 0)))*(log10(abs(phi1)));
	// funcion sign:
	// 	1 if the corresponding element of x is greater than 0.
	// 	0 if the corresponding element of x equals 0.
	// 	-1 if the corresponding element of x is less than 0.

	//Otra opcion:
	//trainingData[w][i] = hu[i] < 0 ? (-1)*log10(abs(hu[i])) : log10(abs(hu[i]));

	phi[0]=normalizaPhi(phi[0]);
	phi[1]=normalizaPhi(phi[1]);
	phi[2]=normalizaPhi(phi[2]);
	phi[3]=normalizaPhi(phi[3]);
	phi[4]=normalizaPhi(phi[4]);
	phi[5]=normalizaPhi(phi[5]);
	phi[6]=normalizaPhi(phi[6]);

	printf("phi1N= %Lf\n",phi[0]);
	printf("phi2N= %Lf\n",phi[1]);
	printf("phi3N= %Lf\n",phi[2]);
	printf("phi4N= %Lf\n",phi[3]);
	printf("phi5N= %Lf\n",phi[4]);
	printf("phi6N= %Lf\n",phi[5]);
	printf("phi7N= %Lf\n",phi[6]);

}

long double normalizaPhi(long double phi){
	return (-1*((phi > 0) ? 1 : ((phi < 0) ? -1 : 0))) * (log10(((phi > 0) ? 1*phi : ((phi < 0) ? -1*phi : 0)))); //31 ultimo valor
}

/*

imagenes 5 ---
-28 mayor, menor 2

imagenes 6 ---
-28 mayor, menor 2

*/
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
