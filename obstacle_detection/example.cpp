#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <math.h>
#include <string.h>

using namespace cv;
using namespace std;

int thresh = 50, N = 11;
const char* wndname = "Détection d'obstacles";

// cherche le cosinus de l'angle entre les vecteurs
// de pt0->pt1 et de pt0->pt2
static double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// retourne la séquence de rectangles détectés sur l'image
static void findSquares( const Mat& image, vector<vector<Point> >& squares )
{
    squares.clear();

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // diminue et agrandi l'image pour filtrer le bruit
    pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    pyrUp(pyr, timg, image.size());
    vector<vector<Point> > contours;

    // cherche des rectangles sur tous plans de couleurs de l'image
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // on essaie plusieurs seuils
        for( int l = 0; l < N; l++ )
        {
            if( l == 0 )
            {
                // on appliquer un filtre canny pour détecter les contours de l'image
                Canny(gray0, gray, 0, thresh, 5);
                // on applique un dilatation pour supprimer les possibles trous entre les lignes des contours
                dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                //on applique le seuil si l != 0
				//tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/N;
            }

            // cherche les contours et les enregistre dans une liste
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

            vector<Point> approx;

            // on teste chaque contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
				// Contour approximatif avec une précision proportionnelle au périmètre du contour
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

                // les contours des rectangles doivent avoir 4 sommets et être convexes
				//on utilise la valeur absolue de l'aire car elle peut être positive ou négative en fonction de l'orientation des contours
                if( approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 1000 &&
                    isContourConvex(Mat(approx)) )
                {
                    double maxCosine = 0;

                    for( int j = 2; j < 5; j++ )
                    {
                        // cherche le cosinus maximum de l'angle entre la jointure des contours
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // si le cosinus de tous les angles sont petits
                    // on écrits les sommet du quadrangle (forme composée de 4 angles...)
                    if( maxCosine < 0.3 )
                        squares.push_back(approx);
                }
            }
        }
    }
}


// Dessine tous les rectangles dans l'image
static void drawSquares( Mat& image, const vector<vector<Point> >& squares )
{
    for( size_t i = 0; i < squares.size(); i++ )
    {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, LINE_AA);
    }

    imshow(wndname, image);
}


int main(int /*argc*/, char** /*argv*/)
{
    static const char* names[] = { "DepthPic.png", 0 };
    help();
    namedWindow( wndname, 1 );
    vector<vector<Point> > squares;

    for( int i = 0; names[i] != 0; i++ )
    {
        Mat image = imread(names[i], 1);
        if( image.empty() )
        {
            cout << "impossible de charger l'image !" << names[i] << endl;
            continue;
        }

        findSquares(image, squares);
        drawSquares(image, squares);

        int c = waitKey();
        if( (char)c == 27 )
            break;
    }

    return 0;
}
