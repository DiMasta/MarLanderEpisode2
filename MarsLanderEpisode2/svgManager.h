#ifndef __SVG_MANAGER_H__
#define __SVG_MANAGER_H__

#include <iostream>
#include <fstream>

#include "svgConstants.h"

class SVGManager {
public:
	SVGManager();
	~SVGManager();

	void fileInit();
	void fileClose();
private:
	std::ofstream svgHtmlFileStream;
};

#endif // __SVG_MANAGER_H__
