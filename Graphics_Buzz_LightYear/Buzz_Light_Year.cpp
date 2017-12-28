#include "..\Libraries\Model_3DS\include\TextureBuilder.h"
#include "..\Libraries\Model_3DS\include\Model_3DS.h"
#include "..\Libraries\Model_3DS\include\GLTexture.h"
#include "..\Libraries\assmip\include\cimport.h"
#include "..\Libraries\assmip\include\scene.h"
#include "..\Libraries\gl\include\glut.h"
#include <math.h>
//#pragma comment(lib, "legacy_stdio_definitions.lib") //Visual Studio 2017

#define DEG2RAD(a) (a * 0.0174532925)
double x = 0;
double y = 0;
double z = 0;
int WIDTH = 1280;
int HEIGHT = 720;
boolean cameraView = false;
std::string lives = "3" , collects = "0";
bool found = false;
bool found2 = false;
bool found3 = false;
bool found_powerup = false;
bool found2_powerup = false;
bool found3_powerup = false;

int light = 0;
float r = 0;
float g = 0;
float b = 0;


int flagrot = 1;
int rot = 0;
int en2 = 0;
double fly = 0;
int  flyf = 1;
double en = 0;
int enn = 1;
int flagrotl = 1;
int rotl = 0;
int flagarmr = 1;
double rotarmr = 0;
double rotarml = 0;
int flagwalk = 1;
double walk = 0;
double down = 0;
GLuint tex_ground;
GLuint tex_wall;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_mercedes;
Model_3DS model_skoda;
Model_3DS model_carN23;
Model_3DS model_sofa;
Model_3DS model_audi;
Model_3DS model_armchairSofa;

Model_3DS model_armchair;
Model_3DS model_bookshelf;
Model_3DS model_desk;
Model_3DS model_lamp;
Model_3DS model_lampTolomeo;
Model_3DS model_tablelamp;
Model_3DS model_tracklight;
Model_3DS model_lusterSmithfield;
Model_3DS model_luster;
Model_3DS model_bed;
Model_3DS model_bookshelf2;
Model_3DS model_retrocase;
Model_3DS model_bookcaseDesk;
Model_3DS model_locker;

Model_3DS model_rack;
Model_3DS model_carpet;
Model_3DS model_officechair;
Model_3DS model_door;
Model_3DS model_door2;
Model_3DS model_door3;
Model_3DS model_garagedoor;
Model_3DS model_toolbox;
Model_3DS model_table;
Model_3DS model_tvset;
Model_3DS model_livingsofa;
Model_3DS model_livingcarpet;

Model_3DS model_shelf;
Model_3DS model_ac;
Model_3DS model_chandelier;

// Textures
GLTexture tex_ground_bedroom;
GLTexture tex_wall_bedroom;
GLTexture tex_ground_livingroom;
GLTexture tex_wall_livingroom;
GLTexture tex_ground_garage;
GLTexture tex_wall_garage;

struct aiScene * powerUp;
struct aiScene * bouncing_Zurg;
struct aiScene * flyzurg_body, *flyzurg_fan;
struct aiScene * potato_eye, *potato_ear, *potato_nose;
struct aiScene * buzz_body, *buzz_left_elbow, *buzz_right_elbow, *buzz_left_leg, *buzz_right_leg;

//=======================================================================
// Lighting Configuration Function
//=======================================================================

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f &v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f &v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 5.0f, float eyeY = 1.1f, float eyeZ = 5.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		//float temp = eye.y;
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
		center.y = eye.y;
		//eye.y = temp;

	}

	void moveY(float d) {
		//float temp = eye.y;
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
		//eye.y = temp;

	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
		center.y = eye.y;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		//float temp = center.x;
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
		up.x = 0;
		up.y = 1;
		up.z = 0;
//		buzz_angle += a;
		//center.x = temp;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
			);
	}
};

extern "C" { //3mork t7lm b kda
	struct aiScene * loadasset(const char* path);
	void recursive_render(const struct aiScene *sc, const struct aiNode* nd);
};

Camera camera;
bool animateBuzz = 0;
Vector3f buzz_pos(4, 0.5, 5);
float camera_buzz_angle = 0.0f, buzz_angle = 0.0f;

char * type = "bedroom";

int time = 0;
float x_axis = 0.0f, y_axis = 0.0f, z_axis = 0.0f;

void setupLights() {


	GLfloat lmodel_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	GLfloat l0Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l0Ambient[] = { .1f, .1f, .1f, 1.0f };
	GLfloat l0Position[] = { 0.0f, 18.0f, 3.0f, true };
	GLfloat l0Direction[] = { 0.0, -1.0, 0 };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, l0Ambient);
	glLightfv(GL_LIGHT1, GL_POSITION, l0Position);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 50.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 5);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l0Direction);

	GLfloat l1Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l1Ambient[] = { .1f, .1f, .1f, 1.0f };
	GLfloat l1Position[] = { 10.0f, 18.0f, 3.0f, true };
	GLfloat l1Direction[] = { 0.0, -1.0, 0 };
	glLightfv(GL_LIGHT2, GL_DIFFUSE, l1Diffuse);
	glLightfv(GL_LIGHT2, GL_AMBIENT, l1Ambient);
	glLightfv(GL_LIGHT2, GL_POSITION, l1Position);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 50.0);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 5);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, l1Direction);




}

