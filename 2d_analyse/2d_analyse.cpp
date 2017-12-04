//--- Description ---//
// Ce programme vous permet de génerer une carte 2D d'ouverture de 57° et génère 2 fichiers exploitables :
// "2d_map_plot.dat" : fichier texte contenant la position de chacuns des points de la ligne de balayage horizontale 
// "2d_map_plot.bmp" : représentation graphique de la carte 2D (distance en fonction de l'angle)

//--- Utilisation ---//
// g++ -lOpeniNI -I /usr/include/ni/ 2d_map_plot.cpp -o 2d_map_plot
// ./2d_map_plot
// gnuplot 2d_map_plot.plt
// gpicview 2d_map_plot.bmp

#include <XnOpenNI.h>
#include <XnLog.h>
#include <XnCppWrapper.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

using namespace std; //force l'utilisation des fontions standards car OpenNI redéfini certaines d'entres elles

int main(int argc, char *argv[]) {
	unsigned int px = 0, x = 639, y = 480;
	int middlelinePx[640], seuil;
	int edges[320];
	int edges_nb = 0;
	float angle;
	int cmd = 1, pixel = 1, distmin, i, j;
	xn::Context context; //créé un contexte : ce qui est branché, start, stop, etc
	xn::DepthGenerator kinect; //créé un générateur de profondeur
	xn::ScriptNode script; //obligatoire pour initialiser le contexte
	xn::DepthMetaData data; //objet contenant le tableau unidimensionel des données de pronfondeurs associés à chaques pixels
	context.Init(); //initialisation du contexte
	context.InitFromXmlFile("SamplesConfig.xml", script); //charge le fichier de configuration de la kinect
	context.FindExistingNode(XN_NODE_TYPE_DEPTH, kinect); //detecte la kinect et configure le DepthGenerator en fonction
	context.StartGeneratingAll(); //on commence à générer des données
	
	printf("Acquisition des données de profondeur de la ligne horizontale de l'image...\n");
	kinect.WaitAndUpdateData(); //attente de nouvelles données		
	kinect.GetMetaData(data); //qui sont stockées dans le tableau data	
	
	for(px = 0; px < 307200; px++) {	//parcourt l'intégralité du tableau "data" (640x480 = 307200 pixels)
		if(x < 1) {
			x = 640; y--;
		}	
		if (y == 240) { //on utilise que la ligne horizontale du mileu du tableau pour obtenir un balayage 2D 
		//enregistre la distance dans le tableau middlelinePx et corrige automatiquement les erreurs de mesure de la kinect	 en remplacant la valeur erronée (0 mm) par celle du pixel le plus proche
			if((data[px] == 0) && (x < 320)){
				j = 0;
				while(data[px-j] == 0) {
					j++;
				}
				middlelinePx[x] = (int)data[px-j];
			}
			else if((data[px] == 0) && (x > 320)){
				j = 0;
				while(data[px+j] == 0) {
					j++;
				}
				middlelinePx[x] = (int)data[px+j];
			}	
			else
				middlelinePx[x] = (int)data[px]; //pixel 640 --> 1
				printf("pixel %d	-	%f°	: %d mm\n", x, (float)x*57/640, middlelinePx[x]);
		}
		x--;		
	}

	while(cmd != 0) {
		printf("\nChoisir une commande ('0' pour quitter) :\n");
		printf("'1' : retourne la distance et l'angle d'un pixel\n");
		printf("'2' : zones d'écart de profondeur avec seuil réglable\n");
		printf("'3' : renseigne la distance et l'angle du point le plus proche\n");
		printf("'4' : relance une acquisition\n");
		printf(">>> ");
		scanf("%d", &cmd);
	
		if(cmd == 1) {
			printf("\n(-1 pour quitter)\n");
			while(pixel != -1) {
				printf("pixel (1-640) ? ");
				scanf("%d", &pixel);
				printf(" --> %d mm (%f°)\n", middlelinePx[pixel], (float)pixel*57/640);
			}
			pixel = 0;
		}

		else if(cmd == 2) {
			printf("\n(-1 pour quitter)\n");
			printf("seuil(mm) ? ");
			scanf("%d", &seuil);
			edges_nb=0;
			for(i = 1; i < 640; i++) {
				if(std::abs(middlelinePx[i] - middlelinePx[i+1]) >= seuil) {
					edges[edges_nb] = i;
					edges_nb++;
				}
			}
			if(edges_nb != 0) {
			printf("\nOn distingue %d zone(s) séparée(s) par %d interzone(s) !\n", edges_nb+1, edges_nb, seuil);
			for(i = 0; i < edges_nb; i++) {
				//printf("zone %d de %f° à %f°\n", i+1, (float)edges[i]*57.0/640.0, (float)edges[i+1]*57.0/640.0); 
				printf("interzone %d à %f°\n", i+1, (float)edges[i]*57.0/640.0); 
			}
			pixel = 0;
			} else {
				printf("\nAucune zone détectée !\n");
			}
		}

		else if(cmd == 3) {
			x = 0, distmin = middlelinePx[1];
			for(i = 1; i < 640; i++) {
				if(middlelinePx[i] < distmin) {
					distmin = middlelinePx[i];
					x = i;
				}
			}
		printf("\nCoordonées du point le plus près (pixel %d) : [%f°; %d mm]\n", x, (float)x*57.0/640, middlelinePx[x]); 
		pixel = 0;
		}

	else if(cmd == 4) {
		printf("\nAcquisition des données de profondeur de la ligne horizontale de l'image...\n");
		kinect.WaitAndUpdateData(); //attente de nouvelles données		
		kinect.GetMetaData(data); //qui sont stockées dans le tableau data	
		px = 0, x = 639, y = 480;
		for(px = 0; px < 307200; px++) {	//parcourt l'intégralité du tableau "data" (640x480 = 307200 pixels)
			if(x < 1) {
				x = 640; y--;
			}	
			if (y == 240) { //on utilise que la ligne horizontale du mileu du tableau pour obtenir un balayage 2D 
		//enregistre la distance dans le tableau middlelinePx et corrige automatiquement les erreurs de mesure de la kinect	 en remplacant la valeur erronée (0 mm) par celle du pixel valide le plus proche
				if((data[px] == 0) && (x < 320)){
					j = 0;
					while(data[px-j] == 0) {
						j++;
					}
					middlelinePx[x] = (int)data[px-j];
				}
				else if((data[px] == 0) && (x > 320)){
					j = 0;
					while(data[px+j] == 0) {
						j++;
					}
					middlelinePx[x] = (int)data[px+j];
				}	
				else
					middlelinePx[x] = (int)data[px]; //pixel 640 --> 1
					printf("pixel %d	-	%f°	: %d mm\n", x, (float)x*57/640, middlelinePx[x]);
			}
		x--;		
		}
	}

	}

	//Relâche les mutex de chaques objets
	context.Release();
	script.Release();
	kinect.Release();
	return 0;
}


