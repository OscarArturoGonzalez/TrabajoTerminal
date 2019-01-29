#!/usr/local/bin/python3.5
# encoding: utf-8
import numpy as np
import cv2
 
cap = cv2.VideoCapture('prueba.MOV')

fgbg = cv2.createBackgroundSubtractorKNN(history=500, dist2Threshold=400, detectShadows=False) # Llamada al método
 
# Deshabilitamos OpenCL, si no hacemos esto no funciona
cv2.ocl.setUseOpenCL(False)
 
while(1):
	ret, frame = cap.read()		# Lectura de cada imagen
 
	if not ret:					# Termina al final del vídeo
		break
 
	fgmask = fgbg.apply(frame)	# Aplicar en cada imagen
 
	contornosimg = fgmask.copy()	# Copiar el umbral para detectar los contornos
 
	# Buscamos contorno en la imagen
	im, contornos, hierarchy = cv2.findContours(contornosimg,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
 
	# Recorremos todos los contornos encontrados
	for c in contornos:
		# Eliminamos los contornos más pequeños
		if cv2.contourArea(c) < 500:
			continue
 
		# Obtenemos el bounds del contorno, el rectángulo mayor que engloba al contorno
		(x, y, w, h) = cv2.boundingRect(c)
		# Dibujamos el rectángulo del bounds
		cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
 
	# Mostramos las capturas

	cv2.namedWindow("Camara", cv2.WINDOW_NORMAL)
	cv2.imshow('Camara',frame)
	cv2.namedWindow("Umbral", cv2.WINDOW_NORMAL)
	cv2.imshow('Umbral',fgmask)
	cv2.namedWindow("Contornos", cv2.WINDOW_NORMAL)
	cv2.imshow('Contornos',contornosimg)
 
	# Sentencias para salir, pulsa 's' y sale
	k = cv2.waitKey(30) & 0xff
	if k == ord("s"):
		break
 
# Liberamos la cámara y cerramos todas las ventanas
cap.release()
cv2.destroyAllWindows()