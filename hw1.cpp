//modified by:Brandon Martinez
//date:1/26/17
//purpose:
//
//cs3350 Spring 2017 Lab-1
//author: Gordon Griesel
//date: 2014 to present
//This program demonstrates the use of OpenGL and XWindows
//
//Assignment is to modify this program.
//You will follow along with your instructor.
//
//Elements to be learned in this lab...
//
//. general animation framework
//. animation loop
//. object definition and movement
//. collision detection
//. mouse/keyboard interaction
//. object constructor
//. coding style
//. defined constants
//. use of static variables
//. dynamic memory allocation
//. simple opengl components
//. git
//
//elements we will add to program...
//. Game constructor
//. multiple particles
//. gravity
//. collision detection
//. more objects
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 20000
#define GRAVITY .8
#define rnd() (float)rand() / (float)RAND_MAX

//X Windows variables
#define UMBRELLA_FLAT  0
#define UMBRELLA_ROUND 1
Display *dpy;
Window win;
GLXContext glc;

//Structures

struct Vec {
    float x, y, z;
};

struct Shape {
    float width, height;
    float radius;
    Vec center;
};

struct Particle {
    Shape s;
    Vec velocity;
};
struct Game {
    Shape box[5];
    Particle particle[MAX_PARTICLES];
    int n;
    int bubbler;
    int mouse[2];
    Game() { n=0; bubbler=0;  }
};
/*
   typedef struct t_umbrella {
   int shape;
   Vec pos;
   Vec lastpos;
   float width;
   float width2;
   float radius;
   } Umbrella;
   Umbrella umbrella;
   */

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);
int xres=640, yres=480;

int main(void)
{
    int done=0;
    srand(time(NULL));
    initXWindows();
    init_opengl();
    //declare game object
    Game game;
    game.n=0;

    //declare a box shape
    game.box[4].width = 100;
    game.box[4].height = 10;
    game.box[4].center.x = 120 + 5*65;
    game.box[4].center.y = 500 - 5*60;

    game.box[3].width = 100;
    game.box[3].height = 10;
    game.box[3].center.x = 120 + 4*65;
    game.box[3].center.y = 500 - 4*60;

    game.box[2].width = 100;
    game.box[2].height = 10;
    game.box[2].center.x = 120 + 3*65;
    game.box[2].center.y = 500 - 3*60;

    game.box[1].width = 100;
    game.box[1].height = 10;
    game.box[1].center.x = 120 + 2*65;
    game.box[1].center.y = 500 - 2*60;

    game.box[0].width = 100;
    game.box[0].height = 10;
    game.box[0].center.x = 120 + 1*65;
    game.box[0].center.y = 500 - 1*60;
    //start animation
    while (!done) {
	while (XPending(dpy)) {
	    XEvent e;
	    XNextEvent(dpy, &e);
	    check_mouse(&e, &game);
	    done = check_keys(&e, &game);
	}
	movement(&game);
	render(&game);
	glXSwapBuffers(dpy, win);
    }
    cleanupXWindows();
    cleanup_fonts();
    return 0;
}

void set_title(void)
{
    //Set the window title bar.
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "335 Lab1   LMB for particle");
}

void cleanupXWindows(void)
{
    //do not change
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}


void initXWindows(void)
{
    //do not change
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;

    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
	std::cout << "\n\tcannot connect to X server\n" << std::endl;
	exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if (vi == NULL) {
	std::cout << "\n\tno appropriate visual found\n" << std::endl;
	exit(EXIT_FAILURE);
    } 
    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
	ButtonPress | ButtonReleaseMask | PointerMotionMask |
	StructureNotifyMask | SubstructureNotifyMask;
    win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
	    InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //Set 2D mode (no perspective)
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    //Set the screen background color
    glClearColor(0.1, 0.1, 0.1, 1.0);
    // umbrellaImage    = ppm6GetImage("./images/umbrella.ppm");
    // glGenTextures(1, &umbrellaTexture);
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();
}

