#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <Libraries\gl\include\glut.h>
#include <Libraries\assmip\include\cimport.h>
#include <Libraries\assmip\include\scene.h>
#include <Libraries\assmip\include\postprocess.h>


const struct aiScene * scene = NULL;

GLuint scene_list = 0;
struct aiVector3D scene_min, scene_max, scene_center; // not used

/* current rotation angle */
static float angle = 0.f;
float x_axis = 0.f, y_axis = 0.f, z_axis = 0.f;
#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

/* ---------------------------------------------------------------------------- */
void reshape(int width, int height)
{
	const double aspectRatio = (float)width / height, fieldOfView = 45.0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fieldOfView, aspectRatio,
		1.0, 1000.0);  /* Znear and Zfar */
	glViewport(0, 0, width, height);
}

/* ---------------------------------------------------------------------------- */
void get_bounding_box_for_node(const struct aiNode* nd,
struct aiVector3D* min,
struct aiVector3D* max,
struct aiMatrix4x4* trafo
	){
	struct aiMatrix4x4 prev;
	unsigned int n = 0, t;

	prev = *trafo;
	aiMultiplyMatrix4(trafo, &nd->mTransformation);

	for (; n < nd->mNumMeshes; ++n) {
		const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			struct aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp, trafo);

			min->x = aisgl_min(min->x, tmp.x);
			min->y = aisgl_min(min->y, tmp.y);
			min->z = aisgl_min(min->z, tmp.z);

			max->x = aisgl_max(max->x, tmp.x);
			max->y = aisgl_max(max->y, tmp.y);
			max->z = aisgl_max(max->z, tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(nd->mChildren[n], min, max, trafo);
	}
	*trafo = prev;
}

/* ---------------------------------------------------------------------------- */
void get_bounding_box(struct aiVector3D* min, struct aiVector3D* max)
{
	struct aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	min->x = min->y = min->z = 1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene->mRootNode, min, max, &trafo);
}

