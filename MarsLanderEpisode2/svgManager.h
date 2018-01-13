#ifndef __SVG_MANAGER_H__
#define __SVG_MANAGER_H__

#include <iostream>
#include <fstream>
#include <vector>

#include "svgConstants.h"

class SVGManager {
public:
	SVGManager();
	~SVGManager();

	void fileInit();
	void fileDone();
	void fileClose();
	void filePrintStr(std::string strToPrint);

	std::string constructStrokeForRGB(int r, int g, int b) const;
	std::string constructStrokeWidth(int width) const;
	std::string constructGId(int id) const;
	std::string constructMultiLineTitle(const std::vector<std::string>& titleLines) const;

private:
	std::ofstream svgHtmlFileStream;
};

#endif // __SVG_MANAGER_H__
