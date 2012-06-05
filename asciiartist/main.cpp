#include "Magick++.h"
#include <iostream>
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
void boxup(Image& piccy, Image& piccyedges, int x, int y, int boxsize, bool** isdone);

int main(int argc, char **argv)
{
	InitializeMagick(*argv);
	
	Image piccy;
	piccy.read("word.gif");
	spaceFill(piccy, 5);

	int x = 0;
	cin >> x;

}

void spaceFill(Image& piccy, int boxsize)
{
	bool** isdone = new bool*[piccy.columns()/boxsize];
	for (size_t i = 0; i < piccy.columns()/boxsize; ++i) {
		isdone[i] = new bool[piccy.rows()/boxsize];
		for (size_t j = 0; j < piccy.rows()/boxsize; ++j) {
			isdone[i][j] = false;
		}
	}


	Image piccyedges(piccy);
	piccyedges.edge();
	piccyedges.write("edges.bmp");
	cerr << "made edges.bmp" << endl;

	boxup(piccy, piccyedges, 10, 10, boxsize, isdone);
	piccy.write("fill.bmp");
	
}

// theoretically this boxes up lots of things maybe
// floodBox!
void boxup(Image& piccy, Image& piccyedges, int x, int y, int boxsize, bool** isdone)
{
	cerr << "boxing" << endl;
	
	// for boxpixels, check for edges
	// if no edges, box over?
	if (x < 0 || y < 0 || x+boxsize > piccy.columns() || y+boxsize > piccy.rows() ) {
		cerr << "out of bounds found at " << x << "," << y << endl;
		return;
	}

	if (isdone[x/boxsize][y/boxsize]) {
		return;
	}
	
	auto piccypix = piccyedges.getPixels(x, y, boxsize, boxsize);
	bool isblack = true;
	for_each( piccypix, piccypix+(boxsize*boxsize*sizeof(MagickCore::PixelPacket)),
		[&isblack](MagickCore::PixelPacket pixel){ 
			cout << "RGB: " << pixel.red << "," << pixel.green << "," << pixel.blue << endl;
			if (isblack == true && (pixel.red != 0 || pixel.green != 0 || pixel.blue != 0)) {
				isblack = false;
			}
		}
	);

	if (isblack) {
		// color black
		piccy.fillColor("black");
		piccy.strokeColor("red");
		piccy.draw(DrawableRectangle(x, y, x+boxsize, y+boxsize));
		isdone[x/boxsize][y/boxsize] = true;

		boxup(piccy, piccyedges, x+boxsize, y, boxsize, isdone);
		boxup(piccy, piccyedges, x, y+boxsize, boxsize, isdone);
		boxup(piccy, piccyedges, x-boxsize, y, boxsize, isdone);
		boxup(piccy, piccyedges, x, y-boxsize, boxsize, isdone);
	} else {
		cerr << "found an edge at " << x <<"," << y << endl;
	}



}