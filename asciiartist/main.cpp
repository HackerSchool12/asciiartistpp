#include "Magick++.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace Magick;

/* in which we obtain ascii art
 * (maybe)
 * plan - 
 * open file
 * get edges
 * space filling:
 *	    pick random? points
 *      spacefill to determine shapes
 *      if an internal shape is found, run recursively to check for internal shapes
 * convert into text
 * have ascii art
 * 
 */
void spaceFill(Image& piccy, int boxsize);
void boxup(Image& piccy, Image& piccyedges, int x, int y, int boxsize, char** isdone);
float calcgrey(PixelPacket* pixels, int boxsize);
char asciifill(float grey, string possibleascii);

int main(int argc, char **argv)
{
	InitializeMagick(*argv);
	
	Image piccy;
	piccy.read("fsm.jpg");
	spaceFill(piccy, 20);
	
	int x = 0;
	cout<< "done";


}


void spaceFill(Image& piccy, int boxsize)
{
	char** isdone = new char*[piccy.columns()/boxsize];
	for (size_t i = 0; i < piccy.columns()/boxsize; ++i) {
		isdone[i] = new char[piccy.rows()/boxsize];
		for (size_t j = 0; j < piccy.rows()/boxsize; ++j) {
			isdone[i][j] = '\n';
		}
	}


	Image piccyedges(piccy);
	piccyedges.edge();
	piccyedges.write("edges.bmp");
	cerr << "made edges.bmp" << endl;

	for (size_t i = 0; i < piccy.columns()/boxsize; ++i) {
		for (size_t j = 0; j < piccy.rows()/boxsize; ++j) {
			if (isdone[i][j] == '\n') {
				boxup(piccy, piccyedges, i*boxsize, j*boxsize, boxsize, isdone);
			}
		}
	}
	
	ofstream theart;
	theart.open("theart.txt");
	
	for (size_t j = 0; j < piccy.rows()/boxsize; ++j) {
		for (size_t i = 0; i < piccy.columns()/boxsize; ++i) {
			theart << isdone[i][j];
		}
		theart << endl;
	}
	theart << endl;
	
	theart.close();

	piccy.write("fill.bmp");
	
}

// theoretically this boxes up lots of things maybe
// floodBox!
void boxup(Image& piccy, Image& piccyedges, int x, int y, int boxsize, char** isdone)
{
	//cerr << "boxing" << endl;
	
	// for boxpixels, check for edges
	// if no edges, box over?
	if (x < 0 || y < 0 || x+boxsize > piccy.columns() || y+boxsize > piccy.rows() ) {
		cerr << "out of bounds found at " << x << "," << y << endl;
		return;
	}

	if (isdone[x/boxsize][y/boxsize] != '\n') {
		return;
	}
	
	auto piccypix = piccyedges.getPixels(x, y, boxsize, boxsize);
	bool isblack = true;
	for_each( piccypix, piccypix+(boxsize*boxsize),
		[&isblack](MagickCore::PixelPacket pixel){ 
			if (isblack == true && (pixel.red != 0 || pixel.green != 0 || pixel.blue != 0)) {
				isblack = false;
			}
		}
	);

	if (isblack) {
		// color black
		//piccy.fillColor("black");
		//piccy.strokeColor("red");
		//piccy.draw(DrawableRectangle(x, y, x+boxsize, y+boxsize));
		auto colorpix = piccy.getPixels(x, y, boxsize, boxsize);
		float boxgrey = calcgrey(colorpix, boxsize);
		isdone[x/boxsize][y/boxsize] = asciifill(boxgrey, ".+o$#"); //ascii character for thing

		boxup(piccy, piccyedges, x+boxsize, y, boxsize, isdone);
		boxup(piccy, piccyedges, x, y+boxsize, boxsize, isdone);
		boxup(piccy, piccyedges, x-boxsize, y, boxsize, isdone);
		boxup(piccy, piccyedges, x, y-boxsize, boxsize, isdone);
	} else {
		auto colorpix = piccy.getPixels(x, y, boxsize, boxsize);
		float boxgrey = calcgrey(colorpix, boxsize);
		isdone[x/boxsize][y/boxsize] = asciifill(boxgrey, ".+o$#");

	//	cerr << "found an edge at " << x <<"," << y << endl;
	}

}

float calcgrey(PixelPacket* const pixels, int const boxsize)
{
	float greycolor =0;
	for_each( pixels, pixels+(boxsize*boxsize),
		[&greycolor](MagickCore::PixelPacket pixel){ 
			float grey = (pixel.red+ pixel.green + pixel.blue) / 3;
			greycolor += grey;
		});
	greycolor = greycolor / (boxsize*boxsize);
	return greycolor;
}

char asciifill(float grey, string possibleascii)
{

	int greyval = (int)((grey/USHRT_MAX)*possibleascii.size()-1) ;
 //   cerr << greyval << endl;
	return possibleascii[greyval];
}
