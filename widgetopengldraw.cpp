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
#include <QtSerialPort/QSerialPortInfo>

// Kompresija
std::fstream recordFile;

byte buffoutbyte = 0b00000000;
int buffoutcount = 0;
byte buffinbyte = 0b00000000;
int buffincount = 8;
std::ofstream outfile;
std::ifstream infile;
std::ifstream innfile;
int encodelast[5] = {0,0,0,0,0};
int decodelast[5] = {0,0,0,0,0};
int repeatcounter[5] = {0,0,0,0,0};
int repeatincounter[5] = {0,0,0,0,0};

std::ofstream outorig;
std::ofstream outdecode;

void writeBit(bool bit)
{
    buffoutbyte |= bit << (7-buffoutcount);
    buffoutcount++;
    if(buffoutcount == 8)
    {
        char lol = buffoutbyte;
        outfile.write(&lol,1);
        buffoutbyte = 0;
        buffoutcount = 0;
    }
}
void clearWriteBits()
{
    char lol = buffoutbyte;
    outfile.write(&lol,1);
    buffoutbyte = 0;
    buffoutcount = 0;
}
bool readBit()
{
    if(buffincount == 8)
    {
        char lol;
        infile.read(&lol,1);
        buffinbyte = lol;
        buffincount=0;
    }
    bool ret = (buffinbyte >> (7-buffincount)) & 1;
    buffincount++;
    return ret;

}
void clearReadBits()
{
        buffincount = 8;
        buffinbyte = 0;
}

int16_t getNumDecode(byte b)
{
    byte num[32]={0b00000000,0b00000001,0b00000010,0b00000011,0b00000100,0b00000101,0b00000110,0b00000111,0b00001000,0b00001001,0b00001010,0b00001011,0b00001100,0b00001101,0b00001110,0b00001111,0b00010000,0b00010001,0b00010010,0b00010011,0b00010100,0b00010101,0b00010110,0b00010111,0b00011000,0b00011001,0b00011010,0b00011011,0b00011100,0b00011101,0b00011110,0b00011111};
    for(int  i = 0;i!=32;i++)
    {
        if(num[i] == b)
            return i;
    }
    return 32;
}

void WidgetOpenGLDraw::startCompressRecording() {
    outorig.open("C:/Users/GTAbl/Desktop/Racunalniska_Grafika/DroneSimulator-master/Compressed/orig.bin");
    outfile.open("C:/Users/GTAbl/Desktop/Racunalniska_Grafika/DroneSimulator-master/Compressed/compressed.bin");
    recording = true;
    float posx = 1.3;
    float posy = 3.3;
    float posz = 0.4;
    float rotx = 1.1;
    float roty = 4.9;
    float rotz = 5;

    outfile.write((const char *)&posx,sizeof(float));
    outfile.write((const char *)&posy,sizeof(float));
    outfile.write((const char *)&posz,sizeof(float));
    outfile.write((const char *)&rotx,sizeof(float));
    outfile.write((const char *)&roty,sizeof(float));
    outfile.write((const char *)&rotz,sizeof(float));



    // Open file for writing
    // File format: najprej shrani trenutno pozicijo in orientacijo drona (6 floatov)
    //              Nato shranjuje kompresirane vrednosti prvih 5 kanalov

    // Predpotavi, da je COM port že odprt
}

void WidgetOpenGLDraw::stopCompressRecording() {
    recording = false;
    outorig.close();
    outfile.close();
}

void WidgetOpenGLDraw::startDecompressReplay() {
    replaying = true;
    // Open file for reading
    float posx;
    float posy;
    float posz;
    float rotx;
    float roty;
    float rotz;
    outdecode.open("C:/Users/GTAbl/Desktop/Racunalniska_Grafika/DroneSimulator-master/Compressed/decode.bin");
    infile.open("C:/Users/GTAbl/Desktop/Racunalniska_Grafika/DroneSimulator-master/Compressed/compressed.bin");
    innfile.open("C:/Users/GTAbl/Desktop/Racunalniska_Grafika/DroneSimulator-master/Compressed/orig.bin");
    infile.read((char *)&posx,sizeof(float));
    infile.read((char *)&posy,sizeof(float));
    infile.read((char *)&posz,sizeof(float));
    infile.read((char *)&rotx,sizeof(float));
    infile.read((char *)&roty,sizeof(float));
    infile.read((char *)&rotz,sizeof(float));
}

void WidgetOpenGLDraw::stopDecompressReplay() {
    replaying = false;
    // Close file
    outdecode.close();
    innfile.close();
    infile.close();
}

