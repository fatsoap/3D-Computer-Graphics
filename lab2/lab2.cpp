#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
using namespace std;

int window_id = -1;
bool pause = false;
int cmd_top = 0;
int square_amount = 0, triangle_amount = 0;
vector<string> cmds;

float matrix[3][3] = {
    { 1, 0, 0 },
    { 0 ,1 ,0 },
    { 0, 0, 1 }
};

struct data {
	float m[3][3];
    int type;
};

struct polygon {
    vector<pair<float, float> >points;
    int color; // white or yello
};

vector<data> datas;
vector<polygon> screen;

void parseCommand();
void display();
void handleSquare();
void handleTriangle();
void handleScale(float x, float y);
void handleRotate(float d);
void handleTranslate(float x, float y);
void handleComment(string comment);
void handleReset();
void handleClearData();
void handleClearScreen();
void handleEnd();
void keyboardFunc(unsigned char key, int x, int y);
void matrixCalculate(float m[3][3], float t[3][3]);
void pointCalculate(float m[3][3], float v[3]);
void printMatrix(float m[3][3]);
vector<pair<float, float> > polygon_clip(vector<pair<float, float> > points, float l, float r, float b, float t);

void display() {
    if (pause) {
        system("read -p 'Press Enter to continue...' var");
    }
    parseCommand();
    // set background color
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
    glLineWidth(1);
    for (int i = 0; i < screen.size(); i++) {
        if (screen[i].color == 1) {
            glColor3f(0.0f, 1.0f, 1.0f);  // yellow
        } else {
            glColor3f(1.0f, 1.0f, 1.0f);  // white
        }
        glBegin(GL_LINE_LOOP);
        for(int j=0; j<screen[i].points.size(); j++) {
            glVertex2i(screen[i].points[j].first, screen[i].points[j].second);
        }
        glEnd();  
    }
    glutSwapBuffers();
    glutPostRedisplay();
}

/* Calculate 3X3 matrix */
void matrixCalculate(float m[3][3], float t[3][3]) {
    float tmp[3][3];
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            float product = 0;
            for(int k=0; k<3; k++) {
                product += t[i][k] * m[k][j];
            }
            tmp[i][j] = product;
        }
    }
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            m[i][j] = tmp[i][j];
        }
    }
}

/* Calculate point vector with matrix */
void pointCalculate(float m[3][3], float v[3]) {
    float tmp[3];
    for(int i=0; i<3; i++) {
        float product = 0;
        for(int k=0; k<3; k++) {
            product += m[i][k] * v[k];
        }
        tmp[i] = product;
    }
    for(int i=0; i<3; i++) {
        v[i] = tmp[i];
    }
}

void printMatrix(float m[3][3]) {
    for(int i=0; i<3; i++) {
        cout<<"[ ";
        for(int j=0; j<3; j++) {
            cout<<m[i][j]<<" ";
        }
        cout<<"]\n";
    }
}

void handleSquare() {
    struct data square;
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            square.m[i][j] = matrix[i][j];
        }
    }
    square.type = 1;
    datas.push_back(square);
    cout<<"You have "<<++square_amount<<" square.\n";
}

void handleTriangle() {
    struct data triangle;
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            triangle.m[i][j] = matrix[i][j];
        }
    }
    triangle.type = 2;
    datas.push_back(triangle);
    cout<<"You have "<<++triangle_amount<<" triangle.\n";
}

void handleScale(float x, float y) {
    float sc[3][3] = {
        {x, 0, 0},
        {0, y, 0},
        {0, 0, 1}
    };
    cout<<"Scale\n";
    matrixCalculate(matrix, sc);
    printMatrix(matrix);
}

void handleRotate(float d) {
    float radius = (d * 2 * M_PI) / 360;
    float rm[3][3] = {
        {cos(radius), -sin(radius), 0},
        {sin(radius), cos(radius), 0},
        {0, 0, 1}
    };
    cout<<"Rotate\n";
    matrixCalculate(matrix, rm);
    printMatrix(matrix);
}

void handleTranslate(float x, float y) {
    float tr[3][3] = {
        {1, 0, x},
        {0, 1, y},
        {0, 0, 1}
    };
    cout<<"Translate\n";
    matrixCalculate(matrix, tr);
    printMatrix(matrix);
}

