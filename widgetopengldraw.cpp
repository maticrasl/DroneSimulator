#include "widgetopengldraw.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>
#include <QOpenGLFunctions_3_3_Core>
#include <QApplication>
#include <cstdlib>
#include <time.h>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
//#include <QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>

struct Vertice {
    glm::vec3 coords;
    glm::vec2 texture;
    glm::vec3 normal;
};

struct Triangle {
    Vertice V[3];
};

//struct Object O;
//std::vector<Object> allObjects;

unsigned int WidgetOpenGLDraw::getObjectCount() {
    return objectCount;
}

void WidgetOpenGLDraw::setSelectedObject(int index) {
    selectedObject = static_cast<unsigned int>(index);
}

glm::vec3 WidgetOpenGLDraw::getObjectColor() {
    glm::vec3 c;
    c[0] = allObjects[selectedObject].color[0];
    c[1] = allObjects[selectedObject].color[1];
    c[2] = allObjects[selectedObject].color[2];
    return c;
}

void WidgetOpenGLDraw::setObjectColor(glm::vec3 color) {
    makeCurrent();
    allObjects[selectedObject].color[0] = color[0];
    allObjects[selectedObject].color[1] = color[1];
    allObjects[selectedObject].color[2] = color[2];
    update();
}

void WidgetOpenGLDraw::setObjectShining(int s) {
    makeCurrent();
    allObjects[selectedObject].shining = s;
    update();
}

int WidgetOpenGLDraw::getObjectShining() {
    return allObjects[selectedObject].shining;
}

void WidgetOpenGLDraw::loadTexture(QString path) {
    QImage t;
    t.load(path);
    t=t.convertToFormat(QImage::Format_ARGB32);
    //t=t.scaled(10,10);

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glGenTextures(1, &allObjects.back().textureID);
    gl->glBindTexture(GL_TEXTURE_2D, allObjects.back().textureID);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t.width(), t.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, t.bits());
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void WidgetOpenGLDraw::setLightPower(float p) {
    makeCurrent();
    lightPower = p;
    update();
}

