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
bool pause = false, backfaces = false;
int cmd_top = 0;
int WINDOW_HEIGHT = 0, WINDOW_WIDTH = 0;
float Hither = 0, Yon = 0, Hav = 0;
float AR, VL, VR, VB, VT;
vector<string> cmds;

float transformation_matrix[4][4] = {
    { 1, 0, 0, 0 },
    { 0 ,1 ,0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
};

float eye_matrix[4][4] = {
    { 1, 0, 0, 0 },
    { 0 ,1 ,0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
};

float mirror_matrix[4][4] = {
    { -1, 0, 0, 0 },
    { 0 ,1 ,0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
};

float projection_matrix[4][4] = {
    { 1, 0, 0, 0 },
    { 0 ,1 ,0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
};

float window_viewport_matrix[4][4] = {
    { 1, 0, 0, 0 },
    { 0 ,1 ,0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
};


struct object {
    vector<vector<float> > points;
    vector<vector<float> > new_points;
    vector<vector<int> > polygons;
    float tm[4][4];
};

vector<object> objects;

vector<vector<pair<float, float> > > screen;

void parseCommand();
void display();
void handleObject(string obj_name);
void handleObserver(float ex, float ey, float ez, float cx, float cy, float cz, float tilt, float hither, float yon , float hav);
void handleScale(float x, float y, float z);
void handleRotate(float x, float y, float z);
void handleTranslate(float x, float y, float z);
void handleComment(string comment);
void handleReset();
void handleDisplay();
void handleNoBackfaces();
void handleEnd();
void handlePM();
void handleWM();
void handleView(float vxl, float vxr, float vyb, float vyt);
void initMatrix(float m[4][4]);
void matrixCalculate(float m[4][4], float t[4][4]);
void pointCalculate(float m[4][4], float v[4]);
void printMatrix(float m[4][4]);
vector<float> cross(vector<float> a, vector<float> b);
void normalized(vector<float> &v);
vector<pair<float, float> > polygon_clip(vector<pair<float, float> > points, float l, float r, float b, float t);

void display() {
    if (pause) {
        system("read -p 'Press Enter to continue...' var");
    }
    parseCommand();
    // set bg color
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
    glLineWidth(1);
    glColor3f(1.0, 1.0, 1.0);  

    // Draw View Window
    glBegin(GL_LINE_LOOP);
    float vxl = (1+VL) * WINDOW_WIDTH / 2, vxr = (1+VR) * WINDOW_WIDTH / 2, vyb = (1+VB) * WINDOW_HEIGHT / 2, vyt = (1+VT) * WINDOW_HEIGHT / 2;
    glVertex2i(vxl, vyb);
    glVertex2i(vxl, vyt);
    glVertex2i(vxr, vyt);
    glVertex2i(vxr, vyb);
    glEnd();

    for (int i = 0; i < screen.size(); i++) {    
        glBegin(GL_LINE_LOOP);
        vector<pair<float, float> > clip_polygon = polygon_clip(screen[i], vxl, vxr, vyb, vyt);
        for(int j=0; j<clip_polygon.size(); j++) {
            glVertex2i(clip_polygon[j].first, clip_polygon[j].second);
        }
        glEnd();  
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

void initMatrix(float m[4][4]) {
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            m[i][j] = i == j ? 1 : 0;
        }
    }
}

void matrixCalculate(float m[4][4], float t[4][4]) {
    float tmp[4][4];
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            float product = 0;
            for(int k=0; k<4; k++) {
                product += t[i][k] * m[k][j];
            }
            tmp[i][j] = product;
        }
    }
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            m[i][j] = tmp[i][j];
        }
    }
}

void pointCalculate(float m[4][4], float v[]) {
    float tmp[4];
    for(int i=0; i<4; i++) {
        float product = 0;
        for(int k=0; k<4; k++) {
            product += m[i][k] * v[k];
        }
        tmp[i] = product;
    }
    for(int i=0; i<4; i++) {
        v[i] = tmp[i];
    }
}

vector<float> cross(vector<float> a, vector<float> b) {
    vector<float> c;
    c.push_back(a[1] * b[2] - b[1] * a[2]);
    c.push_back(-(a[0] * b[2] - b[0] * a[2]));
    c.push_back(a[0] * b[1] - b[0] * a[1]);
    return c;
}

void normalized(vector<float> &v) {
    float l = 0;
    for(int i=0; i<v.size(); i++){ 
        l += v[i] * v[i];
    }
    l = sqrt(l);
    for(int i=0; i<v.size(); i++){ 
        v[i] /= l;
    }
}

void printMatrix(float m[4][4]) {
    for(int i=0; i<4; i++) {
        cout<<"[ ";
        for(int j=0; j<4; j++) {
            cout<<m[i][j]<<" ";
        }
        cout<<"]\n";
    }
}

void handleObject(string obj_name) {
    string line;
    ifstream input_file("./" + obj_name);
    struct object new_object;

    // Skip Empty Line
    while (getline(input_file, line) && line.size() < 2) {}

    // Parse Point & Face amount
    int point_amount, polygon_amount;
    {
        istringstream iss(line);
        iss >> point_amount;
        iss >> polygon_amount;
    }

    // Parse Points
    for(int i=0; i<point_amount; i++) {
        vector<float> p;
        float tmp;
        getline(input_file, line);
        istringstream iss(line);
        for(int j=0; j<3; j++) {
            iss >> tmp;
            p.push_back(tmp);
        }
        new_object.points.push_back(p);
        new_object.new_points.push_back(p);
    }

    // Parse Faces
    for(int i=0; i<polygon_amount; i++) {
        vector<int> py;
        getline(input_file, line);
        istringstream iss(line);
        int num, tmp;
        if (iss >> num) {
            vector<int> p;
            for(int j=0; j<num; j++) {
                iss >> tmp;
                p.push_back(tmp);
            }
            new_object.polygons.push_back(p);
        }
    }

    // Save TM
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            new_object.tm[i][j] = transformation_matrix[i][j];
        }
    }

    objects.push_back(new_object);
}