void handleView(float wxl, float wxr, float wyb, float wyt, float vxl, float vxr, float vyb, float vyt) {
    float sx = (vxr - vxl) / (wxr - wxl);
    float sy = (vyt - vyb) / (wyt - wyb);
    float tx = ((wxr * vxl) - (wxl * vxr)) /  (wxr - wxl);
    float ty = ((wyt * vyb) - (wyb * vyt)) /  (wyt - wyb);
    float t[3][3] = {
        {sx, 0, tx},
        {0, sy, ty},
        {0, 0, 1}
    };
    cout<<"View\n";
    cout<<"WVM\n";
    printMatrix(t);
    {
        vector<pair<float, float> > points;
        points.push_back(make_pair(vxl, vyb));
        points.push_back(make_pair(vxr, vyb));
        points.push_back(make_pair(vxr, vyt));
        points.push_back(make_pair(vxl, vyt));
        polygon p;
        p.points = points;
        p.color = 2;
        screen.push_back(p);
    }
    for (int i=0; i<datas.size(); i++) {
        data new_data = datas[i];
        matrixCalculate(new_data.m, t);
        vector<pair<float, float> > points;
        if (new_data.type == 1) {
            float tr[3] = {1 , 1, 1};
            float tl[3] = {-1 , 1, 1};
            float br[3] = {1 , -1, 1};
            float bl[3] = {-1 , -1, 1};
            pointCalculate(new_data.m, tr);
            pointCalculate(new_data.m, tl);
            pointCalculate(new_data.m, br);
            pointCalculate(new_data.m, bl);
            points.push_back(make_pair(tr[0], tr[1]));
            points.push_back(make_pair(br[0], br[1]));
            points.push_back(make_pair(bl[0], bl[1]));
            points.push_back(make_pair(tl[0], tl[1]));   
        } else {
            float t[3] = {0 , 1 , 1};
            float l[3] = {-1 , -1 , 1};
            float r[3] = {1 , -1 , 1};
            pointCalculate(new_data.m, t);
            pointCalculate(new_data.m, l);
            pointCalculate(new_data.m, r);
            points.push_back(make_pair(t[0], t[1]));
            points.push_back(make_pair(r[0], r[1]));
            points.push_back(make_pair(l[0], l[1]));
        }
        // Clip polygon
        polygon p;
        p.points = polygon_clip(points, vxl,  vxr,  vyb,  vyt);
        p.color = 1;
        screen.push_back(p);
    }
}

/* get cross point with two point */
pair<float, float> getMid(pair<float, float> pa, pair<float, float> pb, pair<float, float> bd) {
    if (bd.second == 1) { // x
        if (pa.second == pb.second) {
            return make_pair(bd.first, pa.second);
        }
        if (pa.first == bd.first) {
            return pa;
        }
        if (pb.first == bd.first) {
            return pb;
        }
        float dy = (bd.first - pa.first) * ((pb.second - pa.second) / (pb.first - pa.first));
        return make_pair(bd.first, pa.second + dy);
    } else { // y
        if (pa.first == pb.first) {
            return make_pair(pa.first, bd.first);
        }
        if (pa.second == bd.first) {
            return pa;
        }
        if (pb.second == bd.first) {
            return pb;
        }
        float dx = (bd.first - pa.second) * ((pb.first - pa.first) / (pb.second - pa.second));
        return make_pair(pa.first + dx, bd.first);
    }
}