void setupLights2() {


	GLfloat lmodel_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	GLfloat l0Diffuse[] = { r, g, b };
	GLfloat l0Ambient[] = { .1f, .1f, .1f, 1.0f };
	GLfloat l0Position[] = { 6.5f, 18.0f, 5.75f, true };
	GLfloat l0Direction[] = { 0, -1.0, -1.0 };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, l0Ambient);
	glLightfv(GL_LIGHT1, GL_POSITION, l0Position);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 50.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 5);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l0Direction);

	GLfloat l1Diffuse[] = { b, g, r, 1.0f };
	GLfloat l1Ambient[] = { .1f, .1f, .1f, 1.0f };
	GLfloat l1Position[] = { 6.5f, 18.0f, 5.75f, true };
	GLfloat l1Direction[] = { 0, -1.0, 1.0 };
	glLightfv(GL_LIGHT2, GL_DIFFUSE, l1Diffuse);
	glLightfv(GL_LIGHT2, GL_AMBIENT, l1Ambient);
	glLightfv(GL_LIGHT2, GL_POSITION, l1Position);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 50.0);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 5);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, l1Direction);

	GLfloat l13Diffuse[] = { g, b, r, 1.0f };
	GLfloat l13Ambient[] = { .1f, .1f, .1f, 1.0f };
	GLfloat l13Position[] = { -3.5f, 18.0f, 5.75f, true };
	GLfloat l13Direction[] = { 0.0, -1.0, 1 };
	glLightfv(GL_LIGHT3, GL_DIFFUSE, l13Diffuse);
	glLightfv(GL_LIGHT3, GL_AMBIENT, l13Ambient);
	glLightfv(GL_LIGHT3, GL_POSITION, l13Position);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 50.0);
	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 5);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, l13Direction);

	GLfloat l14Diffuse[] = { b, r, g, 1.0f };
	GLfloat l14Ambient[] = { .1f, .1f, .1f, 1.0f };
	GLfloat l14Position[] = { -3.5f, 18.0f, 5.75f, true };
	GLfloat l14Direction[] = { 0.0, -1.0, -1 };
	glLightfv(GL_LIGHT4, GL_DIFFUSE, l14Diffuse);
	glLightfv(GL_LIGHT4, GL_AMBIENT, l14Ambient);
	glLightfv(GL_LIGHT4, GL_POSITION, l14Position);
	glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, 50.0);
	glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, 5);
	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, l14Direction);

}

//=======================================================================
// Render Ground Function
//=======================================================================

void RenderGroundAndWalls(char* type)
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	if (type == "bedroom") {
		tex_ground = tex_ground_bedroom.texture[0];
		tex_wall = tex_wall_bedroom.texture[0];
	}
	else if (type == "livingroom") {
		tex_ground = tex_ground_livingroom.texture[0];
		tex_wall = tex_wall_livingroom.texture[0];
	}
	else {
		tex_ground = tex_ground_garage.texture[0];
		tex_wall = tex_wall_garage.texture[0];
	}

	glBindTexture(GL_TEXTURE_2D, tex_ground);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, tex_wall);	// Bind the ground texture

	glPushMatrix();
	glTranslated(0, 0, 20);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, -20, 0);
	glTexCoord2f(5, 0);
	glVertex3f(20, -20, 0);
	glTexCoord2f(5, 5);
	glVertex3f(20, 20, 0);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 20, 0);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, -20);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, -20, 0);
	glTexCoord2f(5, 0);
	glVertex3f(20, -20, 0);
	glTexCoord2f(5, 5);
	glVertex3f(20, 20, 0);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 20, 0);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslated(20, 0, 0);
	glRotated(90, 0, 1, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, -20, 0);
	glTexCoord2f(5, 0);
	glVertex3f(20, -20, 0);
	glTexCoord2f(5, 5);
	glVertex3f(20, 20, 0);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 20, 0);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-20, 0, 0);
	glRotated(90, 0, 1, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, -20, 0);
	glTexCoord2f(5, 0);
	glVertex3f(20, -20, 0);
	glTexCoord2f(5, 5);
	glVertex3f(20, 20, 0);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 20, 0);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 20, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.


	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, WIDTH / HEIGHT, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void draw_buzz() {
	glDisable(GL_COLOR_MATERIAL);
	glPushMatrix();
	/* rotate it around the y axis */
	if (walk<0)
		glRotatef(-walk / 4 * 3, 2.f, 8.f, 0.f);
	else
		glRotatef(-walk / 4 * 3, -2.f, 8.f, 0.f);

	//	glTranslated(0, down, 0);

	//glRotatef(angle, 0.f, 1.f, 0.f);
	//glScalef(.00025, .00025, .00025);
	glScalef(.025, .025, .025);

	/* center the model */
	//glTranslatef(-scene_center.x, -scene_center.y, -scene_center.z); //Not Used

	recursive_render(buzz_body, buzz_body->mRootNode);

	//elbow left
	glPushMatrix();
	glTranslated(0.11, 0.0, 0.03);
	glTranslated(-2.4, +0.6, 0);
	glRotatef(rotarml, 1.f, 0.f, 0.f);
	glTranslated(2.4, -0.6, 0);
	recursive_render(buzz_left_elbow, buzz_left_elbow->mRootNode);
	glPopMatrix();

	//elbow right
	glPushMatrix();
	glTranslated(-0.59, 1.68, -0.32);
	glTranslated(+2.8, -1, +0.4);
	glRotatef(rotarmr, 1.f, 0.f, 0.f);
	glTranslated(-2.8, 1, -0.4);
	recursive_render(buzz_right_elbow, buzz_right_elbow->mRootNode);
	glPopMatrix();


	//leg left
	glPushMatrix();
	glTranslated(2.34, -2.32, 0.14);
	glTranslated(-2.9, +0.8, 0);
	glRotatef(rot, 1.f, 0.f, 0.f);
	glTranslated(2.9, -0.8, 0);
	recursive_render(buzz_left_leg, buzz_left_leg->mRootNode);
	glPopMatrix();

	//leg right
	glPushMatrix();
	glTranslated(2.34, -2.32, 0.04);
	glTranslated(-1.6, 0.8, 0);
	glRotatef(rotl, 1.f, 0.f, 0.f);
	glTranslated(1.6, -0.8, 0);
	recursive_render(buzz_right_leg, buzz_right_leg->mRootNode);
	glPopMatrix();

	glPopMatrix();
	glEnable(GL_COLOR_MATERIAL);
}

