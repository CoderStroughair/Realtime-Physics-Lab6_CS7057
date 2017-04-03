#pragma once
#include <GL/glew.h>
#include "Antons_maths_funcs.h"
#include <GL/freeglut.h>


class Framebuffer {

public:
	GLuint framebuffer, tex;
	Framebuffer(){}

	void init(int width, int height, int index)
	{
		glGenFramebuffers(1, &framebuffer);
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

		GLuint depth_tex;
		glGenTextures(1, &depth_tex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depth_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach depth texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);
		GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, draw_bufs);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (GL_FRAMEBUFFER_COMPLETE != status) {
			fprintf(stderr, "ERROR: incomplete framebuffer\n");
			if (GL_FRAMEBUFFER_UNDEFINED == status) {
				fprintf(stderr, "GL_FRAMEBUFFER_UNDEFINED\n");
			}
			else if (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT == status) {
				fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
			}
			else if (GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT == status) {
				fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
			}
			else if (GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER == status) {
				fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n");
			}
			else if (GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER == status) {
				fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n");
			}
			else if (GL_FRAMEBUFFER_UNSUPPORTED == status) {
				fprintf(stderr, "GL_FRAMEBUFFER_UNSUPPORTED\n");
			}
			else if (GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE == status) {
				fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n");
			}
			else if (GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS == status) {
				fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS\n");
			}
			else {
				fprintf(stderr, "unspecified error\n");
			}
			exit(-1);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, index);
	}
};