/* Clip */
vector<pair<float, float> > polygon_clip(vector<pair<float, float> > points, float l, float r, float b, float t) {
    vector<pair<float, float> > queue = points, tmp;

    {
    // Left Clip
    int len = queue.size();
    for (int i=0; i<len; i++) {
        if (queue[i].first < l && queue[((i+1)%len)].first < l) {
            // do nothing
        } else if (queue[i].first < l && queue[((i+1)%len)].first >= l) {
            // get mid & ((i+1)%len)
            pair<float, float> mid = getMid(queue[i], queue[((i+1)%len)], make_pair(l, 1));
            tmp.push_back(mid);
            tmp.push_back(queue[((i+1)%len)]);
        } else if (queue[i].first >= l && queue[((i+1)%len)].first < l) {
            // get mid
            pair<float, float> mid = getMid(queue[i], queue[((i+1)%len)], make_pair(l, 1));
            tmp.push_back(mid);
        } else if (queue[i].first >= l && queue[((i+1)%len)].first >= l) {
            // get ((i+1)%len)
            tmp.push_back(queue[((i+1)%len)]);
        }
    }
    queue = tmp;
    tmp.clear();
    }
    {
    // Bottom Clip
    int len = queue.size();
    for (int i=0; i<len; i++) {
        if (queue[i].second < b && queue[((i+1)%len)].second < b) {
            // do nothing
        } else if (queue[i].second < b && queue[((i+1)%len)].second >= b) {
            // get mid & ((i+1)%len)
            pair<float, float> mid = getMid(queue[i], queue[((i+1)%len)], make_pair(b, 2));
            tmp.push_back(mid);
            tmp.push_back(queue[((i+1)%len)]);
        } else if (queue[i].second >= b && queue[((i+1)%len)].second < b) {
            // get mid
            pair<float, float> mid = getMid(queue[i], queue[((i+1)%len)], make_pair(b, 2));
            tmp.push_back(mid);
        } else if (queue[i].second >= b && queue[((i+1)%len)].second >= b) {
            // get ((i+1)%len)
            tmp.push_back(queue[((i+1)%len)]);
        }
    }
    queue = tmp;
    tmp.clear();
    }
    {
    // Right Clip
    int len = queue.size();
    for (int i=0; i<len; i++) {
        if (queue[i].first > r && queue[((i+1)%len)].first > r) {
            // do nothing
        } else if (queue[i].first > r && queue[((i+1)%len)].first <= r) {
            // get mid & ((i+1)%len)
            pair<float, float> mid = getMid(queue[i], queue[((i+1)%len)], make_pair(r, 1));
            tmp.push_back(mid);
            tmp.push_back(queue[((i+1)%len)]);
        } else if (queue[i].first <= r && queue[((i+1)%len)].first > r) {
            // get mid
            pair<float, float> mid = getMid(queue[i], queue[((i+1)%len)], make_pair(r, 1));
            tmp.push_back(mid);
        } else if (queue[i].first <= r && queue[((i+1)%len)].first <= r) {
            // get ((i+1)%len)
            tmp.push_back(queue[((i+1)%len)]);
        }
    }
    queue = tmp;
    tmp.clear();
    }
    {
    // Top Clip
    int len = queue.size();
    for (int i=0; i<len; i++) {
        if (queue[i].second > t && queue[((i+1)%len)].second > t) {
            // do nothing
        } else if (queue[i].second > t && queue[((i+1)%len)].second <= t) {
            // get mid & ((i+1)%len)
            pair<float, float> mid = getMid(queue[i], queue[((i+1)%len)], make_pair(t, 2));
            tmp.push_back(mid);
            tmp.push_back(queue[((i+1)%len)]);
        } else if (queue[i].second <= t && queue[((i+1)%len)].second > t) {
            // get mid
            pair<float, float> mid = getMid(queue[i], queue[((i+1)%len)], make_pair(t, 2));
            tmp.push_back(mid);
        } else if (queue[i].second <= t && queue[((i+1)%len)].second <= t) {
            // get ((i+1)%len)
            tmp.push_back(queue[((i+1)%len)]);
        }
    }
    queue = tmp;
    tmp.clear();
    }

    return queue;
}

void handleReset() {
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            matrix[i][j] = i == j ? 1 : 0;
        }
    }
}

void handleClearData() {
    square_amount = 0, triangle_amount = 0;
	datas.clear();
}

void handleClearScreen() {
    screen.clear();
}

void handleEnd() {
    if (window_id != -1) {
        glutDestroyWindow(window_id);
        exit(0);
    }	
}

void handleComment(string comment) {
    cout<<"##"<<comment<<" ###\n";
}

void keyboardFunc(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
		if (window_id != -1) {
			glutDestroyWindow(window_id);
			exit(0);
		}		
		break;
	}
}

void parseCommand() {
    if ( cmd_top >= cmds.size() ) {
        return;
    }
    string line = cmds[cmd_top++];
    istringstream iss(line);
    string cmd;
    if(iss >> cmd) {}
    float param;
    vector<float> params;
    while(iss >> param) {
        params.push_back(param);
    }
    pause = false;
    if (cmd == "scale") {
        handleScale(params[0], params[1]);
    } else if (cmd == "rotate") {
        handleRotate(params[0]);
    } else if (cmd == "translate") {
        handleTranslate(params[0], params[1]);
    } else if (cmd == "square") {
        handleSquare();
    } else if (cmd == "triangle") {
        handleTriangle();
    } else if (cmd == "view") {
        handleView(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7]);
        pause = true;
    } else if (cmd == "clearData") {
        handleClearData();
    } else if (cmd == "clearScreen") {
        handleClearScreen();
    } else if (cmd == "reset") {
        handleReset();
    } else if (cmd == "end") {
        handleEnd();
    } else if (cmd == "#") {
        handleComment(line);
    } else {} // do nothing
}

int main(int argc, char** argv) {
    // TODO replace to system("pause")
    system("read -p 'Press Enter to continue...' var");
    if (argc != 2) {
        return 0;
    }

    // read file
    string line;
    ifstream input_file(argv[1]);
    while(getline(input_file, line)) {
        cmds.push_back(line);
    }

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	window_id = glutCreateWindow("107502570 lab2");

    // set bg color
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunc);
	gluOrtho2D(-1, 800, -1, 800);
	glutMainLoop();
    
}
