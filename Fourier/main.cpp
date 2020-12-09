#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <signal.h>
#ifndef MAIN_CPP
#define MAIN_CPP
#define complex _Complex

#include "my_draw.hpp"
#include "fourier.hpp"

/* GLUT callback Handlers */
volatile extern int max_depth_computed;
static void resize(int w, int h)
	{
		if (h == 0) h = 1;
		double ratio = w / h;

		// Use the Projection Matrix
		glMatrixMode(GL_PROJECTION);

		// Reset Matrix
		glLoadIdentity();

		// Set the viewport to be the entire window
		glViewport(0, 0, w, h);

		// Set the correct perspective.
		gluPerspective(1, ratio, 0, 0);

		// Get Back to the Modelview
		glMatrixMode(GL_MODELVIEW);
	}

double TIME = 0;
double l_TIME = 0;
double speed = 1;
double zoom = 1;

int mouse_press_state = GLUT_UP;
volatile bool isInFourierDrawMode = false;
bool show_original = true;
bool show_red_trace = true;
bool screen_following = true;
int eff_len = 0;

//Speed is stored here when fourier re-drawing is paused
double storespd = 0;
bool ispaused = 0;

double mouseX = 0;
double mouseY = 0;
double lastMouseX = 0;
double lastMouseY = 0;

int next_index = 0;
int max_index_reached = 0;
const int max_arr_draw = 100000; //Maximum number of pixels that can be in the image

//The x,y coordinates of the user drawing
double drawn_x_arr[max_arr_draw];
double drawn_y_arr[max_arr_draw];

bool break_line_arr[max_arr_draw] = { false };
double mouse_drag_TIME = 0;
pthread_t calc_thread;
double complex cplx_ret[max_arr_draw];
double rind;
void drawing_as_func(double x, double *ox, double *oy)
	{
		/**
		 Take x in as an input, between 0.0 and 1.0, and return the corresponding x, y coords
		 **/
		rind = x * (next_index - 1);
		if (rind < 0) rind += next_index - 1;
		else if (rind > next_index - 1) rind -= next_index - 1;
		else if (rind <= 1) {
			*ox = (1 - rind) * drawn_x_arr[0] + (rind) * drawn_x_arr[1];
			*oy = (1 - rind) * drawn_y_arr[0] + (rind) * drawn_y_arr[1];
			return;
		}
		*ox = (ceil(rind) - rind) * drawn_x_arr[(int) rind - 1]
		    + (rind - floor(rind)) * drawn_x_arr[(int) rind];
		*oy = (ceil(rind) - rind) * drawn_y_arr[(int) rind - 1]
		    + (rind - floor(rind)) * drawn_y_arr[(int) rind];
		return;
	}
static void mouse_passive(int x, int y)
	{
		lastMouseX = mouseX;
		lastMouseY = mouseY;
		mouseX = (-glutGet(GLUT_WINDOW_WIDTH) / 2.0f + x)
		    / glutGet(GLUT_WINDOW_WIDTH) * 2.0f;
		mouseY = (glutGet(GLUT_WINDOW_HEIGHT) / 2.0f - y)
		    / glutGet(GLUT_WINDOW_HEIGHT) * 2.0f;
		mouse_drag_TIME = TIME;

	}
static void mouse_drag(int x, int y)
	{
		mouseX = (-glutGet(GLUT_WINDOW_WIDTH) / 2.0f + x)
		    / glutGet(GLUT_WINDOW_WIDTH) * 2.0f;
		mouseY = (glutGet(GLUT_WINDOW_HEIGHT) / 2.0f - y)
		    / glutGet(GLUT_WINDOW_HEIGHT) * 2.0f;
		max_depth_computed = 0;
		if ((mouseX - lastMouseX) * (mouseX - lastMouseX)
		    + (mouseY - lastMouseY) * (mouseY - lastMouseY) > .01 * .01) {
			lastMouseX = mouseX;
			lastMouseY = mouseY;
			//printf("%f, %f\n", mouseX, mouseY);
			mouse_drag_TIME = TIME;
			if (!isInFourierDrawMode) {
				drawn_x_arr[next_index] = mouseX;
				drawn_y_arr[next_index] = mouseY;
				next_index++;
				next_index = next_index % max_arr_draw;
				if (next_index > max_index_reached) {
					max_index_reached = next_index;
				}
			}
		}
	}