void WidgetOpenGLDraw::compressRecord(uint16_t value[8]) {
    byte num[32]={0b00000000,0b00000001,0b00000010,0b00000011,0b00000100,0b00000101,0b00000110,0b00000111,0b00001000,0b00001001,0b00001010,0b00001011,0b00001100,0b00001101,0b00001110,0b00001111,0b00010000,0b00010001,0b00010010,0b00010011,0b00010100,0b00010101,0b00010110,0b00010111,0b00011000,0b00011001,0b00011010,0b00011011,0b00011100,0b00011101,0b00011110,0b00011111};

    if(encodelast[0]==0){
        for(int i = 0; i!= 5 ;i++)
        {
            encodelast[i] = value[i];
            outfile.write((const char *)&value[i],sizeof(uint16_t));
            outorig.write((const char *)&value[i],sizeof(uint16_t));
        }
    }else{
    int diff[5];
    for(int i = 0; i!= 5 ;i++)
    {
        diff[i] = encodelast[i] - value[i];
        encodelast[i] = value[i];
        if(repeatcounter[i] != 0 && diff != 0){
            writeBit(0);
            writeBit(1);
            writeBit((num[repeatcounter[i]-1] >> 2)&1);
            writeBit((num[repeatcounter[i]-1] >> 1)&1);
            writeBit((num[repeatcounter[i]-1])&1);
            repeatcounter[i] = 0;
        }else{
            if(abs(diff[i]) == 0)
            {
                repeatcounter[i]++;
                if(repeatcounter[i]==8)
                {
                    writeBit(0);
                    writeBit(1);
                    writeBit(1);
                    writeBit(1);
                    writeBit(1);
                    repeatcounter[i] = 0;
                }
            }else if(abs(diff[i])<3){
                writeBit(0);
                writeBit(0);
                writeBit(0);
                writeBit(0);
                writeBit((num[diff[i]+2] >> 2)&1);
                writeBit((num[diff[i]+2] >> 1)&1);
                writeBit((num[diff[i]+2])&1);
            }else if(abs(diff[i])<7){
                writeBit(0);
                writeBit(0);
                writeBit(0);
                writeBit(1);
                writeBit((num[diff[i]+6] >> 3)&1);
                writeBit((num[diff[i]+6] >> 2)&1);
                writeBit((num[diff[i]+6] >> 1)&1);
                writeBit((num[diff[i]+6])&1);
            }else if(abs(diff[i])<15){
                writeBit(0);
                writeBit(0);
                writeBit(1);
                writeBit(0);
                writeBit((num[diff[i]+14] >> 4)&1);
                writeBit((num[diff[i]+14] >> 3)&1);
                writeBit((num[diff[i]+14] >> 2)&1);
                writeBit((num[diff[i]+14] >> 1)&1);
                writeBit((num[diff[i]+14])&1);
            }else if(abs(diff[i])<31){
                writeBit(0);
                writeBit(0);
                writeBit(1);
                writeBit(1);
                writeBit((num[diff[i]+30] >> 5)&1);
                writeBit((num[diff[i]+30] >> 4)&1);
                writeBit((num[diff[i]+30] >> 3)&1);
                writeBit((num[diff[i]+30] >> 2)&1);
                writeBit((num[diff[i]+30] >> 1)&1);
                writeBit((num[diff[i]+30])&1);
            }else if(abs(diff[i])>30){
                writeBit(1);
                writeBit(0);
                clearWriteBits();
                outfile.write((const char *)&value[i],sizeof(uint16_t));
            }
        }
    }
    }
    outorig.write((const char *)&value[0],sizeof(uint16_t));
    outorig.write((const char *)&value[1],sizeof(uint16_t));
    outorig.write((const char *)&value[2],sizeof(uint16_t));
    outorig.write((const char *)&value[3],sizeof(uint16_t));
    outorig.write((const char *)&value[4],sizeof(uint16_t));
}

uint16_t * WidgetOpenGLDraw::decompressReplay() {
    uint16_t ret[5];
    if(decodelast[0]==0){
        for(int i = 0; i!= 5 ;i++)
        {
            infile.read((char *)&ret[i],sizeof(uint16_t));
            decodelast[i] = ret[i];
        }
        outdecode.write((const char *)&ret[0],sizeof(uint16_t));
        outdecode.write((const char *)&ret[1],sizeof(uint16_t));
        outdecode.write((const char *)&ret[2],sizeof(uint16_t));
        outdecode.write((const char *)&ret[3],sizeof(uint16_t));
        outdecode.write((const char *)&ret[4],sizeof(uint16_t));
        return ret;
    }
    int diff[5];
    int first[5];
    byte buff = 0;
    for(int i = 0; i!= 5 ;i++)
    {
        buff |= readBit() << 1;
        buff |= readBit();
        first[i] = getNumDecode(buff);
        buff = 0;
        if(repeatincounter[i] == 0)
        {
        if(first[i] == 0)
        {
            buff |= readBit() << 1;
            buff |= readBit();
            diff[i] = getNumDecode(buff);
            buff = 0;
            if(diff[i] == 0){
                buff |= readBit() << 1;
                buff |= readBit();
                ret[i] = decodelast[i]+getNumDecode(buff)-2;
            }else if(diff[i] == 1){
                buff |= readBit() << 2;
                buff |= readBit() << 1;
                buff |= readBit();
                ret[i] = decodelast[i]+getNumDecode(buff)-6;
            }else if(diff[i] == 2){
                buff |= readBit() << 3;
                buff |= readBit() << 2;
                buff |= readBit() << 1;
                buff |= readBit();
                ret[i] = decodelast[i]+getNumDecode(buff)-14;
            }else if(diff[i] == 3){
                buff |= readBit() << 4;
                buff |= readBit() << 3;
                buff |= readBit() << 2;
                buff |= readBit() << 1;
                buff |= readBit();
                ret[i] = decodelast[i]+getNumDecode(buff)-30;
            }
            buff = 0;
        }else if(first[i] == 1){
            buff |= readBit() << 2;
            buff |= readBit() << 1;
            buff |= readBit();
            repeatincounter[i] = getNumDecode(buff)+1;
            ret[i] = decodelast[i];
            repeatincounter[i]--;
        }else if(first[i] == 2){
            clearReadBits();
            infile.read((char *)&ret[i],sizeof(uint16_t));
        }else if(first[i] == 3){
            //stopReplay(); //TODO?
            ret[i] = decodelast[i];
        }

        }else{
            ret[i] = decodelast[i];
            repeatincounter[i]--;
        }
        decodelast[i] = ret[i];

    }
    //ret = calccoords();
    outdecode.write((const char *)&ret[0],sizeof(uint16_t));
    outdecode.write((const char *)&ret[1],sizeof(uint16_t));
    outdecode.write((const char *)&ret[2],sizeof(uint16_t));
    outdecode.write((const char *)&ret[3],sizeof(uint16_t));
    outdecode.write((const char *)&ret[4],sizeof(uint16_t));
    return ret;
}

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
    selectedGroup = static_cast<unsigned int>(index);
}