void makeParticle(Game *game, int x, int y)
{
    if (game->n >= MAX_PARTICLES)
	return;
    std::cout << "makeParticle() " << x << " " << y << std::endl;
    //position of particle
    Particle *p = &game->particle[game->n];
    p->s.center.x = x;
    p->s.center.y = y;
    p->velocity.y =  rnd() * 4.0;
    p->velocity.x =  rnd() * 2;
    game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
    static int savex = 0;
    static int savey = 0;
    //static int n = 0;

    if (e->type == ButtonRelease) {
	return;
    }
    if (e->type == ButtonPress) {
	if (e->xbutton.button==1) {
	    //Left button was pressed
	    int y = WINDOW_HEIGHT - e->xbutton.y;
	    for(int i=0; i<10; i++){
		makeParticle(game, e->xbutton.x, y);
	    }
	    return;
	}
	if (e->xbutton.button==3) {
	    //Right button was pressed
	    return;
	}
    }
    //Did the mouse move?
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
	savex = e->xbutton.x;
	savey = e->xbutton.y;
	int y = WINDOW_HEIGHT - e->xbutton.y;
	if(game->bubbler == 0){
	    game->mouse[0] = savex;
	    game->mouse[1] = y;
	}
	for(int i=0; i<5; i++){
	    makeParticle(game, e->xbutton.x, y);
	}
	//if (++n < 10)
	//    return;
    }
}

int check_keys(XEvent *e, Game *game)
{
    //Was there input from the keyboard?
    if (e->type == KeyPress) {
	int key = XLookupKeysym(&e->xkey, 0);
	if (key == XK_b){
	    game->bubbler ^= 1; 
	}
	if (key == XK_Escape) {
	    return 1;
	}
	//You may check other keys here.



    }
    return 0;
}

void movement(Game *game)
{
    Particle *p;

    if (game->n <= 0)
	return;
    if (game->bubbler != 0){
	makeParticle(game, game->mouse[0], game->mouse[1]);
    }
    //game->n
    for(int i=0; i<game->n; i++){
	p = &game->particle[i];
	p->velocity.y -= GRAVITY;
	p->s.center.x += p->velocity.x;
	p->s.center.y += p->velocity.y;

	//check for collision with shapes...
	for (int j=0; j<5; j++){
	    Shape *s;
	    s = &game->box[j];
	    if (p->s.center.y < s->center.y + s->height &&
		    p->s.center.x >= s->center.x - s->width &&
		    p->s.center.x <= s->center.x + s->width){
		p->s.center.y = s->center.y + s->height;
		p->velocity.y = -p->velocity.y * 0.4f;
		p->velocity.x += 0.1f;

	    } 
	}

	//check for off-screen
	if (p->s.center.y < 0.0) {
	    std::cout << "off screen" << std::endl;
	    game->particle[i] = game->particle[--game->n];
	}
    }
}

void render(Game *game)
{
    float w, h;

    Rect r;
    glClear(GL_COLOR_BUFFER_BIT);
    //draw box

    	glColor3ub(90,140,90);
    for(int i=0; i<5; i++){
    	Shape *s;
    	s = &game->box[i];
    	glPushMatrix();
    	glTranslatef(s->center.x, s->center.y, s->center.z);
    	w = s->width;
    	h = s->height;
    	glBegin(GL_QUADS);
    	glVertex2i(-w,-h);
    	glVertex2i(-w, h);
    	glVertex2i( w, h);
    	glVertex2i( w,-h);
    	glEnd();
    	glPopMatrix();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned int color = 0x8DEEEE;
    r.bot = WINDOW_HEIGHT - 20;
    r.left = 10;
    r.center = 0;
    ggprint12(&r, 30, color, "Waterfall Model Brandon Martinez %s");
    r.bot = WINDOW_HEIGHT - 170;
    r.left = 110;
    ggprint12(&r, 30, color, "Requirements %s");
    r.bot = WINDOW_HEIGHT - 230;
    r.left = 210;
    ggprint12(&r, 30, color, "Design  %s");
    r.bot = WINDOW_HEIGHT - 290;
    r.left = 310;
    ggprint12(&r, 30, color, "Coding  %s");
    r.bot = WINDOW_HEIGHT - 350;
    r.left = 360;
    ggprint12(&r, 30, color, "Testing  %s");
    r.bot = WINDOW_HEIGHT - 410;
    r.left = 420;
    ggprint12(&r, 30, color, "Maintenance  %s");

    for(int i=0; i<game->n; i++){
	//draw all particles here
	glPushMatrix();
	glColor3ub(150,160,220);
	Vec *c = &game->particle[i].s.center;
	w = 2;
	h = 2;
	glBegin(GL_QUADS);
	glVertex2i(c->x-w, c->y-h);
	glVertex2i(c->x-w, c->y+h);
	glVertex2i(c->x+w, c->y+h);
	glVertex2i(c->x+w, c->y-h);
	glEnd();
	glPopMatrix();
    }
}