void WidgetOpenGLDraw::keyPressEvent(QKeyEvent *event) {
    //makeCurrent();

    float temp;
    glm::vec4 tocka;
    glm::mat4 V = glm::mat4(1);
    switch(event->key()) {
    // ZOOM
    case Qt::Key_Plus:
        FOV -= 1.0f;
        temp = FOV;
        break;
    case Qt::Key_Minus:
        FOV += 1.0f;
        break;

    // ROTACIJA KAMERE
    case Qt::Key_T:
        camera[STATIC_CAM].rot[X] -= 1.0f;
        break;
    case Qt::Key_G:
        camera[STATIC_CAM].rot[X] += 1.0f;
        break;
    case Qt::Key_F:
        camera[STATIC_CAM].rot[Y] -= 1.0f;
        break;
    case Qt::Key_H:
        camera[STATIC_CAM].rot[Y] += 1.0f;
        break;

    // PREMIK KAMERE PO SCENI
    case Qt::Key_W:
        V = glm::rotate(V, glm::radians(-camera[STATIC_CAM].rot[Y]), glm::vec3(0, 1, 0));
        V = glm::translate(V, glm::vec3(0, 0, 0.1f));
        V = glm::rotate(V, glm::radians(camera[STATIC_CAM].rot[Y]), glm::vec3(0, 1, 0));
        tocka = V * glm::vec4(camera[STATIC_CAM].pos[X], camera[STATIC_CAM].pos[Y], camera[STATIC_CAM].pos[Z], 1);
        camera[STATIC_CAM].pos[X] = tocka[0];
        camera[STATIC_CAM].pos[Y] = tocka[1];
        camera[STATIC_CAM].pos[Z] = tocka[2];
        break;
    case Qt::Key_S:
        V = glm::rotate_slow(V, glm::radians(camera[STATIC_CAM].rot[Y]), glm::vec3(0, -1, 0));
        V = glm::translate(V, glm::vec3(0, 0, -0.1f));
        V = glm::rotate_slow(V, glm::radians(camera[STATIC_CAM].rot[Y]), glm::vec3(0, 1, 0));
        tocka = V * glm::vec4(camera[STATIC_CAM].pos[X], camera[STATIC_CAM].pos[Y], camera[STATIC_CAM].pos[Z], 1);
        camera[STATIC_CAM].pos[X] = tocka[0];
        camera[STATIC_CAM].pos[Y] = tocka[1];
        camera[STATIC_CAM].pos[Z] = tocka[2];
        break;
    case Qt::Key_A:
        V = glm::rotate_slow(V, glm::radians(camera[STATIC_CAM].rot[Y]), glm::vec3(0, -1, 0));
        V = glm::translate(V, glm::vec3(0.1f, 0.0f, 0.0f));
        V = glm::rotate_slow(V, glm::radians(camera[STATIC_CAM].rot[Y]), glm::vec3(0, 1, 0));
        tocka = V * glm::vec4(camera[STATIC_CAM].pos[X], camera[STATIC_CAM].pos[Y], camera[STATIC_CAM].pos[Z], 1);
        camera[STATIC_CAM].pos[X] = tocka[0];
        camera[STATIC_CAM].pos[Y] = tocka[1];
        camera[STATIC_CAM].pos[Z] = tocka[2];
        break;
    case Qt::Key_D:
        V = glm::rotate_slow(V, glm::radians(camera[STATIC_CAM].rot[Y]), glm::vec3(0, -1, 0));
        V = glm::translate(V, glm::vec3(-0.1f, 0.0f, 0.0f));
        V = glm::rotate_slow(V, glm::radians(camera[STATIC_CAM].rot[Y]), glm::vec3(0, 1, 0));
        tocka = V * glm::vec4(camera[STATIC_CAM].pos[X], camera[STATIC_CAM].pos[Y], camera[STATIC_CAM].pos[Z], 1);
        camera[STATIC_CAM].pos[X] = tocka[0];
        camera[STATIC_CAM].pos[Y] = tocka[1];
        camera[STATIC_CAM].pos[Z] = tocka[2];
        break;
    case Qt::Key_Q:
        V = glm::rotate_slow(V, glm::radians(camera[STATIC_CAM].rot[Y]), glm::vec3(0, -1, 0));
        V = glm::translate(V, glm::vec3(0.0f, 0.1f, 0.0f));
        V = glm::rotate_slow(V, glm::radians(camera[STATIC_CAM].rot[Y]), glm::vec3(0, 1, 0));
        tocka = V * glm::vec4(camera[STATIC_CAM].pos[X], camera[STATIC_CAM].pos[Y], camera[STATIC_CAM].pos[Z], 1);
        camera[STATIC_CAM].pos[X] = tocka[0];
        camera[STATIC_CAM].pos[Y] = tocka[1];
        camera[STATIC_CAM].pos[Z] = tocka[2];
        break;
    case Qt::Key_E:
        V = glm::rotate_slow(V, glm::radians(camera[STATIC_CAM].rot[Y]), glm::vec3(0, -1, 0));
        V = glm::translate(V, glm::vec3(0.0f, -0.1f, 0.0f));
        V = glm::rotate_slow(V, glm::radians(camera[STATIC_CAM].rot[Y]), glm::vec3(0, 1, 0));
        tocka = V * glm::vec4(camera[STATIC_CAM].pos[X], camera[STATIC_CAM].pos[Y], camera[STATIC_CAM].pos[Z], 1);
        camera[STATIC_CAM].pos[X] = tocka[0];
        camera[STATIC_CAM].pos[Y] = tocka[1];
        camera[STATIC_CAM].pos[Z] = tocka[2];
        break;
    case Qt::Key_C:
        selectedCamera = ++selectedCamera % camera.size();
        break;

    // ROTACIJA PIRAMIDE
    case Qt::Key_K:
        allObjects[selectedObject].rot[X] += 1.0f;
        break;
    case Qt::Key_I:
        allObjects[selectedObject].rot[X] -= 1.0f;
        break;
    case Qt::Key_L:
        allObjects[selectedObject].rot[Y] += 1.0f;
        break;
    case Qt::Key_J:
        allObjects[selectedObject].rot[Y] -= 1.0f;
        break;
    case Qt::Key_U:
        allObjects[selectedObject].rot[Z] += 1.0f;
        break;
    case Qt::Key_O:
        allObjects[selectedObject].rot[Z] -= 1.0f;
        break;

    // SKALIRANJE PIRAMIDE
    case Qt::Key_N:
        allObjects[selectedObject].scale[X] *= 0.9f;
        allObjects[selectedObject].scale[Y] *= 0.9f;
        allObjects[selectedObject].scale[Z] *= 0.9f;
        break;
    case Qt::Key_M:
        allObjects[selectedObject].scale[X] /= 0.9f;
        allObjects[selectedObject].scale[Y] /= 0.9f;
        allObjects[selectedObject].scale[Z] /= 0.9f;
        break;
    }

    update();
}

