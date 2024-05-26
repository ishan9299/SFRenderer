#ifndef OBJPARSER_H
#define OBJPARSER_H

union Vertex {
	struct {
		float x;
		float y;
		float z;
	};
    
	float v[3];
};

union VertexParamSpace {
	struct {
		float x;
		float y;
		float z;
	};
    
	float vp[3];
};

union VertexNormal {
	struct {
		float i;
		float j;
		float k;
	};
    
	float vn[3];
};

union VertexTexture {
	struct {
		float tx;
		float ty;
		float tz;
	};
    
	float vt[3];
};

typedef union Vertex Vertex;
typedef union VertexNormal VertexNormal;
typedef union VertexTexture VertexTexture;
typedef union VertexParamSpace VertexParamSpace;

union Faces {
	struct {
		int vI;
		int vtI;
		int vnI;
	};
    
	int f[3];
};

typedef union Faces Faces;

#endif
