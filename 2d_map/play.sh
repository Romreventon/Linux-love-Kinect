#!/bin/bash
# 2d_map_plot vous permet de réaliser une carte 2D couvrant un angle horizontal maximum de 57° (Kinect XBOX 360 v1)
file="2d_map_plot"
myvar="$PWD" #enregistre le chemin du répertoire courant
red="\e[1;31m"
blue="\e[1;34m"
green="\e[1;32m"
if ! [ -f "$file" ]; then #si le fichier éxécutable n'existe pas
	printf "$red[Warning] L'éxécutable nommé '2d_map_plot' n'est pas présent dans ce répertoire ($myvar) !\n\e[1;32m"
	printf  "$blue[Message] Pressez Entrer pour compiler '2d_map_plot.cpp' ou CRTL+C pour annuler : "	
	read -p ""
   printf "$blue[Message] Compilation de '2d_map_plot'...\e[1;32m\n"
	g++ -lOpenNI -I /usr/include/ni/ 2d_map_plot.cpp -o 2d_map_plot
	printf "$blue[Message] Compilation terminée :D !\e[1;32m\n"
fi
	#si le fichier éxécutable existe
	printf "$blue[Message] Pressez Entrer pour lancer l'acquisition ou CRTL+C pour annuler : \e[1;32m"
	read -p ""
	printf "$green[2d_map_plot] "
	./2d_map_plot #éxécution de "2d_map_plot"	
	if [ -f "2d_map_plot.dat" ]; then #Vérifie si le fichier des données de profondeur à bien été créé
		printf "$blue[Message] '2d_map_plot.dat' à correctement été créé\n\e[1;32m"
		gnuplot 2d_map_plot.plt #éxécute le script "2d_map_plot.plt" qui génère l'image de la carte 2D
		if [ -f "2d_map_plot.bmp" ]; then #si l'image a bien été créée
			printf "$blue[Message] '2d_map_plot.bmp' à correctement été créé\n\e[1;32m"
			printf "$blue[Message] Pressez Entrer pour visualiser la carte 2D ou CRTL+C pour annuler : \e[1;32m"
			read -p ""			
			gpicview 2d_map_plot.bmp #affiche l'image de la carte 2D depuis l'utilitaire "gpicview"
		fi
	fi
exit 0