void WidgetOpenGLDraw::generateCube(glm::vec3 origin) {
    Object O;
    O.vertices.push_back(glm::vec3(-0.5, -0.5, -0.5) + origin);  //levo
    O.vertices.push_back(glm::vec3(-0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, -0.5, -0.5) + origin);  //Spredaj
    O.vertices.push_back(glm::vec3(0.5, -0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, -0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, -0.5, -0.5) + origin); //Desno
    O.vertices.push_back(glm::vec3(0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, 0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, -0.5, 0.5) + origin); //Zadaj
    O.vertices.push_back(glm::vec3(0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, 0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, -0.5) + origin); //Top
    O.vertices.push_back(glm::vec3(-0.5, 0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, 0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, -0.5, -0.5) + origin); //Bottom
    O.vertices.push_back(glm::vec3(-0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, -0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, -0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, -0.5, 0.5) + origin);

    O.index = objectCount++;
    O.verticesStartingPosition = static_cast<unsigned int>(allVertices.size());
    O.color = glm::vec4(0.0, 1.0, 0.1, 0.0);
    for(unsigned int i = 0; i < static_cast<unsigned int>(O.vertices.size()); i++)
        allVertices.push_back(O.vertices[i]);

    allObjects.push_back(O);
    //loadTexture("C:/Faks/RG/Vaja5/b.jpg");
}

void WidgetOpenGLDraw::loadObjFile(QString fileName) {
    //QString fileName = QFileDialog::getOpenFileName(nullptr, "Select .obj File", "", "Object File (*.obj);;All Files(*)");
    //QString fileName = "F:/Faks/RG/build-RG_Template-Desktop_Qt_5_12_5_MinGW_64_bit-Debug/vaja2_icoSphere.obj";
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(nullptr, "Unable to open file", file.errorString());
    }
    QTextStream in(&file);
    QString line;
    in.readLine();
    in.readLine();

    glm::vec3 v3;
    glm::vec2 v2;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textures;
    std::vector<glm::vec3> normals;
    int a;
    char c;
    Triangle T;
    Vertice V;
    Object O;

    while(!in.atEnd()) {
        in >> line;
        if(line == "v") {    //1. ali 2. vrstica, komentar, preskoči
            in >> v3[0];
            in >> v3[1];
            in >> v3[2];
            vertices.push_back(v3);
        }
        else if(line == "vt") {
            in >> v2[0];
            in >> v2[1];
            textures.push_back(v2);
        }
        else if(line == "vn") {
            in >> v3[0];
            in >> v3[1];
            in >> v3[2];
            normals.push_back(v3);
        }
        else if(line == "s") {
            in >> a;
        }
        else if(line == "f") {
            for(unsigned int i = 0; i < 3; i++) {
                in >> a;
                in >> c;
                V.coords = vertices[a - 1];
                in >> a;
                in >> c;
                V.texture = textures[a - 1];
                in >> a;
                V.normal = normals[a - 1];
                T.V[i] = V;
            }
            for(int i = 0; i < 3; i++) {
                O.vertices.push_back(T.V[i].coords);
                O.textures.push_back(T.V[i].texture);
                O.normals.push_back(T.V[i].normal);
            }
        }
    }
    file.close();
    O.index = objectCount++;
    O.verticesStartingPosition = static_cast<unsigned int>(allVertices.size()) * sizeof(glm::vec3);
    O.color = glm::vec4((float(rand() % 8) / 8), (float(rand() % 8) / 8), (float(rand() % 8) / 8), 0);
    for(unsigned int i = 0; i < O.vertices.size(); i++) {
        allVertices.push_back(O.vertices[i]);
        allNormals.push_back(O.normals[i]);
    }
    allObjects.push_back(O);
}