static void display_non_fourier(void)
	{
		TIME += speed * (glutGet(GLUT_ELAPSED_TIME) - l_TIME) / 1000.0f;
		l_TIME = glutGet(GLUT_ELAPSED_TIME);
		glEnable(GL_LINE_SMOOTH);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(1, 1, 1);
		glLineWidth(1);
		for (int i = 1; i < max_index_reached; i++) {
			//printf("%f\n", drawn_x_arr[i-1]);
			int j = next_index - i;
			if (j < 0) {
				j += max_arr_draw;
			}
			if (!break_line_arr[j]) {
				line2(drawn_x_arr[j - 1], drawn_y_arr[j - 1], drawn_x_arr[j],
				    drawn_y_arr[j]);
			}
		}
		glutSwapBuffers();
	}
double follow_x_arr[max_arr_draw] = { 0 };
double follow_y_arr[max_arr_draw] = { 0 };
double follow_TIME[max_arr_draw] = { -1 };
int follow_next = 0;
bool draw_lines = true;


fourier four;
static void display_fourier(void)
	{
		TIME += speed * (glutGet(GLUT_ELAPSED_TIME) - l_TIME) / 1000.0f;
		l_TIME = glutGet(GLUT_ELAPSED_TIME);
//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_LINE_SMOOTH);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(1, 1, 1);
		glLineWidth(2);
		double complex rot;
		double sumx = 0;
		double sumy = 0;
		double tsumx = 0;
		double tsumy = 0;
		double l_sumx = 0;
		double l_sumy = 0;
		int i;
		double arlen = .02; //len parameter for vector arrowheads
		if (max_depth_computed < depth_goal) {
			glColor3d(.45, .45, 1); //Blue color while loading in coeffs
		}
		for (int j = 0; j < (max_depth_computed) * 2 + 1; j++) {
			//draw the rotating lines
			if (j % 2 == 0) {
				i = j / 2;
			} else {
				i = -(j + 1) / 2;
			}
			rot = coefficients[i + MAX_DEPTH + 1] * cpow(M_E, I * i * TIME);
			sumx += creal(rot);
			sumy += cimag(rot);
			tsumx += sumx;
			tsumy += sumy;
			line2(sumx, sumy, l_sumx, l_sumy);
			l_sumx = sumx;
			l_sumy = sumy;
		}

		follow_x_arr[follow_next] = sumx;
		follow_y_arr[follow_next] = sumy;
		follow_TIME[follow_next] = TIME;
		follow_next++;
		follow_next = follow_next % max_arr_draw;
		if (show_original) {
			//Draw the originally drawn array in green
			glLineWidth(2);
			glColor3f(0, 1, 0);
			for (int i = 1; i < max_index_reached; i++) {
				//printf("%f\n", drawn_x_arr[i-1]);
				int j = next_index - i;
				if (j < 0) {
					j += max_arr_draw;
				}
				if (!break_line_arr[j]) {
					line2(drawn_x_arr[j - 1], drawn_y_arr[j - 1], drawn_x_arr[j],
					    drawn_y_arr[j]);
				}
			}
		}
//glRasterPos2d(0,0);
//glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'h');
		if (show_red_trace) {
			//Draw the follow array in red
			glLineWidth(3);
			glColor4f(1, 0, 0, 0);
			int j; //Used for indexing into follow_*_arr
			for (int i = 1; i < follow_next; i++) {
				j = follow_next - i;
				if (j < 0) {
					j += max_arr_draw;
				}

				if (follow_TIME[j] < TIME - 2) {
					continue;
				}
				glColor4f(1, 0, 0, 1 - i * (speed == 0 ? storespd : speed) / 100);
				if (follow_x_arr[j - 1] != -M_E && follow_x_arr[j] != -M_E) {
					line2(follow_x_arr[j - 1], follow_y_arr[j - 1], follow_x_arr[j],
					    follow_y_arr[j], -0.1);
				}

			}
		}
		if (screen_following) {
			glPopMatrix();
			glPushMatrix();
			glScalef(zoom, zoom, 1.0f);
			if (zoom < 2) {
				glTranslatef(-tsumx / depth_goal / 2, -tsumy / depth_goal / 2, 0);
			} else {
				glTranslatef(-sumx, -sumy, 0);
			}
		}

		glutSwapBuffers();
	}
