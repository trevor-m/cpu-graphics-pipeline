#include <stdio.h>

#include "Parser.h"
#include "Scene.h"
#include "Texture.h"

int cells_wide = 5;
int cells_high = 5;
int scene_width = 0;
int scene_height = 0;
int wall_height = 5;
int cell_size = 5;

/* for textures */
int num_tex = 0;
Texture **tex = NULL;
int *tex_codes = NULL;

FILE *fp;

extern Scene scene;

void parseTexture(int count, char *line) {
	char *start, *end;
	char tex_name[MAX_LINE_SIZE];
	char arg[MAX_LINE_SIZE];

	end = findFirstSpace(line);
	copyString(arg, line, end);
	end = skipSpace(end);

	tex_codes[count] = atoi(arg);

	start = skipSpace(end);
	end = findFirstSpace(start);
	copyString(tex_name, start, end);

	tex[count] = new Texture(tex_name);

	return;
}

int getWall(char **line) {
	char arg[3];
	char *start, *end;

	start = skipSpace(*line);
	end = findFirstSpace(start);
	copyString(arg, start, end);
	*line = skipSpace(end);

	return (atoi(arg));
}

void parseFloorPlan(int count, char *line) {
	int i;
	int *walls;
	int row;
	float x,y;

	/* make room for the data */
	walls = (int *)malloc(sizeof(int) * cells_wide);

	/* first parse the line and determine where the walls are */
	for (i = 0; i < cells_wide; i++) {
		walls[i] = getWall(&line);
		/* note that a 0 indicates no wall, else is the wall number */

		if ((walls[i] < 0) || (walls[i] > num_tex)) {
			fprintf(stderr,"ERROR!  Invalid wall specified\n");
			fflush(stderr);
			exit(-1);
		}
	}

	row = count/2;
	
	y = (((float)scene_height) / (float)2) - row * cell_size;
	x = -(((float)scene_width) / (float)2);

	if (count %2) {
		/* count is odd, so we are specifying vertical walls */

		for (i = 0; i < cells_wide; i++) {
			if (walls[i]) {
				/* add the wall to the list */
				scene.addWall(x, y, x, y - (float)cell_size, wall_height, tex[walls[i] - 1]); 
			}

			x += (float)cell_size;
		}
	}
	else {
		/* count is even, so we are specifying horizontal walls */

		for (i = 0; i < cells_wide; i++) {
			if (walls[i]) {			
				scene.addWall(x, y, x + (float)cell_size, y, wall_height, tex[walls[i] - 1]); 
			}

			x += (float)cell_size;
		}
	}

	free(walls);

	return;
}

void loadTextures(void) {
	int count = 0;
	char line_buffer[MAX_LINE_SIZE];

	fprintf(stderr,"Loading textures\n");

	/* first make room for all the textures */
	tex = (Texture **)malloc(sizeof(Texture*) * num_tex);
	tex_codes = (int *)malloc(sizeof(int) * num_tex);

	while((count < num_tex) && fgets(line_buffer, MAX_LINE_SIZE, fp)) {	
		/* only do something if it's not a comment and the line is not blank */
		if (!lineCanBeIgnored(line_buffer)) {
			fprintf(stderr,"%s\n", line_buffer);
			parseTexture(count, line_buffer);
			count++;
		}
	}

	return;
}

void loadFloorPlan(void) {
	int count = 0;
	char line_buffer[MAX_LINE_SIZE];
	fprintf(stderr,"Loading floor plan\n");

	/* compute the size of the scene */
	scene_width = cells_wide * cell_size;
	scene_height = cells_high * cell_size;

	fprintf(stderr,"size of scene: %d x %d\n", scene_width, scene_height);

	while((count < cells_high * 2) && fgets(line_buffer, MAX_LINE_SIZE, fp)) {	
		/* only do something if it's not a comment and the line is not blank */
		if (!lineCanBeIgnored(line_buffer)) {
			parseFloorPlan(count, line_buffer);
			count++;
		}
	}

	/* after i have added all the walls, let us add the floor */
	float x = -(((float)scene_width) / (float)2);
	float y = (((float)scene_height) / (float)2);
	scene.addFloor(x, y, x + scene_width, y - scene_height, tex[num_tex-1], scene_width, scene_height);

	return;
}

void parseScene(char *line) {
	char command[MAX_LINE_SIZE];
	char arg[MAX_LINE_SIZE];
	char *start, *end;


	/* i need to get the first word */
	end = findFirstSpace(line);

	/* observe what the command is */
	copyString(command, line, end);

	if (!strcmp(command,"DIMENSIONS")) {
		/* parse the dimensions of the scene */
		
		start = skipSpace(end);
		end = findFirstSpace(start);
		copyString(arg, start, end);
		end = skipSpace(end);

		cells_wide = atoi(arg);

		start = skipSpace(end);
		end = findFirstSpace(start);
		copyString(arg, start, end);
		end = skipSpace(end);

		cells_high = atoi(arg);
		assert(lineCanBeIgnored(end));	/* the rest of the line should be able to be ignored */

		fprintf(stderr,"width: %d   height: %d\n", cells_wide, cells_high);
	}
	else if (!strcmp(command,"HEIGHT")) {
		start = skipSpace(end);
		end = findFirstSpace(start);
		copyString(arg, start, end);
		end = skipSpace(end);

		wall_height = atoi(arg);
		fprintf(stderr,"wall height: %d\n", wall_height);
	}
	else if (!strcmp(command,"CELL")) {
		start = skipSpace(end);
		end = findFirstSpace(start);
		copyString(arg, start, end);
		end = skipSpace(end);

		cell_size = atoi(arg);
		fprintf(stderr,"cell size: %d\n", cell_size);
	}
	else if (!strcmp(command,"TEXTURES")) {
		start = skipSpace(end);
		end = findFirstSpace(start);
		copyString(arg, start, end);
		end = skipSpace(end);

		num_tex = atoi(arg);
		fprintf(stderr,"number of textures: %d\n", num_tex);

		loadTextures();
	}
	else if (!strcmp(command,"FLOORPLAN")) {
		assert(lineCanBeIgnored(end));	/* the rest of the line should be able to be ignored */

		loadFloorPlan();
	}
	else {
		fprintf(stderr,"ERROR! Unrecognized command in scene file \n");
		fprintf(stderr,"%s\n", line);
		exit(-1);
	}

	return;
}


void loadScene(char *name) {
	char line_buffer[MAX_LINE_SIZE];

	fp = fopen(name,"r");

	if(!fp) {	/* file could not be opened properly */
		fprintf(stderr,"ERROR: Scene file %s could not be loaded\n", name);
		fflush(stderr);
		exit(-1);
	}

	/* now go through all the lines in the file */
	while(fgets(line_buffer, MAX_LINE_SIZE, fp)) {	
		/* only do something if it's not a comment and the line is not blank */
		if (!lineCanBeIgnored(line_buffer)) {
			/* at this point it is neither a comment nor a blank line */
			parseScene(line_buffer);
		}
	}

	fclose(fp);


	return;
}

void setDefaults(void) {
	wall_height = 5;
}

void sceneCleanup(void) {
	int i;

	/* remove all the textures */
	for (i = 0; i < num_tex; i++) {
		delete tex[i];
	}

	free(tex);
	free(tex_codes);

	return;
}
