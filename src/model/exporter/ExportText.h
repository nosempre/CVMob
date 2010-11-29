/*
 * ExportText.h
 *
 *  Created on: 27/09/2008
 *      Author: paulista
 */

#ifndef EXPORTTEXT_H_
#define EXPORTTEXT_H_

#include "IExportStrategy.h"
#include <fstream>
#include "../../view/reportdialog.h"
using namespace std;

namespace model{


class ExportText: public IExportStrategy {
public:
	ExportText();
	void setFile(char* filename);
        void exportData(int count, QVector <double> time, QVector <double> velocity, QVector <double> acceleration,
                        QVector <double> work, QVector <CvPoint2D32f> trajectorie, double horizontalRazao,
                        double verticalRazao,int initFrame,QVector <double> velocityX,QVector <double> velocityY,
                        QVector <double> accelerationX,QVector <double> accelerationY);
        void exportReport(int count, QVector <double> time, QVector <double> velocity, QVector <double> acceleration,
                          QVector <double> work, QVector <CvPoint2D32f> trajectorie, double horizontalRazao,
                          double verticalRazao,int initFrame,QVector <double> velocityX,QVector <double> velocityY,
                          QVector <double> accelerationX,QVector <double> accelerationY);
	void closeFile();
	virtual ~ExportText();
private:
	ofstream file;
        reportDialog repo;
        void makeLine(QString lo, double *v);

};

}

#endif /* EXPORTTEXT_H_ */