/* ---------------------------------------------------------------------------- */
void color4_to_float4(const struct aiColor4D *c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

/* ---------------------------------------------------------------------------- */
void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

/* ---------------------------------------------------------------------------- */
void apply_material(const struct aiMaterial *mtl)
{
	float c[4];

	GLenum fill_mode;
	int ret1, ret2;
	struct aiColor4D diffuse;
	struct aiColor4D specular;
	struct aiColor4D ambient;
	struct aiColor4D emission;
	float shininess, strength;
	int two_sided;
	int wireframe;
	unsigned int max;

	set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		color4_to_float4(&diffuse, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
		color4_to_float4(&specular, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

	set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
		color4_to_float4(&ambient, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
		color4_to_float4(&emission, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

	max = 1;
	ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
	if (ret1 == AI_SUCCESS) {
		max = 1;
		ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
		if (ret2 == AI_SUCCESS)
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
		else
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	}
	else {
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
		set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
	}

	max = 1;
	if (AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, &max))
		fill_mode = wireframe ? GL_LINE : GL_FILL;
	else
		fill_mode = GL_FILL;
	glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

	//max = 1;
	//if ((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
	//	glDisable(GL_CULL_FACE);
	//else
	//	glEnable(GL_CULL_FACE);
}

/* ---------------------------------------------------------------------------- */
void recursive_render(const struct aiScene *sc, const struct aiNode* nd)
{
	unsigned int i;
	unsigned int n = 0, t;
	struct aiMatrix4x4 m = nd->mTransformation;

	/* update transform */
	aiTransposeMatrix4(&m);
	glPushMatrix();
	glMultMatrixf((float*)&m);

	/* draw all meshes assigned to this node */
	for (; n < nd->mNumMeshes; ++n) {
		const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];

		apply_material(sc->mMaterials[mesh->mMaterialIndex]);

		if (mesh->mNormals == NULL) {
			glDisable(GL_LIGHTING);
		}
		else {
			glEnable(GL_LIGHTING);
		}

		for (t = 0; t < mesh->mNumFaces; ++t) {
			const struct aiFace* face = &mesh->mFaces[t];
			GLenum face_mode;

			switch (face->mNumIndices) {
			case 1: face_mode = GL_POINTS; break;
			case 2: face_mode = GL_LINES; break;
			case 3: face_mode = GL_TRIANGLES; break;
			default: face_mode = GL_POLYGON; break;
			}

			glBegin(face_mode);

			for (i = 0; i < face->mNumIndices; i++) {
				int index = face->mIndices[i];
				if (mesh->mColors[0] != NULL)
					glColor4fv((GLfloat*)&mesh->mColors[0][index]);
				if (mesh->mNormals != NULL)
					glNormal3fv(&mesh->mNormals[index].x);
				glVertex3fv(&mesh->mVertices[index].x);
			}

			glEnd();
		}

	}

	/* draw all children */
	for (n = 0; n < nd->mNumChildren; ++n) {
		recursive_render(sc, nd->mChildren[n]);
	}

	glPopMatrix();
}

/* ---------------------------------------------------------------------------- */
void do_motion(void)
{
	static GLint prev_time = 0;
	static GLint prev_fps_time = 0;
	static int frames = 0;

	int time = glutGet(GLUT_ELAPSED_TIME);
	//angle += (time-prev_time)*0.01;
	prev_time = time;

	frames += 1;
	if ((time - prev_fps_time) > 1000) /* update every seconds */
	{
		int current_fps = frames * 1000 / (time - prev_fps_time);
		printf("%d fps\n", current_fps);
		frames = 0;
		prev_fps_time = time;
	}


	glutPostRedisplay();
}
/* ---------------------------------------------------------------------------- */

//void display(void)
//{
//	float tmp;
//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	/*
//	glClearColor(0.0, 0.0, 0.0, 0.0);
//
//	glMatrixMode(GL_PROJECTION);
//
//	glLoadIdentity();
//
//	gluPerspective(100.0, 900.0 / 600.0, 0.1, 250);
//	*/
//	//*******************************************************************************************//
//	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
//	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
//	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
//	//*******************************************************************************************//
//
//	glMatrixMode(GL_MODELVIEW);
//
//	glLoadIdentity();
//	//gluLookAt(1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
//	gluLookAt(0.f, 0.f, 3.f, 0.f, 0.f, -5.f, 0.f, 1.f, 0.f);
//	glClear(GL_COLOR_BUFFER_BIT);
//	/*
//	glPushMatrix();
//	glBegin(GL_LINES);
//	glColor3d(1, 0, 00);
//	glVertex3d(-100, 0, 0);
//	glVertex3d(100, 0, 0);
//	glEnd();
//	glBegin(GL_LINES);
//	glColor3d(0, 0, 1);
//	glVertex3d(0, -100, 0);
//	glVertex3d(0, 100, 0);
//	glEnd();
//	glBegin(GL_LINES);
//	glColor3d(0, 1, 0);
//	glVertex3d(0, 0, -100);
//	glVertex3d(0, 0, 100);
//	glEnd();
//	glPopMatrix();
//	*/
//	//draw_buzz();
//	//draw_robotito();
//
//	glutSwapBuffers();
//	do_motion();
//}


/* ---------------------------------------------------------------------------- */
struct aiScene * loadasset(const char* path)
{
	printf("Loading %s \n", path);
	struct aiScene * model;
	model = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (model)
		return model;
	exit(-1);
	return NULL;
}

void keyboard(unsigned char key, int x, int y){
	float d = 0.01;
	switch (key) {
	case 'w':
		y_axis += d;
		break;
	case 's':
		y_axis -= d;
		break;
	case 'a':
		x_axis -= d;
		break;
	case 'd':
		x_axis += d;
		break;
	case 'z':
		z_axis -= d;
		break;
	case 'x':
		z_axis += d;

		break;
	case 'r':
		angle += 10;
		break;

	case 'c':
		scene_list = 0;
		break;

	case 27:
		exit(EXIT_SUCCESS); break;

	}
	glutPostRedisplay();
}
/* ---------------------------------------------------------------------------- */
/*
int main(int argc, char **argv)
{
struct aiLogStream stream;

glutInitWindowSize(900, 600);
glutInitWindowPosition(100, 100);
glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
glutInit(&argc, argv);

glutCreateWindow("OpenGL sample");
glutDisplayFunc(display);
glutReshapeFunc(reshape);

glutIdleFunc(Anim);
stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
aiAttachLogStream(&stream);

// ... same procedure, but this stream now writes the log messages to assimp_log.txt
stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE, "assimp_log.txt");
aiAttachLogStream(&stream);

//-----------------------------Buzz_Model-----------------------------------
buzz_body = loadasset("../Models/buzz_model/Buzz_Body.obj");
buzz_left_elbow = loadasset("../Models/buzz_model/Buzz_Left_Elbow.obj");
buzz_right_elbow = loadasset("../Models/buzz_model/Buzz_Right_Elbow.obj");
buzz_left_leg = loadasset("../Models/buzz_model/Buzz_Left_Leg.obj");
buzz_right_leg = loadasset("../Models/buzz_model/Buzz_Right_Leg.obj");
//--------------------------------------------------------------------------

//-----------------------------Enemies_Models------------------------------- More To Be Added
robotito = loadasset("../Models/enemy_model/robotito.obj");
//--------------------------------------------------------------------------

glClearColor(0.1f, 0.1f, 0.1f, 1.f);

glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);

glEnable(GL_DEPTH_TEST);
glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
glEnable(GL_NORMALIZE);


if (getenv("MODEL_IS_BROKEN"))
glFrontFace(GL_CW);

glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
glShadeModel(GL_SMOOTH);

glutGet(GLUT_ELAPSED_TIME);
glutKeyboardFunc(keyboard);
glutMainLoop();

aiReleaseImport(buzz_body);
aiReleaseImport(buzz_left_elbow);
aiReleaseImport(buzz_right_elbow);
aiReleaseImport(buzz_left_leg);
aiReleaseImport(buzz_right_leg);


aiDetachAllLogStreams();
return 0;
}
*/