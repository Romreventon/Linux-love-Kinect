//--- Description ---//
// Ce programme vous permet de génerer une carte 2D d'ouverture de 57° et génère 2 fichiers exploitables :
// "DistGraph.dat" : fichier texte contenant la position de chacuns des points de la ligne de balayage horizontale 
// "DistGraph.bmp" : représentation graphique de la carte 2D (distance en fonction de l'angle)

//--- Utilisation ---//
// g++ -lOpeniNI -I /usr/include/ni/ DistGraph.cpp -o DistGraph
// ./DistGraph
// gnuplot DistGraph.plt
// gpicview DistGraph.bmp

#include <XnOpenNI.h>
#include <XnLog.h>
#include <XnCppWrapper.h>
#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std; //force l'utilisation des fontions standards car OpenNI redéfini certaines d'entres elles

int main(int argc, char *argv[]) {
	unsigned int px = 0, x = 640, y = 480;
	xn::Context context; //créé un contexte : ce qui est branché, start, stop, etc
	xn::DepthGenerator kinect; //créé un générateur de profondeur
	xn::ScriptNode script; //obligatoire pour initialiser le contexte
	xn::DepthMetaData data; //objet contenant le tableau unidimensionel des données de pronfondeurs associés à chaques pixels
	context.Init(); //initialisation du contexte
	context.InitFromXmlFile("SamplesConfig.xml", script); //charge le fichier de configuration de la kinect
	context.FindExistingNode(XN_NODE_TYPE_DEPTH, kinect); //detecte la kinect et configure le DepthGenerator en fonction
	context.StartGeneratingAll(); //on commence à générer des données
	
	ofstream myfile("2d_map_plot.dat"); //crée le fichier contenant la position des points dans le répertoire courant
	printf("Acquisition des données de profondeur...\n");
	kinect.WaitAndUpdateData(); //attente de nouvelles données		
	kinect.GetMetaData(data); //qui sont stockées dans le tableau data	
	myfile << "#x y" << endl; //ajoute une en-tête au fichier
	
	for(px = 0; px < 307200; px++) {	//parcourt l'intégralité du tableau "data" (640x480 = 307200 pixels)
		if(x < 1) {
			x = 640;
			y--;
		}	
		//enregistre la distance mesurée en fonction de l'angle 
		if (y == 240) //on utilise que la ligne horizontale du mileu du tableau pour obtenir un balayage 2D 
			myfile << x*57/640 << " " << data[px] << "\n"; //enregistrement des données au format : angle distance
		x--;		
	}

	myfile.close(); //ferme l'accès au fichier "DistGtaph.dat"
	//Relâche les mutex de chaques objets
	context.Release();
	script.Release();
	kinect.Release();
	return 0;
}