void WidgetOpenGLDraw::addObject(QString fileName) {
    makeCurrent();
    loadObjFile(fileName);
    GLint b;
    id_VAO_object.push_back(b);

    gl->glGenVertexArrays(1, &id_VAO_object.back());
    gl->glBindVertexArray(id_VAO_object.back());

    unsigned int a;
    id_buffer_object.push_back(a);

    gl->glGenBuffers(1, &id_buffer_object.back());
    gl->glBindBuffer(GL_ARRAY_BUFFER, id_buffer_object.back());
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * static_cast<unsigned int>(allObjects.back().vertices.size()), &allObjects.back().vertices[0], GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(0);  // uporabjamo: layout(location=0) in vec3 in_Pos;
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

    unsigned int c;
    id_buffer_normal.push_back(c);
    gl->glGenBuffers(1, &id_buffer_normal.back());
    gl->glBindBuffer(GL_ARRAY_BUFFER, id_buffer_normal.back());
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * static_cast<unsigned int>(allObjects.back().normals.size()), &allObjects.back().normals[0], GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0/*sizeof(glm::vec3)*/, nullptr);

    /*
    unsigned int d;
    id_buffer_texture.push_back(d);
    gl->glGenBuffers(1, &id_buffer_texture.back());
    gl->glBindBuffer(GL_ARRAY_BUFFER, id_buffer_texture.back());
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * static_cast<unsigned int>(allObjects.back().textures.size()), &allObjects.back().normals[0], GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(2);
    gl->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
*/

    const unsigned int err = gl->glGetError();
    if (err != 0) {
        std::cerr << "OpenGL init napaka: " << err << std::endl;
    }
    update();
}

void WidgetOpenGLDraw::openSerialPort(QString s) {
    serialPort.setPortName(s);
    serialPort.setBaudRate(QSerialPort::Baud115200, QSerialPort::AllDirections);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);
    serialPort.setStopBits(QSerialPort::OneStop);

    if(!serialPort.open(QIODevice::ReadOnly)) {
        QString errorS = serialPort.errorString();
    }

    infiniteReadSerial();
}

void WidgetOpenGLDraw::infiniteReadSerial() {
    QByteArray readData;
    QString readString;
    QStringList list;
    uint16_t value[8];
    uint8_t i;
    while(1) {
        //makeCurrent();
        if(serialPort.waitForReadyRead(1)) {
            readData = serialPort.readAll();
            readString = readData;
            list = readString.split(" ",QString::SkipEmptyParts);
            for(i = 0; i < 8; i++)
                value[i] = static_cast<uint16_t>(list[i].toUInt());
            if(replaying == true)
                replay();
            else {
                if(recording)
                    record(value);
                calculateForces(value);
            }
        }
        update();
        QCoreApplication::processEvents();
    }
}

void WidgetOpenGLDraw::calculateForces(uint16_t value[]) {
    if(value[4] > 1100) {       // Če je drone arman
        T = (value[2] - 599) / 1002.0f;
        fm = T * Tfull;
        addRot = (value[3] - 1100) / 50.0f;
        rot = rot + addRot;
        if(rot >= 360.0f)
            rot -= 360.0f;
        if(rot < 0.0f)
            rot += 360.0f;
        angle = glm::vec3(static_cast<float>(value[1] - 1100) / -10.0f, rot, static_cast<float>(value[0] - 1100) / -10.0f);

        norm = glm::vec3(0.0, 1.0, 0.0);
        glm::mat4 rotM = glm::mat4(1);
        rotM = glm::rotate_slow(rotM, glm::radians(angle[1]), glm::vec3(0, 1, 0));
        rotM = glm::rotate_slow(rotM, glm::radians(angle[0]), glm::vec3(1, 0, 0));
        rotM = glm::rotate_slow(rotM, glm::radians(angle[2]), glm::vec3(0, 0, 1));

        norm = glm::vec3(rotM * glm::vec4(norm, 1.0));

        fThr = fm * norm;
        fr = fThr + fg;
        a = fr / m;
        vNew = a * t;
        v += vNew;
        vzuSize = glm::length(v);
        vzu = -static_cast<float>(pow(vzuSize / vMax, 2)) * v;
        v += vzu;
        if(drone->pos[Y] <= 0 && v[1] < 0) {
            v = glm::vec3(0.0, 0.0, 0.0);
            drone->rot[X] = 0.0f;
            rot = rot - addRot;
            if(rot > 360.0f)
                rot -= 360.0f;
            if(rot < 0.0f)
                rot += 360.0f;
            drone->rot[Y] = rot;
            drone->rot[Z] = 0.0f;
        }
        else {
            drone->rot[X] = angle[0];
            drone->rot[Y] = angle[1];
            drone->rot[Z] = angle[2];
        }

        drone->pos[X] += (v[0] * t);
        drone->pos[Y] += (v[1] * t);
        drone->pos[Z] += (v[2] * t);
    }
}