void handleObserver(float ex, float ey, float ez, float cx, float cy, float cz, float tilt, float hither, float yon , float hav) {
    float grm[4][4], trans[4][4], tilt_m[4][4];
    // Init
    initMatrix(eye_matrix);
    initMatrix(grm);
    initMatrix(trans);
    initMatrix(tilt_m);

    {   // T (-x, -y, -z)
        trans[0][3] = -ex;
        trans[1][3] = -ey;
        trans[2][3] = -ez;
    }
    {   // GRM
        float x = cx - ex, y = cy - ey, z = cz - ez;
        float l = sqrt(x * x + y * y + z * z);

        vector<float> v3, t;
        v3.push_back(x), v3.push_back(y), v3.push_back(z), v3.push_back(0);
        t.push_back(0), t.push_back(1), t.push_back(0), t.push_back(0);
        vector<float> v1 = cross(t, v3);
        vector<float> v2 = cross(v3, v1);
        normalized(v1);
        normalized(v2);
        normalized(v3);
        
        grm[0][0]=v1[0];    grm[0][1]=v1[1];    grm[0][2]=v1[2];
        grm[1][0]=v2[0];    grm[1][1]=v2[1];    grm[1][2]=v2[2];
        grm[2][0]=v3[0];    grm[2][1]=v3[1];    grm[2][2]=v3[2];

        // Print GRM
        cout<<"GRM\n";
        printMatrix(grm);
    }
    {   // Tilt Martix
        float tilt_z = (-tilt * 2 * M_PI / 360.0);
        tilt_m[0][0] = cos(tilt_z), tilt_m[0][1] = -sin(tilt_z);
        tilt_m[1][0] = sin(tilt_z), tilt_m[1][1] = cos(tilt_z);

        // Print Tilt Matrix
        cout<<"Tilt\n";
        printMatrix(tilt_m);
    }

    // Calculate EM
    matrixCalculate(eye_matrix, trans);
    matrixCalculate(eye_matrix, grm);
    matrixCalculate(eye_matrix, mirror_matrix);
    matrixCalculate(eye_matrix, tilt_m);

    // Print EM
    cout<<"EM\n";
    printMatrix(eye_matrix);

    // Save Params For PM
    Hither = hither, Yon = yon, Hav = hav;
}