static bool shift = false;
static bool ctrl = false;
static void mouse_press(int button, int state, int x, int y)
	{

		if (button == 2 && state == 0) {
			if (isInFourierDrawMode) {
				follow_next = 0;
			} else {
				next_index = 0;
				max_index_reached = 0;
				draw_lines = !draw_lines;
				glutPostRedisplay();
			}
			return;
		} else if (button == 3) {
			zoom *= 1.1;
			speed /= 1.1;
			return;
		} else if (button == 4) {
			zoom /= 1.1;
			speed *= 1.1;
			if (zoom < 1) {
				//zoom = 1;
			}
			return;
		}
		mouse_press_state = state;
		if (state == 1) {
			break_line_arr[next_index] = true;
			//next_index++;
			//next_index = next_index % max_arr_draw;
		}
	}
static void idle(void)
	{
		glutPostRedisplay();
	}

static void key(unsigned char key, int x, int y)
	{
		shift = glutGetModifiers() == GLUT_ACTIVE_SHIFT;
		ctrl = glutGetModifiers() == GLUT_ACTIVE_CTRL;
		//static pthread_t *last_p = NULL;
		if (key == 'o') {
			//Set whether or not to display the original image in fourier mode
			show_original = !show_original;
		} else if (key == 'f') {
			//Set whether or not to display the original image in fourier mode
			screen_following = !screen_following;
			if (!screen_following) {
				glPopMatrix();
			}
		} else if (key == 'w') {
			//Increase the draw speed
			speed *= 1.1;
		} else if (key == 's') {
			//Decrease the draw speed
			speed /= 1.1;
		} else if (key == 't') {
			//Toggle the red tracer
			show_red_trace = !show_red_trace;
		} else if (key == '+') {
			int diff = 5;

			if (shift) {
				diff = 25;
			} else if (ctrl) {
				diff = 1;
			}
			if (depth_goal + diff > MAX_DEPTH) {
				depth_goal = MAX_DEPTH;
			} else if (depth_goal <= 0) {
				depth_goal = 1;
			} else {
				depth_goal += diff;
			}
		} else if (key == '-') {
			int diff = -5;
			if (shift) {
				diff = -25;
			} else if (ctrl) {
				diff = -1;
			}
			if (depth_goal + diff > MAX_DEPTH) {
				depth_goal = MAX_DEPTH;
			} else if (depth_goal + diff <= 0) {
				depth_goal = 1;
			} else {
				depth_goal += diff;
			}
		} else if (key == 'r') {
			speed *= -1;
		} else if (key == 'p') {
			if (!ispaused) {
				storespd = speed;
				speed = 0;
			} else {
				speed = storespd;
				storespd = 0;
			}
			ispaused = !ispaused;
		} else if (key == ' ') {
			if (isInFourierDrawMode) {
				glPopMatrix();
				isInFourierDrawMode = false;
				max_depth_computed = 0;
				if (calc_thread != 0) pthread_kill(calc_thread, 11);
				glutDisplayFunc(display_non_fourier);
			} else {
				zoom = 1;
				speed = 1;
				int aind = 0;
				for (int i = 0; i < next_index; i++) {
					cplx_ret[i] = (drawn_x_arr[(int) floor(i)]
					    + I * drawn_y_arr[(int) floor(i)]);
				}
				follow_next = 0;
				printf("%s", "Key presssed; Generating fourier...\n");
				eff_len = aind + 1;
				drawn_x_arr[next_index + 1] = drawn_x_arr[0];
				drawn_y_arr[next_index + 1] = drawn_y_arr[0];
				//Shrink the step size based on the # of pixels drawn
				step_size = base_step_size / next_index;
				isInFourierDrawMode = true;
				max_depth_computed = 0;
				//Create the calc thread
				pthread_create(&calc_thread, NULL, [](void*)->void* {
					signal(11, [](int)->void{
						calc_thread = 0;
						pthread_exit(0);
					});
					while (true) {
						four.set_coeffs();
						usleep(1);
					}
					calc_thread = 0;
					pthread_exit(0);
					}, NULL); 
				glutDisplayFunc(display_fourier);
			}
		}

		glutPostRedisplay();
	}
int main(int argc, char *argv[])
	{
		drawfunc = drawing_as_func;
		max_depth_computed = 0;
		//start computation loop
		glutInit(&argc, argv);
		glutInitWindowSize(320, 320);
		glutInitWindowPosition(10, 10);
		glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

		glutCreateWindow("Drawing");

//glutReshapeFunc(resize);
		glutDisplayFunc(display_non_fourier);
		glutKeyboardFunc(key);
		glutMotionFunc(mouse_drag);
		glutPassiveMotionFunc(mouse_passive);
		glutMouseFunc(mouse_press);
		glutIdleFunc(idle);

		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glClearColor(0, 0, 0, 1);

		glutMainLoop();

		return EXIT_SUCCESS;
	}
#endif