void draw_Bouncing_Zurg() {
	glDisable(GL_COLOR_MATERIAL);
	glPushMatrix();
	glTranslated(2 * cos((DEG2RAD(en))), 2 * sin((DEG2RAD((en)))), 0);
	glRotatef(en2, 0.f, 1.f, 0.f);
	glScalef(.25, .25, .25);
	recursive_render(bouncing_Zurg, bouncing_Zurg->mRootNode);
	glPopMatrix();
	glEnable(GL_COLOR_MATERIAL);
}

void print_Text(int x, int y, int z, std::string string)
{
	int len, i;

	glRasterPos2f(x, y);

	len = (int)string.size();

	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}

}

void print_text(int x, int y, std::string string)
{
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluOrtho2D(0, viewport[2], viewport[3], 0);

	glDepthFunc(GL_ALWAYS);
	glColor3f(255, 255, 255);
	glRasterPos2f(x, y);
	for (int i = 0; i < string.size(); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	glDepthFunc(GL_LESS);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

int current_fps;
char keyw, keys, keya, keyd, keyspace;


float myY;
float myX;

void draw_power_up() {
	glDisable(GL_COLOR_MATERIAL);
	glPushMatrix();
	recursive_render(powerUp, powerUp->mRootNode);
	glPopMatrix();
	glEnable(GL_COLOR_MATERIAL);
}

void draw_potato_ear(){
	glDisable(GL_COLOR_MATERIAL);
	glPushMatrix();
	recursive_render(potato_ear, potato_ear->mRootNode);
	glPopMatrix();
	glEnable(GL_COLOR_MATERIAL);
}

void draw_potato_eye() {
	glDisable(GL_COLOR_MATERIAL);
	glPushMatrix();
	recursive_render(potato_eye, potato_eye->mRootNode);
	glPopMatrix();
	glEnable(GL_COLOR_MATERIAL);
}

void draw_potato_nose() {
	glDisable(GL_COLOR_MATERIAL);
	glPushMatrix();
	recursive_render(potato_nose, potato_nose->mRootNode);
	glPopMatrix();
	glEnable(GL_COLOR_MATERIAL);
}

void draw_Flyzurg() {
	glDisable(GL_COLOR_MATERIAL);
	glPushMatrix();
	glTranslated(fly, 0, 0);
	glPushMatrix();
	glTranslated(0, 1, 0);
	glScalef(.5, .5, .5);
	recursive_render(flyzurg_body, flyzurg_body->mRootNode);
	glPushMatrix();
	glRotatef(en2, 0.f, 1.f, 0.f);
	glTranslated(-0.26, 1.48, -0.64);
	recursive_render(flyzurg_fan, flyzurg_fan->mRootNode);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glEnable(GL_COLOR_MATERIAL);
}

//=======================================================================
// Calculate Fps Function
//=======================================================================

void fps_counter(void)
{
	static GLint prev_time = 0;
	static GLint prev_fps_time = 0;
	static int frames = 0;

	int time = glutGet(GLUT_ELAPSED_TIME);
	prev_time = time;
	frames += 1;
	if ((time - prev_fps_time) > 1000) /* update every seconds */
	{
		current_fps = frames * 1000 / (time - prev_fps_time);
		//printf("%d FPS\n", current_fps);
		frames = 0;
		prev_fps_time = time;
	}
}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{

	setupCamera();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderGroundAndWalls(type);


	if (type == "bedroom") {
		setupLights();
		// Draw Bed Model
		glColor3f(0.6, 0.5, 0.4);
		glPushMatrix();
		glTranslatef(-11.8, 0, -10);
		glScalef(0.008, 0.008, 0.008);
		model_bed.Draw();
		glPopMatrix();
		glColor3f(1, 1, 1);

		// Draw Desk Model
		glPushMatrix();
		glTranslatef(6, 0, -16);
		glRotatef(90, 0, 1, 0);
		glScalef(0.018, 0.019, 0.018);
		model_desk.Draw();
		glPopMatrix();

		// Draw Armchair Model
		glPushMatrix();
		glTranslatef(16, 0, 10);
		glRotatef(-90, 0, 1, 0);
		glScalef(0.2, 0.2, 0.2);
		model_armchair.Draw();
		glPopMatrix();

		// Draw Luster Model
		glPushMatrix();
		glTranslatef(5, 20, 0);
		glScalef(0.01, 0.01, 0.01);
		model_luster.Draw();
		glPopMatrix();

		// Draw Luster Model
		glPushMatrix();
		glTranslatef(-5, 20, 0);
		glScalef(0.01, 0.01, 0.01);
		model_luster.Draw();
		glPopMatrix();

		//Draw Rack Model
		glPushMatrix();
		glTranslatef(-10, 0, 18);
		glRotatef(114, 0, 1, 0);
		glScalef(0.008, 0.008, 0.008);
		model_rack.Draw();
		glPopMatrix();

		//Draw Carpet Model
		glPushMatrix();
		glTranslatef(4, 0, 8);
		glRotatef(-90, 1, 0, 0);
		glScalef(0.015, 0.015, 0.015);
		model_carpet.Draw();
		glPopMatrix();

		//Draw OfficeChair Model
		glColor3f(0.3, 0.3, 0.3);
		glPushMatrix();
		glTranslatef(6, -1.5, -12);
		glRotatef(180, 0, 1, 0);
		glScalef(0.0028, 0.003, 0.0025);
		model_officechair.Draw();
		glPopMatrix();
		glColor3f(1, 1, 1);

		// Draw Door Model
		glPushMatrix();
		glTranslatef(21.8, -1, -8);
		glRotatef(-90, 0, 1, 0);
		glScalef(0.018, 0.02, 0.018);
		model_door.Draw();
		glPopMatrix();

		// monster1
		glPushMatrix();
		glTranslated(-5, 0.2, 5);
		glScaled(0.2, 0.2, 0.2);
		draw_Bouncing_Zurg();
		glPopMatrix();

		// monster2
		glPushMatrix();
		glTranslated(-5, 0.05, 0);
		glScaled(0.2, 0.2, 0.2);
		draw_Flyzurg();
		glPopMatrix();

		// monster3
		glPushMatrix();
		glTranslated(0, 0.05, 10);
		glScaled(0.2, 0.2, 0.2);
		draw_Flyzurg();
		glPopMatrix();

		if (!found) {
			// mr potato part 1
			glPushMatrix();
			glTranslated(18, 0.2, 5);
			glScaled(0.2, 0.2, 0.2);
			draw_potato_eye();
			glPopMatrix();
		}

		if (!found_powerup) {
			// power up
			glPushMatrix();
			glTranslated(15, 0.2, 5);
			glScaled(0.5, 0.5, 0.5);
			draw_power_up();
			glPopMatrix();
		}

	}
	else if (type == "livingroom") {
		setupLights();
		//Draw AC Model
		glColor3f(0.9, 0.9, 0.9);
		glPushMatrix();
		glTranslatef(0, 17, -18);
		glScalef(0.0235, 0.02, 0.0205);
		model_ac.Draw();
		glPopMatrix();

		glColor3f(0.5, 0.5, 0.5);
		// Draw Chandelier Model
		glPushMatrix();
		glTranslatef(0, 18, -5);
		glScalef(0.01, 0.01, 0.01);
		model_chandelier.Draw();
		glPopMatrix();

		// Draw Chandelier Model
		glPushMatrix();
		glTranslatef(0, 18, 5);
		glScalef(0.01, 0.01, 0.01);
		model_chandelier.Draw();
		glPopMatrix();

		//Draw LivingSofa Model
		glPushMatrix();
		glTranslatef(-33, 0, 11);
		glScalef(0.1, 0.1, 0.1);
		glRotatef(180, 0, 1, 0);
		model_livingsofa.Draw();
		glPopMatrix();

		//Draw TVSet Model
		glPushMatrix();
		glTranslatef(0, 0, -15);
		glScalef(2.0, 2.0, 2.0);
		model_tvset.Draw();
		glPopMatrix();


		// Draw Door Model
		glPushMatrix();
		glTranslatef(15.8, 0, 13);
		glRotatef(90, 0, 1, 0);
		glScalef(0.007, 0.007, 0.007);
		model_door3.Draw();
		glPopMatrix();

		//Draw LivingCarpet Model
		glPushMatrix();
		glTranslatef(0, 0, -2);
		glScalef(0.25, 0.25, 0.25);
		glRotatef(90, 0, 1, 0);
		model_livingcarpet.Draw();
		glPopMatrix();

		//Draw Table Model
		glColor3f(0.5, 0.4, 0.3);
		glPushMatrix();
		glTranslatef(-3, 0, -6);
		glScalef(0.008, 0.008, 0.008);
		model_table.Draw();
		glPopMatrix();

		// monster1
		glPushMatrix();
		glTranslated(-5, 0.2, 5);
		glScaled(0.2, 0.2, 0.2);
		draw_Bouncing_Zurg();
		glPopMatrix();

		// monster2
		glPushMatrix();
		glTranslated(-5, 0.2, 0);
		glScaled(0.2, 0.2, 0.2);
		draw_Bouncing_Zurg();
		glPopMatrix();

		// monster3
		glPushMatrix();
		glTranslated(5, 0.05, 0);
		glScaled(0.2, 0.2, 0.2);
		draw_Flyzurg();
		glPopMatrix();

		if (!found2) {
			// mr potato part 1
			glPushMatrix();
			glTranslated(0, 5, 16.5);
			glScaled(0.2, 0.2, 0.2);
			draw_potato_ear();
			glPopMatrix();
		}

		if (!found2_powerup) {
			// power up
			glPushMatrix();
			glTranslated(10, 0.2, 0);
			glScaled(0.5, 0.5, 0.5);
			draw_power_up();
			glPopMatrix();
		}
	}
	else if (type == "garage") {
		setupLights2();
		// Draw tracklight Model
		glPushMatrix();
		glTranslatef(0, 14, 0);
		glScalef(0.01, 0.01, 0.01);
		model_tracklight.Draw();
		glPopMatrix();


		// Draw Mercedes Model
		glPushMatrix();
		glTranslatef(-10, 0, 2);
		glRotatef(90, 0, 1, 0);
		glScalef(0.000015, 0.000015, 0.000015);
		//model_mercedes.Draw();
		glPopMatrix();

		// Draw GarageDoor Model
		glPushMatrix();
		glTranslatef(54.55, 0, -19.5);
		glScalef(0.45, 0.3, 0.3);
		model_garagedoor.Draw();
		glPopMatrix();

		// Draw Skoda Model
		glPushMatrix();
		glTranslatef(32, 0, 13.5);
		glScalef(0.44, 0.44, 0.44);
		glRotatef(-90, 0, 1, 0);
		model_skoda.Draw();
		glPopMatrix();

		// Draw Toolbox Model
		glPushMatrix();
		glTranslatef(0, 0, 15);
		glScalef(0.012, 0.012, 0.012);
		model_toolbox.Draw();
		glPopMatrix();

		// Draw Door Model
		glPushMatrix();
		glTranslatef(-20, 0, 10);
		glRotatef(90, 0, 1, 0);
		glScalef(0.08, 0.08, 0.08);
		model_door2.Draw();
		glPopMatrix();

		// monster1
		glPushMatrix();
		glTranslated(-5, 0.05, 5);
		glScaled(0.2, 0.2, 0.2);
		draw_Flyzurg();
		glPopMatrix();

		// monster2
		glPushMatrix();
		glTranslated(-5, 0.05, 0);
		glScaled(0.2, 0.2, 0.2);
		draw_Flyzurg();
		glPopMatrix();

		// monster3
		glPushMatrix();
		glTranslated(0, 0.2, 10);
		glScaled(0.2, 0.2, 0.2);
		draw_Bouncing_Zurg();
		glPopMatrix();

		if (!found3) {
			// mr potato part 3
			glPushMatrix();
			glTranslated(-3,0.2,15);
			glScaled(0.2, 0.2, 0.2);
			draw_potato_nose();
			glPopMatrix();
		}

		if (!found3_powerup) {
			// power up
			glPushMatrix();
			glTranslated(0, 0.2, 13);
			glScaled(0.5, 0.5, 0.5);
			draw_power_up();
			glPopMatrix();
		}

	}
	//(18, 0.5, 5);

	Vector3f add = buzz_pos; // d2a mo2ktn



	if (type == "bedroom") {
		if ((add.x > 17 && add.x < 19) && (add.y > 0 && add.y < 2) && (add.z > 4 && add.z < 6)) {
			collects = "1";
			found = true;
		}
		//(15, 0.5, 5)
		if ((add.x > 14 && add.x < 16) && (add.y > -0.5 && add.y < 1.5) && (add.z > 4 && add.z < 6)) {
			if (lives == "2")
				lives = "3";
			else
				if (lives == "1")
					lives = "2";
			found_powerup = true;
		}
	}

	//(0, 5, 16.5);
	if (type == "livingroom") {

		if ((add.x > -1 && add.x < 1) && (add.y > 4 && add.y < 6) && (add.z > 15.5 && add.z < 17.5)) {
			collects = "2";
			found2 = true;
		}
		//(10, 0.5, 0)
		if ((add.x > 9 && add.x < 11) && (add.y > -0.5 && add.y < 1.5) && (add.z > -1 && add.z < 1)) {
			if (lives == "2")
				lives = "3";
			else
				if (lives == "1")
					lives = "2";
			found2_powerup = true;
		}
	}

	//(-3, 1, 15)
	if (type == "garage") {
		if ((add.x > -4 && add.x < -2) && (add.y > 0 && add.y < 2) && (add.z > 14 && add.z < 16)) {
			collects = "3";
			found3 = true;
		}
		//(0, 0.5, 13)
		if ((add.x > -1 && add.x < 1) && (add.y > -0.5 && add.y < 1.5) && (add.z > 12 && add.z < 14)) {
			if (lives == "2")
				lives = "3";
			else
				if (lives == "1")
					lives = "2";
			found3_powerup = true;
		}
	}

	glPushMatrix();
	glTranslated(buzz_pos.x, buzz_pos.y, buzz_pos.z);
	glRotated(buzz_angle, 0, 1, 0);
	draw_buzz();
	glPopMatrix();

	print_text(7, 24, "Parts Collected for Mr Potato: " + collects + "/3  -  Lives remaining: " + lives);
	print_text(484, 24, "Time: " + std::to_string(time) + " / 180");
	print_text(1500, 24, std::to_string(current_fps));
	
	fps_counter();
	glutSwapBuffers();
}

bool detectcollision(int x1, int y1, int z1, int radis1, int x2, int y2, int z2, int radis2)
{
	int xd, yd, zd, Distance;
	xd = x2 - x1;
	yd = y2 - y1;
	zd = z2 - z1;
	Distance = sqrt(xd*xd + yd*yd + zd*zd);
	if (radis1 + radis2 >= Distance)
		return true; //collision
	return false;    //no collision    
}

bool roomBorderCol(Vector3f pos){
	return (pos.x < 19 && pos.x > -19 && pos.y < 19 && pos.y >= 0.5 && pos.z < 19 && pos.z > -19);
}

bool col(Vector3f cur0, Vector3f cur1, Vector3f cur2, Vector3f cur3, Vector3f buzz_pos) {
	if ((cur0.x <= buzz_pos.x & buzz_pos.x <= cur1.x) && (cur0.z <= buzz_pos.z & buzz_pos.z <= cur2.z))
		return true;
	if ((cur2.x <= buzz_pos.x & buzz_pos.x <= cur3.x) && (cur1.z <= buzz_pos.z & buzz_pos.z <= cur3.z))
		return true;
	return false;
}

bool roomCol(Vector3f buzz_pos) {
	if (type == "garage")
		return /*!col(Vector(-16.4, 0, -12.8), Vector(-3.6, 0, -12.8), Vector(-16.4, 0, 17.2), Vector(-3.6, 0, 17.2), buzz_pos) &*/
		!col(Vector3f(-3, 0, 12.2), Vector3f(3, 0, 12.2), Vector3f(-3, 0, 17.8), Vector3f(3, 0, 17.8), buzz_pos) &
		!col(Vector3f(4.4, 0, -12), Vector3f(16.6, 0, -12), Vector3f(4.4, 0, 12.8), Vector3f(16.6, 0, 12.8), buzz_pos) &
		roomBorderCol(buzz_pos);
	if (type == "bedroom")
		return !col(Vector3f(12, 0, 6), Vector3f(18.6, 0, 6), Vector3f(12, 0, 14), Vector3f(18.6, 0, 14), buzz_pos) &
		!col(Vector3f(-15, 0, 15.8), Vector3f(-5, 0, 15.8), Vector3f(-15, 0, 18.8), Vector3f(-5, 0, 18.8), buzz_pos) &
		!col(Vector3f(-20, 0, -19.2), Vector3f(-3.8, 0, -19.2), Vector3f(-20, 0, -0.6), Vector3f(-3.8, 0, -0.6), buzz_pos) &
		!col(Vector3f(-0.4, 0, -19.4), Vector3f(12.4, 0, -19.4), Vector3f(-0.4, 0, -13), Vector3f(12.4, 0, -13), buzz_pos) &
		!col(Vector3f(3, 0, -13.4), Vector3f(9, 0, -13.4), Vector3f(3, 0, -10), Vector3f(9, 0, -10), buzz_pos) &
		roomBorderCol(buzz_pos);
	if (type == "livingroom")
		return !col(Vector3f(-8.2, 0, 6), Vector3f(9.2, 0, 6), Vector3f(-8.2, 0, 16.4), Vector3f(9.2, 0, 16.4), buzz_pos) &
		!col(Vector3f(-13, 0, -21), Vector3f(13.2, 0, -21), Vector3f(-13, 0, -11.8), Vector3f(13.2, 0, -11.8), buzz_pos) &
		!col(Vector3f(-4.6, 0, -11.8), Vector3f(4.8, 0, -11.8), Vector3f(-4.6, 0, -9.8), Vector3f(4.8, 0, -9.8), buzz_pos) &
		!col(Vector3f(-4.4, 0, -6.8), Vector3f(4.6, 0, -6.8), Vector3f(-4.4, 0, 2), Vector3f(4.6, 0, 2), buzz_pos) &
		roomBorderCol(buzz_pos);
	return true;
}

void camera_3rdPerson(){
	float distance_from_camera = 0.35;
	camera.eye = buzz_pos + Vector3f(distance_from_camera * cosf((-88.6 + camera_buzz_angle) * 3.14159 / 180), 0.125,
		distance_from_camera * sinf((-88.6 + camera_buzz_angle) * 3.14159 / 180));
	camera.up = Vector3f(0, 1, 0);
	camera.center = buzz_pos;

	/*camera.center = camera.center + (camera.eye - buzz_pos).unit() * distance_from_camera;
	camera.center.y = camera.eye.y;*/
}

void buzz_pos_reset(bool live_update){
	buzz_angle = 0;
	camera_buzz_angle = 0;
	buzz_pos = Vector3f(4, 0.5, 5);
	if (live_update)
		lives = std::to_string((std::stoi(lives) - 1));
	if (std::stoi(lives) <= 0){
		lives = "3";
		type = "bedroom";
	}
}

//=======================================================================
// Keyboard Function
//=======================================================================
void Special(int key, int x, int y) {
	float a = 1.5;
	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}
	glutPostRedisplay();
}

void myKeyboard(unsigned char button, int x, int y)
{
	float d = 0.2;
	switch (button) {
	case '1':
		type = "bedroom";
		buzz_pos_reset(false);
		break;
	case '2':
		if (found){
			type = "livingroom";
			buzz_pos_reset(false);
		}
		break;
	case '3':
		if (found2){
			type = "garage";
			buzz_pos_reset(false);
		}
		break;
	case '[':
		buzz_angle -= 10;
		break;
	case ']':
		buzz_angle += 10;
		break;
	case 'q':
		camera.moveY(d);
		break;
	case 'e':
		camera.moveY(-d);
		break;
	case 'w':
		keyw = button;
		break;
	case 's':
		keys = button;
		break;
	case 'a':
		keya = button;
		break;
	case 'd':
		keyd = button;
		break;
	case 32:
		keyspace = button;
		break;
	case 'k':
		buzz_angle += d * 10;
		camera_buzz_angle -= d * 10;
		camera_3rdPerson();
		break;
	case 'l':
		buzz_angle -= d * 10;
		camera_buzz_angle += d * 10;
		camera_3rdPerson();
		break;
	case 27: // Escape key
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void myUpKeyboard(unsigned char button, int x, int y) {
	if (button == 'w') {
		animateBuzz = false;
		keyw = '_';
		//camera.moveZ(d);
	}
	if (button == 's') {
		animateBuzz = false;
		keys = '_';
	}
	if (button == 'a') {
		animateBuzz = false;
		keya = '_';
	}
	if (button == 'd') {
		animateBuzz = false;
		keyd = '_';
	}
	if (button == 32) {
		keyspace = '_';
	}
}

//=======================================================================
// Motion Function
//=======================================================================
void myMouseMotion(int x, int y)
{
	//printf("%d   %d\n", x,  y);
	//y = HEIGHT - y;

	float a = 1.5;
	//To warp the cursor to a screen position
	/*if (y < HEIGHT/2) {
		camera.rotateX(a);
	} else if (y > HEIGHT / 2) {
		camera.rotateX(-a);
	}*/
	if (x < WIDTH/2) {
		buzz_angle += a;
		camera_buzz_angle -= a;
		camera_3rdPerson();
	} else if (x > WIDTH/2) {
		buzz_angle -= a;
		camera_buzz_angle += a;
		camera_3rdPerson();
	}
	glutWarpPointer(WIDTH / 2, HEIGHT / 2);
	glutPostRedisplay();


	/*cameraZoom = y;

	//glutWarpPointer(625, 450);

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	*/
}

void Timer(int value) {
	time += 1;
	if (time >= 180) {
		collects = "0";
		lives = "3";
		type = "bedroom";
		
		time = 0;
	}
	glutPostRedisplay();
	glutTimerFunc(1 * 1000, Timer, 0);
}

void time1(int val) {

	if (light == 2) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT4);
		glDisable(GL_LIGHT3);
		glEnable(GL_LIGHT2);
		//	light = (light+1) % 4;
	}
	else {
		if (light == 3) {
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT4);
			glDisable(GL_LIGHT2);
			glEnable(GL_LIGHT3);
		}
		else {
			if (light == 0) {
				glDisable(GL_LIGHT2);
				glDisable(GL_LIGHT4);
				glDisable(GL_LIGHT3);
				glEnable(GL_LIGHT1);
			}
			else {
				glDisable(GL_LIGHT1);
				glDisable(GL_LIGHT2);
				glEnable(GL_LIGHT4);
				glDisable(GL_LIGHT3);
			}
		}
	}
	//g = r;
	r = ((rand() % 255) / 255.0);
	b = ((rand() % 255) / 255.0);;
	g = ((rand() % 255) / 255.0);;



	light = (light + 1) % 4;
	//	glutTimerFunc(500, time, 0);

	//glutTimerFunc(10, time, 0);

	glutTimerFunc(250, time1, 0);

}
//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{

	// Loading Model files
	model_mercedes.Load("Models/elements_model/mercedes/mercedes.3DS");
	model_skoda.Load("Models/elements_model/skoda/Car_Skoda_Fabia N010909.3ds");
	model_carN23.Load("Models/elements_model/carN230709/Car N230709.3DS");
	model_sofa.Load("Models/elements_model/sofa/Sofa.3DS");
	model_armchairSofa.Load("Models/elements_model/armchairSofa/Sofa 20.3DS");

	model_armchair.Load("Models/elements_model/armchair/Armchair.3DS");
	model_desk.Load("Models/elements_model/desk/Table.3DS");

	model_lampTolomeo.Load("Models/elements_model/lampTolomeo/Lamp Tolomeo Micro N151211.3DS");
	model_tablelamp.Load("Models/elements_model/tablelamp/Table-lamp.3DS");
	model_tracklight.Load("Models/elements_model/tracklight/track-light.3DS");
	model_lusterSmithfield.Load("Models/elements_model/lusterSmithfield/Luster SMITHFIELD FLOS N251113.3DS");
	model_luster.Load("Models/elements_model/luster/Luster 5.3DS");

	model_bed.Load("Models/elements_model/Bed/Bed Relais N050912.3DS");

	model_bookcaseDesk.Load("Models/elements_model/bookcaseDesk/Bookcase.3DS");
	model_rack.Load("Models/elements_model/rack/Rack 2.3DS");
	model_carpet.Load("Models/elements_model/carpet/Carpet GAELIC BROWN N230616.3DS");
	model_officechair.Load("Models/elements_model/officechair/Armchair Poltrona Frau Onda Office Bridge slitte N290611.3DS");
	model_door.Load("Models/elements_model/door/Door close.3DS");
	model_door2.Load("Models/elements_model/door2/Door Endless Boiserie Sliding Door Longhi N291216.3DS");
	model_door3.Load("Models/elements_model/door3/Door N191016.3DS");
	model_garagedoor.Load("Models/elements_model/garagedoor2/Gate 1.3DS");
	model_toolbox.Load("Models/elements_model/toolbox/Box N140212.3DS");

	model_table.Load("Models/elements_model/table/Table.3DS");
	model_tvset.Load("Models/elements_model/tvset2/Tv stand N141211.3DS");
	model_livingsofa.Load("Models/elements_model/livingsofa/Sofa prado 1.3DS");
	model_livingcarpet.Load("Models/elements_model/livingcarpet/Carpet N220916.3DS");

	model_shelf.Load("Models/elements_model/shelf/Shelf 9.3DS");
	model_ac.Load("Models/elements_model/ac/Conditioner_Samsung_AQ12ASA N220411.3ds");
	model_chandelier.Load("Models/elements_model/chandelier/Chandelier axo light  muse N050611.3DS");

	// Loading texture files
	tex_ground_bedroom.Load("Textures/ground_bedroom.bmp");
	tex_wall_bedroom.Load("Textures/wall_bedroom.bmp");
	tex_ground_livingroom.Load("Textures/ground_livingroom.bmp");
	tex_wall_livingroom.Load("Textures/wall_livingroom.bmp");
	tex_ground_garage.Load("Textures/ground_garage.bmp");
	tex_wall_garage.Load("Textures/wall_garage.bmp");

	powerUp = loadasset("Models/powerup_models/battery/Battery.obj");
	//-----------------------------Buzz_Model-----------------------------------
	buzz_body = loadasset("Models/buzz_model/Buzz_Body.obj");
	buzz_left_elbow = loadasset("Models/buzz_model/Buzz_Left_Elbow.obj");
	buzz_right_elbow = loadasset("Models/buzz_model/Buzz_Right_Elbow.obj");
	buzz_left_leg = loadasset("Models/buzz_model/Buzz_Left_Leg.obj");
	buzz_right_leg = loadasset("Models/buzz_model/Buzz_Right_Leg.obj");
	//--------------------------------------------------------------------------
	//-----------------------------Enemies_Models-------------------------------
	bouncing_Zurg = loadasset("Models/enemy_model/Bouncing_Zurg.obj");
	flyzurg_body = loadasset("Models/enemy_model/Flyzurg.obj");
	flyzurg_fan = loadasset("Models/enemy_model/Flyzurg_Fan.obj");
	//--------------------------------------------------------------------------

	//-----------------------------MrPotato_Parts_Model-------------------------------
	potato_ear = loadasset("Models/mrpotato_model/Ear.obj");
	potato_nose = loadasset("Models/mrpotato_model/Nose.obj");
	potato_eye = loadasset("Models/mrpotato_model/eyes.obj");
	//--------------------------------------------------------------------------

	//-----------------------------MrPotato_Models------------------------------- More To Be Added
	//mrpotato_eye = loadasset("Models/mrpotato_model/eyes.obj");
	//mrpotato_nose= loadasset("Models/mrpotato_model/eyes.obj");
	//--------------------------------------------------------------------------

}

