#ifndef WIDGETOPENGLDRAW_H
#define WIDGETOPENGLDRAW_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <glm/glm.hpp>
#include <vector>
#include <QtSerialPort>

enum Coordinate {X, Y, Z};
enum CAM {LOOKAT_CAM, DRONE_CAM, FREE_CAM};

class Group;

class Object {
public:
    unsigned int index;                              // Index of object created
    unsigned int verticesStartingPosition;  // Starting position of object vertices in buffer
    glm::vec4 color;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textures;
    std::vector<glm::vec3> normals;

    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    int shining = 5;
    unsigned int textureID;
    glm::mat4 M;
    Group* parent;
    GLuint id_VAO_object;

    bool visible = true;

    void generateM();
};

class Group {
public:
    std::vector<Object> parts;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    void generateMs();
    void addPart(Object o);
};

class QOpenGLFunctions_3_3_Core;
class WidgetOpenGLDraw : public QOpenGLWidget{
public:
    WidgetOpenGLDraw(QWidget* parent);

    ~WidgetOpenGLDraw();
    void MoveObjXPlus();
    void MoveObjXMinus();
    void MoveObjYPlus();
    void MoveObjYMinus();
    void MoveObjZPlus();
    void MoveObjZMinus();

    void addObject(QString fileName);

    unsigned int getObjectCount();
    void setSelectedObject(int index);
    glm::vec3 getObjectColor();
    void setObjectColor(glm::vec3 color);
    void setObjectShining(int s);
    int getObjectShining();
    void loadTexture(QString path);
    void setLightPower(float p);
    void openSerialPort(QString s);
    void infiniteReadSerial();
    void calculateForces(uint16_t value[]);

    void startRecording();
    void stopRecording();
    void startReplay();
    void stopReplay();
    void record(uint16_t value[]);
    void replay();

    void addDrone();
    void addHouse(glm::vec3 coords);
    void addTree(glm::vec3 coords);


private:
    void PrevediSencilnike();
    void printProgramInfoLog(GLuint obj);
    void printShaderInfoLog(GLuint obj);

    void generateFloor(glm::vec3 izhodisce);
    void loadObjFile(QString fileName);

    QOpenGLFunctions_3_3_Core* gl;
    unsigned int id_sencilni_program;
    std::vector<unsigned int> id_buffer_object;
    std::vector<unsigned int> id_buffer_normal;
    std::vector<unsigned int> id_buffer_texture;
    std::vector<GLuint> id_VAO_object;

    unsigned int objectCount = 0;
    unsigned int selectedGroup = 0;

    std::vector<Object> camera;
    int selectedCamera = 0;
    float FOV = 60.0f;

    //uint8_t projectionType = 0;     // 0 = perspektivna; 1 = paralelna; 2 = ortogonalna

    std::vector<glm::vec4> cubeColor;
    std::vector<glm::vec3> allVertices;
    std::vector<glm::vec3> allNormals;
    std::vector<Object> allObjects;
    std::vector<Group*> allGroups;

    Group* drone;

    Object Light;
    glm::vec3 lightPos = glm::vec3(0.0f, 15.0f, 0.0f);
    glm::vec3 lightColor = glm::vec3(lightColor);
    float lightPower = 150.0f;

    QSerialPort serialPort;

    // Variables, used for simulation
    const float vMax = 150.0f;   // Maksimalna hitrost (m/s)
    const float Tfull = 8.0f;  // Maksimalna moč motorjev (N)
    const float m = 0.4f;       // Masa drona (kg)
    const float g = 9.81f;      // Grafitacijski pospešek (m/s^2)
    const glm::vec3 fg = glm::vec3(0, -3.924, 0);   // Sila gravitacije (N)
    const float t = 0.02;       // Časovna enota (s)

    float T;                    // Trenutni pogon (N)
    float fm;                   // Sila motorjev (N)
    float addRot;               // Dodatna rotacija (stopinje)
    float rot = 180.0f;         // Rotacija drona (stopinje)
    float vzuSize;              // Trenutna hitrost (m/s)
    glm::vec3 angle;            // Nagib (rotacija) po vseh treh oseh (stopinje)
    glm::vec3 norm;             // Normala drona
    glm::vec3 fThr;             // Sila pogona (T * norm)
    glm::vec3 fr;               // Rezultanta vseh sil (N)
    glm::vec3 a;                // Vektor pospeška (m/s^2)
    glm::vec3 v = glm::vec3(0.0, 0.0, 0.0);     // Vektor hitrosti (m/s)
    glm::vec3 vNew;             // Vektor razlike hitrosti v tej časovni enoti (m/s)
    glm::vec3 vzu;              // Vektor zračnega upora (-x * v)

    bool recording = false;
    bool replaying = false;

protected:

    void paintGL() override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;

    void keyPressEvent(QKeyEvent *event) override;

signals:

public slots:

};

#endif // WIDGETOPENGLDRAW_H
