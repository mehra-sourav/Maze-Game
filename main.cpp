#include <windows.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 580
#define DELTA_TIME 0
#define R GLUT_KEY_RIGHT
#define L GLUT_KEY_LEFT
#define T GLUT_KEY_UP
#define B GLUT_KEY_DOWN
#define MYINFINITE 99

int MAZESIZE, CELLSIZE, flag=0, shortestPathIterator,count=0;
int currentCellX, currentCellY, currentDirection, sourceCellX, sourceCellY, destCellX, destCellY, curObjX = 0, curObjY = 0;
int graph[10000][10000];
bool Switch=false;

enum {
	LEFT, RIGHT, TOP, BOTTOM
};

int oppositeDirection[] = { RIGHT, LEFT, BOTTOM, TOP };
int dx[] = { -1, 1, 0, 0 };
int dy[] = { 0, 0, -1, 1 };

struct Cell {
	int x, y;
	bool wall[4];
	bool visited;
	bool alreadyVisited;
	bool shortestPath;
	bool colorvisited = false;
	bool current =false;
};

struct Maze {
	struct Cell cell[100][100];
} maze;

Cell visitedCellsList[10000];
int visitedCellIterator = 0;

Cell shortestPathCellsList[10000];

void SetColor(int r, int g, int b) {
	glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
}

void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

void RandomizeArray(int *arr, int n) {
	int i, j;
	for (i = n - 1; i > 0; i--) {
		j = rand() % (i + 1);
		swap(&arr[i], &arr[j]);
	}
}


void Text(char *string, float x, float y, float z)
{
	char *c;
	glRasterPos3f(x, y, z);

	for (c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	}
}

void DrawMaze(float x, float y, float size, int cellNoX, int cellNoY) {
	//Draw filled rectangle if not visited
	SetColor(128, 128, 128);//grey
	if (maze.cell[cellNoX][cellNoY].visited == false) {
		glBegin(GL_POLYGON);
		glVertex2f(x - size / 2, y - size / 2);
		glVertex2f(x + size / 2, y - size / 2);
		glVertex2f(x + size / 2, y + size / 2);
		glVertex2f(x - size / 2, y + size / 2);
		glEnd();
	}

	//Draw filled rectangle if visited
	SetColor(240, 150, 190);//pink
	if (maze.cell[cellNoX][cellNoY].visited) {
		glBegin(GL_POLYGON);
		glVertex2f(x - size / 2, y - size / 2);
		glVertex2f(x + size / 2, y - size / 2);
		glVertex2f(x + size / 2, y + size / 2);
		glVertex2f(x - size / 2, y + size / 2);
		glEnd();
	}

	SetColor(90, 200, 120);//green
	if (cellNoX == currentCellX && cellNoY == currentCellY) {
		glBegin(GL_POLYGON);
		glVertex2f(x - size / 2, y - size / 2);
		glVertex2f(x + size / 2, y - size / 2);
		glVertex2f(x + size / 2, y + size / 2);
		glVertex2f(x - size / 2, y + size / 2);
		glEnd();
	}

	SetColor(255, 255, 255);//white
	if (maze.cell[cellNoX][cellNoY].alreadyVisited) {
		glBegin(GL_POLYGON);
		glVertex2f(x - size / 2, y - size / 2);
		glVertex2f(x + size / 2, y - size / 2);
		glVertex2f(x + size / 2, y + size / 2);
		glVertex2f(x - size / 2, y + size / 2);
		glEnd();
	}

	SetColor(0, 0, 0);
	//Left Wall
	if (maze.cell[cellNoX][cellNoY].wall[LEFT]) {
		glBegin(GL_LINES);
		glVertex2f(x - size / 2, y - size / 2);
		glVertex2f(x - size / 2, y + size / 2);
		glEnd();
	}

	//Right Wall
	if (maze.cell[cellNoX][cellNoY].wall[RIGHT]) {
		glBegin(GL_LINES);
		glVertex2f(x + size / 2, y - size / 2);
		glVertex2f(x + size / 2, y + size / 2);
		glEnd();
	}

	//Top Wall
	if (maze.cell[cellNoX][cellNoY].wall[TOP]) {
		glBegin(GL_LINES);
		glVertex2f(x - size / 2, y - size / 2);
		glVertex2f(x + size / 2, y - size / 2);
		glEnd();
	}
	//Bottom Wall
	if (maze.cell[cellNoX][cellNoY].wall[BOTTOM]) {
		glBegin(GL_LINES);
		glVertex2f(x - size / 2, y + size / 2);
		glVertex2f(x + size / 2, y + size / 2);
		glEnd();
	}

	SetColor(255,255,0);//Yellow
	if (maze.cell[cellNoX][cellNoY].colorvisited == true)
	{
		glBegin(GL_POLYGON);
		glVertex2f(x - size / 2, y - size / 2);
		glVertex2f(x + size / 2, y - size / 2);
		glVertex2f(x + size / 2, y + size / 2);
		glVertex2f(x - size / 2, y + size / 2);
		glEnd();
	}
	SetColor(135,206,250);//blue
	if (maze.cell[cellNoX][cellNoY].shortestPath&&maze.cell[cellNoX][cellNoY].current&&Switch) {
		glBegin(GL_POLYGON);
		glVertex2f(x - size / 2, y - size / 2);
		glVertex2f(x + size / 2, y - size / 2);
		glVertex2f(x + size / 2, y + size / 2);
		glVertex2f(x - size / 2, y + size / 2);
		glEnd();
	}
	SetColor(255, 215, 0);//Gold for destination
	if (cellNoX == MAZESIZE - 1 && cellNoY == MAZESIZE - 1) {
		glBegin(GL_POLYGON);
		glVertex2f(x - size / 2, y - size / 2);
		glVertex2f(x + size / 2, y - size / 2);
		glVertex2f(x + size / 2, y + size / 2);
		glVertex2f(x - size / 2, y + size / 2);
		glEnd();
	}
}

