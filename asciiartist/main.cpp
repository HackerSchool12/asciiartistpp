#include "Magick++.h"
#include <iostream>
#include <string>

using namespace std;

typedef Magick::Image image;

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
int main()
{
	image piccy("word.png");
	spaceFill(piccy, 20);

}

void spaceFill(image& piccy, int boxsize)
{
	bool** isdone = new bool*[piccy.columns()/boxsize];
	for (int i = 0; i < piccy.columns()/boxsize; ++i) {
		isdone[i] = new bool[piccy.rows()/boxsize];
	}

	image piccyedges(piccy);
	piccyedges.edge();

	boxup(piccy, piccyedges, 0, 0, boxsize, isdone);
	
	piccy.write("fill.bmp");
	piccyedges.write("edges.bmp");
}

// theoretically this boxes up lots of things maybe
// floodBox!
void boxup(image& piccy, image& piccyedges, int x, int y, int boxsize, bool** isdone)
{
	// for boxpixels, check for edges
	// if no edges, box over?
	if (x < 0 || y < 0 || x+boxsize > piccy.columns() || y+boxsize > piccy.rows() ) {
		return;
	}

	if (isdone[x/boxsize][y/boxsize]) {
		return;
	}
	
	auto piccypix = piccyedges.getPixels(x, y, boxsize, boxsize);
	bool isblack = true;
	for_each( piccypix, piccypix+(boxsize*boxsize*sizeof(MagickCore::PixelPacket)),
		[&isblack](MagickCore::PixelPacket pixel){ 
			if (isblack == true && (pixel.red != 0 || pixel.green != 0 || pixel.blue != 0)) {
				isblack = false;
			}
		}
	);

	if (isblack) {
		// color black
		piccy.fillColor("black");
		piccy.strokeColor("red");
		piccy.draw(Magick::DrawableRectangle(x, y, x+boxsize, y+boxsize));
		isdone[x/boxsize][y/boxsize] = true;

		boxup(piccy, piccyedges, x+boxsize, y, boxsize, isdone);
		boxup(piccy, piccyedges, x, y+boxsize, boxsize, isdone);
		boxup(piccy, piccyedges, x-boxsize, y, boxsize, isdone);
		boxup(piccy, piccyedges, x, y-boxsize, boxsize, isdone);
	}



}