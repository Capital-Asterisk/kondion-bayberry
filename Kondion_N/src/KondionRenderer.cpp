/*
 * KondionRenderer.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: neal
 */

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Kondion.h"

namespace Kondion {


	namespace Renderer {

		void Setup() {

		}

		void Three(OKO_Camera_* camera, int width, int height) {
			//glViewport(0, 0, width, height);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(45.0f,((GLfloat) width) / ((GLfloat) height), 0.1f, 100.0f);
			camera->prespective(); // gluLookAt
			glMatrixMode(GL_MODELVIEW);
			//glLoadIdentity();
			// glCullFace(GL_BACK);
			glEnable(GL_BLEND);
			// glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_POINT_SMOOTH);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthFunc(GL_LEQUAL);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
			glClearDepth(10.0f);

		}



	}
}

