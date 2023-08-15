/**************************
 * Includes
 *
 **************************/

#include <windows.h>
#include <gl/gl.h>
#include <math.h>

#include <stdio.h>

/**************************
 * Defines
 *
 **************************/

#define CUBE_SIZE 1.0f
#define W CUBE_SIZE

#define CAMERA_DISTANCE 2.5 * CUBE_SIZE

#define R_SPEED 500
#define R_ANGLE 2 * M_PI / R_SPEED

#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

/**************************
 * Function Declarations
 *
 **************************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
WPARAM wParam, LPARAM lParam);
void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC);

/*
	          5-----------1          y
	        / |         / |          |
	      /   |       /   |          |
	    4-----------0     |          |
	    |     |     |     |          |
	    |     7-----|-----3          0----------- x
	    |   /       |   /          /
	    | /         | /          /
	    6-----------2          z
*/

static float vertices[16][4] = {
	// (x, y, z, w)
	
	/* CUBE A */
	    { CUBE_SIZE ,  CUBE_SIZE ,  CUBE_SIZE , -CUBE_SIZE},
	    { CUBE_SIZE ,  CUBE_SIZE , -CUBE_SIZE , -CUBE_SIZE},
	    { CUBE_SIZE , -CUBE_SIZE ,  CUBE_SIZE , -CUBE_SIZE},
	    { CUBE_SIZE , -CUBE_SIZE , -CUBE_SIZE , -CUBE_SIZE},
	    {-CUBE_SIZE ,  CUBE_SIZE ,  CUBE_SIZE , -CUBE_SIZE},
	    {-CUBE_SIZE ,  CUBE_SIZE , -CUBE_SIZE , -CUBE_SIZE},
	    {-CUBE_SIZE , -CUBE_SIZE ,  CUBE_SIZE , -CUBE_SIZE},
	    {-CUBE_SIZE , -CUBE_SIZE , -CUBE_SIZE , -CUBE_SIZE},
	
	/* CUBE B */
		{ CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, CUBE_SIZE},
	    { CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE},
	    { CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, CUBE_SIZE},
	    { CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE},
	    {-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE, CUBE_SIZE},
	    {-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE},
	    {-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE, CUBE_SIZE},
	    {-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE}
	
	};
	
static int edges[32][2] = {
	    {  0,  1 }, {  0,  2 }, { 0,  4 }, { 0, 8 },
	    {  1,  3 }, {  1,  5 }, { 1,  9 },
	    {  2,  3 }, {  2,  6 }, { 2, 10 },
	    {  3,  7 }, {  3, 11 },
	    {  4,  5 }, {  4,  6 }, { 4, 12 },
	    {  5,  7 }, {  5, 13 },
	    {  6,  7 }, {  6, 14 },
	    {  7, 15 },
	    {  8,  9 }, {  8, 10 }, { 8, 12 },
	    {  9, 11 }, {  9, 13 },
	    { 10, 11 }, { 10, 14 },
	    { 11, 15 },
	    { 12, 13 }, { 12, 14 },
	    { 13, 15 },
	    { 14, 15 },
	};

  static int faces[24][4] = {
    // Cube A
    { 0, 1, 3, 2 },
    { 4, 5, 7, 6 },
    { 4, 0, 2, 6 },
    { 5, 1, 3, 7 },
    { 5, 1, 0, 4 },
    { 7, 3, 2, 6 },

    // Cube B
    {  8,  9, 11, 10 },
    { 12, 13, 15, 14 },
    { 12,  8, 10, 14 },
    { 13,  9, 11, 15 },
    { 13,  9,  8, 12 },
    { 15, 11, 10, 14 },

    // Flaps
    { 0,  1,  9,  8 },
    { 4,  5, 13, 12 },
    { 4,  0,  8, 12 },
    { 5,  1,  9, 13 },
    { 2,  3, 11, 10 },
    { 6,  7, 15, 14 },
    { 6,  2, 10, 14 },
    { 7,  3, 11, 15 },
    { 0,  8, 10,  2 },
    { 1,  9, 11,  3 },
    { 4, 12, 14,  6 },
    { 5, 13, 15,  7 },
};