void enemy_col_detect(Vector3f add){

	if (type == "bedroom" && (detectcollision(add.x, add.y, add.z, 0.8, -5, 0.2, 5, 0.8) ||
		detectcollision(add.x, add.y, add.z, 0.8, -5, 0.05, 0, 0.8) ||
		detectcollision(add.x, add.y, add.z, 0.8, 0, 0.05, 10, 0.8)))
		buzz_pos_reset(true);

	if (type == "livingroom" && (detectcollision(add.x, add.y, add.z, 0.8, -5, 0.2, 5, 0.8) ||
		detectcollision(add.x, add.y, add.z, 0.8, -5, 0.2, 0, 0.8) ||
		detectcollision(add.x, add.y, add.z, 0.8, 5, 0.05, 0, 0.8)))
		buzz_pos_reset(true);

	if (type == "garage" && (detectcollision(add.x, add.y, add.z, 0.8, -5, 0.05, 5, 0.8) ||
		detectcollision(add.x, add.y, add.z, 0.8, -5, 0.05, 0, 0.8) ||
		detectcollision(add.x, add.y, add.z, 0.8, 0, 0.2, 10, 0.8)))
		buzz_pos_reset(true);
}

void Anim() {
	if (fly < 2 && flyf == 1) {
		fly += 0.1;
	}
	else {
		flyf = 0;
		if (fly > 0)
			fly--;
		else
			flyf = 1;
	}
	en2 += 10;
	//enemy 1
	if (en <= 180 && enn == 1) {
		en += 10;
	}
	else {
		enn = 0;
		if (en > 0) {
			en -= 10;
		}
		else {
			enn = 1;
		}
	}

	//--------------------------Buzz Animation--------------------------
	if (animateBuzz) {
		if (flagrot && rot < 30) {
			rot += 4 * 4;
		}
		else {
			flagrot = 0;
			if (rot > -30) {
				rot -= 4 * 4;
			}
			else {
				flagrot = 1;
			}
		}
		//left
		if (flagrotl && rotl > -30) {
			rotl -= 4 * 4;
		}
		else {
			flagrotl = 0;
			if (rotl < 30) {
				rotl += 4 * 4;
			}
			else {
				flagrotl = 1;
			}
		}
		if (flagarmr && rotarmr > -80) {
			rotarmr -= 5.25 * 4;
			if (rotarml <30)
				rotarml += 5.25 * 4;
		}
		else {
			flagarmr = 0;
			if (rotarmr < 30) {
				rotarmr += 5.25 * 4;
				rotarml -= 5.25 * 4;
			}
			else {
				flagarmr = 1;
			}
		}
		//walk
		if (flagwalk && walk < 40) {
			walk += 4 * 4;
			//	down -= 0.1;
		}
		else {
			//	down += 0.01;
			flagwalk = 0;
			if (walk > -40) {
				walk -= 4 * 4;
			}
			else {
				flagwalk = 1;
			}
		}
	}
	else {
		walk = 0;
		rotarmr = 0;
		rotarml = 0;
		rotl = 0;
		rot = 0;
	}
	float distance = 0.1;
	if (keyw == 'w') {
		if (roomCol(Vector3f((buzz_pos.x + (distance * sinf(buzz_angle * 3.14159 / 180))), buzz_pos.y, (buzz_pos.z + (distance * cosf(buzz_angle * 3.14159 / 180)))))) {
			animateBuzz = 1;
			buzz_pos.x += (distance * sinf(buzz_angle * 3.14159 / 180));
			buzz_pos.z += (distance * cosf(buzz_angle * 3.14159 / 180));
			enemy_col_detect(buzz_pos);
		}
		camera_3rdPerson();
	}
	if (keys == 's') {
		if (roomCol(Vector3f((buzz_pos.x - (distance * sinf(buzz_angle * 3.14159 / 180))), buzz_pos.y, (buzz_pos.z - (distance * cosf(buzz_angle * 3.14159 / 180)))))) {
			animateBuzz = 1;
			buzz_pos.x -= (distance * sinf(buzz_angle * 3.14159 / 180));
			buzz_pos.z -= (distance * cosf(buzz_angle * 3.14159 / 180));
			enemy_col_detect(buzz_pos);
		}
		camera_3rdPerson();
	}
	if (keya == 'a') {
		if (roomCol(Vector3f((buzz_pos.x + (distance * cosf(buzz_angle * 3.14159 / 180))), buzz_pos.y, (buzz_pos.z - (distance * sinf(buzz_angle * 3.14159 / 180)))))) {
			animateBuzz = 1;
			buzz_pos.x += (distance * cosf(buzz_angle * 3.14159 / 180));
			buzz_pos.z -= (distance * sinf(buzz_angle * 3.14159 / 180));
			enemy_col_detect(buzz_pos);
		}
		camera_3rdPerson();
	}
	if (keyd == 'd') {
		if (roomCol(Vector3f((buzz_pos.x - (distance * cosf(buzz_angle * 3.14159 / 180))), buzz_pos.y, (buzz_pos.z + (distance * sinf(buzz_angle * 3.14159 / 180)))))) {
			animateBuzz = 1;
			buzz_pos.x -= (distance * cosf(buzz_angle * 3.14159 / 180));
			buzz_pos.z += (distance * sinf(buzz_angle * 3.14159 / 180));
			enemy_col_detect(buzz_pos);
		}
		camera_3rdPerson();
	}
	if (keyspace == 32) {
		if (roomCol(Vector3f(buzz_pos.x, buzz_pos.y + distance, buzz_pos.z))){
			buzz_pos.y += distance;
			enemy_col_detect(buzz_pos);
		}
		camera_3rdPerson();
	}
	else
		if (keyspace == '_') {
			if (roomCol(Vector3f(buzz_pos.x, buzz_pos.y - distance, buzz_pos.z))){
				buzz_pos.y -= distance;
				enemy_col_detect(buzz_pos);
			} else
				keyspace = '/n';
			camera_3rdPerson();
		}

	float a = 2;

	if (myY < 450) {
		//camera.rotateX(a);
	}
	if (myY > 450) {
		//camera.rotateX(-a);
	}
	if (myX < 625) {
		//	camera.rotateY(a);
	}
	if (myX > 625) {
		//	camera.rotateY(-a);
	}
	//	glutWarpPointer(625, 450);

	//--------------------------Buzz Animation--------------------------

	//for (int i = 0; i < 1000000; i++);
	glutPostRedisplay();
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(0, 0);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutIdleFunc(Anim);

	glutKeyboardFunc(myKeyboard);

	glutKeyboardUpFunc(myUpKeyboard);

	glutTimerFunc(0, Timer, 0);

	glutTimerFunc(0, time1, 0);

	glutSpecialFunc(Special);

	glutPassiveMotionFunc(myMouseMotion);

	//To hide the cursor
	glutSetCursor(GLUT_CURSOR_NONE);


	LoadAssets();

	glClearColor(0.1f, 0.1f, 0.1f, 1.f);
	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
	glEnable(GL_LIGHT5);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);
	glutFullScreen();
	glutMainLoop();

	aiReleaseImport(buzz_body);
	aiReleaseImport(buzz_left_leg);
	aiReleaseImport(buzz_left_elbow);
	aiReleaseImport(buzz_right_leg);
	aiReleaseImport(buzz_right_elbow);

	aiReleaseImport(flyzurg_fan);
	aiReleaseImport(flyzurg_body);
	aiReleaseImport(bouncing_Zurg);

	aiReleaseImport(potato_eye);
	aiReleaseImport(potato_ear);
	aiReleaseImport(potato_nose);

	aiReleaseImport(powerUp);
}