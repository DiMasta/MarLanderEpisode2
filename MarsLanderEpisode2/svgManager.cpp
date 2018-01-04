#include "SVGManager.h"

using namespace std;

SVGManager::SVGManager() :
	svgHtmlFileStream()
{
	fileInit();
}

//*************************************************************************************************************
//*************************************************************************************************************

SVGManager::~SVGManager() {
	fileClose();
}

//*************************************************************************************************************
//*************************************************************************************************************

void SVGManager::fileInit() {
	svgHtmlFileStream.open(SVG_HTML_FILE_NAME, ofstream::out | ofstream::trunc);
	svgHtmlFileStream << FILE_START;
}

//*************************************************************************************************************
//*************************************************************************************************************

void SVGManager::fileDone() {
	svgHtmlFileStream << FILE_END;
}

//*************************************************************************************************************
//*************************************************************************************************************

void SVGManager::fileClose() {
	svgHtmlFileStream.close();
}

//*************************************************************************************************************
//*************************************************************************************************************

void SVGManager::filePrintStr(string strToPrint) {
	svgHtmlFileStream << strToPrint;
}