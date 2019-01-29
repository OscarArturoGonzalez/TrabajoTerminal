import cv2
import numpy as np
from PIL import Image
import os

def mimagen(img):
    n=0
    for y in range(height):
        for x in range(width):
            aux=img[y][x]
            if aux==255:
                n=0
            if aux==0:
                n=255
            img[y][x]=n
    return img

#Primer momeno m00 obtiene el area del objeto de la forma SUM(x^i y^j)(I(x,y))<<-Img binarizada
def momento(img,i,j):
    #Arreglo al sacar el momento de la imagen
    arr=[]
    for y in range(height):
        for x in range(width):
            aux2=img[y][x]
            if aux2==255:
                aux=(pow(x,i)*pow(y,j))
                arr.append(aux)
    res=[]
    cont=0
    
    #Multiplicacion de arr (momento) * Imagen binarizada
    for y in range(height):
        for x in range(width):
            auxi=img[y][x]
            if (auxi)==255:
                auxu=1
                aux=auxu*arr[cont]
                res.append(aux)
                cont+=1
    
    #Sumatoria de (x^i y^j)(I(x,y))
    sum=0 
    for ii in range(len(res)):
        sum=sum+res[ii]
    return sum

def momento_central(img,i,j):
    Cx=(momento(img,0,1))/(momento(img,0,0))
    Cy=(momento(img,1,0))/(momento(img,0,0))
    #print("cx=",Cx,"Cy=",Cy)
    res=[]
    #Multiplicacion de momentos centrales
    for y in range (height):
        for x in range(width):
            aux=img[y][x]
            if aux==255:
                auxp=(pow((x-Cx),i))*(pow((y-Cy),j))
                res.append(auxp)
    #Suma de la multiplicacion de momentos centrales por imagen bin
    sum=0 
    for ii in range(len(res)):
        sum=sum+res[ii]
    return sum

def normalizar(img,i,j):
    m1=momento_central(img,i,j)
    m2=momento_central(img,0,0)
    #print(m1,m2)
    #Momento central normalizados op=p+q/2
    operacion=1+((i+j)/2)
    #print("Operacion=",operacion)
    mnormal=m1/(pow(m2,operacion))
    #print("normal=",mnormal)
    return mnormal

def momentos4_hu(img):
    I1=(normalizar(img,2,0)+normalizar(img,0,2))
    print("I1=",I1)
    I2=pow((normalizar(img,2,0)+normalizar(img,0,2)),2) + pow(4*normalizar(img,1,1),2)
    print("I2=",I2)
    I3=pow((normalizar(img,3,0)+ 3*normalizar(img,1,2)),2) + pow((3*normalizar(img,2,1))- normalizar(img,0,3),2)
    print("I3=",I3)
    I4=pow(normalizar(img,3,0)*normalizar(img,1,2),2) + pow(normalizar(img,2,1)- normalizar(img,0,3),2)
    print("I4=",I4)

    I5=(normalizar(img,3,0)- 3*normalizar(img,1,2))*(normalizar(img,3,0)+normalizar(img,1,2))*(pow(normalizar(img,3,0)+normalizar(img,1,2),2) - 3*pow(normalizar(img,2,1)+normalizar(img,0,3),2)) +(3*normalizar(img,2,1) - normalizar(img,0,3))*(normalizar(img,2,1)+normalizar(img,0,3))*(3*pow(normalizar(img,3,0)+normalizar(img,1,2),2)) - pow(normalizar(img,2,1)+normalizar(img,0,3),2)
    print("I5=",I5)

    I6=(normalizar(img,2,0)-normalizar(img,0,2))*(pow(normalizar(img,3,0)-normalizar(img,1,2),2)) - pow(normalizar(img,2,1)-normalizar(img,0,3),2)+4*(normalizar(img,1,1))*normalizar(img,3,0)+normalizar(img,1,2)*normalizar(img,2,1)+normalizar(img,0,3)
    print("I6=",I6)

    I7=(3*normalizar(img,2,1)- normalizar(img,3,0))*(normalizar(img,3,0)+ normalizar(img,1,2))*(pow(normalizar(img,3,0)+ normalizar(img,1,2),2)) - 3*pow(normalizar(img,2,1)+ normalizar(img,0,3),2)+(3*normalizar(img,1,2)- normalizar(img,3,0))*(normalizar(img,2,1)+ normalizar(img,0,3))*((3*pow(normalizar(img,3,0)- normalizar(img,1,2),2)) - pow(normalizar(img,2,1)+ normalizar(img,0,3),2) )
    print("I7=",I7)


#Abrir imagen
img = cv2.imread('silueta.png',0)
# Imagen binarizada
ret1,bin = cv2.threshold(img,180,255,cv2.THRESH_BINARY)
#Dimensiones de la imagen
dim=img.shape
height = img.shape[0]
width = img.shape[1]
print("Tamaño de imagen: "+str(width)+"x"+str(height))
#Invertir imagen Sossa maneja 1 objeto 0 fondo, varia un poco en el calculo
img2=mimagen(bin)
cv2.imwrite('bin2.jpg',img2)

m=momento(img2,0,0)
print("Area del objeto",m)
#mc00=momento_central(bin,0,1)
#print("MC00=",mc00)
#mn00=normalizar(img2,0,0)
#print("Momento normalizado 00= ",mn00)
#mn00=normalizar(img2,0,1)
#print("Momento normalizado 01= ",mn00)
momentos4_hu(img2)