static float rotationMatrixXY[4][4] = {
		{cos(R_ANGLE) , -sin(R_ANGLE), 0, 0 },
	  	{sin(R_ANGLE) , cos(R_ANGLE), 0, 0 },
	  	{0, 0, 1 ,0 },
	  	{0, 0, 0 ,1 }
	};
	
static float rotationMatrixXZ[4][4] = {
		{cos(R_ANGLE) , 0, -sin(R_ANGLE), 0 },
	  	{0, 1, 0 ,0 },
	  	{sin(R_ANGLE) , 0, cos(R_ANGLE),  0 },
	  	{0, 0, 0 ,1 }
	};
	
static float rotationMatrixYZ[4][4] = {
	  	{1, 0, 0 ,0 },
	  	{0, cos(R_ANGLE), sin(R_ANGLE), 0 },
	  	{0, -sin(R_ANGLE), cos(R_ANGLE),  0 },
	  	{0, 0, 0 ,1 }
	};

static float rotationMatrixXW[4][4] = {
	  	{ cos(R_ANGLE), 0, 0, sin(R_ANGLE) },
		{0, 1, 0 ,0 },
	  	{0, 0, 1 ,0 },
	  	{-sin(R_ANGLE), 0, 0, cos(R_ANGLE) }
	};
	
static float rotationMatrixYW[4][4] = {
		{1, 0, 0 ,0 },
	  	{0, cos(R_ANGLE), 0, -sin(R_ANGLE) },
	  	{0, 0, 1 ,0 },
	  	{0, sin(R_ANGLE), 0, cos(R_ANGLE) }
	};
	
static float rotationMatrixZW[4][4] = {
	  	{1, 0, 0 ,0 },
	  	{0, 1, 0 ,0 },
	  	{0, 0, cos(R_ANGLE) , -sin(R_ANGLE) },
	  	{0, 0, sin(R_ANGLE) , cos(R_ANGLE) }
	};


// Funcion para multiplicar matrices
void matrixMul(float rotationMat[4][4], float matrix[16][4]) {
	
	float val = 0.0f;
	
	float oldVertex[16][4];
	
	// Se crea una copia de la matriz de vertices
	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 4; j++) {
			oldVertex[i][j] = matrix[i][j];
		}
	}
	
	// Multiplicacion de matrices
	for(int r= 0; r < 16; r++) {
		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 4; j++) {
				val += rotationMat[i][j] * oldVertex[r][j] ;
			}
			vertices[r][i] = val;
			val = 0.0f;
		}
	}
}

float addPerspective(float matrix[16][4]) {
	for(int i = 0; i < 16; i++) {
		/*
		
		3D :
		[1/(distance-z), 0, 0]
		[0, 1/(distance-z), 0]
		
		float z = matrix[i][2];
		
		matrix[i][0] = matrix[i][0] / (CAMERA_DISTANCE - z);
		matrix[i][1] = matrix[i][1] / (CAMERA_DISTANCE - z);
		*/
		
		float w = matrix[i][3];
		
		matrix[i][0] = matrix[i][0] / (CAMERA_DISTANCE - w);
		matrix[i][1] = matrix[i][1] / (CAMERA_DISTANCE - w);
		matrix[i][2] = matrix[i][2] / (CAMERA_DISTANCE - w);
		
	}
}

// Inicializa la vista.
// Limpia la ventana y configura el modo de visualizacion.
// Posteriormente se aplican operaciones de rotacion y escala
// para posicionar la camara y darle perspectiva.
static void init() {
	/* set camera mode */
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	/* set camera position */
	glRotatef(30 ,1.0, 0.0,0.0);
    glRotatef(-45 ,0.0, 1.0,0.0);
    glScalef(0.5f, 0.5f, 0.5f);
    glClearColor(1.0f/255, 15.0f/255, 20.0f/255, 1.0f);
}

// Dibuja el vertice x de color rojo, el y de color verde y el z de color azul.
// xAxis = tamanio del eje x
// yAxis = tamanio del eje y
// zAxis = tamanio del eje z
static void drawAxis(float xAxis, float yAxis, float zAxis) {
	glBegin(GL_LINES);
	    glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(xAxis, 0, 0);
	    glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, yAxis, 0);
	    glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, zAxis);
	glEnd();
}