glm::vec3 WidgetOpenGLDraw::getObjectColor() {
    glm::vec3 c;
    c[0] = allObjects[selectedGroup].color[0];
    c[1] = allObjects[selectedGroup].color[1];
    c[2] = allObjects[selectedGroup].color[2];
    return c;
}

void WidgetOpenGLDraw::setObjectColor(glm::vec3 color) {
    makeCurrent();
    allObjects[selectedGroup].color[0] = color[0];
    allObjects[selectedGroup].color[1] = color[1];
    allObjects[selectedGroup].color[2] = color[2];
    update();
}

void WidgetOpenGLDraw::setObjectShining(int s) {
    makeCurrent();
    allObjects[selectedGroup].shining = s;
    update();
}

int WidgetOpenGLDraw::getObjectShining() {
    return allObjects[selectedGroup].shining;
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
        camera[FREE_CAM].rot[X] -= 1.0f;
        break;
    case Qt::Key_G:
        camera[FREE_CAM].rot[X] += 1.0f;
        break;
    case Qt::Key_F:
        camera[FREE_CAM].rot[Y] -= 1.0f;
        break;
    case Qt::Key_H:
        camera[FREE_CAM].rot[Y] += 1.0f;
        break;

    // PREMIK KAMERE PO SCENI
    case Qt::Key_W:
        V = glm::rotate(V, glm::radians(-camera[FREE_CAM].rot[Y]), glm::vec3(0, 1, 0));
        V = glm::translate(V, glm::vec3(0, 0, 0.1f));
        V = glm::rotate(V, glm::radians(camera[FREE_CAM].rot[Y]), glm::vec3(0, 1, 0));
        tocka = V * glm::vec4(camera[FREE_CAM].pos[X], camera[FREE_CAM].pos[Y], camera[FREE_CAM].pos[Z], 1);
        camera[FREE_CAM].pos[X] = tocka[0];
        camera[FREE_CAM].pos[Y] = tocka[1];
        camera[FREE_CAM].pos[Z] = tocka[2];
        break;
    case Qt::Key_S:
        V = glm::rotate_slow(V, glm::radians(camera[FREE_CAM].rot[Y]), glm::vec3(0, -1, 0));
        V = glm::translate(V, glm::vec3(0, 0, -0.1f));
        V = glm::rotate_slow(V, glm::radians(camera[FREE_CAM].rot[Y]), glm::vec3(0, 1, 0));
        tocka = V * glm::vec4(camera[FREE_CAM].pos[X], camera[FREE_CAM].pos[Y], camera[FREE_CAM].pos[Z], 1);
        camera[FREE_CAM].pos[X] = tocka[0];
        camera[FREE_CAM].pos[Y] = tocka[1];
        camera[FREE_CAM].pos[Z] = tocka[2];
        break;
    case Qt::Key_A:
        V = glm::rotate_slow(V, glm::radians(camera[FREE_CAM].rot[Y]), glm::vec3(0, -1, 0));
        V = glm::translate(V, glm::vec3(0.1f, 0.0f, 0.0f));
        V = glm::rotate_slow(V, glm::radians(camera[FREE_CAM].rot[Y]), glm::vec3(0, 1, 0));
        tocka = V * glm::vec4(camera[FREE_CAM].pos[X], camera[FREE_CAM].pos[Y], camera[FREE_CAM].pos[Z], 1);
        camera[FREE_CAM].pos[X] = tocka[0];
        camera[FREE_CAM].pos[Y] = tocka[1];
        camera[FREE_CAM].pos[Z] = tocka[2];
        break;
    case Qt::Key_D:
        V = glm::rotate_slow(V, glm::radians(camera[FREE_CAM].rot[Y]), glm::vec3(0, -1, 0));
        V = glm::translate(V, glm::vec3(-0.1f, 0.0f, 0.0f));
        V = glm::rotate_slow(V, glm::radians(camera[FREE_CAM].rot[Y]), glm::vec3(0, 1, 0));
        tocka = V * glm::vec4(camera[FREE_CAM].pos[X], camera[FREE_CAM].pos[Y], camera[FREE_CAM].pos[Z], 1);
        camera[FREE_CAM].pos[X] = tocka[0];
        camera[FREE_CAM].pos[Y] = tocka[1];
        camera[FREE_CAM].pos[Z] = tocka[2];
        break;
    case Qt::Key_Q:
        V = glm::rotate_slow(V, glm::radians(camera[FREE_CAM].rot[Y]), glm::vec3(0, -1, 0));
        V = glm::translate(V, glm::vec3(0.0f, 0.1f, 0.0f));
        V = glm::rotate_slow(V, glm::radians(camera[FREE_CAM].rot[Y]), glm::vec3(0, 1, 0));
        tocka = V * glm::vec4(camera[FREE_CAM].pos[X], camera[FREE_CAM].pos[Y], camera[FREE_CAM].pos[Z], 1);
        camera[FREE_CAM].pos[X] = tocka[0];
        camera[FREE_CAM].pos[Y] = tocka[1];
        camera[FREE_CAM].pos[Z] = tocka[2];
        break;
    case Qt::Key_E:
        V = glm::rotate_slow(V, glm::radians(camera[FREE_CAM].rot[Y]), glm::vec3(0, -1, 0));
        V = glm::translate(V, glm::vec3(0.0f, -0.1f, 0.0f));
        V = glm::rotate_slow(V, glm::radians(camera[FREE_CAM].rot[Y]), glm::vec3(0, 1, 0));
        tocka = V * glm::vec4(camera[FREE_CAM].pos[X], camera[FREE_CAM].pos[Y], camera[FREE_CAM].pos[Z], 1);
        camera[FREE_CAM].pos[X] = tocka[0];
        camera[FREE_CAM].pos[Y] = tocka[1];
        camera[FREE_CAM].pos[Z] = tocka[2];
        break;
    case Qt::Key_C:
        selectedCamera = ++selectedCamera % camera.size();
        //if(selectedCamera == FREE_CAM) {
        //    camera[FREE_CAM].rot = camera[LOOKAT_CAM].rot;
        //}
        break;

    // ROTACIJA DRONA
    case Qt::Key_K:
        drone->rot[X] += 1.0f;
        break;
    case Qt::Key_I:
        drone->rot[X] -= 1.0f;
        break;
    case Qt::Key_L:
        drone->rot[Y] += 1.0f;
        break;
    case Qt::Key_J:
        drone->rot[Y] -= 1.0f;
        break;
    case Qt::Key_U:
        drone->rot[Z] += 1.0f;
        break;
    case Qt::Key_O:
        drone->rot[Z] -= 1.0f;
        break;

    // SKALIRANJE DRONA
    case Qt::Key_N:
        drone->scale[X] *= 0.9f;
        drone->scale[Y] *= 0.9f;
        drone->scale[Z] *= 0.9f;
        break;
    case Qt::Key_M:
        drone->scale[X] /= 0.9f;
        drone->scale[Y] /= 0.9f;
        drone->scale[Z] /= 0.9f;
        break;
    }
    drone->generateMs();
    update();
}