void mazeSolver(int cost[][10000], int source, int destination)
{
	//printf("mazeSolver Called\n");
	int dist[10000], prev[10000], selected[10000], i, j, nextCell, min, start, d, temp, end;
	int tempCellX, tempCellY;
	int path[10000];
	for (i = 0; i< MAZESIZE*MAZESIZE; i++)
	{
		dist[i] = MYINFINITE;
		prev[i] = -1;
		selected[i] = 0;
	}
	start = source;
	/*for(i=0;i<10000;i++)
        selected[i]=0;*/
	selected[start] = 1;
	dist[start] = 0;
	//printf("\nInside mazeSolver\n");
		while (selected[destination] == 0)
		{
			min = MYINFINITE;
			nextCell = 0;
			for (i = 1; i < MAZESIZE*MAZESIZE; i++)
			{
				d = dist[start] + cost[start][i];
				if (d < dist[i] && selected[i] == 0)
				{
					dist[i] = d;
					prev[i] = start;

				}
				if (min > dist[i] && selected[i] == 0)
				{
					min = dist[i];
					nextCell = i;
				}
			}
			start = nextCell;
			selected[start] = 1;
			//printf("\nStill inside\n");
		}

	start = destination;
	shortestPathIterator = 0;

	for (i = 0; i < MAZESIZE; i++)
	{
		for (j = 0; j < MAZESIZE; j++)
		{maze.cell[i][j].current = false;
		}
    }

	while (start != -1)
	{
		tempCellX = shortestPathCellsList[start].x;
		tempCellY = shortestPathCellsList[start].y;
		maze.cell[tempCellX][tempCellY].shortestPath = true;
		maze.cell[tempCellX][tempCellY].current = true;
		path[shortestPathIterator++] = start;
		start = prev[start];

	}
	//printf("mazeSolver Completed\n");
	//Reversing the path

	end = shortestPathIterator - 1;
	for (i = 0; i < shortestPathIterator / 2; i++) {
		temp = path[i];
		path[i] = path[end];
		path[end] = temp;
		end--;
	}

}

void AdjacencyMatrix() {
	int i, j, cellNum, k = 0;
	for (i = 0; i < MAZESIZE; i++)
	{
		for (j = 0; j < MAZESIZE; j++)
		{
			cellNum = i*MAZESIZE + j;
			shortestPathCellsList[cellNum].x = i;
			shortestPathCellsList[cellNum].y = j;
			if (maze.cell[i][j].wall[LEFT] == false)
			{
				graph[k][cellNum - MAZESIZE] = 1;
			}

			if (maze.cell[i][j].wall[RIGHT] == false)
			{
				graph[k][cellNum + MAZESIZE] = 1;
			}

			if (maze.cell[i][j].wall[TOP] == false)
			{
				graph[k][cellNum - 1] = 1;
			}

			if (maze.cell[i][j].wall[BOTTOM] == false)
			{
				graph[k][cellNum + 1] = 1;
			}
			k++;
		}
	}

	mazeSolver(graph, curObjX*MAZESIZE+curObjY, MAZESIZE*MAZESIZE - 1);
}