void WidgetOpenGLDraw::startRecording() {
    recording = true;
    // Open file for writing
    // File format: najprej shrani trenutno pozicijo in orientacijo drona (6 floatov)
    //              Nato shranjuje kompresirane vrednosti prvih 5 kanalov

    // Predpotavi, da je COM port že odprt
}

void WidgetOpenGLDraw::stopRecording() {
    recording = false;
    // Close file
}

void WidgetOpenGLDraw::startReplay() {
    replaying = true;
    // Open file for reading
}

void WidgetOpenGLDraw::stopReplay() {
    replaying = false;
    // Close file
}

void WidgetOpenGLDraw::record(uint16_t value[]) {

}

void WidgetOpenGLDraw::replay() {

}

WidgetOpenGLDraw::WidgetOpenGLDraw(QWidget *parent) : QOpenGLWidget(parent) {
    Object c1, c2, c3;
    c1.pos = glm::vec3(0.0f, 2.0f, -15.0f);
    c2.pos = glm::vec3(0.0f, 2.0f, -15.0f);
    c3.pos = glm::vec3(0.0f, 2.0f, -15.0f);
    c1.rot = glm::vec3(0.0f, 0.0f, 0.0f);
    c2.rot = glm::vec3(0.0f, 0.0f, 0.0f);
    c3.rot = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.push_back(c1);
    camera.push_back(c2);
    camera.push_back(c3);

    QFile file("test.txt");
      if(file.open(QIODevice::WriteOnly | QIODevice::Text))
      {
          // We're going to streaming text to the file
          QTextStream stream(&file);

          stream << "test";

          file.close();
      }
}

WidgetOpenGLDraw::~WidgetOpenGLDraw() {
    //počisti stanje
    for(unsigned int i = 0; i < id_VAO_object.size(); i++)
        gl->glDeleteVertexArrays(1,&id_VAO_object[i]);
    gl->glDeleteProgram(id_sencilni_program);
}


void WidgetOpenGLDraw::printProgramInfoLog(GLuint obj) {
    int infologLength = 0;
    gl->glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0) {
        std::unique_ptr<char[]> infoLog(new char[infologLength]);
        int charsWritten = 0;
        gl->glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog.get());
        std::cerr << infoLog.get() << "\n";
    }
}

void WidgetOpenGLDraw::printShaderInfoLog(GLuint obj) {
    int infologLength = 0;
    gl->glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0) {
        std::unique_ptr<char[]> infoLog(new char[infologLength]);
        int charsWritten = 0;
        gl->glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog.get());
        std::cerr << infoLog.get() << "\n";
    }
}

