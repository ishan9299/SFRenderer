#ifndef SFPLOT_H
#define SFPLOT_H

#include "sfmath/sfmath.h"

char b[64];
int buffer[800*600];

struct Bitmap {
	char *memory;
	unsigned int width;
	unsigned int height;
	unsigned int bpp; // bytes
};

typedef struct Bitmap Bitmap;

union RGBAColor {
	struct {
		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char a;
	};
	unsigned int color;
};

typedef union RGBAColor RGBAColor;

Bitmap bitmap;

int IsInsideTriangle(int *a, int *b, int *c)
{
	// NOTE(not-set): make sure the winding order of the vertices is counter clockwise
	int result = ((b[0] - a[0]) * (c[1] - a[1])) - ((b[1] - a[1]) * (c[0] - a[0]));
	return result;
}

void plotPixel(int x, int y, RGBAColor *color)
{
	if ((x < bitmap.width) && (y < bitmap.height)) {
		int offset = 0;
        
		bitmap.memory[(((y * bitmap.width) + x) * bitmap.bpp) + offset] = color->b;
		offset++;
        
		bitmap.memory[(((y * bitmap.width) + x) * bitmap.bpp) + offset] = color->g;
		offset++;
        
		bitmap.memory[(((y * bitmap.width) + x) * bitmap.bpp) + offset] = color->r;
		offset++;
        
		bitmap.memory[(((y * bitmap.width) + x) * bitmap.bpp) + offset] = color->a;
		offset++;
	}
}

void plotLine(int *v1, int *v2, RGBAColor *color)
{
	int x0 = v1[0];
	int y0 = v1[1];
    
	int x1 = v2[0];
	int y1 = v2[1];
    
	int steep = 0;
    
	if (abs(y1 - y0) < abs(x1 - x0)) {
		if (x1 < x0) {
			SWAP(int, x0, x1);
			SWAP(int, y0, y1);
		} 
        
	} else {
		steep = 1;
        
		SWAP(int, x0, y0);
		SWAP(int, x1, y1);
        
		if (y1 < y0) {
			SWAP(int, x0, x1);
			SWAP(int, y0, y1);
		}
        
		if (x1 < x0) {
			SWAP(int, x0, x1);
			SWAP(int, y0, y1);
		}
	}
    
	int deltaY = y1 - y0;
	int deltaX = x1 - x0;
    
	int increment = 1;
    
	if (deltaY < 0) {
		increment = -1;
		deltaY = -deltaY;
	}
    
	int decisionParam = (2 * deltaY) - deltaX;
	int y = y0;
    
	for (int X = x0; X <= x1; X++) {
		if (steep == 1) {
			plotPixel(y, X, color);
		} else {
			plotPixel(X, y, color);
		}
		if (decisionParam > 0) {
			y = y + increment;
			decisionParam = decisionParam + (2 * (deltaY - deltaX));
		} else {
			decisionParam = decisionParam + (2 * deltaY);
		}
	}
    
}

void plotTriangle(int *v1,
                  int *v2,
                  int *v3, RGBAColor *color) {
    
	plotLine(v1, v2, color);
	plotLine(v2, v3, color);
	plotLine(v1, v3, color);
}

void plotFilledTriangle(vec3i32 *v, float *z_buffer, RGBAColor *colors)
{
    
	int anticlockwise = 1;
    
	int maxY = MAX(MAX(v[0].y, v[1].y), v[2].y);
	int minY = MIN(MIN(v[0].y, v[1].y), v[2].y);
    
	int maxX = MAX(MAX(v[0].x, v[1].x), v[2].x);
	int minX = MIN(MIN(v[0].x, v[1].x), v[2].x);
    
	// check if triangle is clockwise or counter clockwise.
	int area =
    ((v[1].x - v[0].x) * (v[2].y - v[0].y)) -
    ((v[2].x - v[0].x) * (v[1].y - v[0].y));
    
	if (area == 0) {
        return;
    }
    
    for (int Y = minY; Y <= bitmap.height; Y++) {
        for (int X = minX; X <= bitmap.width; X++) {
            
            int p[2];
            p[0] = X;
            p[1] = Y;
            
            float w0 =
            ((v[1].x - p[0]) * (v[2].y - p[1])) -
            ((v[2].x - p[0]) * (v[1].y - p[1]));
            
            float w1 =
            ((v[2].x - p[0]) * (v[0].y - p[1])) -
            ((v[0].x - p[0]) * (v[2].y - p[1]));
            
            float w2 =
            ((v[0].x - p[0]) * (v[1].y - p[1])) -
            ((v[1].x - p[0]) * (v[0].y - p[1]));
            
            w0 = w0 / area;
            w1 = w1 / area;
            w2 = w2 / area;
            
            if (w0 > 0 && w1 > 0 && w2 > 0) {
                
                float z_value[3];
                
                z_value[0] = 1.0f / TYPE_PUN(v[0].z, float);
                z_value[1] = 1.0f / TYPE_PUN(v[1].z, float);
                z_value[2] = 1.0f / TYPE_PUN(v[2].z, float);
                
                float z = (w0 * z_value[0] + w1 * z_value[1] + w2 * z_value[2]);
				z = 1.0f / z;
                
                if (z <= z_buffer[(Y * bitmap.width + X)]) {
                    z_buffer[(Y * bitmap.width + X)] = z;
                    
                    RGBAColor color = {
                        .r =
                        ((colors[0].r * w0) +
                         (colors[1].r * w1) +
                         (colors[2].r * w2)) * z,
                        
                        .g =
                        ((colors[0].g * w0) +
                         (colors[1].g * w1) +
                         (colors[2].g * w2)) * z,
                        
                        .b =
                        ((colors[0].b * w0) +
                         (colors[1].b * w1) +
                         (colors[2].b * w2)) * z,
                        
                        .a =
                        ((colors[0].a * w0) +
                         (colors[1].a * w1) +
                         (colors[2].a * w2)) * z,
                    };
                    
                    plotPixel(X, Y, &color);
                }
            }
        }
    }
}


#endif
