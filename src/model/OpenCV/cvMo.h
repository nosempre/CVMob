/*
 * cvMo.h
 *
 *  Created on: 19/08/2008
 *      Author: paulista
 */

#ifndef CVMO_H_
#define CVMO_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <qcolor.h>

#define MAX_COUNT  4
#define MAX_TIME  2000
#define FPS 25

#include <iostream>
using namespace std;


namespace model {

class cvMo {
private:
	double CalcAngG(double Xc, double Yc, double X, double Y);
	void on_mouse(int event, int x, int y, int flags);
	void on_track(int pos); // Callback for the trackbar
	int NFrame;
	char Rec; // Flag indicating that the trajectories ares recording.
	int flags;
	CvCapture* videoStream;
	CvCapture* capture;
	IplImage* frame;
	CvFont font;
	int count;
	int countA; // Quantidade de pontos do tipo 2 (angulo)
	int countAG;
	char* status;
	int add_remove_pt;
	int add_remove_ag;
	CvPoint2D32f* points[2];
	CvPoint2D32f *swap_points;
	CvPoint2D32f pts[MAX_COUNT][MAX_TIME]; // Vetor com a trajetoria
	int pti[MAX_COUNT];
	int win_size;
	char *ptTipo; // Tipos dos pontos caso seja 1 trajetoria, 2 angulo.
	double Ang1, Ang2;
	char FlPlay; // Flag para play/pause da captura
	int GridStepW;
	char FlStep; // Flag para step-by-step da captura
	int GridStepH;
	char FlGrid; // Flag para ligar Grid
	char FlisOpen; // Flag para indicar que existe um arquivo aberto

	static cvMo *opencv;

public:
	cvMo();

	static cvMo *getInstance();

	bool openVideo(char *fileName);


	static void on_mouseStatic(int event, int x, int y, int flags, void* param);
	void PutText(IplImage *image, char *text, int x, int y, CvFont *font,
			int R, int G, int B);
	char IsRecording();
	int Init(); //Iniciates Opencv empty window
	int Init(char *file); // Iniciates capture from file
	void Record(FILE *Frg);
	bool freePoints();
	bool freeTrajectories();
	void Capture(int Step);
	void LocatePoints();
	void StopRecord();
	void SetPlay(char flag); // Set play/pause captura
	void SetStep(char flag); // Set step-by-step capture.
	void DrawGrid();
	void ShowImage();
	void SetGridStep(int p);
	void SetGrid(char onoff); // Turn on and of the grid
	char IsStepping();
	FILE* AbreArquivo(char *name);
	int GetNTraject(int i); // return size of trajectory i
	int GetPCount(); // return number of points detected
	double GetTrajectX(int point, int time); // Get the X point of trajectory
	double GetTrajectY(int point, int time); // Get the Y point of trajectory
	char GetpTipo(int i); // get the type of point i
	bool ThereIsAngle(); // diz se existe algulos definidos
	IplImage *image, *grey, *prev_grey, *pyramid, *prev_pyramid, *swap_temp;
	int FilePos;
	int TotFrames; // Total de frames do arquivo AVI
	int FrameWidth;
	int FrameHeight;
	int FrameRate;
	int Fc; // Contador de frames
	int FcA; // Contador de frames Anterior
	FILE* FlRec; // Arquivo de saida de dados.
	char FlName[400]; // Name of actual input file
	double Ang[MAX_TIME];
	double TimeA[MAX_TIME];
	int NApoints;
	char* screenName;
	int getTotalFrames();
	~cvMo();

};

}

#endif /* CVMO_H_ */