void WidgetOpenGLDraw::PrevediSencilnike() {
    id_sencilni_program = gl->glCreateProgram();

    {  // vhod v senčilnik oglišč je in_Pos, izhod pa gl_Position (rezervirana beseda)
        GLuint vs = gl->glCreateShader(GL_VERTEX_SHADER);
        std::string vss;      // priporočamo hrambo spodnjega niza v datoteki (vsencilnik.vert), potem dobite barvanje sintakse in autocomplete
        vss += " #version 330                                                                   \n";
        vss += " layout(location = 0) in vec3 in_Pos;                                           \n";    // vertexPosition_modelspace
        vss += " layout(location = 1) in vec3 in_Normal;                                        \n";    // vertexNormal_modelspace
        //vss += " layout(location = 2) in vec2 vertexUV;                                         \n";
        vss += " uniform mat4 PVM;                                                              \n";
        vss += " uniform mat4 M;                                                                \n";
        vss += " uniform mat4 V;                                                                \n";
        vss += " uniform vec3 LightPosition_worldspace;                                         \n";
        vss += " out vec3 gPos;                                                                 \n";
        vss += " out vec3 Normal_cameraspace;                                                   \n";
        vss += " out vec3 LightDirection_cameraspace;                                           \n";
        vss += " out vec3 EyeDirection_cameraspace;                                             \n";
        vss += " out vec3 lightPos;                                                             \n";
        //vss += " out vec2 UV;                                                                   \n";
        vss += " void main(){                                                                   \n";
        vss += "    gl_Position = PVM * vec4(in_Pos, 1);                                        \n";
        vss += "    vec3 Position_worldspace = (M * vec4(in_Pos, 1)).xyz;                       \n";
        vss += "    vec3 vertexPosition_cameraspace = ( V * M * vec4(in_Pos, 1)).xyz;           \n";
        vss += "    EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;        \n";
        vss += "    vec3 LightPosition_cameraspace = (V * vec4(LightPosition_worldspace,1)).xyz;        \n";
        vss += "    LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;  \n";
        vss += "    Normal_cameraspace = (V * M * vec4(in_Normal,0)).xyz;                       \n";
        vss += "    lightPos = LightPosition_worldspace;                                        \n";
        //vss += "    UV = vertexUV;                                                              \n";
        vss+="      gPos = in_Pos;                                                              \n";
        vss += " }                                                                              \n";
        //std::cout << vss;
        const char *vssc = vss.c_str();
        gl->glShaderSource(vs, 1, &vssc, nullptr);
        gl->glCompileShader(vs);
        printShaderInfoLog(vs);
        gl->glAttachShader(id_sencilni_program, vs);
    }

    {  // out_Color je barva, ki bo prišla do zaslona
        GLuint fs = gl->glCreateShader(GL_FRAGMENT_SHADER);
        std::string fss;  // priporočamo hrambo spodnjega niza v datoteki (fsencilnik.frag), potem dobite barvanje sintakse in autocomplete
        fss += " #version 330                                                               \n";
        fss += " out vec4 out_Color;                                                        \n";
        fss += " in vec3 Normal_cameraspace;                                                \n";
        fss += " in vec3 LightDirection_cameraspace;                                        \n";
        fss += " in vec3 EyeDirection_cameraspace;                                          \n";
        fss += " in vec3 lightPos;                                                          \n";
        fss += " in vec3 gPos;                                                              \n";
        //fss += " in vec2 UV;                                                                \n";
        fss += " uniform vec4 DodajBarvo;                                                   \n";
        fss += " uniform int shining;                                                       \n";
        fss += " uniform float lightPower;                                                  \n";
        //fss += " uniform sampler2D myTextureSampler;                                        \n";
        fss += " vec4 materialColor;                                                        \n";
        fss += " void main(){                                                               \n";
        //fss += "    materialColor = texture(myTextureSampler, UV);                          \n";
        //fss += "    materialColor.xy = UV;                                           \n";
        fss += "    materialColor = DodajBarvo;                                             \n";
        fss += "    vec3 n = normalize(Normal_cameraspace);                                 \n";
        fss += "    vec3 l = normalize(LightDirection_cameraspace);                         \n";
        fss += "    float cosTheta = dot(n, l);                                             \n";
        fss += "    float dist = distance(gPos, lightPos);                                  \n";
        fss += "    vec3 E = normalize(EyeDirection_cameraspace);                           \n";
        fss += "    vec3 R = reflect(-l, n);                                                \n";
        fss += "    float cosAlpha = clamp(dot(E, R), 0, 1);                                \n";
        fss += "    vec4 MaterialAmbientColor = 0.3f * materialColor;                       \n";
        fss += "    out_Color = MaterialAmbientColor +                                        ";
        fss += "                materialColor * cosTheta * lightPower / (dist * dist) +     \n";
        fss += "                vec4(1, 1, 1, 1) * lightPower * pow(cosAlpha, shining) / (dist * dist);  \n";
        //fss += "    out_Color = materialColor;                                              \n";
        fss += " }                                                                          \n";

        //std::cout << fss;
        const char *fssc = fss.c_str();
        gl->glShaderSource(fs, 1, &fssc, nullptr);
        gl->glCompileShader(fs);
        printShaderInfoLog(fs);
        gl->glAttachShader(id_sencilni_program, fs);
    }

    gl->glLinkProgram(id_sencilni_program);
    printProgramInfoLog(id_sencilni_program);
}