void WidgetOpenGLDraw::generateFloor(glm::vec3 origin) {
    Object O;
    O.vertices.push_back(glm::vec3(-0.5, -0.5, -0.5) + origin); //levo
    O.vertices.push_back(glm::vec3(-0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, -0.5, -0.5) + origin); //Spredaj
    O.vertices.push_back(glm::vec3(0.5, -0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, -0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, -0.5, -0.5) + origin);  //Desno
    O.vertices.push_back(glm::vec3(0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, 0.5, -0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, 0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, -0.5, 0.5) + origin);  //Zadaj
    O.vertices.push_back(glm::vec3(0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, -0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(0.5, 0.5, 0.5) + origin);
    O.vertices.push_back(glm::vec3(-0.5, 0.5, -0.5) + origin);  //Top
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
    O.color = glm::vec4(0.6, 0.9, 0.2, 0.0);
    for(unsigned int i = 0; i < static_cast<unsigned int>(O.vertices.size()); i++)
        allVertices.push_back(O.vertices[i]);

    O.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    allObjects.push_back(O);
    //loadTexture("C:/Faks/RG/Vaja5/b.jpg");
}

void WidgetOpenGLDraw::loadObjFile(QString fileName) {
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
    O.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    for(unsigned int i = 0; i < O.vertices.size(); i++) {
        allVertices.push_back(O.vertices[i]);
        allNormals.push_back(O.normals[i]);
    }
    allObjects.push_back(O);
}

void WidgetOpenGLDraw::addObject(QString fileName) {
    makeCurrent();
    loadObjFile(fileName);

    gl->glGenVertexArrays(1, &allObjects.back().id_VAO_object);
    gl->glBindVertexArray(allObjects.back().id_VAO_object);

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
    initPrediction();
    while(1) {
        if(serialPort.waitForReadyRead(1)) {
            readData = serialPort.readAll();
            readString = readData;
            list = readString.split(" ",QString::SkipEmptyParts);
            for(i = 0; i < 8; i++)
                value[i] = static_cast<uint16_t>(list[i].toUInt());

            if(value[7] > 1500 && infoMode == false) {
                infoMode = true;
                showPrediction();
            }
            else if(value[7] < 700 && infoMode == true) {
                infoMode = false;
                drone->parts[2].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
                drone->parts[3].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
                drone->parts[4].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
                drone->parts[5].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
                hidePrediction();
            }

            if(value[6] < 700)
                selectedCamera = DRONE_CAM;
            else if(value[6] < 1200)
                selectedCamera = LOOKAT_CAM;
            else
                selectedCamera = FREE_CAM;

            if(replaying == true)
                replay(value);
            else {
                if(recording)
                    record(value);
                calculateForces(value);
            }
        }
        drone->generateMs();

        update();
        QCoreApplication::processEvents();
    }
}

void WidgetOpenGLDraw::calculateForces(uint16_t value[]) {
    //glm::vec4 rotorSpeed;
    if(value[4] > 1100) {       // Če je drone arman
        T = (value[2] - 599) / 1002.0f;
        fm = T * Tfull;
        addRot = (value[3] - 1100) / 80.0f;
        rot = rot - addRot;
        if(rot >= 360.0f)
            rot -= 360.0f;
        if(rot < 0.0f)
            rot += 360.0f;
        angle = glm::vec3(static_cast<float>(value[1] - 1100) / -15.0f, rot, static_cast<float>(value[0] - 1100) / -15.0f);

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
            drone->pos[Y] = 0;
            v = glm::vec3(0.0, 0.0, 0.0);
            drone->rot[X] = 0.0f;
            rot = rot + addRot;
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

        glm::vec4 rotorSpeed = glm::vec4(value[0], value[1], value[2], value[3]);
        rotorSpeed = calculateRotorSpeed(rotorSpeed);
        for(int i = 0; i < 4; i++) {
            drone->parts[i + 2].rot[Y] = static_cast<uint16_t>(drone->parts[i + 2].rot[Y] + rotorSpeed[i] * 75.0f);
            if(drone->parts[i + 2].rot[Y] > 360.0f)
                drone->parts[i + 2].rot[Y] -= 360.0f;
        }
    }

    if(infoMode) {
        //prediction->rot = drone->rot;
        prediction->pos = drone->pos;
        glm::vec3 vTemp;
        for(uint8_t i = 0; i < 10; i++) {
            vTemp = v + a * t * static_cast<float>(i);
            vzuSize = glm::length(vTemp);
            vzu = -static_cast<float>(pow(vzuSize / vMax, 2)) * vTemp;
            vTemp += vzu;
            prediction->parts[i].pos = vTemp * t * 3.0f * static_cast<float>(i + 1);
            prediction->generateMs();
        }
    }
}

glm::vec4 WidgetOpenGLDraw::calculateRotorSpeed(glm::vec4 value) {
    float x, y, z, T;
    T = (value[2] - 599) / 1002.0f;
    x = (value[1] - 1100) / 1503;       // Nagib naprej - nazaj (max 1/3)
    z = (value[3] - 1100) / 1503;       // Nagib levo - desno (max 1/3)
    y = (value[0] - 1100) / 1503;       // Obrat levo - desno (max 1/3)
    glm::vec4 rotorSpeed = glm::vec4(T, T, T, T);
    rotorSpeed += (x >= 0) ? glm::vec4(0.0f, 0.0f, x, x) : glm::vec4(-x, -x, 0.0f, 0.0f);       // Nagib naprej - nazaj
    rotorSpeed += (z >= 0) ? glm::vec4(z, 0.0f, 0.0f, z) : glm::vec4(0.0f, -z, -z, 0.0f);       // Nagib levo - desno
    rotorSpeed += (y >= 0) ? glm::vec4(0.0f, y, 0.0f, y) : glm::vec4(-y, 0.0f, -y, 0.0f);       // Obrat levo - desno

    float max = rotorSpeed[0];
    if(rotorSpeed[1] > max)
        max = rotorSpeed[1];
    if(rotorSpeed[2] > max)
        max = rotorSpeed[2];
    if(rotorSpeed[3] > max)
        max = rotorSpeed[3];
    if(max > 1.0f)
        rotorSpeed /= max;
    if(max < 0.02f)
        rotorSpeed = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    float temp;
    if(infoMode) {
        for(uint8_t i = 0; i < 4; i++) {
            if(rotorSpeed[i] <= 0.33333f) {
                temp = rotorSpeed[i] * 3.0f;
                drone->parts[2 + i].color = glm::vec4(0.0f, temp, 1.0f - temp, 0.0f);
            }
            else if(rotorSpeed[i] <= 0.66667f) {
                temp = (rotorSpeed[i] - 0.33333f) * 3.0f;
                drone->parts[2 + i].color = glm::vec4(temp, 1.0f, 0.0f, 0.0f);
            }
            else {
                temp = (rotorSpeed[i] - 0.66667f) * 3.0f;
                drone->parts[2 + i].color = glm::vec4(1.0f, 1.0f - temp, 0.0f, 0.0f);
            }
        }
    }
    return rotorSpeed;
}

void WidgetOpenGLDraw::startRecording() {
    recording = true;
    // Open file for writing
    // File format: najprej shrani trenutno pozicijo in orientacijo drona (6 floatov)
    //              Nato shranjuje kompresirane vrednosti prvih 5 kanalov

    // Predpostavi, da je COM port že odprt

    recordFile.open("recording.bin", std::ios::out | std::ios::trunc | std::ios::binary);

    recordFile.write((char*)&drone->pos[X], sizeof(float));
    recordFile.write((char*)&drone->pos[Y], sizeof(float));
    recordFile.write((char*)&drone->pos[Z], sizeof(float));
    recordFile.write((char*)&drone->rot[X], sizeof(float));
    recordFile.write((char*)&drone->rot[Y], sizeof(float));
    recordFile.write((char*)&drone->rot[Z], sizeof(float));
}

void WidgetOpenGLDraw::stop() {
    if(recording)
        stopRecording();
    else
        stopReplay();
}

void WidgetOpenGLDraw::stopRecording() {
    recording = false;
    // Close file
    recordFile.close();
}

void WidgetOpenGLDraw::startReplay() {
    replaying = true;
    // Open file for reading
    recordFile.open("recording.bin", std::ios::in | std::ios::binary);
    recordFile.read((char*)&drone->pos[X], sizeof(float));
    recordFile.read((char*)&drone->pos[Y], sizeof(float));
    recordFile.read((char*)&drone->pos[Z], sizeof(float));
    recordFile.read((char*)&drone->rot[X], sizeof(float));
    recordFile.read((char*)&drone->rot[Y], sizeof(float));
    recordFile.read((char*)&drone->rot[Z], sizeof(float));
}

void WidgetOpenGLDraw::stopReplay() {
    replaying = false;
    // Close file
    recordFile.close();
}

void WidgetOpenGLDraw::record(uint16_t value[]) {
    recordFile.write((char*)&value[0], 2);
    recordFile.write((char*)&value[1], 2);
    recordFile.write((char*)&value[2], 2);
    recordFile.write((char*)&value[3], 2);
    recordFile.write((char*)&value[4], 2);
}

void WidgetOpenGLDraw::replay(uint16_t value[]) {
    recordFile.read((char*)&value[0], 2);
    recordFile.read((char*)&value[1], 2);
    recordFile.read((char*)&value[2], 2);
    recordFile.read((char*)&value[3], 2);
    recordFile.read((char*)&value[4], 2);

    if(recordFile.eof())
        stopReplay();

    calculateForces(value);
}

void WidgetOpenGLDraw::initPrediction() {
    prediction = new Group();
    for(float i = 0.0f; i < 0.95f; i += 0.1f) {
        addObject("Objects/kocka.obj");
        allObjects.back().color = glm::vec4(1.0f, 1.0f, 0.8f, i);
        allObjects.back().visible = false;
        allObjects.back().scale = glm::vec3(0.02f - (i * 0.015f), 0.02f - (i * 0.015f), 0.02f - (i * 0.015f));
        allObjects.back().rot = glm::vec3(45.0f, 0.0f, 45.0f);
        prediction->addPart(allObjects.back());
    }
    allGroups.push_back(prediction);
}

void WidgetOpenGLDraw::hidePrediction() {
    for(uint8_t i = 0; i < 10; i++)
        prediction->parts[i].visible = false;
}

void WidgetOpenGLDraw::showPrediction() {
    for(uint8_t i = 0; i < 10; i++)
        prediction->parts[i].visible = true;
}

void WidgetOpenGLDraw::clearGroup(Group* g) {
    for(unsigned int i = 0; i < g->parts.size(); i++) {
        gl->glDeleteVertexArrays(1, &g->parts[i].id_VAO_object);
    }
    g->parts.clear();
}

void Object::generateM() {
    M = glm::mat4(1);

    M = glm::translate(M, this->parent->pos);
    M = glm::rotate_slow(M, glm::radians(this->parent->rot[Y]), glm::vec3(0, 1, 0));
    M = glm::rotate_slow(M, glm::radians(this->parent->rot[X]), glm::vec3(1, 0, 0));
    M = glm::rotate_slow(M, glm::radians(this->parent->rot[Z]), glm::vec3(0, 0, 1));
    glm::mat4 scaleM = glm::mat4(glm::vec4(this->parent->scale[X], 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, this->parent->scale[Y], 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, this->parent->scale[Z], 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    M = M * scaleM;

    M = glm::translate(M, this->pos);
    M = glm::rotate_slow(M, glm::radians(this->rot[X]), glm::vec3(1, 0, 0));
    M = glm::rotate_slow(M, glm::radians(this->rot[Y]), glm::vec3(0, 1, 0));
    M = glm::rotate_slow(M, glm::radians(this->rot[Z]), glm::vec3(0, 0, 1));
    scaleM = glm::mat4(glm::vec4(this->scale[X], 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, this->scale[Y], 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, this->scale[Z], 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    M = M * scaleM;
}

void Group::generateMs() {
    for(unsigned int i = 0; i < this->parts.size(); i++) {
        this->parts[i].generateM();
    }
}

void Group::addPart(Object o) {
    o.parent = this;
    this->parts.push_back(o);
    this->parts.back().generateM();
}

void WidgetOpenGLDraw::addDrone() {
    Group *g = new Group();
    g->rot = glm::vec3(0.0f, 180.0f, 0.0f);

    addObject("Objects/drone.obj");
    g->addPart(allObjects.back());
    addObject("Objects/DronePCB.obj");
    g->addPart(allObjects.back());
    addObject("Objects/Propeller.obj");
    g->addPart(allObjects.back());
    g->parts.back().pos = glm::vec3(0.084944f, 0.031319f, -0.086413f);  // FL
    addObject("Objects/Propeller.obj");
    g->addPart(allObjects.back());
    g->parts.back().pos = glm::vec3(-0.084944f, 0.031319f, -0.086413f); // FR
    addObject("Objects/Propeller.obj");
    g->addPart(allObjects.back());
    g->parts.back().pos = glm::vec3(0.084944f, 0.031319f, 0.086413f);   // BR
    addObject("Objects/Propeller.obj");
    g->addPart(allObjects.back());
    g->parts.back().pos = glm::vec3(-0.084944f, 0.031319f, 0.086413f);   // BL

    g->parts[0].color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    g->parts[1].color = glm::vec4(0.0f, 0.6f, 0.0f, 0.0f);
    g->parts[2].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    g->parts[3].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    g->parts[4].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    g->parts[5].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    allGroups.push_back(g);

    drone = g;
    drone->generateMs();
}

void WidgetOpenGLDraw::addHouse(glm::vec3 coords) {
    Group *H = new Group();
    float scaleX = rand() % 3 + 7;
    float scaleY = rand() % 3 + 7;
    float scaleZ = rand() % 5 + 9;
    addObject("Objects/kocka.obj");
    allObjects.back().color = glm::vec4(1.0f, 1.0f, 0.5f, 0.0f);
    H->addPart(allObjects.back());
    addObject("Objects/streha.obj");
    allObjects.back().color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    allObjects.back().scale = glm::vec3(1.1f, 1.0f, 1.1f);
    H->addPart(allObjects.back());
    H->pos = coords + glm::vec3(rand() % 91 - 45, 0, rand() % 91 - 45);
    H->rot = glm::vec3(0.0f, rand() % 360, 0.0f);
    H->scale = glm::vec3(scaleX, scaleY, scaleZ);
    H->generateMs();

    allGroups.push_back(H);
}

void WidgetOpenGLDraw::addTree(glm::vec3 coords) {
    uint8_t height = rand() % 10 + 6;
    uint8_t krosnja = rand() % 3 + (height >> 2) + 2;
    Group *tree = new Group();
    addObject("Objects/deblo.obj");
    allObjects.back().color = glm::vec4(0.6f, 0.23f, 0.0f, 0.0f);
    allObjects.back().scale = glm::vec3(1.0f, height / 2, 1.0f);
    tree->addPart(allObjects.back());
    addObject("Objects/icosphere.obj");
    allObjects.back().color = glm::vec4(0.0f, 0.4f, 0.0f, 0.0f);
    allObjects.back().scale = glm::vec3(krosnja, krosnja, krosnja);
    allObjects.back().pos = glm::vec3(0.0f, height, 0.0f);
    tree->addPart(allObjects.back());
    tree->pos = coords + glm::vec3(rand() % 41 - 20, 0, rand() % 41 - 20);
    tree->rot = glm::vec3(0.0f, rand() % 360, 0.0f);
    tree->generateMs();

    allGroups.push_back(tree);
}

void WidgetOpenGLDraw::generateTerrain() {
    // Od -500 do 500 = 10x10 tilov
    uint8_t tileType;
    for(int8_t i = -5; i < 5; i++) {
        for(int8_t j = -5; j < 5; j++) {
            tileType = rand() % 4;
            if(tileType == 0)
                addHouse(glm::vec3(i * 100 + 50, 0, j * 100 + 50));
            else {
                addTree(glm::vec3(i * 100 + 25, 0, j * 100 + 25));
                addTree(glm::vec3(i * 100 + 25, 0, j * 100 + 75));
                addTree(glm::vec3(i * 100 + 75, 0, j * 100 + 25));
                addTree(glm::vec3(i * 100 + 75, 0, j * 100 + 75));
            }
        }
    }
}

WidgetOpenGLDraw::WidgetOpenGLDraw(QWidget *parent) : QOpenGLWidget(parent) {
    Object c1, c2, c3;
    c1.pos = glm::vec3(0.0f, 2.0f, -5.0f);
    c2.pos = glm::vec3(0.0f, -2.0f, -5.0f);
    c3.pos = glm::vec3(0.0f, -2.0f, -5.0f);
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
    for(unsigned int i = 0; i < allGroups.size(); i++) {
        for(unsigned int j = 0; j < allGroups[i]->parts.size(); j++) {
            gl->glDeleteVertexArrays(1, &allGroups[i]->parts[j].id_VAO_object);
        }
        delete allGroups[i];
    }
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

    generateFloor(glm::vec3(0, -1.5f, 0));        // Tla

    Group *g = new Group();
    g->addPart(allObjects[0]);
    allGroups.push_back(g);

    gl->glGenVertexArrays(1, &allGroups.back()->parts.back().id_VAO_object);
    gl->glBindVertexArray(allGroups.back()->parts.back().id_VAO_object);

    //Light.pos = glm::vec3(0.0f, 30.0f, 0.0f);

    //addObject("Objects/kocka.obj");
    //allObjects[0].pos[Y] = -1.0f;
    //allObjects[0].scale = glm::vec3(20.0f, 1.0f, 20.0f);
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

    addObject("Objects/kocka.obj");
    allObjects.back().scale = glm::vec3(1000.0f, 1.0f, 1000.0f);
    allObjects.back().color = glm::vec4(0.6, 0.9, 0.2, 0.0);
    allObjects.back().pos = glm::vec3(0.0f, -1.0f, 0.0f);
    g->addPart(allObjects.back());

    addDrone();
    //addHouse(glm::vec3(10.0f, 0.0f, 10.0f));
    generateTerrain();

    selectedGroup = 1;
    //makeCurrent();
}

void WidgetOpenGLDraw::resizeGL(int w, int h) {
    gl->glViewport(0, 0, w, h);
}

void WidgetOpenGLDraw::paintGL() {
    // počisti ozadje in globinski pomnilnik (za testiranje globine)
    gl->glClearColor(0.7f, 0.9f, 0.9f, 1);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Light.pos = glm::vec3(drone->pos[X], drone->pos[Y] + 30.0f, drone->pos[Z]);

    for(unsigned int i = 0; i < allGroups.size(); i++) {
        // projekcijska matrika
        glm::mat4 P;

        P = glm::perspective(glm::radians(FOV), float(width()) / height(), 0.01f, 1000.0f);

        // Matrika pogleda (view)
        glm::mat4 V = glm::mat4(1);

        switch(selectedCamera) {
        case LOOKAT_CAM:
            V = glm::lookAt(camera[LOOKAT_CAM].pos, glm::normalize(drone->pos - camera[LOOKAT_CAM].pos) + camera[LOOKAT_CAM].pos, glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case DRONE_CAM:
            V = glm::translate(V, glm::vec3(0.0f, -0.15f, -0.6f));
            V = glm::rotate_slow(V, glm::radians(-drone->rot[Y]), glm::vec3(0.0f, 1.0f, 0.0f));
            V = glm::translate(V, -drone->pos);
            break;
        case FREE_CAM:
            V = glm::rotate_slow(V, glm::radians(camera[FREE_CAM].rot[X]), glm::vec3(1, 0, 0));
            V = glm::rotate_slow(V, glm::radians(camera[FREE_CAM].rot[Y]), glm::vec3(0, 1, 0));
            V = glm::rotate_slow(V, glm::radians(camera[FREE_CAM].rot[Z]), glm::vec3(0, 0, 1));
            V = glm::translate(V, camera[FREE_CAM].pos);
            break;
        }

        for(unsigned int j = 0; j < allGroups[i]->parts.size(); j++) {
            if(!allGroups[i]->parts[j].visible)
                continue;
            gl->glBindVertexArray(allGroups[i]->parts[j].id_VAO_object);
            gl->glUseProgram(id_sencilni_program);

            glm::mat4 PVM = P * V * allGroups[i]->parts[j].M;

            gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_sencilni_program, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
            gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_sencilni_program, "M"), 1, GL_FALSE, glm::value_ptr(allGroups[i]->parts[j].M));
            gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_sencilni_program, "V"), 1, GL_FALSE, glm::value_ptr(V));

            //gl->glActiveTexture(GL_TEXTURE0);
            //gl->glBindTexture(GL_TEXTURE_2D, allObjects[i].textureID);
            //gl->glUniform1i(gl->glGetUniformLocation(id_sencilni_program, "myTextureSampler"), 0);
            gl->glUniform4f(gl->glGetUniformLocation(id_sencilni_program, "DodajBarvo"), allGroups[i]->parts[j].color[0], allGroups[i]->parts[j].color[1], allGroups[i]->parts[j].color[2], allGroups[i]->parts[j].color[3]);
            gl->glUniform3f(gl->glGetUniformLocation(id_sencilni_program, "LightPosition_worldspace"), Light.pos[X], Light.pos[Y], Light.pos[Z]);
            gl->glUniform1i(gl->glGetUniformLocation(id_sencilni_program, "shining"), allGroups[i]->parts[j].shining);
            gl->glUniform1f(gl->glGetUniformLocation(id_sencilni_program, "lightPower"), lightPower);
            //gl->glDrawArrays(GL_TRIANGLES, static_cast<int>(allObjects[i].verticesStartingPosition), static_cast<int>(allObjects[i].vertices.size()));
            gl->glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(allGroups[i]->parts[j].vertices.size()));
        }
    }

    const unsigned int err = gl->glGetError();
    if (err != 0) {
        std::cerr << "OpenGL napaka: " << err << std::endl;
    }
}

void WidgetOpenGLDraw::MoveObjXPlus() {
    makeCurrent();
    drone->pos[X] += 0.2f;
    drone->generateMs();
    update();
}

void WidgetOpenGLDraw::MoveObjXMinus() {
    makeCurrent();
    drone->pos[X] -= 0.2f;
    drone->generateMs();
    update();
}

void WidgetOpenGLDraw::MoveObjYPlus() {
    makeCurrent();
    drone->pos[Y] += 0.2f;
    drone->generateMs();
    update();
}

void WidgetOpenGLDraw::MoveObjYMinus() {
    makeCurrent();
    drone->pos[Y] -= 0.2f;
    drone->generateMs();
    update();
}

void WidgetOpenGLDraw::MoveObjZPlus() {
    makeCurrent();
    drone->pos[Z] += 0.2f;
    drone->generateMs();
    update();
}

void WidgetOpenGLDraw::MoveObjZMinus() {
    makeCurrent();
    drone->pos[Z] -= 0.2f;
    drone->generateMs();
    update();
}