void handleScale(float x, float y, float z) {
    float sc[4][4] = {
        {x, 0, 0, 0},
        {0, y, 0, 0},
        {0, 0, z, 0},
        {0, 0, 0, 1}
    };
    // Scaling
    matrixCalculate(transformation_matrix, sc);

    // Print Result Matrix
    cout<<"Scale\n";
    printMatrix(transformation_matrix);
}

void handleRotate(float x, float y, float z) {
    float radius_x = (x * 2 * M_PI) / 360;
    float radius_y = (y * 2 * M_PI) / 360;
    float radius_z = (z * 2 * M_PI) / 360;
    float rotate_matrix_z[4][4] = {
        {cos(radius_z), -sin(radius_z), 0, 0},
        {sin(radius_z), cos(radius_z), 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
    float rotate_matrix_x[4][4] = {
        {1, 0, 0, 0},
        {1, cos(radius_x), -sin(radius_x), 0},
        {1, sin(radius_x), cos(radius_x), 0},
        {0, 0, 0, 1}
    };
    float rotate_matrix_y[4][4] = {
        {cos(radius_y), 0, sin(radius_y), 0},
        {0, 1, 0, 0},
        {-sin(radius_y), 0, cos(radius_y), 0},
        {0, 0, 0, 1}
    };
    
    // Rotate Z-axis
    if (z) matrixCalculate(transformation_matrix, rotate_matrix_z);
    // Rotate X-axis
    if (x) matrixCalculate(transformation_matrix, rotate_matrix_x);
    // Rotate Y-axis
    if (y) matrixCalculate(transformation_matrix, rotate_matrix_y);

    // Print Result Martix
    cout<<"Rotate\n";
    printMatrix(transformation_matrix);
}

void handleTranslate(float x, float y, float z) {
    float translate_matrix[4][4] = {
        {1, 0, 0, x},
        {0, 1, 0, y},
        {0, 0, 1, z},
        {0, 0, 0, 1}
    };
    // Translating
    matrixCalculate(transformation_matrix, translate_matrix);

    // Print Result Matrix
    cout<<"Translate\n";
    printMatrix(transformation_matrix);
}

void handleView(float vxl, float vxr, float vyb, float vyt) {
    AR = (vxr - vxl) / (vyt - vyb), VL = vxl, VR = vxr, VB = vyb, VT = vyt;
}

void handlePM() {
    initMatrix(projection_matrix);
    float hav = Hav * 2 * M_PI / 360.0;
    float yon = Yon;
    float hither = Hither;
    float ar = AR;
    projection_matrix[1][1] = ar;
    projection_matrix[2][2] = yon / (yon - hither) * tan(hav);
    projection_matrix[2][3] = yon * hither / (hither - yon ) * tan(hav);
    projection_matrix[3][2] = tan(hav);
    projection_matrix[3][3] = 0;

    cout<<"PM\n";
    printMatrix(projection_matrix);
}

void handleWM() {
    initMatrix(window_viewport_matrix);

    float vxl = (1+VL) * WINDOW_WIDTH / 2, vxr = (1+VR) * WINDOW_WIDTH / 2, vyb = (1+VB) * WINDOW_HEIGHT / 2, vyt = (1+VT) * WINDOW_HEIGHT / 2;

    float t1[4][4] = {
        {1, 0, 0, 1},
        {0, 1, 0, 1},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
    float sc[4][4] = {
        {(vxr - vxl) / 2.0, 0, 0, 0},
        {0, (vyt - vyb) / 2.0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 1}
    };
    float t2[4][4] = {
        {1, 0, 0, vxl},
        {0, 1, 0, vyb},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    matrixCalculate(window_viewport_matrix, t1);
    matrixCalculate(window_viewport_matrix, sc);
    matrixCalculate(window_viewport_matrix, t2);

    cout<<"WVM\n";
    printMatrix(window_viewport_matrix);
}

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
    initMatrix(transformation_matrix);
}

void handleDisplay() {

    cout<< "Display\n";
    screen.clear();
    // PM calculate
    handlePM();
    // WM calculate
    handleWM();

    for(int k=0; k<objects.size(); k++) {
        for(int i=0; i<objects[k].points.size(); i++) {
            float tmp[4] = { objects[k].points[i][0], objects[k].points[i][1], objects[k].points[i][2], 1 };
            pointCalculate(objects[k].tm, tmp);
            pointCalculate(eye_matrix, tmp);
            pointCalculate(projection_matrix, tmp);
            pointCalculate(window_viewport_matrix, tmp);

            // perspective division
            tmp[0] /= tmp[3];
            tmp[1] /= tmp[3];
            tmp[2] /= tmp[3];
            tmp[3] /= tmp[3];
            
            objects[k].new_points[i][0] = tmp[0];
            objects[k].new_points[i][1] = tmp[1];
            objects[k].new_points[i][2] = tmp[2];
        }
        for(int i=0; i<objects[k].polygons.size(); i++) {
            // Backfaces
            if (backfaces) {
                vector<float> av, bv;
                for(int j=0; j<4; j++) {
                    av.push_back(objects[k].new_points[objects[k].polygons[i][1]-1][j] - objects[k].new_points[objects[k].polygons[i][0]-1][j]);
                    bv.push_back(objects[k].new_points[objects[k].polygons[i][2]-1][j] - objects[k].new_points[objects[k].polygons[i][1]-1][j]);
                } 
                vector<float> bk = cross(av, bv);
                if(bk[2] >= 0) {
                    continue;
                }
            }

            vector<pair<float, float> > p;
            for(int j=0; j<objects[k].polygons[i].size(); j++) {
                p.push_back(make_pair(objects[k].new_points[objects[k].polygons[i][j]-1][0], objects[k].new_points[objects[k].polygons[i][j]-1][1]));
            }

            screen.push_back(p);
        }
    }
}

void handleNoBackfaces() {
    backfaces = true;
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
        handleScale(params[0], params[1], params[2]);
    } else if (cmd == "rotate") {
        handleRotate(params[0], params[1], params[2]);
    } else if (cmd == "translate") {
        handleTranslate(params[0], params[1], params[2]);
    } else if (cmd == "object") {
        handleObject(line.substr(7, line.find(".asc") - 7 + 4));
    } else if (cmd == "observer") {
        handleObserver(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]);
    } else if (cmd == "viewport") {
        handleView(params[0], params[1], params[2], params[3]);
    } else if (cmd == "display") {
        handleDisplay();
        pause = true;
    } else if (cmd == "nobackfaces") {
        handleNoBackfaces();
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

    if (getline(input_file, line)) {
        // get window Height & Width
        istringstream iss(line);
        iss >> WINDOW_WIDTH >> WINDOW_HEIGHT;
    }


    while(getline(input_file, line)) {
        cmds.push_back(line);
    }

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(100, 100);
	window_id = glutCreateWindow("107502570 Lab3");

    // set bg color
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glutDisplayFunc(display);
	gluOrtho2D(-1, WINDOW_WIDTH, -1, WINDOW_HEIGHT);
	glutMainLoop();
    
}