void WidgetOpenGLDraw::initializeGL() {
    srand(static_cast<unsigned int>(time(nullptr)));
    // naložimo funkcije za OpenGL
    std::cout << "OpenGL context version: "<< context()->format().majorVersion() <<"." <<context()->format().minorVersion()<<std::endl;

    gl=context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!gl) {
        std::cerr << "Could not obtain required OpenGL context version";
        QApplication::exit(1);
    }

    std::cout << gl->glGetString(GL_VENDOR) << std::endl;
    std::cout << gl->glGetString(GL_VERSION) << std::endl;
    std::cout << gl->glGetString(GL_RENDERER) << std::endl;

    PrevediSencilnike();

    glEnable(GL_DEPTH_TEST);

    GLuint b;
    id_VAO_object.push_back(b);

    gl->glGenVertexArrays(1, &id_VAO_object.back());
    gl->glBindVertexArray(id_VAO_object.back());

    Light.pos[Y] = 15.0f;

    generateCube(glm::vec3(0, -0.5f, 0));        // Tla
    //loadObjFile(" F:/Faks/RG/build-RG_Template-Desktop_Qt_5_12_5_MinGW_64_bit-Debug/vaja2_mon.obj");
    //              F:/Faks/RG/RG_Template/RG_Template_texture/a.jpg
    unsigned int a;
    id_buffer_object.push_back(a);

    gl->glGenBuffers(1, &id_buffer_object[0]);
    gl->glBindBuffer(GL_ARRAY_BUFFER, id_buffer_object[0]);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * static_cast<unsigned int>(allVertices.size()), &allVertices[0], GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(0);  // uporabjamo: layout(location=0) in vec3 in_Pos;
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    gl->glEnableVertexAttribArray(2);
    gl->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

    const unsigned int err = gl->glGetError();
    if (err != 0) {
        std::cerr << "OpenGL init napaka: " << err << std::endl;
    }

    addObject("Objects/vaja2_mon.obj");
    drone = &allObjects[1];
    //makeCurrent();
}

void WidgetOpenGLDraw::resizeGL(int w, int h) {
    gl->glViewport(0, 0, w, h);
}