void ShortestPath(unsigned char key, int x, int y)
{
	//int i, j;
	if (key == 's' || key == 'S')
		{AdjacencyMatrix();
		 Switch=!Switch;//++;
		}
	/*for (i = 0; i < MAZESIZE; i++)
	{
		for (j = 0; j < MAZESIZE; j++)
		{
			maze.cell[i][j].shortestPath = false;
		}
	}*/
}

void GrowingTree() {
	int directionArray[] = { LEFT, RIGHT, TOP, BOTTOM };
	RandomizeArray(directionArray, 4);

	int i, tempCellX, tempCellY;
		currentCellX = visitedCellsList[visitedCellIterator - 1].x;
		currentCellY = visitedCellsList[visitedCellIterator - 1].y;

		for (i = 0; i < 4; i++) {
			currentDirection = directionArray[i];
			tempCellX = currentCellX + dx[currentDirection];
			tempCellY = currentCellY + dy[currentDirection];
			if (tempCellX >= 0 && tempCellX < MAZESIZE && tempCellY >= 0 && tempCellY < MAZESIZE &&
				maze.cell[tempCellX][tempCellY].visited == false) {

				maze.cell[currentCellX][currentCellY].wall[currentDirection] = false;
				maze.cell[tempCellX][tempCellY].wall[oppositeDirection[currentDirection]] = false;

				currentCellX = tempCellX;
				currentCellY = tempCellY;
				maze.cell[currentCellX][currentCellY].visited = true;

				visitedCellsList[visitedCellIterator] = maze.cell[currentCellX][currentCellY];
				visitedCellIterator++;

				return;
			}
		}


	visitedCellIterator--;
	maze.cell[currentCellX][currentCellY].alreadyVisited = true;
	//printf("No unvisited neighbours\n");
}

void Key(int key, int x, int y)
{
	if (curObjX == MAZESIZE - 1 && curObjY == MAZESIZE - 1)
	{
		maze.cell[curObjX][curObjY].colorvisited = true;
		//exit(0);
	}

	else
	{
		if (key == R && maze.cell[curObjX][curObjY].wall[RIGHT] == false)// && maze.cell[curObjX+1][curObjY].wall[LEFT]==false)
		{
			maze.cell[curObjX][curObjY].colorvisited = true;
			curObjX++;
			//printf("\nMoving Right");

		}
		else if (key == L && maze.cell[curObjX][curObjY].wall[LEFT] == false)
		{
			maze.cell[curObjX][curObjY].colorvisited = true;
			curObjX--;
			//printf("\nMoving Left");

		}
		else if (key == T && maze.cell[curObjX][curObjY].wall[TOP] == false)
		{
			maze.cell[curObjX][curObjY].colorvisited = true;
			curObjY--;
			//printf("\nMoving Up");

		}
		else if (key == B && maze.cell[curObjX][curObjY].wall[BOTTOM] == false)
		{
			maze.cell[curObjX][curObjY].colorvisited = true;
			curObjY++;
			//printf("\nMoving Down");

		}

	}
}


