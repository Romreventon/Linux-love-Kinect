#include <XnOpenNI.h>
#include <XnLog.h>
#include <XnCppWrapper.h>
#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std; //Force l'utilisation des fontions standards

int main(int argc, char *argv[]) {
	unsigned int px, x = 1;
	// Créé les 3 composantes Rouge-Vert-Bleue des 12 couleurs triées selon leur température (arc-en-ciel)
	unsigned int R[] = {255, 255, 255, 127, 0, 0, 0, 0, 0, 127, 255, 255};	
	unsigned int G[] = {0, 127, 255, 255, 255, 255, 255, 127, 0, 0, 0, 0};	
	unsigned int B[] = {0, 0, 0, 0, 0, 127, 255, 255, 255, 255, 255, 127};	
	xn::Context context; //créé un contexte : ce qui est branché, start, stop, etc
	xn::DepthGenerator kinect; //créé un générateur de profondeur
	xn::ScriptNode script; //obligatoire pour initialiser le contexte
	xn::DepthMetaData data; //objet contenant le tableau de données
	context.Init(); //initialisation du contexte
	context.InitFromXmlFile("SamplesConfig.xml", script); //charge le fichier de configuration de la kinect
	context.FindExistingNode(XN_NODE_TYPE_DEPTH, kinect); //detecte la kinect et configure le DepthGenerator en fonction
	context.StartGeneratingAll(); //on commence à générer des données
	
	ofstream myfile;
  	myfile.open ("DepthPic.txt"); //Ouvre l'accès au fichier "3DRainbow.txt"
	//while(1) {
		kinect.WaitAndUpdateData(); //attente de nouvelles données		
		kinect.GetMetaData(data); //qui sont stockées dans data
		printf("Acquisition des données...\n");
		for(px = 0; px < 307200; px++) {	
			if(x > 640) {
				x = 1;
				myfile << "\n";
			}	
			myfile << data[px] << " ";		
			x++;		
		}
	//}

	myfile.close(); //Ferme l'accès au fichier "3d_map_color.txt"
	//Relâche les mutex de chaques objets
	context.Release();
	script.Release();
	kinect.Release();
	return 0;
}