void WidgetOpenGLDraw::paintGL() {
    // počisti ozadje in globinski pomnilnik (za testiranje globine)
    gl->glClearColor(0.2f, 0.2f, 0.2f, 1);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    for(unsigned int i = 0; i < allObjects.size(); i++) {
        gl->glBindVertexArray(id_VAO_object[i]);
        gl->glUseProgram(id_sencilni_program);
        // projekcijska matrika
        glm::mat4 P;

        /*if(projectionType == 1) {
            float wh = float(double(width()) / double(height()));
            P = glm::orthoLH((0.1f * FOV * wh) / (-2.0f), (0.1f * FOV * wh) / (2.0f), (0.1f * FOV) / (-2.0f), (0.1f * FOV) / 2.0f, 100.0f, -100.0f);
        }
        else*/

        P = glm::perspective(glm::radians(FOV), float(width()) / height(), 0.01f, 1000.0f);

        // Matrika pogleda (view) (premikanje kamere...)
        /*
        glm::mat4 V = glm::mat4(1);
        V = glm::rotate_slow(V, glm::radians(camRotX), glm::vec3(1, 0, 0));
        V = glm::rotate_slow(V, glm::radians(camRotY), glm::vec3(0, 1, 0));
        V = glm::rotate_slow(V, glm::radians(camRotZ), glm::vec3(0, 0, 1));
        V = glm::translate(V, glm::vec3(camPosX, camPosY, camPosZ));
        */
        lookAtDrone = glm::normalize(glm::vec3(drone->pos[X], drone->pos[Y], drone->pos[Z]) - glm::vec3(camera[STATIC_CAM].pos[X], camera[STATIC_CAM].pos[Y], camera[STATIC_CAM].pos[Z]));
        glm::vec3 upVector = glm::vec3(0, 1, 0);

        glm::mat4 V = glm::translate(V, glm::vec3(camera[STATIC_CAM].pos[X], camera[STATIC_CAM].pos[Y], camera[STATIC_CAM].pos[Z]));
        V = glm::lookAt(glm::vec3(camera[STATIC_CAM].pos[X], camera[STATIC_CAM].pos[Y], camera[STATIC_CAM].pos[Z]), lookAtDrone + glm::vec3(camera[STATIC_CAM].pos[X], camera[STATIC_CAM].pos[Y], camera[STATIC_CAM].pos[Z]), upVector);


        glm::mat4 M = glm::mat4(1);  // model matrix: premikanje objektov

        glm::mat4 scale;

        if(i == 0) {
            M = glm::translate(M, glm::vec3(0, 0, 0));
            scale = glm::mat4(glm::vec4(20.0f, 0.0f, 0.0f, 0.0f),
                              glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                              glm::vec4(0.0f, 0.0f, 20.0f, 0.0f),
                              glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        }
        else {
            M = glm::translate(M, glm::vec3(allObjects[i].pos[X], allObjects[i].pos[Y], allObjects[i].pos[Z]));
            M = glm::rotate_slow(M, glm::radians(allObjects[i].rot[Y]), glm::vec3(0, 1, 0));
            M = glm::rotate_slow(M, glm::radians(allObjects[i].rot[X]), glm::vec3(1, 0, 0));
            M = glm::rotate_slow(M, glm::radians(allObjects[i].rot[Z]), glm::vec3(0, 0, 1));
            scale = glm::mat4(glm::vec4(allObjects[i].scale[X], 0.0f, 0.0f, 0.0f),
                              glm::vec4(0.0f, allObjects[i].scale[Y], 0.0f, 0.0f),
                              glm::vec4(0.0f, 0.0f, allObjects[i].scale[Z], 0.0f),
                              glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        }
        glm::mat4 PVM = P * V * M;
        PVM *= scale;

        gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_sencilni_program, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
        gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_sencilni_program, "M"), 1, GL_FALSE, glm::value_ptr(M));
        gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_sencilni_program, "V"), 1, GL_FALSE, glm::value_ptr(V));

        //gl->glActiveTexture(GL_TEXTURE0);
        //gl->glBindTexture(GL_TEXTURE_2D, allObjects[i].textureID);
        //gl->glUniform1i(gl->glGetUniformLocation(id_sencilni_program, "myTextureSampler"), 0);
        gl->glUniform4f(gl->glGetUniformLocation(id_sencilni_program, "DodajBarvo"), allObjects[i].color[0], allObjects[i].color[1], allObjects[i].color[2], allObjects[i].color[3]);
        gl->glUniform3f(gl->glGetUniformLocation(id_sencilni_program, "LightPosition_worldspace"), Light.pos[X], Light.pos[Y], Light.pos[Z]);
        gl->glUniform1i(gl->glGetUniformLocation(id_sencilni_program, "shining"), allObjects[i].shining);
        gl->glUniform1f(gl->glGetUniformLocation(id_sencilni_program, "lightPower"), lightPower);
        //gl->glDrawArrays(GL_TRIANGLES, static_cast<int>(allObjects[i].verticesStartingPosition), static_cast<int>(allObjects[i].vertices.size()));
        gl->glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(allObjects[i].vertices.size()));
    }

    const unsigned int err = gl->glGetError();
    if (err != 0) {
        std::cerr << "1OpenGL napaka: " << err << std::endl;
    }
}

void WidgetOpenGLDraw::MoveObjXPlus() {
    makeCurrent();
    drone->pos[X] += 0.2f;
    update();
}

void WidgetOpenGLDraw::MoveObjXMinus() {
    makeCurrent();
    drone->pos[X] -= 0.2f;
    update();
}

void WidgetOpenGLDraw::MoveObjYPlus() {
    makeCurrent();
    drone->pos[Y] += 0.2f;
    update();
}

void WidgetOpenGLDraw::MoveObjYMinus() {
    makeCurrent();
    drone->pos[Y] -= 0.2f;
    update();
}

void WidgetOpenGLDraw::MoveObjZPlus() {
    makeCurrent();
    drone->pos[Z] += 0.2f;
    update();
}

void WidgetOpenGLDraw::MoveObjZMinus() {
    makeCurrent();
    drone->pos[Z] -= 0.2f;
    update();
}