// Dibuja el teseracto.
void drawTesseract() {
	
	
	// Multiplicacion de matrices de rotacion
	//matrixMul(rotationMatrixXY, vertices);
	//matrixMul(rotationMatrixXZ, vertices);
	//matrixMul(rotationMatrixYZ, vertices);
	//matrixMul(rotationMatrixXW, vertices);
	//matrixMul(rotationMatrixYW, vertices);
	matrixMul(rotationMatrixZW, vertices);
	
	float oldVertex[16][4];
	
	// Se crea una copia de la matriz de vertices
	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 4; j++) {
			oldVertex[i][j] = vertices[i][j];
		}
	}
	
	addPerspective(vertices);
	
	
	// Render
	glColor4f(1.0, 1.0, 1.0, 0.9);
	
	// Edges
	for (int i = 0; i < 32; ++i) {
	    glBegin(GL_LINES);
	    for (int j = 0; j < 2; ++j) {
	    	
	    	glVertex3fv(vertices[edges[i][j]]);
	    }
	    glEnd();
	}
	
	// Draw faces
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glColor4f(0.1, 0.6, 0.2, 0.2);
	glColor4f(0.9, 0.0, 0.0, 0.1);
	
	for (int i = 0; i < 24; ++i) {
	    glBegin(GL_POLYGON);
	    	for (int j = 0; j < 4; ++j) {
	        	glVertex3fv(vertices[faces[i][j]]);
	      	}
	    glEnd();
	  }
	
	glDisable(GL_BLEND);
	
	// Se restaura la matriz de vertices
	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 4; j++) {
			vertices[i][j] = oldVertex[i][j];
		}
	}
}

void saveImage(const char* route) {
	/* Obtiene las medidas de la imagen */
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	/* Guarda los pixeles en RAM */
	int buffer[viewport[2]][viewport[3]];
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_BGR, GL_UNSIGNED_BYTE, &buffer);

	/* Crea el archivo y escribe los pixeles */
	FILE *out = fopen(route, "wb");
	short  TGAhead[] = {0, 2, 0, 0, 0, 0, viewport[2], viewport[3], 24};
	fwrite(&TGAhead, sizeof(TGAhead), 1, out);
	fwrite(buffer, 3 * viewport[2] * viewport[3], 1, out);
	fclose(out);
}

/**************************
 * WinMain
 *
 **************************/

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow) {
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;        
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;
    int frame = 0;

    /* register window class */
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "GLSample";
    RegisterClass (&wc);

    /* create main window */
    hWnd = CreateWindow (
      "GLSample", "OpenGL Sample", 
      WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
      50, 50, 700, 700,
      NULL, NULL, hInstance, NULL);

    /* enable OpenGL for the window */
    EnableOpenGL (hWnd, &hDC, &hRC);
    
    init();

    /* program main loop */
    
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
            }
        } else {
        	glClear(GL_COLOR_BUFFER_BIT);
        	
        	glPushMatrix();
        		//drawAxis(5, 5, 5);
        		//if(theta < M_PI) {
        			drawTesseract();
			    //	theta += R_ANGLE;
				//}
			glPopMatrix();
        	
        	SwapBuffers (hDC);
        	
            Sleep (1);
            
            /*
            char filename[10];
        	sprintf(filename, "screenshots/%dsc.tga", frame);
        	saveImage(filename);
        	frame++;
        	*/
		}
    }

    /* shutdown OpenGL */
    DisableOpenGL (hWnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow (hWnd);

    return msg.wParam;
}


/********************
 * Window Procedure
 *
 ********************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message)
    {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage (0);
        return 0;

    case WM_DESTROY:
        return 0;

    case WM_KEYDOWN:
        switch (wParam)
        {
        	case VK_ESCAPE:
	            PostQuitMessage(0);
	            return 0;
	        	
        }
        return 0;

    default:
        return DefWindowProc (hWnd, message, wParam, lParam);
    }
}


/*******************
 * Enable OpenGL
 *
 *******************/

void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC (hWnd);

    /* set the pixel format for the DC */
    ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL  | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat (*hDC, &pfd);
    SetPixelFormat (*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );
}


/******************
 * Disable OpenGL
 *
 ******************/

void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRC);
    ReleaseDC (hWnd, hDC);
}

