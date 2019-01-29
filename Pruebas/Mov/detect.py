# encoding: utf-8
import numpy as np
import cv2
import time
#en python 2 no hay error por la camara en python 3 si los hay

camara = cv2.VideoCapture("prueba.mp4")
#camara = cv2.VideoCapture(0)
 
fondo = None 							# Inicializar el fondo 
 
while True:								# Recorrer todo el video

	(grabbed, frame) = camara.read() 	# Obtener frame del video
 
	if not grabbed: 					# Si hemos llegado al final del vídeo salimos
		break
 
	gris = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)		# Escala de grises
 
	gris = cv2.GaussianBlur(gris, (23, 23), 0)			# 23 Tamanio del kernel, Suavizado para eliminar ruido, calcula el 0 por default
 
	if fondo is None:									# Si todavía no hemos obtenido el fondo, lo obtenemos
														# Será el primer frame que obtengamos
		fondo = gris
		continue
 													
	resta = cv2.absdiff(fondo, gris)					# Calculo de la diferencia entre el fondo y el frame actual
 
	umbral = cv2.threshold(resta, 25, 255, cv2.THRESH_BINARY)[1] # Umbralizado
 	#revisar con adaptativeThreshold
	umbral = cv2.dilate(umbral, None, iterations=2)		# Dilatar umbral para evitar huecos
 
	contornosimg = umbral.copy()						# Copiamos el umbral para detectar los contornos
 	#im,
	contornos, hierarchy = cv2.findContours(contornosimg,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)	# Buscamos contorno en la imagen
 	# (imagen, contornos, jerarquia) = cv2.findContours(imagenbinarizada, modo_contorno, metodo_aproximacion)
 	# CHAIN_APPROX_SIMPLE=elimina todos los puntos redundantes, RETR_TREE=Recupera todos los contornos y crea una lista de jerarquía familiar completa.
	for c in contornos:									# Recorremos todos los contornos encontrados
		if cv2.contourArea(c) < 500:					# Eliminamos los contornos más pequeños
			continue
 
		(x, y, w, h) = cv2.boundingRect(c)				# Obtenemos el bounds del contorno, el rectángulo mayor que engloba al contorno
		cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)	# Dibujamos el rectángulo del bounds

	cv2.namedWindow("Original", cv2.WINDOW_NORMAL)
	cv2.imshow("Original", frame)
	cv2.namedWindow("Original con umbral", cv2.WINDOW_NORMAL)
	cv2.imshow("Original con umbral", umbral)
	cv2.namedWindow("Resta", cv2.WINDOW_NORMAL)
	cv2.imshow("Resta", resta)
	cv2.namedWindow("Contorno", cv2.WINDOW_NORMAL)
	cv2.imshow("Contorno", contornosimg)
 
	# Capturamos una tecla para salir
	key = cv2.waitKey(1) & 0xFF
 
	# Tiempo de espera para que se vea bien
	time.sleep(0.015)
 
	# Si ha pulsado la letra s, salimos
	if key == ord("s"):
		break
 
camara.release()
cv2.destroyAllWindows()