void display() {
	int i, j;
	int directionArray[] = { LEFT, RIGHT, TOP, BOTTOM };
	RandomizeArray(directionArray, 4);
	int tempCellX, tempCellY;

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	for (i = 0; i < MAZESIZE; ++i) {
		for (j = 0; j < MAZESIZE; ++j) {
			DrawMaze(340 + i * CELLSIZE + CELLSIZE, 20 + j * CELLSIZE + CELLSIZE, CELLSIZE, i, j);

		}
	}

	if (visitedCellIterator >= 0) {
		GrowingTree();
	}
	else
	{
		while (count < MAZESIZE/2)
		{
			//printf("Hello There\n");
			currentCellX = rand() % MAZESIZE;
			currentCellY = rand() % MAZESIZE;
			for (i = 0; i < 4; i++) {
				currentDirection = directionArray[i];
				tempCellX = currentCellX + dx[currentDirection];
				tempCellY = currentCellY + dy[currentDirection];
				//printf("Current cell = %d, %d\n", tempCellX, tempCellY);
				if (tempCellX >= 0 && tempCellX < MAZESIZE && tempCellY >= 0 && tempCellY < MAZESIZE &&
					maze.cell[currentCellX][currentCellY].wall[currentDirection] == true) {

					maze.cell[currentCellX][currentCellY].wall[currentDirection] = false;
					maze.cell[tempCellX][tempCellY].wall[oppositeDirection[currentDirection]] = false;
					//maze.cell[currentCellX][currentCellY].visited = true;
					count++;
					//printf("Count++\n");
					break;
				}
			}
		}
		//Initializing the Adjacency Matrix
		for (i = 0; i < MAZESIZE*MAZESIZE; i++)
		{
			for (j = 0; j < MAZESIZE*MAZESIZE; j++)
			{
				graph[i][j] = MYINFINITE;
			}
		}

	}

	Sleep(DELTA_TIME);

	SetColor(0, 0, 0);
	Text("Source ->", 250, 20+CELLSIZE, 0);
	Text("<- Destination", 340 + MAZESIZE * CELLSIZE+CELLSIZE, 20 + MAZESIZE * CELLSIZE, 0);
	Text("Use Arrow keys to play", 20, 100, 0);
	Text("Press 's' to toggle shortest path", 20, 200, 0);
	glColor3f(0, 0, 0);
	if(curObjX==MAZESIZE-1 && curObjY==MAZESIZE-1)
		Text("CONGRATULATIONS!", 900, 300, 0);

	glBegin(GL_TRIANGLES);
	glVertex2f(340 - CELLSIZE / 2 + curObjX*CELLSIZE + CELLSIZE, 20 - CELLSIZE / 2 + curObjY*CELLSIZE + CELLSIZE);
	glVertex2f(340 + CELLSIZE / 2 + curObjX*CELLSIZE + CELLSIZE, 20 - CELLSIZE / 2 + curObjY*CELLSIZE + CELLSIZE);
	glVertex2f(340 + curObjX*CELLSIZE + CELLSIZE, 20 + CELLSIZE / 2 + curObjY*CELLSIZE + CELLSIZE);
	glEnd();

	glColor3f(1, 1, 1);
	glPointSize(40/MAZESIZE);
	glBegin(GL_POINTS);
	glVertex2f(340 - CELLSIZE / 4 + curObjX*CELLSIZE + CELLSIZE, 20 - CELLSIZE / 4 + curObjY*CELLSIZE + CELLSIZE);
	glVertex2f(340 + CELLSIZE / 4 + curObjX*CELLSIZE + CELLSIZE, 20 - CELLSIZE / 4 + curObjY*CELLSIZE + CELLSIZE);
	glVertex2f(340 + curObjX*CELLSIZE + CELLSIZE, 20 + CELLSIZE / 4 + curObjY*CELLSIZE + CELLSIZE);
	glEnd();

	maze.cell[curObjX][curObjY].colorvisited = true;
	glFlush();


	glutPostRedisplay();

}

void InitMaze() {
	printf("Enter Maze Size : ");
	scanf("%d", &MAZESIZE);
	CELLSIZE = (SCREEN_HEIGHT-80) / MAZESIZE;
	//count = MAZESIZE / 2;
	int i, j, k;
	for (i = 0; i < MAZESIZE; ++i) {
		for (j = 0; j < MAZESIZE; ++j) {
			maze.cell[i][j].visited = false;
			maze.cell[i][j].alreadyVisited = false;
			maze.cell[i][j].shortestPath = false;
			maze.cell[i][j].x = i;
			maze.cell[i][j].y = j;
			for (k = 0; k < 4; k++) { maze.cell[i][j].wall[k] = true; }
		}
	}

	currentCellX = rand() % MAZESIZE;
	currentCellY = rand() % MAZESIZE;
	maze.cell[currentCellX][currentCellY].visited = true;

	visitedCellsList[visitedCellIterator] = maze.cell[currentCellX][currentCellY];
	visitedCellIterator++;
	sourceCellY = 0;
	destCellY = MAZESIZE - 1;

	sourceCellX = 0;
	destCellX = MAZESIZE - 1;
}


int main(int argc, char **argv) {
	glutInit(&argc, argv);
	srand(time(NULL));
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("Maze");

	glutDisplayFunc(display);
	glutSpecialFunc(Key);
	glutKeyboardFunc(ShortestPath);
	gluOrtho2D(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	glLineWidth(5);
	InitMaze();

	glutMainLoop();

	return 0;
}
